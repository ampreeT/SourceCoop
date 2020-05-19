#if defined _srccoop_classdef_included
 #endinput
#endif
#define _srccoop_classdef_included

#include <sdktools>

methodmap Array_t < ArrayList
{
	public Array_t(ArrayList pHandle = null)
	{
		return view_as<Array_t>(pHandle);
	}
	public bool IsValid()
	{
		return this != null;
	}
	public void Close()
	{
		if (this.IsValid())
		{
			CloseHandle(this);
		}
	}
}

Handle g_pSetCollisionBounds;

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
	public bool IsValidIndex()
	{
		return this.GetEntIndex() != -1;
	}
	public Address GetAddress()
	{
		return GetEntityAddress(this.GetEntIndex());
	}
	public bool GetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityClassname(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public bool GetNetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityNetClass(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public bool GetTargetname(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szBuffer, iMaxLength));
	}
	public int SetTargetname(const char[] szTargetname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szTargetname); 
	}
	public bool GetTarget(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_target", szBuffer, iMaxLength));
	}
	public int SetTarget(const char[] szTarget)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_target", szTarget); 
	}
	public bool IsClassname(const char[] szClassname)
	{
		char szBuffer[MAX_CLASSNAME];
		this.GetClassname(szBuffer, sizeof(szBuffer));
		return StrEqual(szClassname, szBuffer);
	}
	public bool IsClassPlayer()
	{
		int index = this.GetEntIndex();
		return view_as<bool>((index > 0) && (index < (MaxClients + 1)));
	}
	public bool IsClassWeapon()
	{
		char szTitle[] = "weapon_";
		char szClassname[sizeof(szTitle)];
		if (this.GetClassname(szClassname, sizeof(szClassname)))
			return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
		return false;
	}
	public bool IsPickupItem()
	{
		char szNetClass[32];
		if(this.GetNetClassname(szNetClass, sizeof(szNetClass))) {
			//LogDebug(szNetClass);
			if(strcmp(szNetClass, "CBasePickup") == 0 || strcmp(szNetClass, "CItem_Battery") == 0)
				return true;
		}
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
				return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
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
	public bool SetKeyValueFl(const char[] szKeyValue, float fValue)
	{
		return DispatchKeyValueFloat(this.GetEntIndex(), szKeyValue, fValue);
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
	public int GetFlags()
	{
		return GetEntityFlags(this.GetEntIndex());
	}
	public bool HasFlag(const int iFlag)
	{
		return (this.GetFlags() & iFlag) == iFlag;
	}
	public void SetFlags(const int iFlags)
	{
		SetEntityFlags(this.GetEntIndex(), iFlags);
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
	public void FireOutput(const char[] output, int activator=-1, float delay=0.0)
	{
		FireEntityOutput(this.GetEntIndex(), output, activator, delay);
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
	public void SetCollisionBounds(const float vecMins[3], const float vecMaxs[3])
	{
		SDKCall(g_pSetCollisionBounds, this.GetEntIndex(), vecMins, vecMaxs);
	}
	public CBaseEntity GetOwner()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hOwnerEntity"));
	}
	public void SetOwner(CBaseEntity pOwnerEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hOwnerEntity", pOwnerEntity.GetEntIndex());
	}
	public MoveType GetMoveType()
	{
		return GetEntityMoveType(this.GetEntIndex());
	}
	public void SetMoveType(MoveType moveType)
	{
		SetEntityMoveType(this.GetEntIndex(), moveType);
	}
	public int GetCollisionGroup()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_CollisionGroup");
	}
	public void SetCollisionGroup(int iCollisionGroup)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_CollisionGroup", iCollisionGroup);
	}
	public int GetSolidType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType");
	}
	public void SetSolidType(SolidType_t iSolidType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType", iSolidType);
	}
	public int GetSolidFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_usSolidFlags");
	}
	public void SetSolidFlags(int iSolidFlags)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_usSolidFlags", iSolidFlags);
	}
	public void SetNextThinkTick(int iTick)
	{
		SetEntProp(view_as<int>(this), Prop_Data, "m_nNextThinkTick", iTick);
	}
	public void SetNextThink(float thinkTime)
	{
		int thinkTick = (thinkTime == -1)? -1 : (TIME_TO_TICKS(thinkTime));
		this.SetNextThinkTick(thinkTick);
	}
	public void GetViewOffset(float vec[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec);
	}
	public void SetViewOffset(const float vec[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec);
	}
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
	public float GetModelScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale");
	}
	public void SetModelScale(float flModelScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale", flModelScale);
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
	
	public void ChangeLevelNow()
	{
		char szMapName[MAX_MAPNAME];
		this.GetMapName(szMapName, sizeof(szMapName));
		this.FireOutput("OnChangeLevel");
		ForceChangeLevel(szMapName, SM_NORMAL_MAPCHANGE);
	}
}

