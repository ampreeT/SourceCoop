methodmap Array_t
{
	public Array_t(Handle pHandle = null)
	{
		return view_as<Array_t>(pHandle);
	}
	
	public Handle GetHandle()
	{
		return view_as<Handle>(this);
	}
	public bool IsValid()
	{
		return view_as<bool>(this.GetHandle() != null);
	}
	public void PushArray(const any[] pValues, const int iSize = -1)
	{
		PushArrayArray(this.GetHandle(), pValues, iSize);
	}
	public int GetArray(const int iIndex, any[] pBuffer, const int iSize = -1)
	{
		return GetArrayArray(this.GetHandle(), iIndex, pBuffer, iSize);
	}
	public int PushCell(const any pValue)
	{
		PushArrayCell(this.GetHandle(), pValue);
	}
	public any GetCell(const int iIndex)
	{
		return GetArrayCell(this.GetHandle(), iIndex);
	}
	public int PushString(const char[] szValue)
	{
		return PushArrayString(this.GetHandle(), szValue);
	}
	public int GetString(const int iIndex, char[] szBuffer, const int iMaxLength)
	{
		return GetArrayString(this.GetHandle(), iIndex, szBuffer, iMaxLength);
	}
	public void Clear()
	{
		ClearArray(this.GetHandle());
	}
	public void Remove(const int iIndex)
	{
		RemoveFromArray(this.GetHandle(), iIndex);
	}
	public void Close()
	{
		if (this.IsValid())
		{
			CloseHandle(this.GetHandle());
		}
	}
	public int Size()
	{
		return GetArraySize(this.GetHandle());
	}
}

public int InternalGetBaseEntity(const int iEntIndex)
{
	return (iEntIndex > -1) ? EntIndexToEntRef(iEntIndex) : iEntIndex;
}

