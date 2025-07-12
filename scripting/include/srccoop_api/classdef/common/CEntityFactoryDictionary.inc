#pragma newdecls required
#pragma semicolon 1

static CEntityFactoryDictionary g_pInstance;
static Handle g_pFindFactory;
static Handle g_pInstallFactory;

methodmap CEntityFactoryDictionary
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, "CEntityFactoryDictionary::FindFactory"))
			LogMessage("Could not obtain gamedata offset CEntityFactoryDictionary::FindFactory");
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pFindFactory = EndPrepSDKCall())) SetFailState("Could not prep SDK call CEntityFactoryDictionary::FindFactory");
		}
		
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, "CEntityFactoryDictionary::InstallFactory"))
			LogMessage("Could not obtain gamedata offset CEntityFactoryDictionary::InstallFactory");
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			g_pInstallFactory = EndPrepSDKCall();
			if (!(g_pInstallFactory = EndPrepSDKCall())) SetFailState("Could not prep SDK call CEntityFactoryDictionary::InstallFactory");
		}
	}

	public static CEntityFactoryDictionary Get()
	{
		return g_pInstance ? g_pInstance : (g_pInstance = IServerTools.Get().GetEntityFactoryDictionary());
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public Address FindFactory(const char[] szClassname)
	{
		return view_as<Address>(SDKCall(g_pFindFactory, this, szClassname));
	}
	public bool CreateAlias(const char[] szNewClassname, const char[] szExistingClassname)
	{
		if (!g_pInstallFactory || !g_pFindFactory)
			return false;
		
		// Make sure this is not already registered
		Address pExistingClass = this.FindFactory(szNewClassname);
		if (pExistingClass == Address_Null)
		{
			pExistingClass = this.FindFactory(szExistingClassname);
			if (pExistingClass != Address_Null)
			{
				SDKCall(g_pInstallFactory, this, pExistingClass, szNewClassname);
				return true;
			}
			else
			{
				LogMessage("InstallFactory failed to get address of existing class: \"%s\"", szExistingClassname);
			}
		}
		return false;
	}
}
