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
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule"));
	}
	public void SetJumpModule(const bool bShouldHaveJumpModule)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule", bShouldHaveJumpModule);
	}
	public void PerformForcedWeaponDeploy(const char[] szWeaponClassname)
	{
		this.AcceptInputStr("PerformForcedWeaponDeploy", szWeaponClassname);
	}
}
