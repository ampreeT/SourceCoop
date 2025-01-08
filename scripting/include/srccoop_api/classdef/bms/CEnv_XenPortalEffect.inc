#pragma newdecls required
#pragma semicolon 1

// TODO: Find what class this really inherits from. 
methodmap CEnv_XenPortalEffect < CBaseEntity
{
	public CEnv_XenPortalEffect(const int iEntIndex = -1)
	{
		return view_as<CEnv_XenPortalEffect>(CBaseEntity(iEntIndex));
	}
	public static CEnv_XenPortalEffect Create()
	{
		return CEnv_XenPortalEffect(CreateEntityByName("env_xen_portal_effect"));
	}

    public float GetSize()
    {
        return GetEntPropFloat(this.entindex, Prop_Data, "m_flSize");
    }
    public void SetSize(const float flSize)
    {
        SetEntPropFloat(this.entindex, Prop_Data, "m_flSize", flSize);
    }
}
