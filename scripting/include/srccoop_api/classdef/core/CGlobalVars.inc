#pragma newdecls required
#pragma semicolon 1

methodmap CGlobalVarsBase
{
	public bool IsValid()
	{
		return view_as<int>(this) != 0;
	}

	// struct is almost 1:1 https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/globalvars_base.h#L19
	public int GetMaxPlayers()
	{
		return LoadFromAddress(view_as<Address>(this) + 0x14, NumberType_Int32);
	}
	public void SetMaxPlayers(const int iMaxPlayers)
	{
		assert(this.IsValid(), "Invalid state")
		assert1(IsInBounds(iMaxPlayers, 0, MaxClients), "Invalid maxplayers [%d]", iMaxPlayers)
		
		StoreToAddress(view_as<Address>(this) + 0x14, iMaxPlayers, NumberType_Int32);
	}
}

methodmap CGlobalVars < CGlobalVarsBase
{
	
}
