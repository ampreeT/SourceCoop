#if defined _srccoop_manager_persistence_included
 #endinput
#endif
#define _srccoop_manager_persistence_included


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
	}

	public static void Clear(char[] steamId)
	{
		CCoopEquipment equipment;
		if (!EquipmentManager.GetEquipment(steamId, equipment))
			return;

		equipment.Close();
		data.m_smEquipmentMap.Remove(steamId);
	}

    public static void StoreEquipment(char[] steamId, CCoopEquipment equipment)
	{
		CCoopEquipment existingEquipment;
		if (EquipmentManager.GetEquipment(steamId, existingEquipment))
		{
			existingEquipment.Close();
			data.m_smEquipmentMap.Remove(steamId);
		}
		data.m_smEquipmentMap.SetArray(steamId, equipment, sizeof(equipment), false);
    }
	
	public static bool GetEquipment(char[] steamId, CCoopEquipment equipment)
	{
		bool success = data.m_smEquipmentMap.GetArray(steamId, equipment, sizeof(equipment));
		return success;
    }

	public static void GetSaveDataPath(char buffer[PLATFORM_MAX_PATH], char szFileName[64])
	{
		char[] szFolder = "data/srccoop/savedata";
		BuildPath(Path_SM, buffer, sizeof(buffer), "%s/%s", szFolder, szFileName);
	}

	public static int GetClientIdFromName(char[] szName)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (!IsClientConnected(i))
				continue;
				
			char name[64];
			if (GetClientName(i, name, sizeof(name)))
			{
				if (strcmp(name, szName, false) == 0)
				{
					return i;
				}
			}
		}

		return -1;
	}

	public static void SavePlayerEquipment()
	{
		for (int iClient = 1; iClient <= MaxClients; iClient++)
		{
			if (strlen(g_szSteamIds[iClient]) == 0)
				continue;

			CBasePlayer pPlayer = CBasePlayer(iClient);
			if (!pPlayer.IsValid())
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

	public static bool LoadPlayerEquipmentFromFile(KeyValues kv, int iClient, int clientId)
	{
		if (strlen(g_szSteamIds[clientId]) == 0)
			return false;

		CBasePlayer pPlayer = CBasePlayer(clientId);
		if (!pPlayer.IsValid())
		{
			MsgReply(iClient, "Failed to load data: invalid player");
			return false;
		}

		if (!kv.JumpToKey(g_szSteamIds[clientId]))
		{
			MsgReply(iClient, "Save data not found for steam id %s", g_szSteamIds[clientId]);
			return false;
		}

		CCoopEquipment equipment;
		equipment.Initialize();

		equipment.m_iHealth = kv.GetNum("hp");
		equipment.m_iArmor = kv.GetNum("armor");

		if (!kv.JumpToKey("weapons"))
		{
			MsgReply(iClient, "Unable to find weapons section in KeyValues file");
			delete kv;
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
			delete kv;
			equipment.Close();
			return false;	
		}

		if (kv.GotoFirstSubKey(false))
		{
			do 
			{
				char ammoKey[3];
				if (kv.GetSectionName(ammoKey, sizeof(ammoKey)))
				{
					int index = StringToInt(ammoKey);
					int count = kv.GetNum(ammoKey);
					equipment.m_iAmmoCount[index] = count;
				}
			}
			while (kv.GotoNextKey(false));
		}

		EquipmentManager.StoreEquipment(g_szSteamIds[clientId], equipment);
		g_SpawnSystem.StripPlayer(pPlayer);
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);

		MsgReply(iClient, "Loaded Equipment for player %s", g_szSteamIds[clientId]);

		return true;
	}

}

public Action Command_Save(int iClient, int iArgs)
{
	if (iArgs != 1)
	{
		MsgReply(iClient, "Format: sc_save <FILENAME>");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	EquipmentManager.GetSaveDataPath(szFullPath, szFileName);

	KeyValues kv = new KeyValues("SaveData");
	bool shouldSave = false;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (strlen(g_szSteamIds[i]) == 0)
			continue;
		
		CCoopEquipment equipment; 

		if (!EquipmentManager.GetEquipment(g_szSteamIds[i], equipment))
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

		shouldSave = true;
	}

	if (!shouldSave)
	{
		MsgReply(iClient, "No valid players found. Could not save data");
		return Plugin_Handled;
	}

	kv.Rewind();
	kv.ExportToFile(szFullPath);
	delete kv;

	MsgReply(iClient, "Saved equipment successfully");
	return Plugin_Handled;
}

public Action Command_Load(int iClient, int iArgs)
{
	if (iArgs != 1 && iArgs != 2)
	{
		MsgReply(iClient, "Format: sc_load <FILENAME> [<PlayerName>]");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	EquipmentManager.GetSaveDataPath(szFullPath, szFileName);

	if (!FileExists(szFullPath))
	{
		MsgReply(iClient, "Could not find specified save file %s", szFullPath);
		return Plugin_Handled;
	}

	
	KeyValues kv = new KeyValues("SaveData");
	if (!kv.ImportFromFile(szFullPath))
	{
		MsgReply(iClient, "Failed to parse KeyValues file %s", szFullPath);
		delete kv;
		return Plugin_Handled;	
	}

	if (iArgs == 2)
	{
		char szPlayerName[64];
		GetCmdArg(2, szPlayerName, sizeof(szPlayerName));
		int clientId = EquipmentManager.GetClientIdFromName(szPlayerName);
		if (clientId == -1)
		{
			MsgReply(iClient, "Player %s not found", szPlayerName);
			return Plugin_Handled;
		}

		CBasePlayer pPlayer = CBasePlayer(clientId);
		if (!pPlayer.IsValid())
		{
			MsgReply(iClient, "Failed to load data for invalid player: %s", szPlayerName);
			return Plugin_Handled;
		}	
		if (!EquipmentManager.LoadPlayerEquipmentFromFile(kv, iClient, clientId))
		{
			MsgReply(iClient, "Failed to load data for player: %s", szPlayerName);
		}
		return Plugin_Handled;
		
	}

	for (int i = 1; i <= MaxClients; i++)
	{
		EquipmentManager.LoadPlayerEquipmentFromFile(kv, iClient, i);
	}

	return Plugin_Handled;
}

public Action Command_Clear_Equipment(int iClient, int iArgs)
{
	if (iArgs != 0)
	{
		MsgReply(iClient, "Format: sc_clear_equipment");
		return Plugin_Handled;
	}

	for (int i = 1; i <= MaxClients; i++)
	{
		if (strlen(g_szSteamIds[i]) == 0)
			continue;

		CBasePlayer pPlayer = CBasePlayer(i);
		if (!pPlayer.IsValid())
			continue;

		EquipmentManager.Clear(g_szSteamIds[i]);

		g_SpawnSystem.StripPlayer(pPlayer);
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);
	}

	return Plugin_Handled;
	
}












