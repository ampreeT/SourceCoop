#pragma newdecls required
#pragma semicolon 1

static ConVar ai_los_mode;

static Handle g_pSetNextThink;
static Handle g_pSetNextThinkNullString;
static Handle g_pWorldSpaceCenter;
static Handle g_pIsNPC;
static Handle g_pSetMinMaxSize;

#define NULL_CBASEENTITY view_as<any>(-1)

methodmap CBaseEntity
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		ai_los_mode = FindConVar("ai_los_mode");
		
		char szSetNextThink[] = "CBaseEntity::SetNextThink";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szSetNextThink))
			LogMessage("Could not obtain gamedata signature %s", szSetNextThink);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
			if (!(g_pSetNextThink = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetNextThink);
		}
		
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szSetNextThink))
			LogMessage("Could not obtain gamedata signature %s", szSetNextThink);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pSetNextThinkNullString = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetNextThink);
		}
		
		char szWorldSpaceCenter[] = "CBaseEntity::WorldSpaceCenter";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szWorldSpaceCenter))
			LogMessage("Could not obtain gamedata offset %s", szWorldSpaceCenter);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_Vector, SDKPass_ByRef);
			if (!(g_pWorldSpaceCenter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szWorldSpaceCenter);
		}
		
		char szIsNpc[] = "CBaseEntity::IsNPC";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szIsNpc))
			LogMessage("Could not obtain gamedata offset %s", szIsNpc);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
			if (!(g_pIsNPC = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szIsNpc);
		}
		
		char szSetMinMaxSize[] = "SetMinMaxSize";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szSetMinMaxSize))
			LogMessage("Could not obtain gamedata signature %s", szSetMinMaxSize);
		else
		{
			PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
			if (!(g_pSetMinMaxSize = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetMinMaxSize);
		}
	}
	public CBaseEntity(const int iEntIndex = -1)
	{
		return view_as<CBaseEntity>(iEntIndex > -1 ? EntIndexToEntRef(iEntIndex) : iEntIndex);
	}
	public static CBaseEntity FromAddress(const Address pAddress)
	{
		return CBaseEntity(GetEntityFromAddress(pAddress));
	}
	public static CBaseEntity Create(const char[] szClassname)
	{
		return CBaseEntity(CreateEntityByName(szClassname));
	}

	property int entindex
	{
		public get()
		{
			return EntRefToEntIndex(view_as<int>(this));
		}
	}
	property int m_spawnflags
	{
		public get()
		{
			return GetEntProp(this.GetEntIndex(), Prop_Data, "m_spawnflags");
		}
		public set(const int iSpawnFlags)
		{
			SetEntProp(this.GetEntIndex(), Prop_Data, "m_spawnflags", iSpawnFlags);
		}
	}
	/** Deprecated: Use entindex property instead */
	public int GetEntIndex()
	{
		return EntRefToEntIndex(view_as<int>(this));
	}
	public bool IsValid()
	{
		return (this != NULL_CBASEENTITY) && IsValidEntity(view_as<int>(this));
	}
	public bool HasEdict()
	{
		return IsValidEdict(this.GetEntIndex());
	}
	public Address GetAddress()
	{
		return GetEntityAddress(this.GetEntIndex());
	}
	public bool GetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityClassname(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public int SetClassname(const char[] szClassname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iClassname", szClassname); 
	}
	public bool GetNetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityNetClass(this.GetEntIndex(), szBuffer, iMaxLength);
	}
	public bool GetTargetname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szBuffer, iMaxLength) != 0;
	}
	public int SetTargetname(const char[] szTargetname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iName", szTargetname); 
	}
	public bool GetTarget(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_target", szBuffer, iMaxLength) != 0;
	}
	public int SetTarget(const char[] szTarget)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_target", szTarget); 
	}
	public bool IsClassname(const char[] szClassname)
	{
		static char szBuffer[MAX_CLASSNAME];
		this.GetClassname(szBuffer, sizeof(szBuffer));
		return StrEqual(szClassname, szBuffer, false);
	}
	public bool Spawn()
	{
		return DispatchSpawn(this.GetEntIndex());
	}
	public void Activate()
	{
		ActivateEntity(this.GetEntIndex());
	}
	public bool AcceptInput(const char[] szInput, const CBaseEntity pActivator = NULL_CBASEENTITY, const CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		return AcceptEntityInput(this.GetEntIndex(), szInput, pActivator.GetEntIndex(), pCaller.GetEntIndex(), iOutputID);
	}
	public bool AcceptInputStr(const char[] szInput, const char[] szParameter = "", const CBaseEntity pActivator = NULL_CBASEENTITY, const CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		SetVariantString(szParameter);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputInt(const char[] szInput, const int iValue, const CBaseEntity pActivator = NULL_CBASEENTITY, const CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		SetVariantInt(iValue);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputVector(const char[] szInput, const float vec3Value[3], const CBaseEntity pActivator = NULL_CBASEENTITY, const CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		// ToDo: Should this use SetVariantPosVector3D or SetVariantVector3D?
		char szBuffer[256];
		Format(szBuffer, sizeof(szBuffer), "%f %f %f", vec3Value[0], vec3Value[1], vec3Value[2]);
		return this.AcceptInputStr(szInput, szBuffer, pActivator, pCaller, iOutputID);
	}
	public bool OutputAdd(const char[] szOutput, const char[] szTarget, const char[] szTargetInput, const char[] szParameter = "", const float flDelay = 0.0, const int iTimesToFire = -1)
	{
		 char szBuffer[256];
		 Format(szBuffer, sizeof(szBuffer), "%s %s:%s:%s:%f:%d", szOutput, szTarget, szTargetInput, szParameter, flDelay, iTimesToFire);
		 return this.AcceptInputStr("AddOutput", szBuffer);
	}
	public bool SetKeyValueStr(const char[] szKeyValue, const char[] szValue)
	{
		return DispatchKeyValue(this.GetEntIndex(), szKeyValue, szValue);
	}
	public bool SetKeyValueFloat(const char[] szKeyValue, const float flValue)
	{
		return DispatchKeyValueFloat(this.GetEntIndex(), szKeyValue, flValue);
	}
	public bool Kill()
	{
		return this.AcceptInputStr("Kill");
	}
	public bool KillAfterTime(const float flTime)
	{
		return this.OutputAdd("OnUser1", "!self", "kill", _, flTime) && this.AcceptInput("FireUser1");
	}
	public int GetTeam()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iTeamNum");
	}
	public void GetEyePosition(float vec3Buffer[3])
	{
		if (this.IsPlayer())
		{
			GetClientEyePosition(this.GetEntIndex(), vec3Buffer);
		}
		else
		{
			this.GetAbsOrigin(vec3Buffer);

			float vec3Offset[3];
			GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec3Offset);

			AddVectors(vec3Buffer, vec3Offset, vec3Buffer);
		}
	}
	// # Citations
	//
	// - [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/shared/baseentity_shared.cpp#L148)
	//
	public void GetEarPosition(float vec3Buffer[3])
	{
		this.GetEyePosition(vec3Buffer);
	}
	public int GetFlags()	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return GetEntityFlags(this.GetEntIndex());
	}
	public bool HasFlags(const int iFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return (this.GetFlags() & iFlags) == iFlags;
	}
	public void SetFlags(const int iFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		SetEntityFlags(this.GetEntIndex(), iFlags);
	}
	public void AddFlags(const int iFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetFlags(this.GetFlags() | iFlags);
	}
	public void RemoveFlags(const int iFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetFlags(this.GetFlags() & ~iFlags);
	}
	public int GetEFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags");
	}
	public bool HasEFlags(const int iEFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return (this.GetEFlags() & iEFlags) == iEFlags;
	}
	public void SetEFlags(const int iEFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags", iEFlags);
	}
	public void AddEFlags(const int iEFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetEFlags(this.GetEFlags() | iEFlags);
	}
	public void RemoveEFlags(const int iEFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetEFlags(this.GetEFlags() & ~iEFlags);
	}
	public void Teleport(const float vec3Origin[3] = NULL_VECTOR, const float vec3Angles[3] = NULL_VECTOR, const float vec3Velocity[3] = NULL_VECTOR)
	{
		TeleportEntity(this.GetEntIndex(), vec3Origin, vec3Angles, vec3Velocity);
	}
	public CBaseEntity GetParent()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hMoveParent"));
	}
	public void SetParent(const CBaseEntity pParent)
	{
		this.AcceptInputStr("SetParent", "!activator", pParent);
	}
	public void SetParentAttachment(const char[] szParentAttachment)
	{
		this.AcceptInputStr("SetParentAttachment", szParentAttachment);
	}
	public void SetParentAttachmentMaintainOffset(const char[] szParentAttachment)
	{
		this.AcceptInputStr("SetParentAttachmentMaintainOffset", szParentAttachment);
	}
	public void ClearParent()
	{
		this.AcceptInputStr("ClearParent");
	}
	public CBaseEntity GetMoveChild()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hMoveChild"));
	}
	public void SetMoveChild(const CBaseEntity pMoveChild)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hMoveChild", pMoveChild.GetEntIndex());
	}
	public void GetAbsOrigin(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vec3Buffer);
	}
	public void SetAbsOrigin(const float vec3Origin[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vec3Origin);
	}
	public void GetOrigin(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vec3Buffer);
	}
	public void SetOrigin(const float vec3Origin[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vec3Origin);
	}
	public void GetAbsAngles(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vec3Buffer);
	}
	public void SetAbsAngles(const float vec3Angles[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vec3Angles);
	}
	public void GetAngles(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Send, "m_angRotation", vec3Buffer);
	}
	public void SetAngles(const float vec3Angles[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Send, "m_angRotation", vec3Angles);
	}
	public void GetBaseVelocity(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecBaseVelocity", vec3Buffer);
	}
	public void SetBaseVelocity(const float vec3Velocity[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecBaseVelocity", vec3Velocity);
	}
	public void GetAbsVelocity(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsVelocity", vec3Buffer);
	}
	public void SetAbsVelocity(const float vec3Velocity[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsVelocity", vec3Velocity);
	}
	public void HookOutput(const char[] szOutput, const EntityOutput pCallback, const bool bOnce)
	{
		HookSingleEntityOutput(this.GetEntIndex(), szOutput, pCallback, bOnce);
	}
	public void FireOutput(const char[] szOutput, const int iActivator = -1, const float flDelay = 0.0)
	{
		FireEntityOutput(this.GetEntIndex(), szOutput, iActivator, flDelay);
	}
	public int GetHammerID()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iHammerID");
	}
	public void GetMins(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMins", vec3Buffer);
	}
	public void SetMins(const float vec3Mins[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMins", vec3Mins);
	}
	public void GetMaxs(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMaxs", vec3Buffer);
	}
	public void SetMaxs(const float vec3Maxs[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecMaxs", vec3Maxs);
	}
	public void SetCollisionBounds(const float vec3Mins[3], const float vec3Maxs[3])
	{
		SDKCall(g_pSetMinMaxSize, this.GetEntIndex(), vec3Mins, vec3Maxs);
	}
	public void WorldSpaceCenter(float vec3Center[3])
	{
		SDKCall(g_pWorldSpaceCenter, this.GetEntIndex(), vec3Center);
	}
	public CBaseEntity GetOwner()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hOwnerEntity"));
	}
	public void SetOwner(const CBaseEntity pOwnerEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hOwnerEntity", pOwnerEntity.GetEntIndex());
	}
	public MoveType GetMoveType()
	{
		return GetEntityMoveType(this.GetEntIndex());
	}
	public void SetMoveType(const MoveType eMoveType)
	{
		SetEntityMoveType(this.GetEntIndex(), eMoveType);
	}
	public MoveCollide_t GetMoveCollide()
	{
		return view_as<MoveCollide_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_MoveCollide"));
	}
	public void SetMoveCollide(const MoveCollide_t eMoveCollide)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_MoveCollide", eMoveCollide);
	}
	public Collision_Group_t GetCollisionGroup()
	{
		return view_as<Collision_Group_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_CollisionGroup"));
	}
	public void SetCollisionGroup(const Collision_Group_t eCollisionGroup)
	{
		SetEntityCollisionGroup(this.GetEntIndex(), view_as<int>(eCollisionGroup));
	}
	public SolidType_t GetSolidType()
	{
		return view_as<SolidType_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType"));
	}
	public void SetSolidType(const SolidType_t eSolidType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType", eSolidType);
	}
	public int GetSolidFlags()	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_usSolidFlags");
	}
	public void SetSolidFlags(const int iSolidFlags)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_usSolidFlags", iSolidFlags);
	}
	public int GetNextThinkTick()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nNextThinkTick");
	}
	public void SetNextThinkTick(const int iTick)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nNextThinkTick", iTick);
	}
	public float GetNextThink()
	{
		int iThinkTick = this.GetNextThinkTick();
		return iThinkTick == -1 ? -1.0 : TICKS_TO_TIME(iThinkTick);
	}
	public void SetNextThink(const float flNextThink, const char[] szContext = NULL_STRING)
	{
		if (strlen(szContext) > 0)
		{
			SDKCall(g_pSetNextThink, this.entindex, flNextThink, szContext);
		}
		else
		{
			SDKCall(g_pSetNextThinkNullString, this.entindex, flNextThink, 0);
		}
	}
	public int GetSimulationTick()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nSimulationTick");
	}
	public void GetViewOffset(float vec3Buffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec3Buffer);
	}
	public void SetViewOffset(const float vec3ViewOffset[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec3ViewOffset);
	}
	public void SetModel(const char[] szModel)
	{
		SetEntityModel(this.GetEntIndex(), szModel);
	}
	public bool GetModelName(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_ModelName", szBuffer, iMaxLength) != 0;
	}
	public void SetModelName(const char[] szModel)
	{
		SetEntPropString(this.GetEntIndex(), Prop_Data, "m_ModelName", szModel);
	}
	public int GetModelIndex()
	{
		GetEntProp(this.GetEntIndex(), Prop_Send, "m_nModelIndex");
	}
	public void SetModelIndex(const int nModelIndex)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nModelIndex", nModelIndex);
	}
	public RenderMode GetRenderMode()
	{
		return GetEntityRenderMode(this.GetEntIndex());
	}
	public void SetRenderMode(const RenderMode eRenderMode)
	{
		SetEntityRenderMode(this.GetEntIndex(), eRenderMode);
	}
	public RenderFx GetRenderFx()
	{
		return GetEntityRenderFx(this.GetEntIndex());
	}
	public void SetRenderFx(const RenderFx eRenderFx)
	{
		SetEntityRenderFx(this.GetEntIndex(), eRenderFx);
	}
	public RGBA GetRenderColor()
	{
		int r, g, b, a;
		GetEntityRenderColor(this.GetEntIndex(), r, g, b, a);
		return RGBA(r, g, b, a);
	}
	public void SetRenderColor(const RGBA col4)
	{
		SetEntityRenderColor(this.GetEntIndex(), col4.r, col4.g, col4.b, col4.a);
	}
	public int GetRenderAlpha()
	{
		return this.GetRenderColor().a;
	}
	public void SetRenderAlpha(const int a)
	{
		RGBA col4 = this.GetRenderColor();
		this.SetRenderColor(RGBA(col4.r, col4.g, col4.b, a));
	}
	public EntityEffectFlags GetEffects()		// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return view_as<EntityEffectFlags>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fEffects"));
	}
	public bool HasEffects(const EntityEffectFlags eEffects)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		return (this.GetEffects() & eEffects) == eEffects;
	}
	public void SetEffects(const EntityEffectFlags eEffects)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_fEffects", eEffects);
	}
	public void AddEffects(const EntityEffectFlags eEffects)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetEffects(this.GetEffects() | eEffects);
	}
	public void RemoveEffects(const EntityEffectFlags eEffects)	// TODO: Remove this function and turn each bitflag into a individual function.
	{
		this.SetEffects(this.GetEffects() & ~eEffects);
	}
	public int GetTakeDamage()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_takedamage");
	}
	public void SetTakeDamage(const int m_takedamage)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_takedamage", m_takedamage); 
	}
	public bool IsWorld()
	{
		return this.GetEntIndex() == 0;
	}
	public bool IsPlayer()
	{
		return (0 < this.GetEntIndex() <= MaxClients);
	}
	public bool IsWeapon()
	{
		char szTitle[] = "weapon_";
		char szClassname[sizeof(szTitle)];
		return this.GetClassname(szClassname, sizeof(szClassname)) && strcmp(szClassname, szTitle, false) == 0;
	}
	public bool IsPickupItem()
	{
		char szNetClass[MAX_NETNAME];
		return this.GetNetClassname(szNetClass, sizeof(szNetClass)) && (strcmp(szNetClass, "CBasePickup") == 0 || strcmp(szNetClass, "CItem_Battery") == 0 || strcmp(szNetClass, "CAmmoCanister") == 0);
	}
	public bool IsNPC()
	{
		return SDKCall(g_pIsNPC, this);
	}
	public WaterLevel GetWaterLevel()
	{
		return view_as<WaterLevel>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_nWaterLevel"));
	}
	public CBaseEntity GetGroundEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hGroundEntity"));
	}
	public void SetGroundEntity(const CBaseEntity pGroundEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hGroundEntity", pGroundEntity.entindex);
	}
	public int GetHealth()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iHealth");
	}
	public void SetHealth(const int iHealth)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iHealth", iHealth);
	}
	public int GetMaxHealth()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iMaxHealth");
	}
	public void SetMaxHealth(const int iMaxHealth)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iMaxHealth", iMaxHealth);
	}
	public bool FVisibleEntity(const CBaseEntity pEntity, int iTraceMask = MASK_BLOCKLOS, CBaseEntity& pBlocker = NULL_CBASEENTITY)
	{
		if (pEntity.HasFlags(FL_NOTARGET))
			return false;

		float vec3LookerOrigin[3];
		this.GetEyePosition(vec3LookerOrigin);
		float vec3TargetOrigin[3];
		pEntity.GetEyePosition(vec3TargetOrigin);

		CTraceRay pTraceRay;
		if (ai_los_mode.BoolValue)
		{
			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisibleAiLosEnabled, this.GetEntIndex());
		}
		else
		{
			// If we're doing an LOS search, include NPCs.
			if (iTraceMask == MASK_BLOCKLOS)
			{
				iTraceMask = MASK_BLOCKLOS_AND_NPCS;
			}

			// Player sees through nodraw
			if (this.IsPlayer())
			{
				iTraceMask &= ~CONTENTS_BLOCKLOS;
			}

			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisible, this.GetEntIndex());
		}

		if (pTraceRay.GetFraction() != 1.0 || pTraceRay.StartedSolid())
		{
			CBaseEntity pTracedEntity = pTraceRay.GetEntity();

			// If we hit the entity we're looking for, it's visible
			if (pEntity == pTracedEntity)
			{
				pTraceRay.Close();
				return true;
			}

			if (pEntity.IsPlayer())
			{
				CBasePlayer pPlayer = view_as<CBasePlayer>(pEntity);
				if (pPlayer.GetVehicle() == pTracedEntity)
				{
					// Got line of sight on the vehicle the player is driving!
					pTraceRay.Close();
					return true;
				}
			}

			// Line of sight is not established
			pBlocker = pTracedEntity;
		}

		pTraceRay.Close();
		return false;
	}
	public bool FVisiblePosition(const float vec3TargetOrigin[3], int iTraceMask = MASK_BLOCKLOS, CBaseEntity& pBlocker = NULL_CBASEENTITY)
	{
		float vec3LookerOrigin[3];
		this.GetEyePosition(vec3LookerOrigin);

		CTraceRay pTraceRay;
		if (ai_los_mode.BoolValue)
		{
			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisibleAiLosEnabled, this.GetEntIndex());
		}
		else
		{
			// If we're doing an LOS search, include NPCs.
			if (iTraceMask == MASK_BLOCKLOS)
			{
				iTraceMask = MASK_BLOCKLOS_AND_NPCS;
			}

			// Player sees through nodraw
			if (this.IsPlayer())
			{
				iTraceMask &= ~CONTENTS_BLOCKLOS;
			}

			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisible, this.GetEntIndex());
		}

		if (pTraceRay.GetFraction() != 1.0)
		{
			// Line of sight is not established
			pBlocker = pTraceRay.GetEntity();
			pTraceRay.Close();
			return false;
		}
		else
		{
			// line of sight is valid.
			pTraceRay.Close();
			return true;
		}
	}
	public CBaseCombatCharacter ToBaseCombatCharacter()
	{
		// TODO:
		// Call `MyCombatCharacterPointer` and compare against `0`.
		ThrowError("Missing implementation.");
	}
	public bool IsEdictChanged()
	{
		return IsEdictFlagSetOnEntity(this.GetEntIndex(), 0);		// FL_EDICT_CHANGED (1 << 0)
	}
	public void SetEdictChanged(const bool bEdictChanged)
	{
		// Game DLL sets this when the entity state changes
		// Mutually exclusive with FL_EDICT_PARTIAL_CHANGE.
		SetEdictFlagOnEntity(this.GetEntIndex(), 0, bEdictChanged);	// FL_EDICT_CHANGED (1 << 0)
	}
	public bool IsEdictFree()
	{
		return IsEdictFlagSetOnEntity(this.GetEntIndex(), 1);		// FL_EDICT_FREE (1 << 1)
	}
	public void SetEdictFree(const bool bEdictFree)
	{
		// this edict if free for reuse
		SetEdictFlagOnEntity(this.GetEntIndex(), 1, bEdictFree);	// FL_EDICT_FREE (1 << 1)
	}
	public bool IsEdictFull()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 2);		// FL_EDICT_FULL (1 << 2)
	}
	public void SetEdictFull(const bool bEdictFull)
	{
	    // this is a full server entity
	    SetEdictFlagOnEntity(this.GetEntIndex(), 2, bEdictFull);	// FL_EDICT_FULL (1 << 2)
	}
	public bool IsEdictAlwaysTransmitting()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 3);				// FL_EDICT_ALWAYS (1 << 3)
	}
	public void SetEdictAlwaysTransmit(const bool bEdictAlwaysTransmite)
	{
	    // always transmit this entity
	    SetEdictFlagOnEntity(this.GetEntIndex(), 3, bEdictAlwaysTransmite);	// FL_EDICT_ALWAYS (1 << 3)
	}
	public bool IsEdictDontSend()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 4);				// FL_EDICT_DONTSEND (1 << 4)
	}
	public void SetEdictDontSend(const bool bEdictDontSend)
	{
	    // don't transmit this entity
	    SetEdictFlagOnEntity(this.GetEntIndex(), 4, bEdictDontSend);		// FL_EDICT_DONTSEND (1 << 4)
	}
	public bool IsEdictAlwaysTransmittingPVSCheck()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 5);						// FL_EDICT_PVSCHECK (1 << 5)
	}
	public void SetEdictAlwaysTransmitPVSCheck(const bool bEdictAlwaysTransmitePVSCheck)
	{
	    // always transmit entity, but cull against PVS
	    SetEdictFlagOnEntity(this.GetEntIndex(), 5, bEdictAlwaysTransmitePVSCheck);	// FL_EDICT_PVSCHECK (1 << 5)
	}
	public bool IsEdictPendingDormantCheck()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 6);						// FL_EDICT_PENDING_DORMANT_CHECK (1 << 6)
	}
	public void SetEdictPendingDormantCheck(const bool bEdictPendingDormantCheck)
	{
	    SetEdictFlagOnEntity(this.GetEntIndex(), 6, bEdictPendingDormantCheck);		// FL_EDICT_PENDING_DORMANT_CHECK (1 << 6)
	}
	public bool IsEdictDirtyPVSInformation()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 7);					// FL_EDICT_DIRTY_PVS_INFORMATION (1 << 7)
	}
	public void SetEdictDirtyPVSInformation(const bool bEdictDirtyPVSInformation)
	{
	    SetEdictFlagOnEntity(this.GetEntIndex(), 7, bEdictDirtyPVSInformation);	// FL_EDICT_DIRTY_PVS_INFORMATION (1 << 7)
	}
	public bool IsFullEdictChanged()
	{
	    return IsEdictFlagSetOnEntity(this.GetEntIndex(), 8);			// FL_FULL_EDICT_CHANGED (1 << 8)
	}
	public void SetFullEdictChanged(const bool bFullEdictChanged)
	{
	    SetEdictFlagOnEntity(this.GetEntIndex(), 8, bFullEdictChanged);	// FL_FULL_EDICT_CHANGED (1 << 8)
	}
}