methodmap CSceneEntity < CBaseEntity
{
	public CSceneEntity(const int iEntIndex = -1)
	{
		return view_as<CSceneEntity>(InternalGetBaseEntity(iEntIndex));
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
		return view_as<bool>(this.IsClassPlayer() && IsClientInGame(this.GetEntIndex()));
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
	public bool IsSprinting()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bIsSprinting"));
	}
	public void SetIsSprinting(const bool bIsSprinting)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bIsSprinting", bIsSprinting);
	}
	public bool IsSprintEnabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bSprintEnabled"));
	}
	public void SetSprintEnabled(const bool bSprintEnabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bSprintEnabled", bSprintEnabled);
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
	public CBaseAnimating GiveItem(const char[] szItemName)
	{
		return CBaseAnimating(GivePlayerItem(this.GetEntIndex(), szItemName));
	}
	public CBaseCombatWeapon GiveWeapon(const char[] szWeaponName)
	{
		return view_as<CBaseCombatWeapon>(this.GiveItem(szWeaponName));
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
	public int GetOldButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nOldButtons");
	}
	public int GetButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nButtons");
	}
	public void SetButtons(int iButtons)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nButtons", iButtons);
	}
	public int GetPressedButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_afButtonPressed");
	}
	public int GetReleasedButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_afButtonReleased");
	}
	public bool WasPressingButton(const int iButton)
	{
		return view_as<bool>(this.GetOldButtons() & iButton);
	}
	public bool IsPressingButton(const int iButton)
	{
		return view_as<bool>(this.GetButtons() & iButton);
	}
	public bool IsDucking()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bDucking"));
	}
	public bool IsDucked()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bDucked"));
	}
	public void StartDucking() // (taken from point_teleport code)
	{
		this.SetButtons(this.GetButtons() | IN_DUCK);
		this.SetFlags(this.GetFlags() | FL_DUCKING);
		int iClient = this.GetEntIndex();
		SetEntProp(iClient, Prop_Send, "m_bDucked", true);
		SetEntProp(iClient, Prop_Send, "m_bDucking", true);
		SetEntPropFloat(iClient, Prop_Send, "m_flDucktime", 0.0);
		float vecViewOffset[3] = VEC_DUCK_VIEW; ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX));
	}
	public void StopDucking()
	{
		this.SetButtons(this.GetButtons() & ~IN_DUCK);
		this.SetFlags(this.GetFlags() & ~FL_DUCKING);
		int iClient = this.GetEntIndex();
		SetEntProp(iClient, Prop_Send, "m_bDucked", false);
		SetEntProp(iClient, Prop_Send, "m_bDucking", false);
		float vecViewOffset[3] = VEC_VIEW; ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX));
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
	public float GetMaxSpeed()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flMaxspeed");
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
	public float GetDeathTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flDeathTime");
	}
	public void SetDeathTime(float flDeathTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flDeathTime", flDeathTime);
	}
	public Obs_Mode GetObserverMode()
	{
		return view_as<Obs_Mode>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_iObserverMode"));
	}
	public int GetTeam()
	{
		return GetClientTeam(this.GetEntIndex());
	}
	public void PlayGameSound(char[] szSound)
	{
		ClientCommand(this.GetEntIndex(), "playgamesound %s", szSound);
	}
	public void PlaySound(char[] szSound)
	{
		EmitSoundToClient(this.GetEntIndex(), szSound);
	}
}

