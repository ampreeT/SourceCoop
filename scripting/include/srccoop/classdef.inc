#if defined _srccoop_classdef_included
 #endinput
#endif
#define _srccoop_classdef_included

#include <sdktools>

#pragma newdecls required
#pragma semicolon 1

Handle g_pGlobalEntityGetIndex;
Handle g_pGlobalEntityGetState;
Handle g_pGlobalEntityGetName;
Handle g_pGlobalEntityGetCounter;
Handle g_pGlobalEntityGetMap;
Handle g_pGlobalEntitySetState;
Handle g_pGlobalEntitySetCounter;
Handle g_pGlobalEntityAdd;
Handle g_pGameShutdown;
Handle g_pSetCollisionBounds;
Handle g_pUpdateEnemyMemory;
Handle g_pGetSequenceLinearMotion;
Handle g_pShouldPlayerAvoid;
Handle g_pRemoveAllItems;
Handle g_pIsNPC;

stock void InitClassdef(GameData pGameConfig)
{
	char szSetCollisionBounds[] = "CBaseEntity::SetCollisionBounds";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szSetCollisionBounds))
		LogMessage("Could not obtain gamedata signature %s", szSetCollisionBounds);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		if (!(g_pSetCollisionBounds = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetCollisionBounds);
	}
		
	char szIsNpc[] = "CBaseEntity::IsNPC";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szIsNpc))
		LogMessage("Could not obtain gamedata offset %s", szIsNpc);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		if (!(g_pIsNPC = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szIsNpc);
	}
	
	char szRemoveAllItems[] = "CBasePlayer::RemoveAllItems";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szRemoveAllItems))
		LogMessage("Could not obtain gamedata offset %s", szRemoveAllItems);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
		if (!(g_pRemoveAllItems = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szRemoveAllItems);
	}
	
	char szGlobalEntity_GetIndex[] = "GlobalEntity_GetIndex";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_GetIndex))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetIndex);
	else
	{
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetIndex = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetIndex);
	}

	char szGlobalEntity_GetState[] = "GlobalEntity_GetState";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_GetState))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetState);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetState);
	}

	char szGlobalEntity_GetName[] = "GlobalEntity_GetName";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_GetName))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetName);
	else
	{	
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
		if (!(g_pGlobalEntityGetName = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetName);
	}

	char szGlobalEntity_GetCounter[] = "GlobalEntity_GetCounter";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_GetCounter))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetCounter);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetCounter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetCounter);
	}

	char szGlobalEntity_GetMap[] = "GlobalEntity_GetMap";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_GetMap))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetMap);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
		if (!(g_pGlobalEntityGetMap = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetMap);
	}

	char szGlobalEntity_SetState[] = "GlobalEntity_SetState";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_SetState))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_SetState);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntitySetState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_SetState);
	}

	char szGlobalEntity_SetCounter[] = "GlobalEntity_SetCounter";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_SetCounter))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_SetCounter);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntitySetCounter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_SetCounter);
	}

	char szGlobalEntity_Add[] = "GlobalEntity_Add";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGlobalEntity_Add))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_Add);
	else
	{
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityAdd = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_Add);
	}
	
	char szGameShutdown[] = "CServerGameDLL::GameShutdown";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGameShutdown))
		LogMessage("Could not obtain gamedata offset %s", szGameShutdown);
	else
	{
		if (!(g_pGameShutdown = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGameShutdown);
	}

	char szUpdateEnemyMemory[] = "CAI_BaseNPC::UpdateEnemyMemory";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szUpdateEnemyMemory))
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
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szShouldPlayerAvoid))
		LogMessage("Could not obtain gamedata offset %s", szShouldPlayerAvoid);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		if (!(g_pShouldPlayerAvoid = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szShouldPlayerAvoid);
	}

	char szGetSequenceLinearMotion[] = "CBaseAnimating::GetSequenceLinearMotion";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szGetSequenceLinearMotion))
		LogMessage("Could not obtain gamedata signature %s", szGetSequenceLinearMotion);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer, _, VENCODE_FLAG_COPYBACK);
		if (!(g_pGetSequenceLinearMotion = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetSequenceLinearMotion);
	}

	#if defined SRCCOOP_BLACKMESA
	InitClassdefBM(pGameConfig);
	#endif
}

#define NULL_CBASEENTITY view_as<any>(-1)

