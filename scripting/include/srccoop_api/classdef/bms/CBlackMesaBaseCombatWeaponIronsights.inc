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
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bIronSights"));
	}
	public bool IsInIronsightsPostProcess()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bIronSightsScopePostprocess"));
	}
	public bool SetIronsights(const bool bIronsights)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bIronSights", bIronsights);
	}
	public bool SetIronsightsPostProcess(const bool bIronsights)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bIronSightsScopePostprocess", bIronsights);
	}
}
