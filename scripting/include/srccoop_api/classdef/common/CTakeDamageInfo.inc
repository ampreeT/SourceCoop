#pragma newdecls required
#pragma semicolon 1

methodmap CTakeDamageInfo
{
    public static CTakeDamageInfo FromAddress(const Address pAddress)
    {
        return view_as<CTakeDamageInfo>(pAddress);
    }

    public Address GetAddress()
    {
        return view_as<Address>(this);
    }
    // xref: "entindex_inflictor"
    public CBaseEntity GetInflictor()
    {
        return CBaseEntity(LoadEntityFromHandleAddress(this.GetAddress() + 0x24));       // m_hInflictor
    }
    public int GetDamageType()
    {
        return LoadFromAddress(this.GetAddress() + 60, NumberType_Int32);    // m_bitsDamageType
    }
}