methodmap CBaseEntity
{
	public CBaseEntity(const int iEntIndex = NULL_CBASEENTITY)
	{
		return view_as<CBaseEntity>(iEntIndex > -1 ? EntIndexToEntRef(iEntIndex) : iEntIndex);
	}
	public static CBaseEntity Create(const char[] szClassname)
	{
		return CBaseEntity(CreateEntityByName(szClassname));
	}

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
		return StrEqual(szClassname, szBuffer);
	}
	public bool Spawn()
	{
		return DispatchSpawn(this.GetEntIndex());
	}
	public void Activate()
	{
		ActivateEntity(this.GetEntIndex());
	}
	public bool AcceptInput(const char[] szInput, CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		return AcceptEntityInput(this.GetEntIndex(), szInput, pActivator.GetEntIndex(), pCaller.GetEntIndex(), iOutputID);
	}
	public bool AcceptInputStr(const char[] szInput, const char[] szBuffer = "", CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		SetVariantString(szBuffer);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputInt(const char[] szInput, const int iValue, CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		SetVariantInt(iValue);
		return this.AcceptInput(szInput, pActivator, pCaller, iOutputID);
	}
	public bool AcceptInputVector(const char[] szInput, const float vecValue[3], CBaseEntity pActivator = view_as<CBaseEntity>(-1), CBaseEntity pCaller = view_as<CBaseEntity>(-1), const int iOutputID = 0)
	{
		// ToDo: Should this use SetVariantPosVector3D or SetVariantVector3D?
		char szBuffer[MAX_FORMAT];
		Format(szBuffer, sizeof(szBuffer), "%f %f %f", vecValue[0], vecValue[1], vecValue[2]);
		return this.AcceptInputStr(szInput, szBuffer, pActivator, pCaller, iOutputID);
	}
	public bool OutputAdd(const char[] szOutput, const char[] szTarget, const char[] szTargetInput, const char[] szParameter = "", float flDelay = 0.0, int timesToFire = -1)
	{
		 char szBuffer[MAX_FORMAT];
		 Format(szBuffer, sizeof(szBuffer), "%s %s:%s:%s:%f:%d", szOutput, szTarget, szTargetInput, szParameter, flDelay, timesToFire);
		 return this.AcceptInputStr("AddOutput", szBuffer);
	}
	public bool SetKeyValueStr(const char[] szKeyValue, const char[] szValue)
	{
		return DispatchKeyValue(this.GetEntIndex(), szKeyValue, szValue);
	}
	public bool SetKeyValueFl(const char[] szKeyValue, const float fValue)
	{
		return DispatchKeyValueFloat(this.GetEntIndex(), szKeyValue, fValue);
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
	public int GetEFlags()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags");
	}
	public void SetEFlags(const int iEFlags)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iEFlags", iEFlags);
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
	public void SetAbsOrigin(const float vecBuffer[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecAbsOrigin", vecBuffer);
	}
	public void GetOrigin(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vecBuffer);
	}
	public void SetOrigin(const float vecBuffer[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecOrigin", vecBuffer);
	}
	public void GetAbsAngles(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_angAbsRotation", vecBuffer);
	}
	public void SetAbsAngles(const float vecAngles[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Send, "m_angAbsRotation", vecAngles);
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
	public MoveCollide_t GetMoveCollide()
	{
		return view_as<MoveCollide_t>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_MoveCollide"));
	}
	public void SetMoveCollide(MoveCollide_t moveCollide)
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
	public void SetSolidType(SolidType_t iSolidType)
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
	public void SetNextThink(float flThinkTime)
	{
		int iThinkTick = (flThinkTime == -1) ? -1 : (TIME_TO_TICKS(flThinkTime));
		this.SetNextThinkTick(iThinkTick);
	}
	public void GetViewOffset(float vec[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec);
	}
	public void SetViewOffset(const float vec[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecViewOffset", vec);
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
	public void SetModelIndex(int nModelIndex)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nModelIndex", nModelIndex);
	}
	public RenderMode GetRenderMode()
	{
		return GetEntityRenderMode(this.GetEntIndex());
	}
	public void SetRenderMode(RenderMode rendermode)
	{
		SetEntityRenderMode(this.GetEntIndex(), rendermode);
	}
	public RenderFx GetRenderFx()
	{
		return GetEntityRenderFx(this.GetEntIndex());
	}
	public void SetRenderFx(RenderFx renderfx)
	{
		SetEntityRenderFx(this.GetEntIndex(), renderfx);
	}
	public void GetRenderColor(int &r, int &g, int &b, int &a)
	{
		GetEntityRenderColor(this.GetEntIndex(), r, g, b, a);
	}
	public void SetRenderColor(int r=255, int g=255, int b=255, int a=255)
	{
		SetEntityRenderColor(this.GetEntIndex(), r, g, b, a);
	}
	public int GetRenderAlpha()
	{
		int a; this.GetRenderColor(a, a, a, a);
		return a;
	}
	public void SetRenderAlpha(int iAmount)
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
		if(this.GetNetClassname(szNetClass, sizeof(szNetClass)))
		{
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
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hGroundEntity"));
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
}

enum struct FireOutputData
{
	char m_szName[32];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
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
		return view_as<CBaseAnimating>(CBaseEntity(iEntIndex));
	}
	public static CBaseAnimating Create(const char[] szClassname)
	{
		return CBaseAnimating(CreateEntityByName(szClassname));
	}

	public float GetModelScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale");
	}
	public void SetModelScale(float flModelScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale", flModelScale);
	}
	public int GetSequence()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence");
	}
	public void SetSequence(int iSequence)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence", iSequence);
	}
	public void GetSequenceLinearMotion(int iSequence, float pVec[3])
	{
		SDKCall(g_pGetSequenceLinearMotion, this, iSequence, pVec);
	}
	public float GetPlaybackRate()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPlaybackRate");
	}
	public void SetPlaybackRate(const float flPlaybackRate)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPlaybackRate", flPlaybackRate);
	}
	public float GetCycle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCycle");
	}
	public void SetCycle(const float flCycle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCycle", flCycle);
	}
}

