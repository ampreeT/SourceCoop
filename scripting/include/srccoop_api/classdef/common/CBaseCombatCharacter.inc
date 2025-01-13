#pragma newdecls required
#pragma semicolon 1

static Handle g_pWeaponSwitch;

methodmap CBaseCombatCharacter < CBaseFlex
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szWeaponSwitch[] = "CBaseCombatCharacter::Weapon_Switch";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szWeaponSwitch))
			LogMessage("Could not obtain gamedata offset %s", szWeaponSwitch);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pWeaponSwitch = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szWeaponSwitch);
		}
	}
	public CBaseCombatCharacter(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatCharacter>(CBaseFlex(iEntIndex));
	}

	public LifeState_t GetLifeState()
	{
		return view_as<LifeState_t>(GetEntProp(this.entindex, Prop_Data, "m_lifeState"));
	}
	public bool IsAlive()
	{
		return this.GetLifeState() == LIFE_ALIVE;
	}
	public CBaseCombatWeapon GetActiveWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.entindex, Prop_Send, "m_hActiveWeapon"));
	}
	public void SetActiveWeapon(const CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.entindex, Prop_Send, "m_hActiveWeapon", pWeapon.entindex);
	}
	public bool HasWeapon(const char[] szWeaponName)
	{
		return this.GetWeapon(szWeaponName) != NULL_CBASEENTITY;
	}
	public CBaseCombatWeapon GetWeapon(const char[] szWeaponName, const int iStartIndex = 0)
	{
		for (int i = iStartIndex; i < MAX_WEAPONS; ++i)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon != NULL_CBASEENTITY && pWeapon.IsClassname(szWeaponName))
			{
				return pWeapon;
			}
		}
		return NULL_CBASEENTITY;
	}
	public bool WeaponSwitch(const CBaseCombatWeapon pWeapon, const int iViewmodelindex = 0)
	{
		return SDKCall(g_pWeaponSwitch, this.entindex, pWeapon.entindex, iViewmodelindex);
	}
	// Deprecated, WeaponSwitch() prefered instead
	public bool SetActiveWeaponByClass(const char[] szWeaponName)
	{
		for (int i = 0; i < MAX_WEAPONS; ++i)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon != NULL_CBASEENTITY && pWeapon.IsClassname(szWeaponName))
			{
				this.SetActiveWeapon(pWeapon);
				return true;
			}
		}
		return false;
	}
	public CBaseCombatWeapon GetLastWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.entindex, Prop_Data, "m_hLastWeapon"));
	}
	public void SetLastWeapon(const CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hLastWeapon", pWeapon.entindex);
	}
	public CBaseCombatWeapon GetWeaponFromIndex(const int iIndex)
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.entindex, Prop_Send, "m_hMyWeapons", iIndex));
	}
	public void SetWeaponAtIndex(const int iIndex, CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.entindex, Prop_Send, "m_hMyWeapons", pWeapon.entindex, iIndex);
	}
	public int GetAmmoFromIndex(const AmmoType_t eAmmoType)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iAmmo", _, view_as<int>(eAmmoType));
	}
	public void SetAmmoFromIndex(const AmmoType_t eAmmoType, const int iAmmoCount)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iAmmo", iAmmoCount, _, view_as<int>(eAmmoType));
	}
	public float GetNextAttack()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flNextAttack");
	}
	public void SetNextAttack(const float flNextAttack)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flNextAttack", flNextAttack);
	}
	public CBasePlayer ToPlayer()
	{
		bool bIsPlayer = this != NULL_CBASEENTITY && this.IsPlayer();
		return bIsPlayer ? view_as<CBasePlayer>(this) : NULL_CBASEENTITY;
	}
	public CAI_BaseNPC ToNPC()
	{
		bool bIsNPC = this != NULL_CBASEENTITY && this.IsNPC();
		return bIsNPC ? view_as<CAI_BaseNPC>(this) : NULL_CBASEENTITY;
	}
}
