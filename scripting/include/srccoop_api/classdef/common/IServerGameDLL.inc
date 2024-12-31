#pragma newdecls required
#pragma semicolon 1

static IServerGameDLL g_ServerGameDLL;

static Handle g_pGameShutdown;

methodmap IServerGameDLL
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		if (!(g_ServerGameDLL = view_as<IServerGameDLL>(GetInterface(hGameConfig, "server", "IServerGameDLL"))))
			SetFailState("Could not get interface for %s", "IServerGameDLL");
		
		char szGameShutdown[] = "CServerGameDLL::GameShutdown";
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGameShutdown))
			LogMessage("Could not obtain gamedata offset %s", szGameShutdown);
		else
		{
			if (!(g_pGameShutdown = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGameShutdown);
		}
	}
	public static IServerGameDLL Get()
	{
		return g_ServerGameDLL;
	}
	
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public void GameShutdown()
	{
		SDKCall(g_pGameShutdown, this); // GameShutdown() just resets global states
	}
}
