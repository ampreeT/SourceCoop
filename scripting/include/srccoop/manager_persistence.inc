#pragma newdecls required
#pragma semicolon 1

#define SAVEDATA_PATH "data/srccoop/savedata"

enum struct EquipmentManagerData
{
	StringMap m_smEquipmentMap;
}

EquipmentManagerData g_pEquipmentManagerData;

#define data g_pEquipmentManagerData

methodmap EquipmentManager 
{
    
	public static void Initialize()
	{
		data.m_smEquipmentMap = new StringMap();
		RegAdminCmd("sc_save", Command_ExportEquipment, ADMFLAG_ROOT, "Exports last saved player equipment state to a file. Equipment state is saved at the end of a map, so this wil record the state as of the start of a map.");
		RegAdminCmd("sourcecoop_save", Command_ExportEquipment, ADMFLAG_ROOT, "Exports last saved player equipment state to a file. Equipment state is saved at the end of a map, so this wil record the state as of the start of a map.");
		RegAdminCmd("sc_load", Command_ImportEquipment, ADMFLAG_ROOT, "Imports saved data from file and attempts to equip each player (if they were present when the data was saved).");
		RegAdminCmd("sourcecoop_load", Command_ImportEquipment, ADMFLAG_ROOT, "Imports saved data from file and attempts to equip each player (if they were present when the data was saved).");
		RegAdminCmd("sc_clear", Command_ClearEquipment, ADMFLAG_ROOT, "Clear persisted equipment and equip players with the map defaults.");
		RegAdminCmd("sourcecoop_clear", Command_ClearEquipment, ADMFLAG_ROOT, "Clear persisted equipment and equip players with the map defaults.");
	}

	public static void Clear(const char[] szSteamId)
	{
		CCoopEquipment equipment;
		if (!EquipmentManager.GetEquipmentSteam(szSteamId, equipment))
			return;

		equipment.Close();
		data.m_smEquipmentMap.Remove(szSteamId);
	}

	public static void ClearAll()
	{
		CCoopEquipment equipment;
		StringMapSnapshot ss = data.m_smEquipmentMap.Snapshot();
		for (int i = 0; i < ss.Length; i++)
		{
			int len = ss.KeyBufferSize(i);
			char[] szKey = new char[len];
			ss.GetKey(i, szKey, len);
			EquipmentManager.GetEquipmentSteam(szKey, equipment);
			equipment.Close();
		}
		ss.Close();
		data.m_smEquipmentMap.Clear();
	}

	public static void StoreEquipment(const char[] szSteamId, CCoopEquipment equipment)
	{
		CCoopEquipment existingEquipment;
		if (EquipmentManager.GetEquipmentSteam(szSteamId, existingEquipment))
		{
			existingEquipment.Close();
		}
		data.m_smEquipmentMap.SetArray(szSteamId, equipment, sizeof(equipment), true);
	}

	public static bool GetEquipmentSteam(const char[] szSteamId, CCoopEquipment equipment)
	{
		return data.m_smEquipmentMap.GetArray(szSteamId, equipment, sizeof(equipment));
	}
	
	public static bool GetEquipment(CBasePlayer pPlayer, CCoopEquipment equipment)
	{
		if (strlen(g_szSteamIds[pPlayer.entindex]) > 0)
		{
			return EquipmentManager.GetEquipmentSteam(g_szSteamIds[pPlayer.entindex], equipment);
		}
		return false;
	}

	public static bool StoreCurrentEquipment(int client)
	{
		if (!IsClientInGame(client) || IsFakeClient(client) || !IsPlayerAlive(client) || strlen(g_szSteamIds[client]) == 0)
			return false;
		
		CBasePlayer pPlayer = CBasePlayer(client);
		CCoopEquipment playerEquipment;
		playerEquipment.Initialize();
		playerEquipment.Fill(pPlayer);
		
		EquipmentManager.StoreEquipment(g_szSteamIds[client], playerEquipment);
		return true;
	}

	public static void StoreCurrentEquipmentAll()
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			EquipmentManager.StoreCurrentEquipment(i);
		}
	}

	public static bool ImportEquipment(KeyValues kv, int iTarget, char[] error, int errorLength)
	{
		if (strlen(g_szSteamIds[iTarget]) == 0)
		{
			FormatEx(error, errorLength, "%t", "equipment load unknown steamid");
			return false;
		}

		if (!kv.JumpToKey(g_szSteamIds[iTarget]))
		{
			FormatEx(error, errorLength, "%t", "equipment load player not found", g_szSteamIds[iTarget]);
			return false;
		}

		CCoopEquipment equipment;
		equipment.Initialize();
		equipment.m_iHealth = kv.GetNum("hp");
		equipment.m_iArmor = kv.GetNum("armor");

		if (!kv.JumpToKey("weapons"))
		{
			FormatEx(error, errorLength, "%t", "equipment load missing section", "weapons");
			equipment.Close();
			return false;	
		}

		if (kv.GotoFirstSubKey(true))
		{
			do
			{
				char weaponName[MAX_CLASSNAME];
				if (kv.GetSectionName(weaponName, sizeof(weaponName)))
				{
					equipment.AddItem(weaponName, kv.GetNum("primaryammo"), kv.GetNum("secondaryammo"));
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack(); //return to weapons node
		}

		kv.GoBack(); //return to player node
		
		if (!kv.JumpToKey("ammo"))
		{
			FormatEx(error, errorLength, "%t", "equipment load missing section", "ammo");
			equipment.Close();
			return false;	
		}

		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			char szKey[MAX_CHARS_INT(MAX_AMMO_TYPES - 1)];
			IntToString(i, szKey, sizeof(szKey));
			equipment.m_iAmmoCount[i] = kv.GetNum(szKey, 0);
		}

		CBasePlayer pTarget = CBasePlayer(iTarget);
		EquipmentManager.StoreEquipment(g_szSteamIds[iTarget], equipment);

		if (pTarget.IsAlive())
		{
			pTarget.RemoveAllItems();
			equipment.Equip(pTarget);
		}

		return true;
	}

	public static bool ExportEquipment(KeyValues kv)
	{
		bool shouldSave = false;
		for (int i = 1; i <= MaxClients; i++)
		{
			if (!IsClientInGame(i) || IsFakeClient(i))
				continue;
			
			CBasePlayer pPlayer = CBasePlayer(i);
			CCoopEquipment equipment;
			bool close;
			if (pPlayer.IsAlive())
			{
				equipment.Initialize();
				equipment.Fill(pPlayer);
				close = true;
			}
			else if (!EquipmentManager.GetEquipment(pPlayer, equipment))
			{
				continue;
			}
			
			kv.JumpToKey(g_szSteamIds[i], true);
			kv.SetNum("hp", equipment.m_iHealth);
			kv.SetNum("armor", equipment.m_iArmor);
			kv.JumpToKey("weapons", true);

			for (int j = 0; j < equipment.m_pWeaponList.Length; j++)
			{
				CCoopWeaponEntry pWeaponEntry;
				if (equipment.m_pWeaponList.GetArray(j, pWeaponEntry))
				{
					kv.JumpToKey(pWeaponEntry.m_szClassname, true);
					kv.SetNum("primaryammo", pWeaponEntry.m_iPrimaryAmmo);
					kv.SetNum("secondaryammo", pWeaponEntry.m_iSecondaryAmmo);
					kv.GoBack();
				}
			}

			kv.GoBack();
			kv.JumpToKey("ammo", true);
			for (int k = 0; k < MAX_AMMO_TYPES; k++)
			{
				char szKey[MAX_CHARS_INT(MAX_AMMO_TYPES - 1)];
				IntToString(k, szKey, sizeof(szKey));
				kv.SetNum(szKey, equipment.m_iAmmoCount[k]);
			}

			if (close)
			{
				equipment.Close();
			}
			
			kv.Rewind();
			shouldSave = true;
		}
		return shouldSave;
	}

}

