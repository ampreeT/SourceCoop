#pragma newdecls required
#pragma semicolon 1

methodmap CPredictedViewModel < CBaseViewModel
{
	public CPredictedViewModel(const int iEntIndex = -1)
	{
		return view_as<CPredictedViewModel>(CBaseViewModel(iEntIndex));
	}
	
	public float GetWeaponTimeIdle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle");
	}
	public void SetWeaponTimeIdle(const float flTimeIdle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle", flTimeIdle);
	}
	public CBaseCombatWeapon GetOwningWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hWeapon"));
	}
}
