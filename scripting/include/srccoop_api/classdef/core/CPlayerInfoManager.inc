#pragma newdecls required
#pragma semicolon 1

methodmap CPlayerInfoManager
{
	public CPlayerInfoManager()
	{
		return g_pPlayerInfoManager;
	}

	public CGlobalVars GetGlobalVars()
	{
		return SDKCall(g_pGetGlobalVars, view_as<Address>(this));
	}
}
