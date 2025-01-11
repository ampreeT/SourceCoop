#pragma newdecls required
#pragma semicolon 1

static Handle g_pQueryHearSound;
static Handle g_pGetSoundInterests;
static Handle g_pHearingSensitivity;
static Handle g_pUpdateEnemyMemory;
static Handle g_pShouldPlayerAvoid;

enum CAI_BaseNPC_SpawnFlags
{
	SF_NPC_WAIT_TILL_SEEN = (1 << 0),		// spawnflag that makes npcs wait until player can see them before attacking.
	SF_NPC_GAG = (1 << 1),					// no idle noises from this npc
	SF_NPC_FALL_TO_GROUND = (1 << 2),		// used my NPC_Maker
	SF_NPC_DROP_HEALTHKIT = (1 << 3),		// Drop a healthkit upon death
	SF_NPC_START_EFFICIENT = (1 << 4),		// Set into efficiency mode from spawn
	// ( 1 << 5  )
	// ( 1 << 6  )
	SF_NPC_WAIT_FOR_SCRIPT = (1 << 7),		// spawnflag that makes npcs wait to check for attacking until the script is done or they've been attacked
	SF_NPC_LONG_RANGE = (1 << 8),			// makes npcs look far and relaxes weapon range limit 
	SF_NPC_FADE_CORPSE = (1 << 9),			// Fade out corpse after death
	SF_NPC_ALWAYSTHINK = (1 << 10),			// Simulate even when player isn't in PVS.
	SF_NPC_TEMPLATE = (1 << 11),			// This NPC will be used as a template by an npc_maker -- do not spawn.
	SF_NPC_ALTCOLLISION = (1 << 12),
	SF_NPC_NO_WEAPON_DROP = (1 << 13),		// This NPC will not actually drop a weapon that can be picked up
	SF_NPC_NO_PLAYER_PUSHAWAY = (1 << 14),	
}

