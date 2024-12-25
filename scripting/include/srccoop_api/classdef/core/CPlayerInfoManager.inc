#pragma newdecls required
#pragma semicolon 1

methodmap CPlayerInfoManager
{
	public CPlayerInfoManager()
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