methodmap CBaseEntity
{
	public CBaseEntity(const int iEntIndex = -1)
	{
		return view_as<CBaseEntity>(InternalGetBaseEntity(iEntIndex));
	}

	public int GetEntIndex()
	{
		return EntRefToEntIndex(view_as<int>(this));
	}
	public bool IsValid()
	{
		return IsValidEntity(this.GetEntIndex());
	}
	public Address GetAddress()
	{
		return GetEntityAddress(this.GetEntIndex());
	}
	public bool GetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityClassname(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public bool GetTargetname(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szBuffer, iMaxLength));
	}
	public int SetTargetname(const char[] szTargetname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szTargetname); 
	}
	public bool IsClassPlayer()
	{
		return view_as<bool>((this.GetEntIndex() > 0) && (this.GetEntIndex() < (MaxClients + 1)));
	}
	public bool IsClassWeapon()
	{
		char szTitle[] = "weapon_";
		char szClassname[sizeof(szTitle)];
		if (this.GetClassname(szClassname, sizeof(szClassname)))
			return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
		return false;
	}
	// needs better implementation
	public bool IsClassNPC()
	{
		if (GetEntSendPropOffs(this.GetEntIndex(), "m_iMaxHealth", true) != -1)
		{
			char szTitle[] = "npc_";
			char szClassname[sizeof(szTitle)];
			if (this.GetClassname(szClassname, sizeof(szClassname)))
			{
				return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
			}
		}
		
		return false;
	}
	public bool IsClassScientist()
	{
		char szTitle[] = "npc_human_scientist";
		char szClassname[sizeof(szTitle)];
		if (this.GetClassname(szClassname, sizeof(szClassname)))
			return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
		return false;
	}
	// incorrect implementation
	/*
	public bool IsClassScene()
	{
		char szClassname[MAX_CLASSNAME];
		if (this.GetClassname(szClassname, sizeof(szClassname)))
		{
			return view_as<bool>((strcmp(szClassname, "instanced_scripted_scene", false) == 0) ||
			(strcmp(szClassname, "logic_choreographed_scene", false) == 0) ||
			(strcmp(szClassname, "scripted_scene", false) == 0));
		}
		return false;
	}
	*/
	public bool Spawn()
	{
		return DispatchSpawn(this.GetEntIndex());
	}
	public void Activate()
	{
		ActivateEntity(this.GetEntIndex());
	}
	public bool AcceptInputStr(const char[] szInput, const char[] szBuffer = "", CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		SetVariantString(szBuffer);
		return AcceptEntityInput(this.GetEntIndex(), szInput, pActivator.GetEntIndex(), pCaller.GetEntIndex(), iOutputID);
	}
	public bool AcceptInputInt(const char[] szInput, const int iValue, CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		char szBuffer[MAX_FORMAT];
		Format(szBuffer, sizeof(szBuffer), "%d", iValue);
		return this.AcceptInputStr(szInput, szBuffer, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputVector(const char[] szInput, const float vecValue[3], CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		char szBuffer[MAX_FORMAT];
		Format(szBuffer, sizeof(szBuffer), "%f %f %f", vecValue[0], vecValue[1], vecValue[2]);
		return this.AcceptInputStr(szInput, szBuffer, pActivator, pCaller, iOutputID);
	}
	public bool OutputAdd(const char[] szOutput, const char[] szTarget, const char[] szTargetInput, const char[] szParameter = "", float flDelay)
	{
		 char szBuffer[MAX_FORMAT];
		 Format(szBuffer, sizeof(szBuffer), "%s %s:%s:%s:%f:-1", szOutput, szTarget, szTargetInput, szParameter, flDelay);
		 return this.AcceptInputStr("AddOutput", szBuffer);
	}
	public bool SetKeyValueStr(const char[] szKeyValue, const char[] szValue)
	{
		return DispatchKeyValue(this.GetEntIndex(), szKeyValue, szValue);
	}
	public bool Kill()
	{
		return this.AcceptInputStr("Kill");
	}
	public int GetSpawnFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_spawnflags");
	}
	public bool HasSpawnFlag(const int iSpawnFlag)
	{
		return view_as<bool>(this.GetSpawnFlags() & iSpawnFlag);
	}
	public void SetSpawnFlags(const int iSpawnFlags)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_spawnflags", iSpawnFlags);
	}
	public void Teleport(const float vecOrigin[3] = NULL_VECTOR, const float vecAngles[3] = NULL_VECTOR, const float vecVelocity[3] = NULL_VECTOR)
	{
		TeleportEntity(this.GetEntIndex(), vecOrigin, vecAngles, vecVelocity);
	}
	public void GetAbsOrigin(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vecBuffer);
	}
	public void GetAbsAngles(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vecBuffer);
	}
	public void HookOutput(const char[] szOutput, EntityOutput pCallback, const bool bOnce)
	{
		HookSingleEntityOutput(this.GetEntIndex(), szOutput, pCallback, bOnce);
	}
	public int GetHammerID()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iHammerID");
	}
	public void GetMins(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMins", vecBuffer);
	}
	public void SetMins(const float vecMins[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMins", vecMins);
	}
	public void GetMaxs(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMaxs", vecBuffer);
	}
	public void SetMaxs(const float vecMaxs[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMaxs", vecMaxs);
	}
	public bool SetParent(CBaseEntity pEntity)
	{
		return this.AcceptInputStr("SetParent", "!activator", pEntity, this);
	}
	public bool ClearParent()
	{
		return this.AcceptInputStr("ClearParent");
	}
	public CBaseEntity GetOwner()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hOwnerEntity"));
	}
}

public int FindEntityByHammerID(int iHammerID)
{
	int startEnt = -1;
	while ((startEnt = FindEntityByClassname(startEnt, "*")) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		if (pEntity.GetHammerID == iHammerID)
		{
			return startEnt;
		}
	}
	return -1;
}

public int FindEntityByTargetname(int startEnt, const char[] targetname)
{
	while ((startEnt = FindEntityByClassname(startEnt, "*")) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		char szTargetname[MAX_CLASSNAME];
		if (pEntity.GetTargetname(szTargetname, sizeof(szTargetname)) && strcmp(szTargetname, targetname) == 0)
		{
			return startEnt;
		}
	}
	return -1;
}

public CBaseEntity CreateByClassname(const char[] szClassname)
{
	return CBaseEntity(CreateEntityByName(szClassname));
}

methodmap CTraceRay
{
	public CTraceRay(const float vecPosition[3], const float vecRayType[3], const int iFlags, const RayType rayType, const TraceEntityFilter traceFilter, const any pData)
	{
		return view_as<CTraceRay>(TR_TraceRayFilterEx(vecPosition, vecRayType, iFlags, rayType, traceFilter, pData));
	}

	public Handle GetHandle()
	{
		return view_as<Handle>(this);
	}
	public bool IsValid()
	{
		return view_as<bool>(this.GetHandle() != null);
	}
	public bool Close()
	{
		if (this.IsValid())
			CloseHandle(this.GetHandle());
	}

	public CBaseEntity GetEntity()
	{
		return CBaseEntity(TR_GetEntityIndex(this.GetHandle()));
	}
}

