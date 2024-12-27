#pragma newdecls required
#pragma semicolon 1

static IServerTools g_ServerTools;

static Handle g_pGetTempEntsSystem;

methodmap IServerTools
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		if (!(g_ServerTools = view_as<IServerTools>(GetInterface(hGameConfig, "server", "IServerTools"))))
			SetFailState("Could not get interface for %s", "IServerTools");

		char szGetTempEntsSystem[] = "CServerTools::GetTempEntsSystem";
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetTempEntsSystem))
			LogMessage("Could not obtain gamedata offset %s", szGetTempEntsSystem);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetTempEntsSystem = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetTempEntsSystem);
		}
	}
	public static IServerTools Get()
	{
		return g_ServerTools;
	}
	
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public Address GetTempEntsSystem()
	{
		return SDKCall(g_pGetTempEntsSystem, this);
	}
}
