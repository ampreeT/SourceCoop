#pragma newdecls required
#pragma semicolon 1

methodmap CWeapon_Satchel < CBlackMesaBaseCombatWeapon
{
	public CWeapon_Satchel(const int iEntIndex = -1)
	{
		return view_as<CWeapon_Satchel>(CBlackMesaBaseCombatWeapon(iEntIndex));
	}
	public static CWeapon_Satchel Create()
	{
		return CWeapon_Satchel(CreateEntityByName("weapon_satchel"));
	}

	public bool HasRadio()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bRadio") != 0;
	}
	public void SetRadio(const bool bRadio)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bRadio", bRadio);
	}
}
