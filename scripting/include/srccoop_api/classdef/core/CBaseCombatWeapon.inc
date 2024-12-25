#pragma newdecls required
#pragma semicolon 1

methodmap CBaseCombatWeapon < CBaseAnimating
{
	public CBaseCombatWeapon(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatWeapon>(CBaseAnimating(iEntIndex));
	}
	
	public CBaseCombatCharacter GetOwner()
	{
		return CBaseCombatCharacter(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hOwner"));
	}
	public int GetPrimaryAmmo()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip1");
	}
	public void SetPrimaryAmmo(const int iPrimaryAmmo)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip1", iPrimaryAmmo); 
	}
	public int GetPrimaryAmmoType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iPrimaryAmmoType");
	}
	public void SetPrimaryAmmoType(const int iPrimaryAmmoType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iPrimaryAmmoType", iPrimaryAmmoType); 
	}
	public int GetSecondaryAmmo()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip2");
	}
	public void SetSecondaryAmmo(const int iSecondaryAmmo)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip2", iSecondaryAmmo); 
	}
	public int GetSecondaryAmmoType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iSecondaryAmmoType");
	}
	public void SetSecondaryAmmoType(const int iSecondaryAmmoType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iSecondaryAmmoType", iSecondaryAmmoType); 
	}
	public float GetNextPrimaryAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextPrimaryAttack");
	}
	public void SetNextPrimaryAttack(const float flNextPrimaryAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextPrimaryAttack", flNextPrimaryAttack); 
	}
	public float GetNextSecondaryAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextSecondaryAttack");
	}
	public void SetNextSecondaryAttack(const float flNextSecondaryAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextSecondaryAttack", flNextSecondaryAttack); 
	}
	public bool CanFireUnderwater()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bFiresUnderwater") != 0;
	}
	public int GetViewModelIndex()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nViewModelIndex");
	}
	public void SetViewModelIndex(const int iViewModelIndex = 0)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nViewModelIndex", iViewModelIndex);
	}
	public int GetWeaponModelIndex(const bool bViewModel)
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, bViewModel ? "m_iViewModelIndex" : "m_iWorldModelIndex");
	}
	public void SetWeaponModelIndex(const int iViewModelIndex, const bool bViewModel)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, bViewModel ? "m_iViewModelIndex" : "m_iWorldModelIndex", iViewModelIndex);
	}
	public float GetWeaponTimeIdle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle");
	}
	public void SetWeaponTimeIdle(const float flTimeIdle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle", flTimeIdle);
	}
	public bool SendWeaponAnim(const int iActivity)
	{
		return SDKCall(g_pSendWeaponAnim, this.GetEntIndex(), iActivity);
	}
	public bool IsReloading()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bInReload") != 0;
	}
	public bool CanRespawn()
	{
		return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 30);			// SF_ITEM_NORESPAWN (1 << 30)
	}
	public void SetRespawn(const bool bRespawnEnabled)
	{
		// if set, item wont respawn in MP
		SetSpawnFlagOnEntity(this.GetEntIndex(), 30, !bRespawnEnabled);	// SF_ITEM_NORESPAWN (1 << 30)
	}
}
