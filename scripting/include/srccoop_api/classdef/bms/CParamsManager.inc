#pragma newdecls required
#pragma semicolon 1

methodmap CParamsManager
{
	public CParamsManager(const Address pAddress)
	{
		return view_as<CParamsManager>(pAddress);
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public bool IsMultiplayer()
	{
		return LoadFromAddress(this.GetAddress() + g_iOffset_CParamsManger_bIsMultiplayer, NumberType_Int32);
	}
	public void SetMultiplayer(const bool bIsMultiplayer)
	{
		StoreToAddress(this.GetAddress() + g_iOffset_CParamsManger_bIsMultiplayer, bIsMultiplayer, NumberType_Int32);
	}
}
