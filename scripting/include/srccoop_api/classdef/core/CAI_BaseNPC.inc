#pragma newdecls required
#pragma semicolon 1

methodmap CAI_BaseNPC < CBaseCombatCharacter
{
	public CAI_BaseNPC(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseNPC>(CBaseCombatCharacter(iEntIndex));
	}
	
	public int GetState()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_NPCState");
	}
	public bool IsInAScript()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bInAScript"));
	}
	public bool IsSoundVisible(CSound pSound)
	{
		float vec3SoundReactOrigin[3];
		pSound.GetSoundReactOrigin(vec3SoundReactOrigin);
		CBaseEntity pBlocker = CBaseEntity();

		return this.FVisiblePosition(vec3SoundReactOrigin, _, pBlocker) || (pBlocker.IsValid() && pBlocker == pSound.GetOwner());
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
		
		if (this.GetState() == view_as<int>(NPC_STATE_SCRIPT) && (pSound.GetType() & SOUND_DANGER))
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
	public int GetSleepState()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepState");
	}
	public void SetSleepState(const int iSleepState)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_SleepState", iSleepState);
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
	public void SetEnemy(CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hEnemy", pEntity.GetEntIndex());
	}
	public int GetSoundInterests()
	{
		return SDKCall(g_pGetSoundInterests, this);
	}
	public bool UpdateEnemyMemory(CBaseEntity pEnemy, const float vecPosition[3], CBaseEntity pInformer)
	{
		return SDKCall(g_pUpdateEnemyMemory, this, pEnemy, vecPosition, pInformer);
	}
	public void SetTargetEnt(CBaseEntity pTarget)
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
	public void SetCine(CAI_ScriptedSequence pCine)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hCine", pCine.entindex);
	}
}