methodmap CBaseTrigger < CBaseEntity
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(CBaseEntity(iEntIndex));
	}
	public bool IsDisabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled"));
	}
	public void SetDisabled(bool bDisabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled", bDisabled);
	}
	
}

methodmap CChangelevel < CBaseTrigger
{
	public CChangelevel(const int iEntIndex = -1)
	{
		return view_as<CChangelevel>(CBaseEntity(iEntIndex));
	}

	public void GetMapName(char[] szBuffer, const int iMaxLength)
	{
		GetEntPropString(this.GetEntIndex(), Prop_Data, "m_szMapName", szBuffer, iMaxLength);
	}
}

methodmap CSceneEntity < CBaseEntity
{
	public CSceneEntity(const int iEntIndex = -1)
	{
		return view_as<CSceneEntity>(CBaseEntity(iEntIndex));
	}
}

methodmap CAI_ScriptConditions < CBaseEntity
{
	public CAI_ScriptConditions(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptConditions>(CBaseEntity(iEntIndex));
	}

	public bool IsDisabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled"));
	}
	public void SetDisabled(bool disabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled", disabled);
	}
}

methodmap CBaseCombatWeapon < CBaseAnimating
{
	public CBaseCombatWeapon(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatWeapon>(CBaseEntity(iEntIndex));
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
	public int GetPrimaryAmmoType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iPrimaryAmmoType");
	}
	public void SetPrimaryAmmoType(const int iPrimaryAmmoType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iPrimaryAmmoType", iPrimaryAmmoType); 
	}
	public int GetSecondaryAmmo()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip2");
	}
	public void SetSecondaryAmmo(const int iSecondaryAmmo)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iClip2", iSecondaryAmmo); 
	}
	public int GetSecondaryAmmoType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iSecondaryAmmoType");
	}
	public void SetSecondaryAmmoType(const int iSecondaryAmmoType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iSecondaryAmmoType", iSecondaryAmmoType); 
	}
	public float GetNextPrimaryAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextPrimaryAttack");
	}
	public void SetNextPrimaryAttack(const float flNextPrimaryAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextPrimaryAttack", flNextPrimaryAttack); 
	}
	public float GetNextSecondaryAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextSecondaryAttack");
	}
	public void SetNextSecondaryAttack(const float flNextSecondaryAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flNextSecondaryAttack", flNextSecondaryAttack); 
	}
	public bool m_bFiresUnderwater()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bFiresUnderwater"));
	}
	public int GetViewModelIndex()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_nViewModelIndex");
	}
	public void SetViewModelIndex(const int iViewModelIndex = 0)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_nViewModelIndex", iViewModelIndex);
	}
	public float GetWeaponTimeIdle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle");
	}
	public void SetWeaponTimeIdle(const float flTimeIdle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle", flTimeIdle);
	}
}

