#pragma newdecls required
#pragma semicolon 1

static ConVar ai_los_mode;

static Handle g_pSetNextThink;
static Handle g_pSetNextThinkNullString;
static Handle g_pWorldSpaceCenter;
static Handle g_pIsNPC;
static Handle g_pSetMinMaxSize;

const any NULL_CBASEENTITY = -1;

//static StringMap g_pEntityData;

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

		/*
		g_pEntityData = new StringMap();
		DHookAddEntityListener(ListenType_Created, Hook_CBaseEntity_OnCreated);
		DHookAddEntityListener(ListenType_Deleted, Hook_CBaseEntity_OnDeleted);

		int iEntIndex = -1;
		while ((iEntIndex = FindEntityByClassname(iEntIndex, "*")) != -1)
		{
			Hook_CBaseEntity_OnCreated(iEntIndex);
		}
		*/
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
	property int entref
	{
		public get()
		{
			return view_as<int>(this);
		}
	}
	property Address address
	{
		public get()
		{
			return (this != NULL_CBASEENTITY) ? GetEntityAddress(this.entindex) : Address_Null;
		}
	}
	property int edictFlags
	{
		public get()
		{
			return GetEdictFlags(this.entindex);
		}
		public set(const int iEdictFlags)
		{
			SetEdictFlags(this.entindex, iEdictFlags);
		}
	}
	property int m_spawnflags
	{
		public get()
		{
			return GetEntProp(this.entindex, Prop_Data, "m_spawnflags");
		}
		public set(const int iSpawnFlags)
		{
			SetEntProp(this.entindex, Prop_Data, "m_spawnflags", iSpawnFlags);
		}
	}
	property int m_fFlags
	{
		public get()
		{
			return GetEntityFlags(this.entindex);
		}
		public set(const int iFlags)
		{
			SetEntityFlags(this.entindex, iFlags);
		}
	}
	property EntityEffectFlags m_fEffects
	{
		public get()
		{
			return view_as<EntityEffectFlags>(GetEntProp(this.entindex, Prop_Data, "m_fEffects"));
		}
		public set(const EntityEffectFlags eEffects)
		{
			SetEntProp(this.entindex, Prop_Data, "m_fEffects", eEffects);
		}
	}
	property EntityFlags m_iEFlags
	{
		public get()
		{
			return view_as<EntityFlags>(GetEntProp(this.entindex, Prop_Data, "m_iEFlags"));
		}
		public set(const EntityFlags eEFlags)
		{
			SetEntProp(this.entindex, Prop_Data, "m_iEFlags", eEFlags);
		}
	}
	property SolidFlags_t m_usSolidFlags
	{
		public get()
		{
			return view_as<SolidFlags_t>(GetEntProp(this.entindex, Prop_Data, "m_usSolidFlags"));
		}
		public set(const SolidFlags_t eSolidFlags)
		{
			SetEntProp(this.entindex, Prop_Data, "m_usSolidFlags", eSolidFlags);
		}
	}
	public bool IsValid()
	{
		return (this != NULL_CBASEENTITY) && IsValidEntity(view_as<int>(this));
	}
	public bool HasEdict()
	{
		return IsValidEdict(this.entindex);
	}
	public bool GetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityClassname(this.entindex, szBuffer, iMaxLength);
	}
	public int SetClassname(const char[] szClassname)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_iClassname", szClassname); 
	}
	public bool GetNetClassname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntityNetClass(this.entindex, szBuffer, iMaxLength);
	}
	public bool GetTargetname(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iName", szBuffer, iMaxLength) != 0;
	}
	public int SetTargetname(const char[] szTargetname)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_iName", szTargetname); 
	}
	public bool GetTarget(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_target", szBuffer, iMaxLength) != 0;
	}
	public int SetTarget(const char[] szTarget)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_target", szTarget); 
	}
	public bool IsClassname(const char[] szClassname)
	{
		static char szBuffer[MAX_CLASSNAME];
		this.GetClassname(szBuffer, sizeof(szBuffer));
		return StrEqual(szClassname, szBuffer, false);
	}
	public bool Spawn()
	{
		return DispatchSpawn(this.entindex);
	}
	public void Activate()
	{
		ActivateEntity(this.entindex);
	}
	public bool AcceptInput(const char[] szInput, const CBaseEntity pActivator = NULL_CBASEENTITY, const CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		return AcceptEntityInput(this.entindex, szInput, pActivator.entindex, pCaller.entindex, iOutputID);
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
	public bool AddOutput(const char[] szOutput, const char[] szTarget, const char[] szTargetInput, const char[] szParameter = "", const float flDelay = 0.0, const int iTimesToFire = -1)
	{
		char szBuffer[256];
		Format(szBuffer, sizeof(szBuffer), "%s %s:%s:%s:%f:%d", szOutput, szTarget, szTargetInput, szParameter, flDelay, iTimesToFire);
		return this.AcceptInputStr("AddOutput", szBuffer);
	}
	public bool KeyValue(const char[] szKeyValue, const char[] szValue)
	{
		return DispatchKeyValue(this.entindex, szKeyValue, szValue);
	}
	public bool KeyValueInt(const char[] szKeyValue, const int iValue)
	{
		return DispatchKeyValueInt(this.entindex, szKeyValue, iValue);
	}
	public bool KeyValueFloat(const char[] szKeyValue, const float flValue)
	{
		return DispatchKeyValueFloat(this.entindex, szKeyValue, flValue);
	}
	public bool KeyValueVector(const char[] szKeyValue, const float vecValue[3])
	{
		return DispatchKeyValueVector(this.entindex, szKeyValue, vecValue);
	}
	public bool Kill()
	{
		return this.AcceptInput("Kill");
	}
	public bool KillDelayed(const float flDelay)
	{
		return this.AddOutput("OnUser1", "!self", "kill", _, flDelay) && this.AcceptInput("FireUser1");
	}
	public int GetTeam()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iTeamNum");
	}
	public void GetEyePosition(float vec3Buffer[3])
	{
		if (this.IsPlayer())
		{
			GetClientEyePosition(this.entindex, vec3Buffer);
		}
		else
		{
			this.GetAbsOrigin(vec3Buffer);

			float vec3Offset[3];
			GetEntPropVector(this.entindex, Prop_Data, "m_vecViewOffset", vec3Offset);

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
	public void Teleport(const float vec3Origin[3] = NULL_VECTOR, const float vec3Angles[3] = NULL_VECTOR, const float vec3Velocity[3] = NULL_VECTOR)
	{
		TeleportEntity(this.entindex, vec3Origin, vec3Angles, vec3Velocity);
	}
	public CBaseEntity GetParent()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hMoveParent"));
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
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hMoveChild"));
	}
	public void SetMoveChild(const CBaseEntity pMoveChild)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hMoveChild", pMoveChild.entindex);
	}
	public void GetAbsOrigin(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecAbsOrigin", vec3Buffer);
	}
	public void SetAbsOrigin(const float vec3Origin[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecAbsOrigin", vec3Origin);
	}
	public void GetOrigin(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecOrigin", vec3Buffer);
	}
	public void SetOrigin(const float vec3Origin[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecOrigin", vec3Origin);
	}
	public void GetAbsAngles(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_angAbsRotation", vec3Buffer);
	}
	public void SetAbsAngles(const float vec3Angles[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_angAbsRotation", vec3Angles);
	}
	public void GetAngles(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Send, "m_angRotation", vec3Buffer);
	}
	public void SetAngles(const float vec3Angles[3])
	{
		SetEntPropVector(this.entindex, Prop_Send, "m_angRotation", vec3Angles);
	}
	public void GetBaseVelocity(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecBaseVelocity", vec3Buffer);
	}
	public void SetBaseVelocity(const float vec3Velocity[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecBaseVelocity", vec3Velocity);
	}
	public void GetAbsVelocity(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecAbsVelocity", vec3Buffer);
	}
	public void SetAbsVelocity(const float vec3Velocity[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecAbsVelocity", vec3Velocity);
	}
	public void HookOutput(const char[] szOutput, const EntityOutput pCallback, const bool bOnce = false)
	{
		HookSingleEntityOutput(this.entindex, szOutput, pCallback, bOnce);
	}
	public void FireOutput(const char[] szOutput, const int iActivator = -1, const float flDelay = 0.0)
	{
		FireEntityOutput(this.entindex, szOutput, iActivator, flDelay);
	}
	public int GetHammerID()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iHammerID");
	}
	public void GetMins(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecMins", vec3Buffer);
	}
	public void SetMins(const float vec3Mins[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecMins", vec3Mins);
	}
	public void GetMaxs(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecMaxs", vec3Buffer);
	}
	public void SetMaxs(const float vec3Maxs[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecMaxs", vec3Maxs);
	}
	public void SetCollisionBounds(const float vec3Mins[3], const float vec3Maxs[3])
	{
		SDKCall(g_pSetMinMaxSize, this.entindex, vec3Mins, vec3Maxs);
	}
	public void WorldSpaceCenter(float vec3Center[3])
	{
		SDKCall(g_pWorldSpaceCenter, this.entindex, vec3Center);
	}
	public CBaseEntity GetOwner()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hOwnerEntity"));
	}
	public void SetOwner(const CBaseEntity pOwnerEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hOwnerEntity", pOwnerEntity.entindex);
	}
	public MoveType GetMoveType()
	{
		return GetEntityMoveType(this.entindex);
	}
	public void SetMoveType(const MoveType eMoveType)
	{
		SetEntityMoveType(this.entindex, eMoveType);
	}
	public MoveCollide_t GetMoveCollide()
	{
		return view_as<MoveCollide_t>(GetEntProp(this.entindex, Prop_Data, "m_MoveCollide"));
	}
	public void SetMoveCollide(const MoveCollide_t eMoveCollide)
	{
		SetEntProp(this.entindex, Prop_Data, "m_MoveCollide", eMoveCollide);
	}
	public Collision_Group_t GetCollisionGroup()
	{
		return view_as<Collision_Group_t>(GetEntProp(this.entindex, Prop_Data, "m_CollisionGroup"));
	}
	public void SetCollisionGroup(const Collision_Group_t eCollisionGroup)
	{
		SetEntityCollisionGroup(this.entindex, view_as<int>(eCollisionGroup));
	}
	public SolidType_t GetSolidType()
	{
		return view_as<SolidType_t>(GetEntProp(this.entindex, Prop_Data, "m_nSolidType"));
	}
	public void SetSolidType(const SolidType_t eSolidType)
	{
		SetEntProp(this.entindex, Prop_Data, "m_nSolidType", eSolidType);
	}
	public int GetNextThinkTick()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_nNextThinkTick");
	}
	public void SetNextThinkTick(const int iTick)
	{
		SetEntProp(this.entindex, Prop_Data, "m_nNextThinkTick", iTick);
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
		return GetEntProp(this.entindex, Prop_Data, "m_nSimulationTick");
	}
	public float GetLocalTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flLocalTime");
	}
	public void SetLocalTime(const float flTime)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flLocalTime", flTime);
	}
	public void GetViewOffset(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecViewOffset", vec3Buffer);
	}
	public void SetViewOffset(const float vec3ViewOffset[3])
	{
		SetEntPropVector(this.entindex, Prop_Data, "m_vecViewOffset", vec3ViewOffset);
	}
	public void SetModel(const char[] szModel)
	{
		SetEntityModel(this.entindex, szModel);
	}
	public bool GetModelName(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_ModelName", szBuffer, iMaxLength) != 0;
	}
	public void SetModelName(const char[] szModel)
	{
		SetEntPropString(this.entindex, Prop_Data, "m_ModelName", szModel);
	}
	public int GetModelIndex()
	{
		GetEntProp(this.entindex, Prop_Send, "m_nModelIndex");
	}
	public void SetModelIndex(const int nModelIndex)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nModelIndex", nModelIndex);
	}
	public RenderMode GetRenderMode()
	{
		return GetEntityRenderMode(this.entindex);
	}
	public void SetRenderMode(const RenderMode eRenderMode)
	{
		SetEntityRenderMode(this.entindex, eRenderMode);
	}
	public RenderFx GetRenderFx()
	{
		return GetEntityRenderFx(this.entindex);
	}
	public void SetRenderFx(const RenderFx eRenderFx)
	{
		SetEntityRenderFx(this.entindex, eRenderFx);
	}
	public RGBA GetRenderColor()
	{
		int r, g, b, a;
		GetEntityRenderColor(this.entindex, r, g, b, a);
		return RGBA(r, g, b, a);
	}
	public void SetRenderColor(const RGBA col4)
	{
		SetEntityRenderColor(this.entindex, col4.r, col4.g, col4.b, col4.a);
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
	public TakeDamage_t GetTakeDamage()
	{
		return view_as<TakeDamage_t>(GetEntProp(this.entindex, Prop_Data, "m_takedamage"));
	}
	public void SetTakeDamage(const TakeDamage_t eTakeDamage)
	{
		SetEntProp(this.entindex, Prop_Data, "m_takedamage", eTakeDamage); 
	}
	public bool IsWorld()
	{
		return this.entindex == 0;
	}
	public bool IsPlayer()
	{
		return (0 < this.entindex <= MaxClients);
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
	public WaterLevel_t GetWaterLevel()
	{
		return view_as<WaterLevel_t>(GetEntProp(this.entindex, Prop_Data, "m_nWaterLevel"));
	}
	public CBaseEntity GetGroundEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hGroundEntity"));
	}
	public void SetGroundEntity(const CBaseEntity pGroundEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hGroundEntity", pGroundEntity.entindex);
	}
	public CBaseEntity GetEffectEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hEffectEntity"));
	}
	public void SetEffectEntity(const CBaseEntity pEffectEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hEffectEntity", pEffectEntity.entindex);
	}
	public int GetHealth()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iHealth");
	}
	public void SetHealth(const int iHealth)
	{
		SetEntProp(this.entindex, Prop_Data, "m_iHealth", iHealth);
	}
	public int GetMaxHealth()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iMaxHealth");
	}
	public void SetMaxHealth(const int iMaxHealth)
	{
		SetEntProp(this.entindex, Prop_Data, "m_iMaxHealth", iMaxHealth);
	}
	public bool FVisibleEntity(const CBaseEntity pEntity, int iTraceMask = MASK_BLOCKLOS, CBaseEntity& pBlocker = NULL_CBASEENTITY)
	{
		if (pEntity.m_fFlags & FL_NOTARGET)
			return false;

		float vec3LookerOrigin[3];
		this.GetEyePosition(vec3LookerOrigin);
		float vec3TargetOrigin[3];
		pEntity.GetEyePosition(vec3TargetOrigin);

		CTraceRay pTraceRay;
		if (ai_los_mode.BoolValue)
		{
			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisibleAiLosEnabled, this.entindex);
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

			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisible, this.entindex);
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
			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisibleAiLosEnabled, this.entindex);
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

			pTraceRay = new CTraceRay(vec3LookerOrigin, vec3TargetOrigin, iTraceMask, RayType_EndPoint, TraceEntityFilter_FVisible, this.entindex);
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
	public void StopSound(const int iChannel, const char[] szName)
	{
		StopSound(this.entindex, iChannel, szName);
	}
	/*
	public any GetUserData(const char[] szKey)
	{
		StringMap pEntityData;
		{
    		char szEntityKey[MAX_CHARS_INT(INT_MIN)];
    		IntToString(this.entref, szEntityKey, sizeof(szEntityKey));
    		g_pEntityData.GetValue(szEntityKey, pEntityData);
		}

		any data;
    	pEntityData.GetValue(szKey, data);
		return data;
	}
	public void SetUserData(const char[] szKey, const any data)
	{
		StringMap pEntityData;
    	char szEntityKey[MAX_CHARS_INT(INT_MIN)];
    	IntToString(this.entref, szEntityKey, sizeof(szEntityKey));
    	g_pEntityData.GetValue(szEntityKey, pEntityData);
		pEntityData.SetValue(szKey, data);
	}
	*/
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

