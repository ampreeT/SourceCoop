#pragma newdecls required
#pragma semicolon 1

#define NULL_CBASEENTITY view_as<any>(-1)

methodmap CBaseEntity
{
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
	/** Deprecated: Use entindex property instead */
	public int GetEntIndex()
	{
		return EntRefToEntIndex(view_as<int>(this));
	}
	public bool IsValid()
	{
		if (this == NULL_CBASEENTITY)
		{
			return false;
		}
		return IsValidEntity(view_as<int>(this));
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
	public bool AcceptInput(const char[] szInput, CBaseEntity pActivator = NULL_CBASEENTITY, CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		return AcceptEntityInput(this.GetEntIndex(), szInput, pActivator.GetEntIndex(), pCaller.GetEntIndex(), iOutputID);
	}
	public bool AcceptInputStr(const char[] szInput, const char[] szBuffer = "", CBaseEntity pActivator = NULL_CBASEENTITY, CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		SetVariantString(szBuffer);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputInt(const char[] szInput, const int iValue, CBaseEntity pActivator = NULL_CBASEENTITY, CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		SetVariantInt(iValue);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputVector(const char[] szInput, const float vecValue[3], CBaseEntity pActivator = NULL_CBASEENTITY, CBaseEntity pCaller = NULL_CBASEENTITY, const int iOutputID = 0)
	{
		// ToDo: Should this use SetVariantPosVector3D or SetVariantVector3D?
		char szBuffer[256];
		Format(szBuffer, sizeof(szBuffer), "%f %f %f", vecValue[0], vecValue[1], vecValue[2]);
		return this.AcceptInputStr(szInput, szBuffer, pActivator, pCaller, iOutputID);
	}
	public bool OutputAdd(const char[] szOutput, const char[] szTarget, const char[] szTargetInput, const char[] szParameter = "", float flDelay = 0.0, int iTimesToFire = -1)
	{
		 char szBuffer[256];
		 Format(szBuffer, sizeof(szBuffer), "%s %s:%s:%s:%f:%d", szOutput, szTarget, szTargetInput, szParameter, flDelay, iTimesToFire);
		 return this.AcceptInputStr("AddOutput", szBuffer);
	}
	public bool SetKeyValueStr(const char[] szKeyValue, const char[] szValue)
	{
		return DispatchKeyValue(this.GetEntIndex(), szKeyValue, szValue);
	}
	public bool SetKeyValueFl(const char[] szKeyValue, const float flValue)
	{
		return DispatchKeyValueFloat(this.GetEntIndex(), szKeyValue, flValue);
	}
	public bool Kill()
	{
		return this.AcceptInputStr("Kill");
	}
	public bool KillAfterTime(float flTime)
	{
		this.OutputAdd("OnUser1", "!self", "kill", _, flTime);
		return this.AcceptInput("FireUser1");
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
	public int GetTeam()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iTeamNum");
	}
	public void GetEyePosition(float vecBuffer[3])
	{
		if (this.IsClassPlayer())
		{
			GetClientEyePosition(this.GetEntIndex(), vecBuffer);
		}
		else
		{
			float vec3Origin[3];
			this.GetAbsOrigin(vec3Origin);

			float vec3Offset[3];
			GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec3Offset);

			AddVectors(vec3Origin, vec3Offset, vecBuffer);
		}
	}
	// # Citations
	//
	// - [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/shared/baseentity_shared.cpp#L148)
	//
	public void GetEarPosition(float vecBuffer[3])
	{
		this.GetEyePosition(vecBuffer);
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
	public void AddFlags(const int iFlags)
	{
		this.SetFlags(this.GetFlags() | iFlags);
	}
	public void RemoveFlags(const int iFlags)
	{
		this.SetFlags(this.GetFlags() & ~iFlags);
	}
	public int GetEFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags");
	}
	public void SetEFlags(const int iEFlags)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags", iEFlags);
	}
	public void AddEFlags(const int iEFlags)
	{
		this.SetEFlags(this.GetEFlags() | iEFlags);
	}
	public void RemoveEFlags(const int iEFlags)
	{
		this.SetEFlags(this.GetEFlags() & ~iEFlags);
	}
	public int GetEdictFlags()
	{
		return GetEdictFlags(this.GetEntIndex());
	}
	public void SetEdictFlags(const int iEdictFlags)
	{
		SetEdictFlags(this.GetEntIndex(), iEdictFlags);
	}
	public void Teleport(const float vecOrigin[3] = NULL_VECTOR, const float vecAngles[3] = NULL_VECTOR, const float vecVelocity[3] = NULL_VECTOR)
	{
		TeleportEntity(this.GetEntIndex(), vecOrigin, vecAngles, vecVelocity);
	}
	public CBaseEntity GetParent()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hMoveParent"));
	}
	public void SetParent(CBaseEntity pParent)
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
	public void SetMoveChild(CBaseEntity pMoveChild)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hMoveChild", pMoveChild.GetEntIndex());
	}
	public void GetAbsOrigin(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vecBuffer);
	}
	public void SetAbsOrigin(const float vecOrigin[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vecOrigin);
	}
	public void GetOrigin(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vecBuffer);
	}
	public void SetOrigin(const float vecOrigin[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vecOrigin);
	}
	public void GetAbsAngles(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vecBuffer);
	}
	public void SetAbsAngles(const float vecAngles[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vecAngles);
	}
	public void GetAngles(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Send, "m_angRotation", vecBuffer);
	}
	public void SetAngles(const float vecAngles[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Send, "m_angRotation", vecAngles);
	}
	public void GetBaseVelocity(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecBaseVelocity", vecBuffer);
	}
	public void SetBaseVelocity(const float vecVel[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecBaseVelocity", vecVel);
	}
	public void GetAbsVelocity(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsVelocity", vecBuffer);
	}
	public void SetAbsVelocity(const float vecVel[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsVelocity", vecVel);
	}
	public void HookOutput(const char[] szOutput, EntityOutput pCallback, const bool bOnce)
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
		SDKCall(g_pSetMinMaxSize, this.GetEntIndex(), vecMins, vecMaxs);
	}
	public void WorldSpaceCenter(float vecCenter[3])
	{
		SDKCall(g_pWorldSpaceCenter, this.GetEntIndex(), vecCenter);
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
	public void SetMoveType(const MoveType moveType)
	{
		SetEntityMoveType(this.GetEntIndex(), moveType);
	}
	public MoveCollide_t GetMoveCollide()
	{
		return view_as<MoveCollide_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_MoveCollide"));
	}
	public void SetMoveCollide(const MoveCollide_t moveCollide)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_MoveCollide", moveCollide);
	}
	public int GetCollisionGroup()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_CollisionGroup");
	}
	public void SetCollisionGroup(const int iCollisionGroup)
	{
		SetEntityCollisionGroup(this.GetEntIndex(), iCollisionGroup);
	}
	public int GetSolidType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType");
	}
	public void SetSolidType(const SolidType_t iSolidType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nSolidType", iSolidType);
	}
	public int GetSolidFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_usSolidFlags");
	}
	public void SetSolidFlags(const int iSolidFlags)
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
	public void SetNextThink(float flNextThink, const char[] szContext = NULL_STRING)
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
	public void GetViewOffset(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vecBuffer);
	}
	public void SetViewOffset(const float vecViewOffset[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vecViewOffset);
	}
	public void SetModel(const char[] szModel)
	{
		SetEntityModel(this.GetEntIndex(), szModel);
	}
	public bool GetModelName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_ModelName", szBuffer, iMaxLength));
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
	public void SetRenderMode(const RenderMode rendermode)
	{
		SetEntityRenderMode(this.GetEntIndex(), rendermode);
	}
	public RenderFx GetRenderFx()
	{
		return GetEntityRenderFx(this.GetEntIndex());
	}
	public void SetRenderFx(const RenderFx renderfx)
	{
		SetEntityRenderFx(this.GetEntIndex(), renderfx);
	}
	public void GetRenderColor(int& r = 0, int& g = 0, int& b = 0, int& a = 0)
	{
		GetEntityRenderColor(this.GetEntIndex(), r, g, b, a);
	}
	public void SetRenderColor(const int r, const int g, const int b, const int a)
	{
		SetEntityRenderColor(this.GetEntIndex(), r, g, b, a);
	}
	public int GetRenderAlpha()
	{
		int a; this.GetRenderColor(a, a, a, a);
		return a;
	}
	public void SetRenderAlpha(const int iAmount)
	{
		int r, g, b, a;
		this.GetRenderColor(r, g, b, a);
		this.SetRenderColor(r, g, b, iAmount);
	}
	public int GetEffects()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_fEffects");
	}
	public void SetEffects(const int iEffects)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_fEffects", iEffects); 
	}
	public void AddEffects(const int iEffects)
	{
		this.SetEffects(this.GetEffects() | iEffects);
	}
	public void RemoveEffects(const int iEffects)
	{
		this.SetEffects(this.GetEffects() & ~iEffects);
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
	public bool IsClassPlayer()
	{
		return (0 < this.GetEntIndex() <= MaxClients);
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
		if (this.GetNetClassname(szNetClass, sizeof(szNetClass)))
		{
			// Todo - support all games
			return (strcmp(szNetClass, "CBasePickup") == 0 || strcmp(szNetClass, "CItem_Battery") == 0 || strcmp(szNetClass, "CAmmoCanister") == 0);
		}
		return false;
	}
	public bool IsClassNPC()
	{
		return SDKCall(g_pIsNPC, this);
	}
	public bool IsClassScientist()
	{
		char szTitle[] = "npc_human_scientist";
		char szClassname[sizeof(szTitle)];
		if (this.GetClassname(szClassname, sizeof(szClassname)))
			return view_as<bool>(strcmp(szClassname, szTitle, false) == 0);
		return false;
	}
	public WaterLevel GetWaterLevel()
	{
		return view_as<WaterLevel>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_nWaterLevel"));
	}
	public CBaseEntity GetGroundEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hGroundEntity"));
	}
	public void SetGroundEntity(CBaseEntity pGroundEntity)
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
	public bool FVisibleEntity(CBaseEntity pEntity, int iTraceMask = MASK_BLOCKLOS, CBaseEntity& pBlocker = NULL_CBASEENTITY)
	{
		if (pEntity.GetFlags() & FL_NOTARGET)
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
			if (this.IsClassPlayer())
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

			if (pEntity.IsClassPlayer())
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
			if (this.IsClassPlayer())
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
}

public bool TraceEntityFilter_FVisibleAiLosEnabled(int iEntIndex, int iMask, int iThisEntIndex)
{
	// The current check prevents the ray from hitting the caller.
	// TODO:
	// Review how to extend the filter to the original implementation.
	return iEntIndex != iThisEntIndex;
	//return CBaseEntity(iEntity).GetCollisionGroup() != COLLISION_GROUP_NONE;
}

public bool TraceEntityFilter_FVisible(int iEntIndex, int iMask, int iThisEntIndex)
{
	// The current check prevents the ray from hitting the caller.
	// TODO:
	// Review how to extend the filter to the original implementation.
	return iEntIndex != iThisEntIndex;
	//return CBaseEntity(iEntity).GetCollisionGroup() != COLLISION_GROUP_NONE;
}