methodmap CPredictedViewModel < CBaseAnimating
{
	public CPredictedViewModel(const int iEntIndex = -1)
	{
		return view_as<CPredictedViewModel>(CBaseEntity(iEntIndex));
	}
	
	public float GetWeaponTimeIdle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle");
	}
	public void SetWeaponTimeIdle(const float flTimeIdle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle", flTimeIdle);
	}
	public CBaseCombatWeapon GetOwningWeapon()
	{
		return view_as<CBaseCombatWeapon>(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hWeapon"));
	}
}

methodmap CBaseCombatCharacter < CBaseAnimating
{
	public CBaseCombatCharacter(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatCharacter>(CBaseEntity(iEntIndex));
	}
	public bool IsAlive()
	{
		//return view_as<bool>(this.GetHealth() > 0);
		
		// Any value above 0 indicates dead or other death information
		return !GetEntProp(this.GetEntIndex(), Prop_Data, "m_lifeState");
	}
	public CBaseCombatWeapon GetActiveWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon"));
	}
	public void SetActiveWeapon(CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon", pWeapon.GetEntIndex());
	}
	public CBaseCombatWeapon GetLastWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLastWeapon"));
	}
}

methodmap CAI_BaseNPC < CBaseCombatCharacter
{
	public CAI_BaseNPC(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseNPC>(CBaseEntity(iEntIndex));
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
		bool bOutOfBounds = view_as<bool>((iCondition > 255) || (iCondition < 0));
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
	public void SetPerformAvoidance(bool bPerformAvoidance)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bPerformAvoidance", bPerformAvoidance);
	}
	public bool GetPlayerAvoidState()
	{
		return !!GetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlayerAvoidState");
	}
	public void SetPlayerAvoidState(bool bPlayerAvoidState)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlayerAvoidState", bPlayerAvoidState);
	}
}

methodmap CProtoSniper < CAI_BaseNPC
{
	public CProtoSniper(const int iEntIndex = -1)
	{
		return view_as<CProtoSniper>(CBaseEntity(iEntIndex));
	}
	
	public bool IsSweepHighestPriority()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bSweepHighestPriority"));
	}
	public CBaseEntity GetSweepTarget()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hSweepTarget"));
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