static bool TraceEntityFilter_FVisibleAiLosEnabled(int iEntIndex, int iMask, int iThisEntIndex)
{
	// The current check prevents the ray from hitting the caller.
	// TODO:
	// Review how to extend the filter to the original implementation.
	return iEntIndex != iThisEntIndex;

	//return CBaseEntity(iEntity).GetCollisionGroup() != COLLISION_GROUP_NONE;
}

static bool TraceEntityFilter_FVisible(int iEntIndex, int iMask, int iThisEntIndex)
{
	// The current check prevents the ray from hitting the caller.
	// TODO:
	// Review how to extend the filter to the original implementation.
	return iEntIndex != iThisEntIndex;

	//return CBaseEntity(iEntity).GetCollisionGroup() != COLLISION_GROUP_NONE;
}

static bool IsEdictFlagSetOnEntity(const int iEntIndex, const int iBitIndex)
{
	return GetBitFromInt(GetEdictFlags(iEntIndex), iBitIndex);
}

static void SetEdictFlagOnEntity(const int iEntIndex, const int iBitIndex, const bool bSetEdict)
{
	int iEdictFlags = GetEdictFlags(iEntIndex);
	SetBitOnInt(iEdictFlags, iBitIndex, bSetEdict);
	SetEdictFlags(iEntIndex, iEdictFlags);
}
