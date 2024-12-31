#pragma newdecls required
#pragma semicolon 1

methodmap CBlackMesaBaseCombatWeaponIronsights < CBlackMesaBaseCombatWeapon
{
	public CBlackMesaBaseCombatWeaponIronsights(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaBaseCombatWeaponIronsights>(CBlackMesaBaseCombatWeapon(iEntIndex));
	}

	public bool IsInIronsights()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bIronSights") != 0;
	}
	public bool IsInIronsightsPostProcess()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bIronSightsScopePostprocess") != 0;
	}
	public bool SetIronsights(const bool bIronsights)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bIronSights", bIronsights);
	}
	public bool SetIronsightsPostProcess(const bool bIronsights)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bIronSightsScopePostprocess", bIronsights);
	}
}
