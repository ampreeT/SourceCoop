#pragma newdecls required
#pragma semicolon 1

methodmap CItem_HealthKit < CItem
{
	public CItem_HealthKit(const int iEntIndex = -1)
	{
		return view_as<CItem_HealthKit>(CItem(iEntIndex));
	}
	public static CItem_HealthKit Create()
	{
		return CItem_HealthKit(CreateEntityByName("item_healthkit"));
	}

    public CRagdollProp GetRagdollMimic()
    {
        return CRagdollProp(GetEntPropEnt(this.entindex, Prop_Data, "m_pRagdollMimic"));
    }
    public void SetRagdollMimic(const CRagdollProp pRagdollProp)
    {
        SetEntPropEnt(this.entindex, Prop_Data, "m_pRagdollMimic", pRagdollProp.entindex);
    }
}
