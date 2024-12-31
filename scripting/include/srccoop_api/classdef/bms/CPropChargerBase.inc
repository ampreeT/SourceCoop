#pragma newdecls required
#pragma semicolon 1

methodmap CPropChargerBase < CBaseAnimating
{
	public CPropChargerBase(const int iEntIndex = -1)
	{
		return view_as<CPropChargerBase>(CBaseAnimating(iEntIndex));
	}

	public float GetChargerRate()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flChargerRate");
	}
	public void SetChargerRate(const float flChargeRate)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flChargerRate", flChargeRate);
	}
	public float GetChargerAmount()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flChargerAmt");
	}
	public void SetChargerAmount(const float flChargeAmount)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flChargerAmt", flChargeAmount);
	}
	public float GetChargerRange()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flChargerRange");
	}
	public void SetChargerRange(const float flChargerRange)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flChargerRange", flChargerRange);
	}
}
