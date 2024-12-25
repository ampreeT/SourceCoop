#pragma newdecls required
#pragma semicolon 1

methodmap IServerGameDLL
{
	public IServerGameDLL(Address pAddress)
	{
		return view_as<IServerGameDLL>(pAddress);
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
