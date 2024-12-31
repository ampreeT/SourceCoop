#pragma newdecls required
#pragma semicolon 1

methodmap CWeapon_Crossbow < CBlackMesaBaseCombatWeaponIronsights
{
	public CWeapon_Crossbow(const int iEntIndex = -1)
	{
		return view_as<CWeapon_Crossbow>(CBlackMesaBaseCombatWeaponIronsights(iEntIndex));
	}
	public static CWeapon_Crossbow Create()
	{
		return CWeapon_Crossbow(CreateEntityByName("weapon_crossbow"));
	}

	public bool IsRearming()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bNeedsToRearm") != 0;
	}
	public void SetRearming(const bool bRearming)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bNeedsToRearm", bRearming);
	}
}
