#pragma newdecls required
#pragma semicolon 1

methodmap CWeapon_Gluon < CBlackMesaBaseCombatWeapon
{
	public CWeapon_Gluon(const int iEntIndex = -1)
	{
		return view_as<CWeapon_Gluon>(CBlackMesaBaseCombatWeapon(iEntIndex));
	}
	public static CWeapon_Gluon Create()
	{
		return CWeapon_Gluon(CreateEntityByName("weapon_gluon"));
	}

	public int GetFiringState()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iFiringState");
	}
	public void SetFiringState(const int iFiringState)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iFiringState", iFiringState);
	}
	public float GetStartFireTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flStartFireTime");
	}
	public void SetStartFireTime(const float flStartFireTime)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flStartFireTime", flStartFireTime);
	}
}