public Action Command_ExportEquipment(int iClient, int iArgs)
{
	if (iArgs != 1)
	{
		MsgReply(iClient, "Format: sc_save <FILENAME>");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	KeyValues kv = new KeyValues("SaveData");
	if (EquipmentManager.ExportEquipment(kv))
	{
		char szFullPath[PLATFORM_MAX_PATH];
		BuildPath(Path_SM, szFullPath, sizeof(szFullPath), "%s/%s", SAVEDATA_PATH, szFileName);
		
		kv.Rewind();
		if (kv.ExportToFile(szFullPath))
			MsgReply(iClient, "%t", "equipment saved");
		else
			MsgReply(iClient, "%t", "equipment export fail", szFullPath);
	}
	else
	{
		MsgReply(iClient, "%t", "equipment export no players");
	}

	delete kv;
	return Plugin_Handled;
}

public Action Command_ImportEquipment(int iClient, int iArgs)
{
	if (iArgs != 1 && iArgs != 2)
	{
		MsgReply(iClient, "Format: sc_load <FILENAME> [<PlayerName>]");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, szFullPath, sizeof(szFullPath), "%s/%s", SAVEDATA_PATH, szFileName);

	if (!FileExists(szFullPath))
	{
		MsgReply(iClient, "%t", "equipment import file not found", szFullPath);
		return Plugin_Handled;
	}

	KeyValues kv = new KeyValues("SaveData");
	if (!kv.ImportFromFile(szFullPath))
	{
		MsgReply(iClient, "%t", "equipment import fail", szFullPath);
		delete kv;
		return Plugin_Handled;	
	}

	int i = 1, iMax = MaxClients;

	if (iArgs == 2)
	{
		char szTarget[MAX_NAME_LENGTH];
		GetCmdArg(2, szTarget, sizeof(szTarget));
		
		i = iMax = FindTarget(iClient, szTarget, true, false);
		if (i == -1)
		{
			return Plugin_Handled;
		}
	}

	char szError[512];
	SetGlobalTransTarget(iClient);

	for (; i <= iMax; i++)
	{
		if (IsClientInGame(i) && !IsFakeClient(i))
		{
			if (EquipmentManager.ImportEquipment(kv, i, szError, sizeof(szError)))
			{
				MsgReply(iClient, "%t", "equipment loaded", i);
			}
			else
			{
				MsgReply(iClient, "%t", "equipment not loaded", i, szError);
			}
			kv.Rewind();
		}
	}

	delete kv;
	return Plugin_Handled;
}

public Action Command_ClearEquipment(int iClient, int iArgs)
{
	if (iArgs != 0)
	{
		MsgReply(iClient, "Format: sc_clear");
		return Plugin_Handled;
	}

	EquipmentManager.ClearAll();
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			pPlayer.RemoveAllItems();
			CCoopSpawnSystem.SpawnPlayerEquipment(pPlayer);
		}
	}

	MsgReply(iClient, "%t", "equipment cleared");
	return Plugin_Handled;
}

#undef data
