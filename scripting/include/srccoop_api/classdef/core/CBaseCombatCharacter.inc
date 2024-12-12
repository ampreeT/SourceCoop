#pragma newdecls required
#pragma semicolon 1

methodmap CBaseCombatCharacter < CBaseFlex
{
	public CBaseCombatCharacter(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatCharacter>(CBaseFlex(iEntIndex));
	}

	public int GetLifeState()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_lifeState");
	}
	public bool IsAlive()
	{
		// Any value above 0 indicates dead or other death information
		return !this.GetLifeState();
	}
	public CBaseCombatWeapon GetActiveWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon"));
	}
	public void SetActiveWeapon(CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon", pWeapon.GetEntIndex());
	}
	public bool HasWeapon(const char[] szWeaponName)
	{
		return this.GetWeapon(szWeaponName) != NULL_CBASEENTITY;
	}
	public CBaseCombatWeapon GetWeapon(const char[] szWeaponName, const int iStartIndex = 0)
	{
		for (int i = iStartIndex; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid() && pWeapon.IsClassname(szWeaponName))
			{
				return pWeapon;
			}
		}
		return NULL_CBASEENTITY;
	}
	public bool WeaponSwitch(CBaseCombatWeapon pWeapon, const int iViewmodelindex = 0)
	{
		return SDKCall(g_pWeaponSwitch, this.GetEntIndex(), pWeapon.GetEntIndex(), iViewmodelindex);
	}
	// Deprecated, WeaponSwitch() prefered instead
	public bool SetActiveWeaponByClass(const char[] szWeaponName)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid() && pWeapon.IsClassname(szWeaponName))
			{
				this.SetActiveWeapon(pWeapon);
				return true;
			}
		}
		return false;
	}
	public CBaseCombatWeapon GetLastWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLastWeapon"));
	}
	public void SetLastWeapon(CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLastWeapon", pWeapon.GetEntIndex());
	}
	public CBaseCombatWeapon GetWeaponFromIndex(const int iIndex)
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hMyWeapons", iIndex));
	}
	public void SetWeaponAtIndex(const int iIndex, CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hMyWeapons", pWeapon.GetEntIndex(), iIndex);
	}
	public int GetAmmoFromIndex(const int iIndex)
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", _, iIndex);
	}
	public void SetAmmoFromIndex(const int iIndex, const int iAmmoCount)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", iAmmoCount, _, iIndex);
	}
	public float GetNextAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flNextAttack");
	}
	public void SetNextAttack(const float flNextAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flNextAttack", flNextAttack);
	}	
}
