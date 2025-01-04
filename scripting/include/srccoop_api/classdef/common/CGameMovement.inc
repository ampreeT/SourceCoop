#pragma newdecls required
#pragma semicolon 1

methodmap CGameMovement
{
    public static CGameMovement FromAddress(const Address pAddress)
    {
        return view_as<CGameMovement>(pAddress);
    }

    public Address GetAddress()
    {
        return view_as<Address>(this);
    }
    public CBasePlayer GetPlayer()
    {
        // easy to find in `CGameMovement::ReduceTimers`
        return view_as<CBasePlayer>(CBaseEntity.FromAddress(LoadFromAddress(this.GetAddress() + 0x4, NumberType_Int32)));
    }
}