enum EntityEffectFlags
{
	EF_BONEMERGE			= 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT 			= 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT 			= 0x004,	// player flashlight
	EF_NOINTERP				= 0x008,	// don't interpolate the next frame
	EF_NOSHADOW				= 0x010,	// Don't cast no shadow
	EF_NODRAW				= 0x020,	// don't draw entity
	EF_NORECEIVESHADOW		= 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL	= 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
	EF_ITEM_BLINK			= 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES		= 0x200,	// always assume that the parent entity is animating
}

// entity flags, CBaseEntity::m_iEFlags
enum EntityFlags
{
	EFL_KILLME = (1 << 0),							// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT = (1 << 1),							// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE = (1 << 2),					// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES = (1 << 3),				// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4),		// This is a special entity that should not be deleted when we restart entities only
	EFL_HAS_PLAYER_CHILD = (1 << 4),				// One of the child entities is a player.
	EFL_DIRTY_SHADOWUPDATE = (1 << 5),				// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY = (1 << 6),							// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

	EFL_BOT_FROZEN = (1 << 8),						// This is set on bots that are frozen.
	EFL_SERVER_ONLY = (1 << 9),						// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10),			// Don't attach the edict; we're doing it explicitly
	
	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	// UNUSED = (1 << 16),

	EFL_IN_SKYBOX = (1 << 17),						// This is set if the entity detects that it's in the skybox.
	// This forces it to pass the "in PVS" for transmission.
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
	EFL_TOUCHING_FLUID = (1 << 19),					// Used to determine if an entity is floating
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
	EFL_NO_ROTORWASH_PUSH = (1 << 21),				// I shouldn't be pushed by the rotorwash
	EFL_NO_THINK_FUNCTION = (1 << 22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),
	EFL_CHECK_UNTOUCH = (1 << 24),
	EFL_DONTBLOCKLOS = (1 << 25),					// I shouldn't block NPC line-of-sight
	EFL_DONTWALKON = (1 << 26),						// NPC;s should not walk on this entity
	EFL_NO_DISSOLVE = (1 << 27),					// These guys shouldn't dissolve
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),		// Mega physcannon can't ragdoll these guys.
	EFL_NO_WATER_VELOCITY_CHANGE =	(1 << 29),		// Don't adjust this entity's velocity when transitioning into water
	EFL_NO_PHYSCANNON_INTERACTION =	(1 << 30),		// Physcannon can't pick these up or punt them
	EFL_NO_DAMAGE_FORCES = (1 << 31),				// Doesn't accept forces from physics damage
}

enum SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST = 0x0001,			// Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST = 0x0002,			// Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID = 0x0004,				// Are we currently not solid?
	FSOLID_TRIGGER = 0x0008,				// This is something may be collideable but fires touch functions
	
	// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE = 0x0010,			// You can't stand on this
	FSOLID_VOLUME_CONTENTS = 0x0020,		// Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED = 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS = 0x0080,		// Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED = 0x0100,	// Collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200,	// This trigger will touch debris objects
}

/*
static void Hook_CBaseEntity_OnCreated(const int iEntIndex)
{
    char szKey[MAX_CHARS_INT(INT_MIN)];
    IntToString(CBaseEntity(iEntIndex).entref, szKey, sizeof(szKey));
    g_pEntityData.SetValue(szKey, new StringMap());
}

static void Hook_CBaseEntity_OnDeleted(const int iEntIndex)
{
    char szKey[MAX_CHARS_INT(INT_MIN)];
    IntToString(CBaseEntity(iEntIndex).entref, szKey, sizeof(szKey));
    g_pEntityData.Remove(szKey);
}
*/
