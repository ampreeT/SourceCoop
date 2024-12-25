#pragma newdecls required
#pragma semicolon 1

methodmap IServerGameClients
{
	public IServerGameClients()
	{
		return g_ServerGameClients;
	}
	
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public CPlayerState GetPlayerState(CBasePlayer pPlayer)
	{
		return SDKCall(g_pGetPlayerState, this, pPlayer.entindex);
	}
}
