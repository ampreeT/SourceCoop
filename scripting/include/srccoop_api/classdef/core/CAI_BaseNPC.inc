#pragma newdecls required
#pragma semicolon 1

methodmap CAI_BaseNPC < CBaseCombatCharacter
{
	public CAI_BaseNPC(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseNPC>(CBaseCombatCharacter(iEntIndex));
	}
	
	public NPC_STATE GetState()
	{
		return view_as<NPC_STATE>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_NPCState"));
	}
	public bool IsInAScript()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bInAScript") != 0;
	}
	public bool IsSoundVisible(CSound pSound)
	{
		float vec3SoundReactOrigin[3];
		pSound.GetSoundReactOrigin(vec3SoundReactOrigin);
		CBaseEntity pBlocker = CBaseEntity();

		return this.FVisiblePosition(vec3SoundReactOrigin, _, pBlocker) || (pBlocker != NULL_CBASEENTITY && pBlocker == pSound.GetOwner());
	}
	public float GetHearingSensitivity()
	{
		return SDKCall(g_pHearingSensitivity, this);
	}
	public bool QueryHearSound(CSound pSound)
	{
		return SDKCall(g_pQueryHearSound, this, pSound.GetAddress());
	}
	public bool CanHearSound(CSound pSound)
	{
		if (this == pSound.GetOwner())
			return false;
		
		if (this.GetState() == NPC_STATE_SCRIPT && (pSound.GetType() & SOUND_DANGER))
			return false;
		
		if (this.IsInAScript())
			return false;
		
		float vec3SoundOrigin[3];
		pSound.GetOrigin(vec3SoundOrigin);
		float vec3EarOrigin[3];
		this.GetEarPosition(vec3EarOrigin);
		float flDistance = GetVectorDistance(vec3SoundOrigin, vec3EarOrigin);
		float flHearDistance = this.GetHearingSensitivity() * pSound.GetVolume();

		return (flDistance <= flHearDistance) && this.QueryHearSound(pSound);
	}
	public float GetWakeRadius()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flWakeRadius");
	}
	public int GetSleepFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepFlags");
	}
	public int SetSleepFlags(const int iSleepFlags)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepFlags", iSleepFlags);
	}
	public bool HasSleepFlags(const int iSleepFlags)
	{
		return (this.GetSleepFlags() & iSleepFlags) == iSleepFlags;
	}
	public void AddSleepFlags(const int iSleepFlags)
	{
		this.SetSleepFlags(this.GetSleepFlags() | iSleepFlags);
	}
	public void RemoveSleepFlags(const int iSleepFlags)
	{
		this.SetSleepFlags(this.GetSleepFlags() & ~iSleepFlags);
	}
	public AI_SleepState_t GetSleepState()
	{
		return view_as<AI_SleepState_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepState"));
	}
	public void SetSleepState(const AI_SleepState_t eSleepState)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepState", eSleepState);
	}
	public void Wake()
	{
		this.AcceptInputStr("Wake");
	}
	public void Sleep()
	{
		// TODO:
		// Reconstruct function from scratch.
		// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc.cpp#L4440
	}
	public AI_Efficiency_t GetEfficiency()
	{
		return view_as<AI_Efficiency_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_Efficiency"));
	}
	public void SetEfficiency(const AI_Efficiency_t iEfficiency)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_Efficiency", iEfficiency);
	}
	public CBaseEntity GetEnemy()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hEnemy"));
	}
	public void SetEnemy(const CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hEnemy", pEntity.GetEntIndex());
	}
	public int GetSoundInterests()
	{
		return SDKCall(g_pGetSoundInterests, this);
	}
	public bool UpdateEnemyMemory(const CBaseEntity pEnemy, const float vec3Position[3], const CBaseEntity pInformer)
	{
		return SDKCall(g_pUpdateEnemyMemory, this, pEnemy, vec3Position, pInformer);
	}
	public void SetTargetEnt(const CBaseEntity pTarget)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hTargetEnt", pTarget.GetEntIndex());
	}
	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hTargetEnt"));
	}
	public bool HasCondition(const int iCondition)
	{
		bool bOutOfBounds = (iCondition > 255) || (iCondition < 0);
		if (bOutOfBounds)
			ThrowError("Received out of bounds index %d; range is 0-255", iCondition);
		
		// m_Conditions is not a netprop/datamap
		// sizeof(CAI_ScheduleBits) == 32 bytes
		//	CAI_ScheduleBits	m_Conditions;
		//	CAI_ScheduleBits	m_CustomInterruptConditions;
		//	CAI_ScheduleBits	m_ConditionsPreIgnore;
		//	CAI_ScheduleBits	m_InverseIgnoreConditions;
		//	bool				m_bForceConditionsGather;
		// m_Conditions = offsetof(this, m_bForceConditionsGather) - (sizeof(CAI_ScheduleBits) * 4)
		
		int iForceConditionsGatherOffset = FindDataMapInfo(this.GetEntIndex(), "m_bForceConditionsGather");		// m_bForceConditionsGather (datamap) - 128 = m_Conditions
		if (iForceConditionsGatherOffset == -1)
			ThrowError("Unable to retrieve offset %s", "m_bForceConditionsGather");
		int iOffsetConditions = iForceConditionsGatherOffset - 128;
		
		Address pAddress = this.GetAddress();
		int iOffsetIndex = iCondition / CHAR_BIT;
		int iBitIndex = iCondition % CHAR_BIT;
		Address pOffset = view_as<Address>(view_as<int>(pAddress) + iOffsetConditions + iOffsetIndex);
		int iConditionList = LoadFromAddress(pOffset, NumberType_Int8);
		return view_as<bool>(iConditionList & (1 << iBitIndex));
	}
	public void SetCondition(const int iCondition, const bool bSetCondition)
	{
		bool bOutOfBounds = (iCondition > 255) || (iCondition < 0);
		if (bOutOfBounds)
			ThrowError("Received out of bounds index %d; range is 0-255", iCondition);
		
		// m_Conditions is not a netprop/datamap
		// sizeof(CAI_ScheduleBits) == 32 bytes
		//	CAI_ScheduleBits	m_Conditions;
		//	CAI_ScheduleBits	m_CustomInterruptConditions;
		//	CAI_ScheduleBits	m_ConditionsPreIgnore;
		//	CAI_ScheduleBits	m_InverseIgnoreConditions;
		//	bool				m_bForceConditionsGather;
		// m_Conditions = offsetof(this, m_bForceConditionsGather) - (sizeof(CAI_ScheduleBits) * 4)
		
		int iForceConditionsGatherOffset = FindDataMapInfo(this.GetEntIndex(), "m_bForceConditionsGather");		// m_bForceConditionsGather (datamap) - 128 = m_Conditions
		if (iForceConditionsGatherOffset == -1)
			ThrowError("Unable to retrieve offset %s", "m_bForceConditionsGather");
		int iOffsetConditions = iForceConditionsGatherOffset - 128;
		
		Address pAddress = this.GetAddress();
		int iOffsetIndex = iCondition / CHAR_BIT;
		int iBitIndex = iCondition % CHAR_BIT;
		Address pOffset = view_as<Address>(view_as<int>(pAddress) + iOffsetConditions + iOffsetIndex);
		int iConditionList = LoadFromAddress(pOffset, NumberType_Int8);

		if (bSetCondition)
		{
			iConditionList |= (1 << iBitIndex); 
		}
		else
		{
			iConditionList &= ~(1 << iBitIndex); 
		}

		StoreToAddress(pOffset, iConditionList, NumberType_Int8);
	}
	public int GetTaskInterrupt()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "iTaskInterrupt");
	}
	public bool IsMoving()
	{
		// CAI_BaseNPC::IsMoving( void ) ==> GetNavigator()->IsGoalSet() ==> GetPath()->GoalType() != GOALTYPE_NONE
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_goalType") != GOALTYPE_NONE;
	}
	public bool ShouldPlayerAvoid()
	{
		return SDKCall(g_pShouldPlayerAvoid, this);
	}
	public bool GetPerformAvoidance()
	{
		return !!GetEntProp(this.GetEntIndex(), Prop_Data, "m_bPerformAvoidance");
	}
	public void SetPerformAvoidance(const bool bPerformAvoidance)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bPerformAvoidance", bPerformAvoidance);
	}
	public bool GetPlayerAvoidState()
	{
		return !!GetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlayerAvoidState");
	}
	public void SetPlayerAvoidState(const bool bPlayerAvoidState)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlayerAvoidState", bPlayerAvoidState);
	}
	public CAI_ScriptedSequence GetCine()
	{
		return CAI_ScriptedSequence(GetEntPropEnt(this.entindex, Prop_Data, "m_hCine"));
	}
	public void SetCine(const CAI_ScriptedSequence pCine)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hCine", pCine.entindex);
	}
	public bool WillWaitTillSeen()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);		// SF_NPC_WAIT_TILL_SEEN (1 << 0)
	}
	public void SetWaitTillSeen(const bool bWaitTillSeen)
	{
		// spawnflag that makes npcs wait until player can see them before attacking.
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bWaitTillSeen);	// SF_NPC_WAIT_TILL_SEEN (1 << 0)
	}
	public bool CanGag()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 1);	// SF_NPC_GAG (1 << 1)
	}
	public void SetGag(const bool bGag)
	{
		// no idle noises from this npc
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 1, bGag);		// SF_NPC_GAG (1 << 1)
	}
	public bool CanFallToGround()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 2);		// SF_NPC_FALL_TO_GROUND (1 << 2)
	}
	public void SetCanFallToGround(const bool bFallToGround)
	{
		// used my NPC_Maker
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 2, bFallToGround);	// SF_NPC_FALL_TO_GROUND (1 << 2)
	}
	public bool CanDropHealthkit()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 3);			// SF_NPC_DROP_HEALTHKIT (1 << 3)
	}
	public void SetCanDropHealthkit(const bool bDropHealthKit)
	{
		// Drop a healthkit upon death
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 3, bDropHealthKit);	// SF_NPC_DROP_HEALTHKIT (1 << 3)
	}
	public bool WillStartEfficient()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 4);			// SF_NPC_START_EFFICIENT (1 << 4)
	}
	public void SetStartEfficient(const bool bStartEfficient)
	{
		// Set into efficiency mode from spawn
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 4, bStartEfficient);	// SF_NPC_START_EFFICIENT (1 << 4)
	}
	public bool WillWaitForScript()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 7);			// SF_NPC_WAIT_FOR_SCRIPT (1 << 7)
	}
	public void SetWillWaitForScript(const bool bWaitForScript)
	{
		// spawnflag that makes npcs wait to check for attacking until the script is done or they've been attacked
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 7, bWaitForScript);	// SF_NPC_WAIT_FOR_SCRIPT (1 << 7)
	}
	public bool IsLongRange()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 8);		// SF_NPC_LONG_RANGE (1 << 8)
	}
	public void SetLongRange(const bool bLongRange)
	{
		// makes npcs look far and relaxes weapon range limit 
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 8, bLongRange);	// SF_NPC_LONG_RANGE (1 << 8)
	}
	public bool CanFadeCorpse()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 9);		// SF_NPC_FADE_CORPSE (1 << 9)
	}
	public void SetFadeCorpse(const bool bFadeCorpse)
	{
		// Fade out corpse after death
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 9, bFadeCorpse);	// SF_NPC_FADE_CORPSE (1 << 9)
	}
	public bool WillAlwaysThink()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 10);		// SF_NPC_ALWAYSTHINK (1 << 10)
	}
	public void SetAlwaysThink(const bool bAlwaysThink)
	{
		// Simulate even when player isn't in PVS.
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 10, bAlwaysThink);	// SF_NPC_ALWAYSTHINK (1 << 10)
	}
	public bool IsTemplated()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 11);		// SF_NPC_TEMPLATE (1 << 11)
	}
	public void SetTemplated(const bool bTemplated)
	{
		// This NPC will be used as a template by an npc_maker -- do not spawn.
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 11, bTemplated);	// SF_NPC_TEMPLATE (1 << 11)
	}
	public bool HasAlternateCollision()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 12);				// SF_NPC_ALTCOLLISION (1 << 12)
	}
	public void SetAlternateCollision(const bool bAlternateCollision)
	{
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 12, bAlternateCollision);	// SF_NPC_ALTCOLLISION (1 << 12)
	}
	public bool CanDropWeapon()
	{
	    return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 13);		// SF_NPC_NO_WEAPON_DROP (1 << 13)
	}
	public void SetWeaponDrop(const bool bWeaponDrop)
	{
		// This NPC will not actually drop a weapon that can be picked up
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 13, !bWeaponDrop);	// SF_NPC_NO_WEAPON_DROP (1 << 13)
	}
	public bool CanPushPlayer()
	{
	    return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 14);		// SF_NPC_NO_PLAYER_PUSHAWAY (1 << 14)
	}
	public void SetPushPlayer(const bool bPushPlayer)
	{
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 14, !bPushPlayer);	// SF_NPC_NO_PLAYER_PUSHAWAY (1 << 14)
	}
}