methodmap CBasePlayer < CBaseCombatCharacter
{
	public CBasePlayer(const int iEntIndex = -1)
	{
		return view_as<CBasePlayer>(CBaseEntity(iEntIndex));
	}
	public bool IsValid()
	{
		return this.IsClassPlayer() && this.IsInGame();
	}
	public bool IsInGame()
	{
		int iEntIndex = this.GetEntIndex();
		return iEntIndex != -1 && IsClientInGame(iEntIndex);
	}
	public bool IsAlive()
	{
		return IsPlayerAlive(this.GetEntIndex());
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
		if (strcmp(szItemName, "item_suit", false) == 0)
		{
			this.SetSuit(true);
			return CBaseAnimating(-1);
		}
		
		int iNameOffset = (strncmp(szItemName, "item_weapon_", 12, false) == 0) ? 5 : 0;
		
		CBaseAnimating pItem = CBaseAnimating(GivePlayerItem(this.GetEntIndex(), szItemName[iNameOffset]));
		if(pItem.IsValid() && !pItem.GetOwner().IsValid())
		{
			// this would drop on the ground if player cant pick up any more
			pItem.KillAfterTime(0.1);
		}
		return pItem;
	}
	public CBaseCombatWeapon GiveWeapon(const char[] szWeaponName)
	{
		return view_as<CBaseCombatWeapon>(this.GiveItem(szWeaponName));
	}
	public void RemoveWeapon(CBaseCombatWeapon pWeapon)
	{
		RemovePlayerItem(this.GetEntIndex(), pWeapon.GetEntIndex());
	}
	public void RemoveAllItems(bool bRemoveSuit = true)
	{
		SDKCall(g_pRemoveAllItems, this.GetEntIndex(), bRemoveSuit);
	}
	public bool HasWeapon(const char[] szWeaponName)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid() && pWeapon.IsClassname(szWeaponName))
			{
				return true;
			}
		}
		return false;
	}
	public bool EquipWeapon(const char[] szWeaponName)	// change to CBaseEntity
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid())
			{
				if (pWeapon.IsClassname(szWeaponName))
				{
					SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon", pWeapon.GetEntIndex());
					return true;
				}
			}
		}
		
		return false;
	}
	public bool EquipPlayerWeapon(CBaseCombatWeapon pWeapon)
	{
		if (pWeapon.IsValid())
		{
			EquipPlayerWeapon(this.GetEntIndex(), pWeapon.GetEntIndex());
			return true;
		}
		
		// still make this possible to set -1
		EquipPlayerWeapon(this.GetEntIndex(), -1);
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
		
		return NULL_CBASEENTITY;
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
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hViewEntity"));
	}
	public void SetViewEntity(CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hViewEntity", (this == pEntity) ? -1 : pEntity.GetEntIndex());
		SetClientViewEntity(this.GetEntIndex(), pEntity.GetEntIndex());
	}
	public CPredictedViewModel GetViewModel(int iIndex = 0)
	{
		return CPredictedViewModel(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewModel", iIndex));
	}
	public void SetViewModel(CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewModel", pEntity.GetEntIndex());
	}
	public CBaseEntity GetRagdoll()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hRagdoll"));
	}
	public void SetRagdoll(CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hRagdoll", pEntity.GetEntIndex());
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
	public void SetObserverMode(Obs_Mode iMode)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iObserverMode", iMode);
	}
	public CBaseEntity GetObserverTarget()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hObserverTarget"));
	}
	public void SetObserverTarget(CBaseEntity pTarget)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hObserverTarget", pTarget.GetEntIndex());
	}
	public int GetTeam()
	{
		return GetClientTeam(this.GetEntIndex());
	}
	public CBaseEntity GetUseEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hUseEntity"));
	}
	public CBaseEntity GetCarriedObject()
	{
		CBaseEntity pUseEntity = this.GetUseEntity();
		if(pUseEntity.IsValid() && pUseEntity.IsClassname("player_pickup"))
		{
			return CBaseEntity(GetEntPropEnt(pUseEntity.GetEntIndex(), Prop_Data, "m_attachedEntity"));
		}
		return NULL_CBASEENTITY;
	}
	public void ForceDropOfCarriedPhysObjects()
	{
		this.AcceptInputStr("ForceDropPhysObjects");
	}
	public void PlayGameSound(char[] szSound)
	{
		ClientCommand(this.GetEntIndex(), "playgamesound %s", szSound);
	}
	public void PlaySound(char[] szSound)
	{
		EmitSoundToClient(this.GetEntIndex(), szSound);
	}
	/**
	 * Returns how many times the given player has died
	 * 
	 * @return				The number of deaths
	 */
	public int GetDeaths()
	{
		return GetClientDeaths(this.GetEntIndex());
	}
	/**
	 * Gets this player's score
	 *
	 * @return				The player's score
	 */
	public int GetScore()
	{
		return GetClientFrags(this.GetEntIndex());
	}
	/**
	 * Sets this player's score to the given value
	 *
	 * @param iScore		Value to set as the score for the player
	 */
	public void SetScore(int iScore)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iFrags", iScore);

		// Force server to actually recognize the score update
		CBaseEntity pTempGameScore = CBaseEntity(CreateEntityByName("game_score"));
		pTempGameScore.Spawn();
		pTempGameScore.AcceptInput("ApplyScore", this);
		pTempGameScore.Kill();
	}
	/**
	 * Modifies the player's score by the provided points
	 * 
	 * @param iPoints		The amount of points to add or subtract
	 * @return				The new score for the player
	 */
	public int ModifyScore(int iPoints)
	{
		int iNewScore = this.GetScore() + iPoints;
		this.SetScore(iNewScore);
		return iNewScore;
	}
	public void Suicide()
	{
		ForcePlayerSuicide(this.GetEntIndex());
	}
}

methodmap CSprite < CBaseEntity
{
	public CSprite(const int iEntIndex = -1)
	{
		return view_as<CSprite>(CBaseEntity(iEntIndex));
	}
	public static CSprite Create()
	{
		return CSprite(CreateEntityByName("env_sprite"));
	}

	public float GetScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale");
	}
	public void SetScale(float val)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale", val);
	}
	public float GetFramerate()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate");
	}
	public void SetFramerate(float val)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate", val);
	}
	public float GetFrame()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame");
	}
	public void SetFrame(float val)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame", val);
	}
	public float Frames()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flMaxFrame");
	}
	public float GetGlowProxySize()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize");
	}
	public void SetGlowProxySize(float val)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize", val);
	}
	public void SetAttachment(CBaseEntity pEntity, int attachment)
	{
		if (pEntity.IsValid())
		{
			SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hAttachedToEntity", pEntity.GetEntIndex());
			SetEntProp(this.GetEntIndex(), Prop_Data, "m_nAttachment", attachment);
			this.SetParent(pEntity);
		}
	}
	public void TurnOff()
	{
		this.AcceptInput("HideSprite");
	}
	public void TurnOn()
	{
		this.AcceptInput("ShowSprite");
	}
	public bool IsOn()
	{
		return !(this.GetEffects() & EF_NODRAW);
	}
}