methodmap CAI_BaseNPC < CBaseCombatCharacter
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szQueryHearSound[] = "CAI_BaseNPC::QueryHearSound";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szQueryHearSound))
			LogMessage("Could not obtain gamedata offset %s", szQueryHearSound);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
			if (!(g_pQueryHearSound = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szQueryHearSound);
		}

		char szGetSoundInterests[] = "CAI_BaseNPC::GetSoundInterests";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetSoundInterests))
			LogMessage("Could not obtain gamedata offset %s", szGetSoundInterests);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetSoundInterests = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetSoundInterests);
		}

		char szHearingSensitivity[] = "CAI_BaseNPC::HearingSensitivity";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szHearingSensitivity))
			LogMessage("Could not obtain gamedata offset %s", szHearingSensitivity);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_Float, SDKPass_Plain);
			if (!(g_pHearingSensitivity = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szHearingSensitivity);
		}

		char szUpdateEnemyMemory[] = "CAI_BaseNPC::UpdateEnemyMemory";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szUpdateEnemyMemory))
			LogMessage("Could not obtain gamedata offset %s", szUpdateEnemyMemory);
		else
		{
			PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
			PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
			if (!(g_pUpdateEnemyMemory = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szUpdateEnemyMemory);
		}

		char szShouldPlayerAvoid[] = "CAI_BaseNPC::ShouldPlayerAvoid";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szShouldPlayerAvoid))
			LogMessage("Could not obtain gamedata offset %s", szShouldPlayerAvoid);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
			if (!(g_pShouldPlayerAvoid = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szShouldPlayerAvoid);
		}
	}
	public CAI_BaseNPC(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseNPC>(CBaseCombatCharacter(iEntIndex));
	}
	
	property CAI_BaseNPC_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CAI_BaseNPC_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CAI_BaseNPC_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
	public NPC_STATE GetState()
	{
		return view_as<NPC_STATE>(GetEntProp(this.entindex, Prop_Data, "m_NPCState"));
	}
	public bool IsInAScript()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bInAScript") != 0;
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
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flWakeRadius");
	}
	property AI_SleepFlags_t m_SleepFlags
	{
		public get()
		{
			return view_as<AI_SleepFlags_t>(GetEntProp(this.entindex, Prop_Data, "m_SleepFlags"));
		}
		public set(const AI_SleepFlags_t eSleepFlags)
		{
			SetEntProp(this.entindex, Prop_Data, "m_SleepFlags", eSleepFlags);
		}
	}
	public AI_SleepState_t GetSleepState()
	{
		return view_as<AI_SleepState_t>(GetEntProp(this.entindex, Prop_Data, "m_SleepState"));
	}
	public void SetSleepState(const AI_SleepState_t eSleepState)
	{
		SetEntProp(this.entindex, Prop_Data, "m_SleepState", eSleepState);
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
		return view_as<AI_Efficiency_t>(GetEntProp(this.entindex, Prop_Data, "m_Efficiency"));
	}
	public void SetEfficiency(const AI_Efficiency_t iEfficiency)
	{
		SetEntProp(this.entindex, Prop_Data, "m_Efficiency", iEfficiency);
	}
	public CBaseEntity GetEnemy()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hEnemy"));
	}
	public void SetEnemy(const CBaseEntity pEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hEnemy", pEntity.entindex);
	}
	public Sound_t GetSoundInterests()
	{
		return SDKCall(g_pGetSoundInterests, this);
	}
	public bool UpdateEnemyMemory(const CBaseEntity pEnemy, const float vec3Position[3], const CBaseEntity pInformer)
	{
		return SDKCall(g_pUpdateEnemyMemory, this, pEnemy, vec3Position, pInformer);
	}
	public void SetTargetEnt(const CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt", pTarget.entindex);
	}
	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt"));
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
		
		int iForceConditionsGatherOffset = FindDataMapInfo(this.entindex, "m_bForceConditionsGather");		// m_bForceConditionsGather (datamap) - 128 = m_Conditions
		if (iForceConditionsGatherOffset == -1)
			ThrowError("Unable to retrieve offset %s", "m_bForceConditionsGather");
		int iOffsetConditions = iForceConditionsGatherOffset - 128;
		
		int iOffsetIndex = iCondition / CHAR_BIT;
		int iBitIndex = iCondition % CHAR_BIT;
		Address pOffset = this.address + iOffsetConditions + iOffsetIndex;
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
		
		int iForceConditionsGatherOffset = FindDataMapInfo(this.entindex, "m_bForceConditionsGather");		// m_bForceConditionsGather (datamap) - 128 = m_Conditions
		if (iForceConditionsGatherOffset == -1)
			ThrowError("Unable to retrieve offset %s", "m_bForceConditionsGather");
		int iOffsetConditions = iForceConditionsGatherOffset - 128;
		
		int iOffsetIndex = iCondition / CHAR_BIT;
		int iBitIndex = iCondition % CHAR_BIT;
		Address pOffset = this.address + iOffsetConditions + iOffsetIndex;
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
		return GetEntProp(this.entindex, Prop_Data, "iTaskInterrupt");
	}
	public GoalType_t GetGoalType()
	{
		return view_as<GoalType_t>(GetEntProp(this.entindex, Prop_Data, "m_goalType"));
	}
	public bool IsMoving()
	{
		// CAI_BaseNPC::IsMoving( void ) ==> GetNavigator()->IsGoalSet() ==> GetPath()->GoalType() != GOALTYPE_NONE
		return this.GetGoalType() != GOALTYPE_NONE;
	}
	public bool ShouldPlayerAvoid()
	{
		return SDKCall(g_pShouldPlayerAvoid, this);
	}
	public bool GetPerformAvoidance()
	{
		return !!GetEntProp(this.entindex, Prop_Data, "m_bPerformAvoidance");
	}
	public void SetPerformAvoidance(const bool bPerformAvoidance)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bPerformAvoidance", bPerformAvoidance);
	}
	public bool GetPlayerAvoidState()
	{
		return !!GetEntProp(this.entindex, Prop_Data, "m_bPlayerAvoidState");
	}
	public void SetPlayerAvoidState(const bool bPlayerAvoidState)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bPlayerAvoidState", bPlayerAvoidState);
	}
	public CAI_ScriptedSequence GetCine()
	{
		return CAI_ScriptedSequence(GetEntPropEnt(this.entindex, Prop_Data, "m_hCine"));
	}
	public void SetCine(const CAI_ScriptedSequence pCine)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hCine", pCine.entindex);
	}
	public void ForgetEntity(const CBaseEntity pEntity)
	{
		static const char g_szPlaceholderName[] = "__srccoop_forget_entity";

		char szTargetname[MAX_CLASSNAME];
		pEntity.GetTargetname(szTargetname, sizeof(szTargetname));
		
		// This input will iterate through the entity list.
		// If this is too expensive, call `CAI_Enemies::ClearMemory` directly instead.
		pEntity.SetTargetname(g_szPlaceholderName);
		this.AcceptInputStr("ForgetEntity", g_szPlaceholderName);
		pEntity.SetTargetname(szTargetname);
	}
}
