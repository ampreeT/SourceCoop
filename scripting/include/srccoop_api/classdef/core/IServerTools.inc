#pragma newdecls required
#pragma semicolon 1

methodmap IServerTools
{
	public IServerTools(Address pAddress)
	{
		return view_as<IServerTools>(pAddress);
	}
	
	public Address GetTempEntsSystem()
	{
		return SDKCall(g_pGetTempEntsSystem, this);
	}
}
