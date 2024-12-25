#pragma newdecls required
#pragma semicolon 1

methodmap IServerTools
{
	public IServerTools(const Address pAddress)
	{
		return view_as<IServerTools>(pAddress);
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
