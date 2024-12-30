#pragma newdecls required
#pragma semicolon 1

methodmap CTriggerHurt < CBaseTrigger
{
	public CTriggerHurt(const int iEntIndex = -1)
	{
		return view_as<CTriggerHurt>(CBaseTrigger(iEntIndex));
	}
	public static CTriggerHurt Create()
	{
		return CTriggerHurt(CreateEntityByName("trigger_hurt"));
	}

	public int GetDamageType()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bitsDamageInflict");
	}
}