methodmap CBaseAnimating < CBaseEntity
{
	public CBaseAnimating(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimating>(InternalGetBaseEntity(iEntIndex));
	}
}

methodmap CBaseTrigger < CBaseEntity
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(InternalGetBaseEntity(iEntIndex));
	}
}

methodmap CChangelevel < CBaseTrigger
{
	public CChangelevel(const int iEntIndex = -1)
	{
		return view_as<CChangelevel>(InternalGetBaseEntity(iEntIndex));
	}

	public bool GetMapName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_szMapName", szBuffer, iMaxLength));
	}
}

methodmap CSceneEntity < CBaseEntity
{
	public CSceneEntity(const int iEntIndex = -1)
	{
		return view_as<CSceneEntity>(InternalGetBaseEntity(iEntIndex));
	}

	public CBaseEntity GetActor()
	{
		return CBaseEntity(GetEntProp(this.GetEntIndex(), Prop_Data, "m_hActor"));
	}
}

methodmap CBaseCombatCharacter < CBaseAnimating
{
	public CBaseCombatCharacter(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatCharacter>(InternalGetBaseEntity(iEntIndex));
	}

	public int GetHealth()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iHealth");
	}
	public void SetHealth(const int iHealth)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iHealth", iHealth);
	}
	public bool IsAlive()
	{
		return view_as<bool>(this.GetHealth() > 0);
	}
	public CBaseEntity GetActiveWeapon()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon"));
	}
}

methodmap CAI_BaseNPC < CBaseCombatCharacter
{
	public CAI_BaseNPC(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseNPC>(InternalGetBaseEntity(iEntIndex));
	}
	
	public void SetSleepState(const int iSleepState)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepState", iSleepState);
	}
	public CBaseEntity GetEnemy()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hEnemy"));
	}
	public void SetEnemy(CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hEnemy", pEntity.GetEntIndex());
	}
	public bool HasCondition(const int iCondition)
	{
		bool bOutOfBounds = view_as<bool>((iCondition > 255) || (iCondition < 0));
		if (bOutOfBounds)
			ThrowError("Received out of bounds index %d; range is 0-255", iCondition);
			
		int iOffsetConditions = 2288;		// m_bForceConditionsGather (datamap) - 0x100 = m_Conditions
		
		Address pAddress = this.GetAddress();
		int iOffsetIndex = iCondition / CHAR_BIT;
		int iBitIndex = iCondition % CHAR_BIT;
		Address pOffset = view_as<Address>(view_as<int>(pAddress) + iOffsetConditions + iOffsetIndex); 
		int iConditionList = LoadFromAddress(pOffset, NumberType_Int8);	
		return view_as<bool>(iConditionList & (1 << iBitIndex));
	}
}

methodmap CProtoSniper < CAI_BaseNPC
{
	public CProtoSniper(const int iEntIndex = -1)
	{
		return view_as<CProtoSniper>(InternalGetBaseEntity(iEntIndex));
	}
	
	public bool IsWeaponLoaded()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fWeaponLoaded"));
	}
	public bool IsEnabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fEnabled"));
	}
}

methodmap CBaseCombatWeapon < CBaseAnimating
{
	public CBaseCombatWeapon(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatWeapon>(InternalGetBaseEntity(iEntIndex));
	}
	
	public CBaseEntity GetOwner()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hOwner"));
	}
	public int GetPrimaryAmmo()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip1");
	}
	public void SetPrimaryAmmo(const int iPrimaryAmmo)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip1", iPrimaryAmmo); 
	}
}

methodmap CBasePlayer < CBaseCombatCharacter
{
	public CBasePlayer(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaPlayer>(InternalGetBaseEntity(iEntIndex));
	}
}

