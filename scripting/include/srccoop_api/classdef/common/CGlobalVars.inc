#pragma newdecls required
#pragma semicolon 1

static CGlobalVars gpGlobals;

methodmap CGlobalVarsBase
{
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	// struct is almost 1:1 https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/globalvars_base.h#L19
	public int GetMaxPlayers()
	{
		return LoadFromAddress(this.GetAddress() + 0x14, NumberType_Int32);
	}
	public void SetMaxPlayers(const int iMaxPlayers)
	{
		if (!IsInBounds(iMaxPlayers, 0, MaxClients))
			ThrowError("Invalid maxplayers [%d]", iMaxPlayers);
		StoreToAddress(this.GetAddress() + 0x14, iMaxPlayers, NumberType_Int32);
	}
}

methodmap CGlobalVars < CGlobalVarsBase
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		if (!(gpGlobals = CPlayerInfoManager.Get().GetGlobalVars()))
			SetFailState("Could not get `gpGlobals` from `PlayerInfoManager`");
	}
	public static CGlobalVars Get()
	{
		return gpGlobals;
	}
}
