#if defined _srccoop_manager_persistence_included
 #endinput
#endif
#define _srccoop_manager_persistence_included

void GetSaveDataPath(char buffer[PLATFORM_MAX_PATH], char szFileName[64])
{
	char[] szFolder = "data/srccoop/savedata";
	BuildPath(Path_SM, buffer, sizeof(buffer), "%s/%s", szFolder, szFileName);
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
	GetSaveDataPath(szFullPath, szFileName);

	KeyValues kv = new KeyValues("SaveData");
	bool shouldSave = false;
	for (int i = 1; i < MaxClients; i++)
	{
		if (strlen(g_szSteamIds[i]) == 0)
			continue;
		
		CCoopEquipment equipment; 

		if (!g_pEquipmentManager.GetEquipment(g_szSteamIds[i], equipment))
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
				kv.SetNum(pWeaponEntry.m_szClassname, 1);
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
	return Plugin_Handled;
}

public Action Command_Load(int iClient, int iArgs)
{
	if (iArgs != 1)
	{
		MsgReply(iClient, "Format: sc_load <FILENAME>");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	GetSaveDataPath(szFullPath, szFileName);

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

	for (int i = 1; i < MaxClients; i++)
	{
		if (strlen(g_szSteamIds[i]) == 0)
			continue;

		if (!kv.JumpToKey(g_szSteamIds[i]))
			continue;

		CBasePlayer pPlayer = CBasePlayer(i);
		if (!pPlayer.IsValid())
			continue;

		CCoopEquipment equipment;
		equipment.Initialize();

		equipment.m_iHealth = kv.GetNum("hp");
		equipment.m_iArmor = kv.GetNum("armor");

		if (!kv.JumpToKey("weapons"))
		{
			MsgReply(iClient, "Unable to find weapons section in KeyValues file");
			delete kv;
			CloseHandle(equipment.m_pWeaponList);
			return Plugin_Handled;	
		}

		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char weaponName[MAX_CLASSNAME];
				if (kv.GetSectionName(weaponName, sizeof(weaponName)))
				{
					CCoopWeaponEntry pWeaponEntry;
					strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), weaponName);
					pWeaponEntry.m_iPrimaryAmmo = -1;
					pWeaponEntry.m_iSecondaryAmmo = -1;
					equipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
				}
			}
			while (kv.GotoNextKey(false));

			kv.GoBack(); //return to weapons node
		}

		kv.GoBack(); //return to player node
		
		if (!kv.JumpToKey("ammo"))
		{
			MsgReply(iClient, "Unable to find ammo section in KeyValues file");
			delete kv;
			CloseHandle(equipment.m_pWeaponList);
			return Plugin_Handled;	
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

		g_pEquipmentManager.StoreEquipment(g_szSteamIds[i], equipment);
		g_SpawnSystem.StripPlayer(pPlayer);
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);

		MsgReply(iClient, "Loaded Equipment for player %s", g_szSteamIds[i]);
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

	for (int i = 1; i < MaxClients; i++)
	{
		if (strlen(g_szSteamIds[i]) == 0)
			continue;

		CBasePlayer pPlayer = CBasePlayer(i);
		if (!pPlayer.IsValid())
			continue;

		g_pEquipmentManager.Clear(g_szSteamIds[i]);

		g_SpawnSystem.StripPlayer(pPlayer);
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);
	}

	return Plugin_Handled;
	
}