methodmap CBlackMesaPlayer < CBaseCombatCharacter
{
	public CBlackMesaPlayer(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaPlayer>(InternalGetBaseEntity(iEntIndex));
	}

	public bool IsValid()
	{
		return view_as<bool>(this.IsClassPlayer() && IsClientConnected(this.GetEntIndex()) && IsClientInGame(this.GetEntIndex()) && IsValidEntity(this.GetEntIndex()));
	}
	public bool GetName(char[] szBuffer, const int iMaxLength)
	{
		return GetClientName(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public int GetArmor()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_ArmorValue", 1);
	}
	public void SetArmor(const int iArmor)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_ArmorValue", iArmor, 1);
	}
	public bool HasSuit()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bWearingSuit"));
	}
	public void SetSuit(const bool bShouldHaveSuit)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bWearingSuit", bShouldHaveSuit);
	}
	public bool HasJumpModule()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule"));
	}
	public void SetJumpModule(const bool bShouldHaveJumpModule)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule", bShouldHaveJumpModule);
	}
	public bool IsAlive()
	{
		return IsPlayerAlive(this.GetEntIndex());
	}
	public CBaseCombatWeapon GetWeaponFromIndex(const int iIndex)
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hMyWeapons", iIndex));
	}
	public int GetAmmoFromIndex(const int iIndex)
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", _, iIndex);
	}
	public void SetAmmoFromIndex(const int iIndex, const int iAmmoCount)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", iAmmoCount, _, iIndex);
	}
	public CBaseCombatWeapon GiveWeapon(const char[] szWeaponName)
	{
		CBaseCombatWeapon pWeapon = view_as<CBaseCombatWeapon>(CreateByClassname(szWeaponName));
		if (pWeapon.IsValid())
		{
			pWeapon.SetSpawnFlags(SF_NORESPAWN);
			pWeapon.Spawn();
			pWeapon.Activate();
			
			float vecOrigin[3];
			this.GetAbsOrigin(vecOrigin);
			pWeapon.Teleport(vecOrigin);
			//pWeapon.Touch(this);
			
			return pWeapon;
		}
		
		return CBaseCombatWeapon();
	}
	public bool EquipWeapon(const char[] szWeaponName)	// change to CBaseEntity
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid())
			{
				char szClassname[MAX_CLASSNAME];
				if (pWeapon.GetClassname(szClassname, sizeof(szClassname)))
				{
					if (strcmp(szWeaponName, szClassname, false) == 0)
					{
						SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon", pWeapon.GetEntIndex());
						return true;
					}
				}
			}
		}
		
		return false;
	}
	//public void PickupObject(CBaseEntity pEntity, const bool bLimitMassAndSize)
	//{
	//	SetPlayerPickup(this.GetEntIndex(), pEntity.GetEntIndex(), bLimitMassAndSize);
	//}
	public int GetOldButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nOldButtons");
	}
	public int GetButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nButtons");
	}
	public bool WasPressingButton(const int iButton)
	{
		return view_as<bool>(this.GetOldButtons() & iButton);
	}
	public bool IsPressingButton(const int iButton)
	{
		return view_as<bool>(this.GetButtons() & iButton);
	}
	public void GetEyePosition(float vecBuffer[3])
	{
		GetClientEyePosition(this.GetEntIndex(), vecBuffer);
	}
	public void GetEyeAngles(float vecBuffer[3])
	{
		GetClientEyeAngles(this.GetEntIndex(), vecBuffer);
	}
	public CBaseEntity GetAimTarget()
	{
		float vecEyePos[3];
		this.GetEyePosition(vecEyePos);
		float vecEyeAngles[3];
		this.GetEyeAngles(vecEyeAngles);
		
		CTraceRay pTraceRay = CTraceRay(vecEyePos, vecEyeAngles, MASK_SHOT, RayType_Infinite, Callback_GetAimTarget, this);
		if (pTraceRay.IsValid())
		{
			CBaseEntity pEntity = pTraceRay.GetEntity();
			pTraceRay.Close();
			return pEntity;
		}
		
		return CBaseEntity();
	}
	public void SetMaxSpeed(const float flMaxSpeed)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flMaxspeed", flMaxSpeed);
	}
	public CBaseEntity GetViewEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewEntity"));
	}
	public void SetViewEntity(CBaseEntity pEntity)
	{
		bool bIsThisEntity = this.GetEntIndex() == pEntity.GetEntIndex();
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hViewEntity", bIsThisEntity ? -1 : pEntity.GetEntIndex());
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewEntity", bIsThisEntity ? -1 : pEntity.GetEntIndex());
		SetClientViewEntity(this.GetEntIndex(), pEntity.GetEntIndex());
	}
}

public bool Callback_GetAimTarget(int iEntity, int iMask, any pData)
{
	return !view_as<bool>(iEntity == pData);
}