methodmap CParticleSystem < CBaseEntity
{
	public CParticleSystem(const int iEntIndex = -1)
	{
		return view_as<CParticleSystem>(CBaseEntity(iEntIndex));
	}
	public static CParticleSystem Create(const char[] szEffectName, bool bStartActive = true)
	{
		CParticleSystem pParticle = CParticleSystem(CreateEntityByName("info_particle_system"));
		pParticle.SetEffectName(szEffectName);
		if (bStartActive)
		{
			pParticle.SetStartActive(bStartActive);
		}
		return pParticle;
	}
	
	public bool IsActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bActive"));
	}
	public bool GetStartActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive"));
	}
	public void SetStartActive(const bool value)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive", value);
	}
	public bool GetWeatherEffect()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect"));
	}
	public void SetWeatherEffect(const bool value)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect", value);
	}
	public bool GetOrientationFollowsViewer()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer"));
	}
	public void SetOrientationFollowsViewer(const bool value)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer", value);
	}
	public float GetStartTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime");
	}
	public void SetStartTime(float value)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime", value);
	}
	public bool GetEffectName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", szBuffer, iMaxLength));
	}
	public int SetEffectName(const char[] value)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", value); 
	}
	public void Start()
	{
		this.AcceptInput("Start");
	}
	public void Stop()
	{
		this.AcceptInput("Stop");
	}
	public void DestroyImmediately()
	{
		this.AcceptInput("DestroyImmediately");
	}
	public void Reset()
	{
		this.AcceptInput("Reset");
	}
}

methodmap CMultiplayRules
{
	public static bool IsTeamplay()
	{
		switch (GetEngineVersion()) {
			case Engine_BlackMesa:
				return GetTeamCount() >= 4;
			case Engine_HL2DM:
				return view_as<bool>(GameRules_GetProp("m_bTeamPlayEnabled"));
		}
		
	}
}

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
	
	public static int GetName(int globalIndex, const char[] szGlobalName, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetName, szGlobalName, iMaxLength, globalIndex);
	}

	public static bool IsValidIndex(int globalIndex)
	{
		return CGlobalState.GetName(globalIndex, NULL_STRING, 0) != -1;
	}
	
	public static int GetCounter(int globalIndex)
	{
		return SDKCall(g_pGlobalEntityGetCounter, globalIndex);
	}
	
	public static int GetMap(int globalIndex, const char[] szGlobalMap, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetMap, szGlobalMap, iMaxLength, globalIndex);
	}
	
	public static bool SetState(int globalIndex, GLOBALESTATE state)
	{
		if (CGlobalState.IsValidIndex(globalIndex))
		{
			SDKCall(g_pGlobalEntitySetState, globalIndex, state);
			return true;
		}
		
		return false;
	}
	
	public static bool SetStateByName(const char[] szGlobalName, GLOBALESTATE state)
	{
		int globalIndex = CGlobalState.GetIndex(szGlobalName);
		if (globalIndex != -1)
		{
			CGlobalState.SetState(globalIndex, state);
			return true;
		}
		
		return false;
	}
	
	public static bool SetCounter(int globalIndex, int iCounter)
	{
		if (CGlobalState.IsValidIndex(globalIndex))
		{
			SDKCall(g_pGlobalEntitySetCounter, globalIndex, iCounter);
			return true;
		}
		
		return false;
	}
	
	public static bool SetCounterByName(const char[] szGlobalName, int iCounter)
	{
		int globalIndex = CGlobalState.GetIndex(szGlobalName);
		if (globalIndex != -1)
		{
			CGlobalState.SetCounter(globalIndex, iCounter);
			return true;
		}
		
		return false;
	}
	
	// Returns index to global state
	public static int Add(const char[] szGlobalName, const char[] szMapName, GLOBALESTATE state)
	{
		return SDKCall(g_pGlobalEntityAdd, szGlobalName, szMapName, state);
	}
}

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

public bool Callback_GetAimTarget(int iEntity, int iMask, any pData)
{
	return !view_as<bool>(iEntity == pData);
}
