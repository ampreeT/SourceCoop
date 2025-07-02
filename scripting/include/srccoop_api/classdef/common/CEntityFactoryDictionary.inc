#pragma newdecls required
#pragma semicolon 1

static Handle g_pGetEntityFactory;
static Handle g_pFindFactory;
static Handle g_pInstallFactory;

methodmap CEntityFactoryDictionary
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, "EntityFactoryDictionary"))
			LogMessage("Could not obtain gamedata signature EntityFactoryDictionary");
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetEntityFactory = EndPrepSDKCall())) PrintToServer("Could not prep SDK call EntityFactoryDictionary");
		}
		
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, "CEntityFactoryDictionary::FindFactory"))
			LogMessage("Could not obtain gamedata signature CEntityFactoryDictionary::FindFactory");
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pFindFactory = EndPrepSDKCall())) PrintToServer("Could not prep SDK call CEntityFactoryDictionary::FindFactory");
		}
		
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, "CEntityFactoryDictionary::InstallFactory"))
			LogMessage("Could not obtain gamedata signature CEntityFactoryDictionary::InstallFactory");
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			g_pInstallFactory = EndPrepSDKCall();
			if (!(g_pInstallFactory = EndPrepSDKCall())) PrintToServer("Could not prep SDK call CEntityFactoryDictionary::InstallFactory");
		}
		
		// Perform registration of class remapping if all calls are valid
		if (g_pInstallFactory != INVALID_HANDLE && g_pFindFactory != INVALID_HANDLE && g_pGetEntityFactory != INVALID_HANDLE)
		{
			// Static mappings:
			CEntityFactoryDictionary.InstallClassMapping("trigger_coop", "trigger_multiple");
			
			char szSMPath[PLATFORM_MAX_PATH];
			BuildPath(Path_SM, szSMPath, sizeof(szSMPath), "configs");
			Format(szSMPath, sizeof(szSMPath), "%s/srccoop/entitymappings.cfg", szSMPath);
			ReplaceString(szSMPath, sizeof(szSMPath), "\\", "/", false);
			if (FileExists(szSMPath, true, NULL_STRING))
			{
				KeyValues hKV = CreateKeyValues("Entities");
				if (!FileToKeyValues(hKV, szSMPath))
				{
					PrintToServer("Failed to get entity list!");
					CloseHandle(hKV);
				}
				else
				{
					bool bFirst = KvGotoFirstSubKey(hKV, true);
					char szEntityClass[128];
					char szBaseClass[128];
					while (bFirst || KvGotoNextKey(hKV, true))
					{
						bFirst = false;
						KvGetSectionName(hKV, szEntityClass, sizeof(szEntityClass));
						KvGetString(hKV, "baseclass", szBaseClass, sizeof(szBaseClass), "");
						
						if (strlen(szBaseClass) < 1)
							continue;
						
						if (CEntityFactoryDictionary.InstallClassMapping(szEntityClass, szBaseClass))
							PrintToServer("New registration '%s' with baseclass '%s' succeeded", szEntityClass, szBaseClass);
					}
					CloseHandle(hKV);
				}
			}
		}
	}
	public static Address FindFactory(const char[] szClassname)
	{
		return view_as<Address>(SDKCall(g_pFindFactory, SDKCall(g_pGetEntityFactory), szClassname));
	}
	public static bool InstallClassMapping(const char[] szNewClassname, const char[] szExistingClassname)
	{
		// Make sure this is not already registered
		Address iExistingClass = CEntityFactoryDictionary.FindFactory(szNewClassname);
		if (view_as<int>(iExistingClass) == 0)
		{
			iExistingClass = CEntityFactoryDictionary.FindFactory(szExistingClassname);
			if (view_as<int>(iExistingClass) != 0)
			{
				SDKCall(g_pInstallFactory, SDKCall(g_pGetEntityFactory), iExistingClass, szNewClassname);
				return true;
			}
			else
			{
				LogMessage("InstallFactory failed to get address of existing class: '%s'", szExistingClassname);
			}
		}
		return false;
	}
}
