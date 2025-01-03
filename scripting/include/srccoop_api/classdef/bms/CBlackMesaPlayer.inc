#pragma newdecls required
#pragma semicolon 1

methodmap CBlackMesaPlayer < CBasePlayer
{
	public CBlackMesaPlayer(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaPlayer>(CBasePlayer(iEntIndex));
	}
	
	public bool HasJumpModule()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bHasJumpModule") != 0;
	}
	public void SetJumpModule(const bool bShouldHaveJumpModule)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bHasJumpModule", bShouldHaveJumpModule);
	}
	public void PerformForcedWeaponDeploy(const char[] szWeaponClassname)
	{
		this.AcceptInputStr("PerformForcedWeaponDeploy", szWeaponClassname);
	}
	public float GetJumpModuleMana()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flLongJumpMana");
	}
	public float SetJumpModuleMana(const float flManaCost)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flLongJumpMana", flManaCost);
	}
}
