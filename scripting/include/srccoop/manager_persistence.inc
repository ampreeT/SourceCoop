#if defined _srccoop_manager_persistence_included
 #endinput
#endif
#define _srccoop_manager_persistence_included

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
		RegAdminCmd("sc_save", Command_ExportEquipment, ADMFLAG_ROOT, "Exports last saved player equipment state. Equipment state is saved at the end of a map, so this wil record the state as of the start of a map.");
		RegAdminCmd("sourcecoop_save", Command_ExportEquipment, ADMFLAG_ROOT, "Exports last saved player equipment state. Equipment state is saved at the end of a map, so this wil record the state as of the start of a map.");
		RegAdminCmd("sc_load", Command_ImportEquipment, ADMFLAG_ROOT, "Imports saved data from file and attempts to equip each player (if they were present when the data was saved).");
		RegAdminCmd("sourcecoop_load", Command_ImportEquipment, ADMFLAG_ROOT, "Imports saved data from file and attempts to equip each player (if they were present when the data was saved).");
		RegAdminCmd("sc_clear", Command_ClearEquipment, ADMFLAG_ROOT, "Clear persisted equipment and equip players with the map defaults.");
		RegAdminCmd("sourcecoop_clear", Command_ClearEquipment, ADMFLAG_ROOT, "Clear persisted equipment and equip players with the map defaults.");
	}

	public static bool Enabled()
	{
		return CoopManager.IsFeatureEnabled(FT_TRANSFER_PLAYER_STATE);
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
			char[] szKey = new char[ss.KeyBufferSize(i)];
			EquipmentManager.GetEquipmentSteam(szKey, equipment);
			equipment.Close();
		}
		CloseHandle(ss);
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
		if (strlen(g_szSteamIds[pPlayer.GetEntIndex()]) > 0)
		{
			return EquipmentManager.GetEquipmentSteam(g_szSteamIds[pPlayer.GetEntIndex()], equipment);
		}
		return false;
	}

	public static void SavePlayerEquipment()
	{
		for (int iClient = 1; iClient <= MaxClients; iClient++)
		{
			if (strlen(g_szSteamIds[iClient]) == 0)
				continue;

			CBasePlayer pPlayer = CBasePlayer(iClient);
			if (!pPlayer.IsValid() || !pPlayer.IsAlive())
				continue;
			
			CCoopEquipment playerEquipment;
			playerEquipment.Initialize();

			playerEquipment.m_iHealth = pPlayer.GetHealth();
			playerEquipment.m_iArmor = pPlayer.GetArmor();

			for (int iWeapon = 0; iWeapon < MAX_WEAPONS; iWeapon++)
			{
				CBaseCombatWeapon pWeapon = pPlayer.GetWeaponFromIndex(iWeapon);
				char weaponClassName[MAX_CLASSNAME];
				if (pWeapon.IsValid() && pWeapon.GetClassname(weaponClassName, sizeof(weaponClassName)))
				{
					if (pPlayer.HasWeapon(weaponClassName))
					{
						CCoopWeaponEntry pWeaponEntry;
						strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), weaponClassName);
						pWeaponEntry.m_iPrimaryAmmo = pWeapon.GetPrimaryAmmo();
						pWeaponEntry.m_iSecondaryAmmo = -1;
						playerEquipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
					}
				}
			}

			if (pPlayer.HasSuit())
			{
				CCoopWeaponEntry pSuitEntry;
				pSuitEntry.m_szClassname = "item_suit";
				pSuitEntry.m_iPrimaryAmmo = -1;
				pSuitEntry.m_iSecondaryAmmo = -1;
				playerEquipment.m_pWeaponList.PushArray(pSuitEntry, sizeof(pSuitEntry));
			}

			for (int iAmmo = 0; iAmmo < MAX_AMMO_TYPES; iAmmo++)
			{
				playerEquipment.m_iAmmoCount[iAmmo] = pPlayer.GetAmmoFromIndex(iAmmo);
			}

			EquipmentManager.StoreEquipment(g_szSteamIds[iClient], playerEquipment);
		}
	}

	public static bool ImportEquipment(KeyValues kv, int iClient, int iTarget)
	{
		if (strlen(g_szSteamIds[iTarget]) == 0)
			return false;

		if (!kv.JumpToKey(g_szSteamIds[iTarget]))
		{
			MsgReply(iClient, "Save data not found for steam id %s", g_szSteamIds[iTarget]);
			return false;
		}

		CCoopEquipment equipment;
		equipment.Initialize();

		equipment.m_iHealth = kv.GetNum("hp");
		equipment.m_iArmor = kv.GetNum("armor");

		if (!kv.JumpToKey("weapons"))
		{
			MsgReply(iClient, "Unable to find weapons section in KeyValues file");
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
					CCoopWeaponEntry pWeaponEntry;
					strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), weaponName);
					pWeaponEntry.m_iPrimaryAmmo = kv.GetNum("primaryammo", 0);
					equipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack(); //return to weapons node
		}

		kv.GoBack(); //return to player node
		
		if (!kv.JumpToKey("ammo"))
		{
			MsgReply(iClient, "Unable to find ammo section in KeyValues file");
			equipment.Close();
			return false;	
		}

		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			char key[3];
			IntToString(i, key, sizeof(key));
			equipment.m_iAmmoCount[i] = kv.GetNum(key, 0);	
		}

		CBasePlayer pTarget = CBasePlayer(iTarget);
		EquipmentManager.StoreEquipment(g_szSteamIds[iTarget], equipment);

		if (pTarget.IsAlive())
		{
			pTarget.RemoveAllItems();
			equipment.Equip(pTarget);
		}

		MsgReply(iClient, "Loaded player state for %N", iTarget);
		return true;
	}

	public static bool ExportEquipment(KeyValues kv)
	{
		bool shouldSave = false;
		for (int i = 1; i <= MaxClients; i++)
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if(!pPlayer.IsValid())
				continue;

			CCoopEquipment equipment;
			if (!EquipmentManager.GetEquipment(pPlayer, equipment))
				continue;

			kv.JumpToKey(g_szSteamIds[i], true);
			kv.SetNum("hp", equipment.m_iHealth);
			kv.SetNum("armor", equipment.m_iArmor);
			kv.JumpToKey("weapons", true);

			for (int j = 0; j < equipment.m_pWeaponList.Length; j++)
			{
				CCoopWeaponEntry pWeaponEntry;
				if (equipment.m_pWeaponList.GetArray(j, pWeaponEntry, sizeof(pWeaponEntry)))
				{
					kv.JumpToKey(pWeaponEntry.m_szClassname, true);
					kv.SetNum("primaryammo", pWeaponEntry.m_iPrimaryAmmo);
					kv.GoBack();
				}
			}

			kv.GoBack();
			kv.JumpToKey("ammo", true);
			for (int k = 0; k < MAX_AMMO_TYPES; k++)
			{
				char key[3];
				IntToString(k, key, sizeof(key));
				kv.SetNum(key, equipment.m_iAmmoCount[k]);
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
			MsgReply(iClient, "Saved equipment successfully.");
		else
			MsgReply(iClient, "Issue occurred when exporting to file: %s", szFullPath);
	}
	else
	{
		MsgReply(iClient, "No valid players found. Could not save data.");
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
		MsgReply(iClient, "Could not find specified save file: %s", szFullPath);
		return Plugin_Handled;
	}

	KeyValues kv = new KeyValues("SaveData");
	if (!kv.ImportFromFile(szFullPath))
	{
		MsgReply(iClient, "Failed to parse KeyValues file: %s", szFullPath);
		delete kv;
		return Plugin_Handled;	
	}

	if (iArgs == 2)
	{
		char szTarget[64];
		GetCmdArg(2, szTarget, sizeof(szTarget));
		int iTarget = FindTarget(iClient, szTarget, true, false);
		if (iTarget != -1)
		{
			if (!EquipmentManager.ImportEquipment(kv, iClient, iTarget))
			{
				MsgReply(iClient, "Failed to load data for player: %N", iTarget);
			}
		}
	}
	else
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i))
			{
				EquipmentManager.ImportEquipment(kv, iClient, i);
				kv.Rewind();
			}
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
		CBasePlayer pPlayer = CBasePlayer(i);
		if (pPlayer.IsInGame() && pPlayer.IsAlive())
		{
			pPlayer.RemoveAllItems();
			g_SpawnSystem.SpawnPlayerEquipment(pPlayer);
		}
	}

	MsgReply(iClient, "Cleared player state successfully.");
	return Plugin_Handled;
}

#undef data
