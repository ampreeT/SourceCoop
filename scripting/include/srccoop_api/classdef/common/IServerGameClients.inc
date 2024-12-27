#pragma newdecls required
#pragma semicolon 1

static IServerGameClients g_ServerGameClients;

static Handle g_pGetPlayerState;

methodmap IServerGameClients
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		if (!(g_ServerGameClients = view_as<IServerGameClients>(GetInterface(hGameConfig, "server", "IServerGameClients"))))
			SetFailState("Could not get interface for %s", "IServerGameClients");
		
		char szGetPlayerState[] = "CServerGameClients::GetPlayerState";
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetPlayerState))
			LogMessage("Could not obtain gamedata offset %s", szGetPlayerState);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Edict, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetPlayerState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetPlayerState);
		}
	}
	public static IServerGameClients Get()
	{
		return g_ServerGameClients;
	}
	
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public CPlayerState GetPlayerState(const CBasePlayer pPlayer)
	{
		return SDKCall(g_pGetPlayerState, this, pPlayer.entindex);
	}
}
