#pragma newdecls required
#pragma semicolon 1

static CPlayerInfoManager g_pPlayerInfoManager;

static Handle g_pGetGlobalVars;

methodmap CPlayerInfoManager
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		if (!(g_pPlayerInfoManager = view_as<CPlayerInfoManager>(GetInterface(hGameConfig, "server", "PlayerInfoManager"))))
			SetFailState("Could not get interface for %s", "PlayerInfoManager");
		
		char szGetGlobalVars[] = "CPlayerInfoManager::GetGlobalVars";
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetGlobalVars))
			LogMessage("Could not obtain gamedata offset %s", szGetGlobalVars);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetGlobalVars = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetGlobalVars);
		}
	}
	public static CPlayerInfoManager Get()
	{
		return g_pPlayerInfoManager;
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public CGlobalVars GetGlobalVars()
	{
		return SDKCall(g_pGetGlobalVars, this.GetAddress());
	}
}