public bool Callback_GetAimTarget(int iEntity, int iMask, any pData)
{
	return !view_as<bool>(iEntity == pData);
}

enum struct FireOutputData
{
	char m_szName[MAX_KEY];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
}

#define STATE_WARMUP "m_StateWarmup"
#define STATE_ROUND "m_StateRound"
#define STATE_INTERMISSION "m_StateIntermission"

enum
{
	STATE_WARMUP_INDEX = 0,
	STATE_INTERMISSION_INDEX,
	STATE_ROUND_INDEX
}

enum
{
	STATE_ELEMENT_DONE_TIME = 0,
	STATE_ELEMENT_NEXT_STATE,
	STATE_ELEMENT_IS_IN_INTERMISSION
}

methodmap CBM_MP_GameRules
{
	public static int GetStateIndex(const char[] szStateProp)
	{
		if(strcmp(szStateProp, STATE_WARMUP) == 0) return STATE_WARMUP_INDEX;
		if(strcmp(szStateProp, STATE_ROUND) == 0) return STATE_ROUND_INDEX;
		if(strcmp(szStateProp, STATE_INTERMISSION) == 0) return STATE_INTERMISSION_INDEX;
		SetFailState("CBM_MP_GameRules.GetStateIndex %s", szStateProp); return -1;
	}
	
	public static void SetStateEndTime(const char[] szStateProp, float flEndTime)
	{
		GameRules_SetPropFloat(szStateProp, flEndTime, STATE_ELEMENT_DONE_TIME);
	}
	
	public static void SetCurrentState(const char[] szStateProp)
	{
		GameRules_SetProp("m_nCurrentStateId", CBM_MP_GameRules.GetStateIndex(szStateProp));
	}
	
	public static void SetStateNextState(const char[] szStateProp, const char[] szNextStateProp)
	{
		GameRules_SetProp(szStateProp, CBM_MP_GameRules.GetStateIndex(szNextStateProp), _, STATE_ELEMENT_NEXT_STATE);
	}
	
	public static void SetStateIsInIntermission(char[] szStateProp, bool bInIntermission)
	{
		GameRules_SetProp(szStateProp, bInIntermission, _, STATE_ELEMENT_IS_IN_INTERMISSION);
	}
}

enum GLOBALESTATE
{
	GLOBAL_OFF = 0,
	GLOBAL_ON = 1,
	GLOBAL_DEAD = 2
};

Handle g_pGlobalEntityGetIndex;
Handle g_pGlobalEntityGetState;

methodmap CGlobalState
{
	public static int GetIndex(const char[] szGlobalname)
	{
		return SDKCall(g_pGlobalEntityGetIndex, szGlobalname);
	}
	
	public static GLOBALESTATE GetStateByIndex(int globalIndex)
	{
		return SDKCall(g_pGlobalEntityGetState, globalIndex);
	}
	
	public static GLOBALESTATE GetState(const char[] szGlobalname)
	{
		return CGlobalState.GetStateByIndex(CGlobalState.GetIndex(szGlobalname));
	}
}

Handle g_pGameShutdown;

methodmap IServerGameDLL
{
	public IServerGameDLL(Address pAddress)
	{
		return view_as<IServerGameDLL>(pAddress);
	}
	
	public void GameShutdown()
	{
		SDKCall(g_pGameShutdown, this); // GameShutdown() just resets global states
	}
}