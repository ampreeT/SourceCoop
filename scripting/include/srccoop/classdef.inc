#if defined _srccoop_classdef_included
 #endinput
#endif
#define _srccoop_classdef_included

#include <sdktools>
#include <sourcescramble>

#pragma newdecls required
#pragma semicolon 1

IServerGameClients g_ServerGameClients;
CPlayerInfoManager g_pPlayerInfoManager;

ConVar ai_los_mode;

Handle g_pGlobalEntityGetIndex;
Handle g_pGlobalEntityGetState;
Handle g_pGlobalEntityGetName;
Handle g_pGlobalEntityGetCounter;
Handle g_pGlobalEntityGetMap;
Handle g_pGlobalEntitySetState;
Handle g_pGlobalEntitySetCounter;
Handle g_pGlobalEntityAdd;
Handle g_pGameShutdown;
Handle g_pGetTempEntsSystem;
Handle g_pSetMinMaxSize;
Handle g_pQueryHearSound;
Handle g_pGetSoundInterests;
Handle g_pHearingSensitivity;
Handle g_pUpdateEnemyMemory;
Handle g_pGetSequenceLinearMotion;
Handle g_pShouldPlayerAvoid;
Handle g_pRemoveAllItems;
Handle g_pCreateViewModel;
Handle g_pSetNextThink;
Handle g_pSetNextThinkNullString;
Handle g_pIsNPC;
Handle g_pSendWeaponAnim;
Handle g_pWeaponSwitch;
Handle g_pWorldSpaceCenter;
Handle g_pGetPlayerState;
#if defined SRCCOOP_BLACKMESA
Handle g_pSelectWeightedSequence;
#endif
Handle g_pSendViewModelMatchingSequence;
Handle g_pGetGlobalVars;
Handle g_pAppendCriteria;
Handle g_pSetSuitUpdate;
Handle g_pGetClusterForOrigin;
Handle g_pGetPVSForCluster;
Handle g_pCheckOriginInPVS;

int g_iCSoundSize = -1;
int g_iCPlayerState_frags = -1;

stock void InitClassdef(GameData pGameConfig)
{
	ai_los_mode = FindConVar("ai_los_mode");

	g_iCSoundSize = GetGamedataInt(pGameConfig, "CSoundSize");
	if (g_iCSoundSize <= 0)
		SetFailState("Could not obtain CSound struct size");

	g_iCPlayerState_frags = pGameConfig.GetOffset("CPlayerState::frags");
	if (g_iCPlayerState_frags < 0)
		SetFailState("Invalid offset CPlayerState::frags");

	if (!(g_ServerGameClients = view_as<IServerGameClients>(GetInterface(pGameConfig, "server", "IServerGameClients"))))
		SetFailState("Could not get interface for %s", "IServerGameClients");
	
	if (!(g_pPlayerInfoManager = view_as<CPlayerInfoManager>(GetInterface(pGameConfig, "server", "PlayerInfoManager"))))
		SetFailState("Could not get interface for %s", "PlayerInfoManager");
	
	char szSetMinMaxSize[] = "SetMinMaxSize";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szSetMinMaxSize))
		LogMessage("Could not obtain gamedata signature %s", szSetMinMaxSize);
	else
	{
		PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		if (!(g_pSetMinMaxSize = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetMinMaxSize);
	}

	char szSetNextThink[] = "CBaseEntity::SetNextThink";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szSetNextThink))
		LogMessage("Could not obtain gamedata signature %s", szSetNextThink);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
		if (!(g_pSetNextThink = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetNextThink);
	}

	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szSetNextThink))
		LogMessage("Could not obtain gamedata signature %s", szSetNextThink);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSetNextThinkNullString = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetNextThink);
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
	
	char szCreateViewModel[] = "CBasePlayer::CreateViewModel";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szCreateViewModel))
		LogMessage("Could not obtain gamedata offset %s", szCreateViewModel);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pCreateViewModel = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCreateViewModel);
	}

	char szSetSuitUpdate[] = "CBasePlayer::SetSuitUpdate";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szSetSuitUpdate))
		LogMessage("Could not obtain gamedata signature %s", szSetSuitUpdate);
	else
	{
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSetSuitUpdate = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetSuitUpdate);
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

	char szGetTempEntsSystem[] = "CServerTools::GetTempEntsSystem";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetTempEntsSystem))
		LogMessage("Could not obtain gamedata offset %s", szGetTempEntsSystem);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetTempEntsSystem = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetTempEntsSystem);
	}

	char szQueryHearSound[] = "CAI_BaseNPC::QueryHearSound";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szQueryHearSound))
		LogMessage("Could not obtain gamedata offset %s", szQueryHearSound);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		if (!(g_pQueryHearSound = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szQueryHearSound);
	}

	char szGetSoundInterests[] = "CAI_BaseNPC::GetSoundInterests";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetSoundInterests))
		LogMessage("Could not obtain gamedata offset %s", szGetSoundInterests);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetSoundInterests = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetSoundInterests);
	}

	char szHearingSensitivity[] = "CAI_BaseNPC::HearingSensitivity";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szHearingSensitivity))
		LogMessage("Could not obtain gamedata offset %s", szHearingSensitivity);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Float, SDKPass_Plain);
		if (!(g_pHearingSensitivity = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szHearingSensitivity);
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

	char szSendWeaponAnim[] = "CBaseCombatWeapon::SendWeaponAnim";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szSendWeaponAnim))
		LogMessage("Could not obtain gamedata offset %s", szSendWeaponAnim);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSendWeaponAnim = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendWeaponAnim);
	}

	char szWeaponSwitch[] = "CBaseCombatCharacter::Weapon_Switch";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szWeaponSwitch))
		LogMessage("Could not obtain gamedata offset %s", szWeaponSwitch);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pWeaponSwitch = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szWeaponSwitch);
	}

	char szWorldSpaceCenter[] = "CBaseEntity::WorldSpaceCenter";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szWorldSpaceCenter))
		LogMessage("Could not obtain gamedata offset %s", szWorldSpaceCenter);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Vector, SDKPass_ByRef);
		if (!(g_pWorldSpaceCenter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szWorldSpaceCenter);
	}

	char szGetPlayerState[] = "CServerGameClients::GetPlayerState";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetPlayerState))
		LogMessage("Could not obtain gamedata offset %s", szGetPlayerState);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Edict, SDKPass_Pointer);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetPlayerState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetPlayerState);
	}

	#if defined SRCCOOP_BLACKMESA
	// only virtual in BM
	char szSelectWeightedSequence[] = "CBaseAnimating::SelectWeightedSequence";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szSelectWeightedSequence))
		LogMessage("Could not obtain gamedata offset %s", szSelectWeightedSequence);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSelectWeightedSequence = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSelectWeightedSequence);
	}
	#endif

	char szSendViewModelMatchingSequence[] = "CBaseViewModel::SendViewModelMatchingSequence";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szSendViewModelMatchingSequence))
		LogMessage("Could not obtain gamedata offset %s", szSendViewModelMatchingSequence);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSendViewModelMatchingSequence = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendViewModelMatchingSequence);
	}

	char szGetGlobalVars[] = "CPlayerInfoManager::GetGlobalVars";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetGlobalVars))
		LogMessage("Could not obtain gamedata offset %s", szGetGlobalVars);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetGlobalVars = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetGlobalVars);
	}
	
	char szAppendCriteria[] = "AI_CriteriaSet::AppendCriteria";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szAppendCriteria))
		LogMessage("Could not obtain gamedata signature %s", szAppendCriteria);
	else
	{
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
		if (!(g_pAppendCriteria = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szAppendCriteria);
	}
	
	char szGetClusterForOrigin[] = "CVEngineServer::GetClusterForOrigin";
	StartPrepSDKCall(SDKCall_Engine);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetClusterForOrigin))
		LogMessage("Could not obtain gamedata offset %s", szGetClusterForOrigin);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetClusterForOrigin = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetClusterForOrigin);
	}
	
	char szGetPVSForCluster[] = "CVEngineServer::GetPVSForCluster";
	StartPrepSDKCall(SDKCall_Engine);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGetPVSForCluster))
		LogMessage("Could not obtain gamedata offset %s", szGetPVSForCluster);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetPVSForCluster = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetPVSForCluster);
	}
	
	char szCheckOriginInPVS[] = "CVEngineServer::CheckOriginInPVS";
	StartPrepSDKCall(SDKCall_Engine);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szCheckOriginInPVS))
		LogMessage("Could not obtain gamedata offset %s", szCheckOriginInPVS);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pCheckOriginInPVS = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCheckOriginInPVS);
	}

	#if defined SRCCOOP_BLACKMESA
	InitClassdefBM(pGameConfig);
	#endif
}

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

enum struct FireOutputData
{
	char m_szName[32];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
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

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetSkin()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nSkin");
	}
	public int SetSkin(const int iSkin)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nSkin", iSkin);
	}
	public float GetModelScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale");
	}
	public void SetModelScale(const float flModelScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale", flModelScale);
	}
	public int GetSequence()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence");
	}
	public void SetSequence(const int iSequence)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence", iSequence);
	}
	public void GetSequenceLinearMotion(const int iSequence, float vecBuffer[3])
	{
		SDKCall(g_pGetSequenceLinearMotion, this, iSequence, vecBuffer);
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
	public int LookupAttachment(const char[] szName)
	{
		return LookupEntityAttachment(this.GetEntIndex(), szName);
	}
	public bool GetAttachment(const int iAttachment, float vecOrigin[3], float vecAngles[3])
	{
		return GetEntityAttachment(this.GetEntIndex(), iAttachment, vecOrigin, vecAngles);
	}
	public int GetForceBone()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nForceBone");
	}
	public void SetForceBone(const int iForceBone)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nForceBone", iForceBone);
	}
	#if defined SRCCOOP_BLACKMESA
	public int SelectWeightedSequence(const int iActivity)
	{
		// TODO:
		// This is currently only implemented in Black Mesa but it may be required for others.
		// Reimplement the logic from scratch.
		return SDKCall(g_pSelectWeightedSequence, this.GetEntIndex(), iActivity);
	}
	#endif
	public float GetPoseParameter(const int iIndex)
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPoseParameter", iIndex);
	}
	public void SetPoseParameter(const int iIndex, const float flPoseParameter)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPoseParameter", flPoseParameter, iIndex);
	}
	public bool IsSequenceFinished()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bSequenceFinished") != 0;
	}
	public void SetSequenceFinished(const bool bFinished)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bSequenceFinished", bFinished);
	}
	public float GetAnimTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flAnimTime");
	}
	public void SetAnimTime(const float flAnimTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flAnimTime", flAnimTime);
	}
}

methodmap CBaseToggle < CBaseEntity
{
	public CBaseToggle(const int iEntIndex = -1)
	{
		return view_as<CBaseToggle>(CBaseEntity(iEntIndex));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}

methodmap CBaseTrigger < CBaseToggle
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(CBaseToggle(iEntIndex));
	}

	public bool IsEnabled()
	{
		return !view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled"));
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled", !bEnabled);
	}
}

methodmap CTriggerPush < CBaseTrigger
{
	public CTriggerPush(const int iEntIndex = -1)
	{
		return view_as<CTriggerPush>(CBaseTrigger(iEntIndex));
	}

	public float GetPushSpeed()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpeed");
	}
	public void SetPushSpeed(const float flSpeed)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpeed", flSpeed);
	}
	public void GetPushDirection(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPushDir", vecBuffer);
	}
	public void SetPushDirection(const float vec3Direction[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPushDir", vec3Direction);
	}
}

methodmap CChangelevel < CBaseTrigger
{
	public CChangelevel(const int iEntIndex = -1)
	{
		return view_as<CChangelevel>(CBaseTrigger(iEntIndex));
	}
	public static CChangelevel Create()
	{
		return CChangelevel(CreateEntityByName("trigger_changelevel"));
	}

	public void GetMapName(char[] szBuffer, const int iMaxLength)
	{
		GetEntPropString(this.GetEntIndex(), Prop_Data, "m_szMapName", szBuffer, iMaxLength);
	}
}

methodmap CTriggerHurt < CBaseTrigger
{
	public CTriggerHurt(const int iEntIndex = -1)
	{
		return view_as<CTriggerHurt>(CBaseTrigger(iEntIndex));
	}
	public static CTriggerHurt Create()
	{
		return CTriggerHurt(CreateEntityByName("trigger_hurt"));
	}

	public int GetDamageType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bitsDamageInflict");
	}
}

methodmap CPointEntity < CBaseEntity
{
	public CPointEntity(const int iEntIndex = -1)
	{
		return view_as<CPointEntity>(CBaseEntity(iEntIndex));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}

methodmap CWorld < CBaseEntity
{
	public CWorld()
	{
		return view_as<CWorld>(CBaseEntity(0));
	}

	public bool IsValid()
	{
		return true;
	}
	public bool GetChapterTitle(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszChapterTitle", szBuffer, iMaxLength) != 0;
	}
}

methodmap CMessage < CPointEntity
{
	public CMessage(const int iEntIndex = -1)
	{
		return view_as<CMessage>(CPointEntity(iEntIndex));
	}
	public static CMessage Create()
	{
		return CMessage(CreateEntityByName("env_message"));
	}

	public bool GetMessage(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszMessage", szBuffer, iMaxLength) != 0;
	}
	public void SetMessage(const char[] szMessage)
	{
		SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszMessage", szMessage);
	}
	public void ShowMessage()
	{
		this.AcceptInputStr("ShowMessage");
	}
}

methodmap CSceneEntity < CPointEntity
{
	public CSceneEntity(const int iEntIndex = -1)
	{
		return view_as<CSceneEntity>(CBaseEntity(iEntIndex));
	}
	public static CSceneEntity Create()
	{
		// Could also be a "scripted_scene`.
		return CSceneEntity(CreateEntityByName("logic_choreographed_scene"));
	}

	public float GetCurrentTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCurrentTime");
	}
	public void SetCurrentTime(const float flCurrentTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCurrentTime", flCurrentTime);
	}
	public float GetForceClientTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flForceClientTime");
	}
	public void SetForceClientTime(const float flForceClientTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flForceClientTime", flForceClientTime);
	}
}

methodmap CAI_ScriptedSequence < CBaseEntity
{
	public CAI_ScriptedSequence(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptedSequence>(CBaseEntity(iEntIndex));
	}
	public static CAI_ScriptedSequence Create()
	{
		return CAI_ScriptedSequence(CreateEntityByName("scripted_sequence"));
	}

	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt"));
	}
	public void SetTargetEnt(CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt", pTarget.entindex);
	}
}

methodmap CSoundEnt < CPointEntity
{
	public CSoundEnt(const int iEntIndex = -1)
	{
		return view_as<CSoundEnt>(CBaseEntity(iEntIndex));
	}
	public static CSoundEnt Get()
	{
		return CSoundEnt(FindEntityByClassname(-1, "soundent"));
	}

	// Gets the index of the first sound in the active sound list.
	public void ActiveList(CSound pSound)
	{
		pSound.m_pSoundEnt = this;
		pSound.m_iSoundIndex = GetEntProp(this.GetEntIndex(), Prop_Data, "m_iActiveSound");
	}
}

enum struct CSound
{
	CSoundEnt m_pSoundEnt;
	int m_iSoundIndex;

	CSoundEnt GetSoundEnt()
	{
		return this.m_pSoundEnt;
	}
	int GetSoundIndex()
	{
		return this.m_iSoundIndex;
	}
	bool IsValid()
	{
		return this.GetSoundEnt().IsValid() && this.m_iSoundIndex >= 0 && this.m_iSoundIndex < MAX_WORLD_SOUNDS_MP;
	}
	Address GetAddress()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		CSoundEnt pSoundEnt = this.GetSoundEnt();
		int iOffsetPoolStart = FindDataMapInfo(pSoundEnt.GetEntIndex(), "m_SoundPool");
		if (iOffsetPoolStart == -1)
			ThrowError("Unable to retrieve offset %s", "m_SoundPool");

		return pSoundEnt.GetAddress() + iOffsetPoolStart + this.GetSoundIndex() * g_iCSoundSize;
	}
	int GetType()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iType"), NumberType_Int32);
	}
	int GetVolume()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iVolume"), NumberType_Int32);
	}
	CBaseEntity GetOwner()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return CBaseEntity(LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_hOwner"), NumberType_Int32)  | (1 << 31));
	}
	void Next()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());

		this.m_iSoundIndex = LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iNext"), NumberType_Int16);
	}
	void GetOrigin(float vecBuffer[3])
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());

		Address pAddress = GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_vecOrigin");
		vecBuffer[0] = LoadFromAddress(pAddress + 0x0, NumberType_Int32);
		vecBuffer[1] = LoadFromAddress(pAddress + 0x4, NumberType_Int32);
		vecBuffer[2] = LoadFromAddress(pAddress + 0x8, NumberType_Int32);
	}
	void GetSoundReactOrigin(float vecBuffer[3])
	{
		int iType = this.GetType();
		if (iType == SOUND_BULLET_IMPACT || iType == SOUND_PHYSICS_DANGER)
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner.IsValid())
			{
				pOwner.GetAbsOrigin(vecBuffer);
			}
			else
			{
				this.GetOrigin(vecBuffer);
			}
			return;
		}

		if (iType & SOUND_CONTEXT_REACT_TO_SOURCE)
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner.IsValid())
			{
				pOwner.GetAbsOrigin(vecBuffer);
				return;
			}
		}

		if ((iType & SOUND_DANGER) && (iType & SOUND_CONTEXT_FROM_SNIPER))
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner.IsValid())
			{
				pOwner.GetAbsOrigin(vecBuffer);
				return;
			}
		}

		this.GetOrigin(vecBuffer);
	}
}

methodmap CPointHurt < CPointEntity
{
	public CPointHurt(const int iEntIndex = -1)
	{
		return view_as<CPointHurt>(CPointEntity(iEntIndex));
	}
	public static CPointHurt Create()
	{
		return CPointHurt(CreateEntityByName("point_hurt"));
	}
	
	public int GetDamageType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bitsDamageType");
	}
}

methodmap CLogicalEntity < CBaseEntity
{
	public CLogicalEntity(const int iEntIndex = -1)
	{
		return view_as<CLogicalEntity>(CBaseEntity(iEntIndex));
	}
	
	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}

methodmap CLogicAchievement < CLogicalEntity
{
	public CLogicAchievement(const int iEntIndex = -1)
	{
		return view_as<CLogicAchievement>(CLogicalEntity(iEntIndex));
	}
	public static CLogicAchievement Create()
	{
		return CLogicAchievement(CreateEntityByName("logic_achievement"));
	}

	public bool GetAchievementEventID(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszAchievementEventID", szBuffer, iMaxLength));
	}
}

methodmap CAI_ScriptConditions < CBaseEntity
{
	public CAI_ScriptConditions(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptConditions>(CBaseEntity(iEntIndex));
	}
	public static CAI_ScriptConditions Create()
	{
		return CAI_ScriptConditions(CreateEntityByName("ai_script_conditions"));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public bool IsEnabled()
	{
		return !view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled"));
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled", !bEnabled);
	}
	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hTarget"));
	}
	public void SetTargetEnt(CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hTarget", pTarget.entindex);
	}
}

methodmap CBaseCombatWeapon < CBaseAnimating
{
	public CBaseCombatWeapon(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatWeapon>(CBaseAnimating(iEntIndex));
	}
	
	public CBaseCombatCharacter GetOwner()
	{
		return CBaseCombatCharacter(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hOwner"));
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
	public bool CanFireUnderwater()
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
	public int GetWeaponModelIndex(const bool bViewModel)
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, bViewModel ? "m_iViewModelIndex" : "m_iWorldModelIndex");
	}
	public void SetWeaponModelIndex(const int iViewModelIndex, const bool bViewModel)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, bViewModel ? "m_iViewModelIndex" : "m_iWorldModelIndex", iViewModelIndex);
	}
	public float GetWeaponTimeIdle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle");
	}
	public void SetWeaponTimeIdle(const float flTimeIdle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flTimeWeaponIdle", flTimeIdle);
	}
	public bool SendWeaponAnim(const int iActivity)
	{
		return SDKCall(g_pSendWeaponAnim, this.GetEntIndex(), iActivity);
	}
	public bool IsReloading()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bInReload") != 0;
	} 
}

methodmap CBaseViewModel < CBaseAnimating
{
	public CBaseViewModel(const int iEntIndex = -1)
	{
		return view_as<CBaseViewModel>(CBaseAnimating(iEntIndex));
	}

	public void SendViewModelMatchingSequence(const int iSequence)
	{
		SDKCall(g_pSendViewModelMatchingSequence, this.GetEntIndex(), iSequence);
	}
}

methodmap CPredictedViewModel < CBaseViewModel
{
	public CPredictedViewModel(const int iEntIndex = -1)
	{
		return view_as<CPredictedViewModel>(CBaseViewModel(iEntIndex));
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

methodmap CBaseProp < CBaseAnimating
{
	public CBaseProp(const int iEntIndex = -1)
	{
		return view_as<CBaseProp>(CBaseAnimating(iEntIndex));
	}
}

methodmap CBreakableProp < CBaseProp
{
	public CBreakableProp(const int iEntIndex = -1)
	{
		return view_as<CBreakableProp>(CBaseProp(iEntIndex));
	}
}

methodmap CDynamicProp < CBreakableProp
{
	public CDynamicProp(const int iEntIndex = -1)
	{
		return view_as<CDynamicProp>(CBreakableProp(iEntIndex));
	}
	public static CDynamicProp Create(const char[] szModelName)
	{
		CDynamicProp pDynamicProp = CDynamicProp(CreateEntityByName("prop_dynamic"));
		if (pDynamicProp.IsValid())
		{
			int iModelIndex = PrecacheModel(szModelName);
			if (iModelIndex)
			{
				pDynamicProp.SetModel(szModelName);
				pDynamicProp.SetModelIndex(iModelIndex);
				pDynamicProp.Activate();
				pDynamicProp.Spawn();
			}
		}
		return pDynamicProp;
	}
	public void SetAnimation(const char[] szSequenceName)
	{
		this.AcceptInputStr("SetAnimation", szSequenceName);
	}
}

methodmap CPropVehicle < CBaseProp
{
	public CPropVehicle(const int iEntIndex = -1)
	{
		return view_as<CPropVehicle>(CBaseProp(iEntIndex));
	}
}

methodmap CPropVehicleDriveable < CBaseAnimating
{
	public CPropVehicleDriveable(const int iEntIndex = -1)
	{
		return view_as<CPropVehicleDriveable>(CBaseAnimating(iEntIndex));
	}
}

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public CBaseAnimatingOverlay(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimatingOverlay>(CBaseAnimating(iEntIndex));
	}
} 

methodmap CBaseFlex < CBaseAnimatingOverlay
{
	public CBaseFlex(const int iEntIndex = -1)
	{
		return view_as<CBaseFlex>(CBaseAnimatingOverlay(iEntIndex));
	}
}

methodmap CBaseCombatCharacter < CBaseFlex
{
	public CBaseCombatCharacter(const int iEntIndex = -1)
	{
		return view_as<CBaseCombatCharacter>(CBaseFlex(iEntIndex));
	}

	public int GetLifeState()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_lifeState");
	}
	public bool IsAlive()
	{
		// Any value above 0 indicates dead or other death information
		return !this.GetLifeState();
	}
	public CBaseCombatWeapon GetActiveWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon"));
	}
	public void SetActiveWeapon(CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hActiveWeapon", pWeapon.GetEntIndex());
	}
	public bool HasWeapon(const char[] szWeaponName)
	{
		return this.GetWeapon(szWeaponName) != NULL_CBASEENTITY;
	}
	public CBaseCombatWeapon GetWeapon(const char[] szWeaponName, const int iStartIndex = 0)
	{
		for (int i = iStartIndex; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid() && pWeapon.IsClassname(szWeaponName))
			{
				return pWeapon;
			}
		}
		return NULL_CBASEENTITY;
	}
	public bool WeaponSwitch(CBaseCombatWeapon pWeapon, const int iViewmodelindex = 0)
	{
		return SDKCall(g_pWeaponSwitch, this.GetEntIndex(), pWeapon.GetEntIndex(), iViewmodelindex);
	}
	// Deprecated, WeaponSwitch() prefered instead
	public bool SetActiveWeaponByClass(const char[] szWeaponName)
	{
		for (int i = 0; i < MAX_WEAPONS; i++)
		{
			CBaseCombatWeapon pWeapon = this.GetWeaponFromIndex(i);
			if (pWeapon.IsValid() && pWeapon.IsClassname(szWeaponName))
			{
				this.SetActiveWeapon(pWeapon);
				return true;
			}
		}
		return false;
	}
	public CBaseCombatWeapon GetLastWeapon()
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLastWeapon"));
	}
	public void SetLastWeapon(CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLastWeapon", pWeapon.GetEntIndex());
	}
	public CBaseCombatWeapon GetWeaponFromIndex(const int iIndex)
	{
		return CBaseCombatWeapon(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hMyWeapons", iIndex));
	}
	public void SetWeaponAtIndex(const int iIndex, CBaseCombatWeapon pWeapon)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hMyWeapons", pWeapon.GetEntIndex(), iIndex);
	}
	public int GetAmmoFromIndex(const int iIndex)
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", _, iIndex);
	}
	public void SetAmmoFromIndex(const int iIndex, const int iAmmoCount)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iAmmo", iAmmoCount, _, iIndex);
	}
	public float GetNextAttack()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flNextAttack");
	}
	public void SetNextAttack(const float flNextAttack)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flNextAttack", flNextAttack);
	}	
}

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

methodmap CProtoSniper < CAI_BaseNPC
{
	public CProtoSniper(const int iEntIndex = -1)
	{
		return view_as<CProtoSniper>(CAI_BaseNPC(iEntIndex));
	}
	public static CProtoSniper Create()
	{
		return CProtoSniper(CreateEntityByName("npc_sniper"));
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
		return view_as<CBasePlayer>(CBaseCombatCharacter(iEntIndex));
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
	public void SetName(const char[] szPlayerName)
	{
		SetClientName(this.GetEntIndex(), szPlayerName);
	}
	public int GetUserId()
	{
		return GetClientUserId(this.GetEntIndex());
	}
	public CPropVehicleDriveable GetVehicle()
	{
		return CPropVehicleDriveable(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hVehicle"));
	}
	public int GetArmor()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_ArmorValue", 1);
	}
	public void SetArmor(const int iArmor)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_ArmorValue", iArmor, 1);
	}
	public bool HasSuit()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bWearingSuit"));
	}
	public void SetSuit(const bool bShouldHaveSuit)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bWearingSuit", bShouldHaveSuit);
	}
	public void SetSuitUpdate(const char[] szName, int fgroup, int iNoRepeat)
	{
		SDKCall(g_pSetSuitUpdate, this.entindex, szName, fgroup, iNoRepeat);
	}
	public void ClearSuitQueue()
	{
		for (int i = 0; i < 4; i++)
		{
			SetEntProp(this.entindex, Prop_Data, "m_rgSuitPlayList", 0, 4, i);
		}
		for (int i = 0; i < 32; i++)
		{
			SetEntProp(this.entindex, Prop_Data, "m_rgiSuitNoRepeat", 0, 4, i);
			SetEntPropFloat(this.entindex, Prop_Data, "m_rgflSuitNoRepeatTime", 0.0, i);
		}
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
	public void Kick()
	{
		KickClient(this.GetEntIndex());
	}
	public CBaseAnimating GiveItem(const char[] szItemName)
	{
		if (strcmp(szItemName, "item_suit", false) == 0)
		{
			this.SetSuit(true);
			return NULL_CBASEENTITY;
		}
		
		CBaseAnimating pItem = CBaseAnimating(GivePlayerItem(this.GetEntIndex(), szItemName));
		if (pItem.IsValid() && !pItem.GetOwner().IsValid())
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
	public void RemoveAllItems(const bool bRemoveSuit = true)
	{
		SDKCall(g_pRemoveAllItems, this.GetEntIndex(), bRemoveSuit);
	}
	public bool EquipWeapon(CBaseCombatWeapon pWeapon)
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
	public int GetLastButtons()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_afButtonLast");
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
		this.AddFlags(FL_DUCKING);
		int iClient = this.GetEntIndex();
		SetEntProp(iClient, Prop_Send, "m_bDucked", true);
		SetEntProp(iClient, Prop_Send, "m_bDucking", true);
		SetEntPropFloat(iClient, Prop_Send, "m_flDucktime", 0.0);
		float vecViewOffset[3] = VEC_DUCK_VIEW;
		ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX));
	}
	public void StopDucking()
	{
		this.SetButtons(this.GetButtons() & ~IN_DUCK);
		this.RemoveFlags(FL_DUCKING);
		int iClient = this.GetEntIndex();
		SetEntProp(iClient, Prop_Send, "m_bDucked", false);
		SetEntProp(iClient, Prop_Send, "m_bDucking", false);
		float vecViewOffset[3] = VEC_VIEW;
		ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX));
	}
	public void GetEyeAngles(float vecBuffer[3])
	{
		GetClientEyeAngles(this.GetEntIndex(), vecBuffer);
	}
	public CBaseEntity GetAimTarget()
	{
		float vecEyePos[3], vecEyeAngles[3];
		this.GetEyePosition(vecEyePos);
		this.GetEyeAngles(vecEyeAngles);
		
		CTraceRay pTraceRay = new CTraceRay(vecEyePos, vecEyeAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilter_IgnoreData, this.entindex);
		CBaseEntity pEntity = pTraceRay.GetEntity();
		pTraceRay.Close();
		return pEntity;
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
	public CPredictedViewModel GetViewModel(const int iIndex = 0)
	{
		return CPredictedViewModel(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewModel", iIndex));
	}
	public void SetViewModel(CBaseEntity pEntity, const int iIndex = 0)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hViewModel", pEntity.GetEntIndex(), iIndex);
	}
	public CPredictedViewModel CreateViewModel(const int iIndex = 0)
	{
		SDKCall(g_pCreateViewModel, this.entindex, iIndex);
		return this.GetViewModel(iIndex);
	}
	public CBaseAnimating GetRagdoll()
	{
		return CBaseAnimating(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hRagdoll"));
	}
	public void SetRagdoll(CBaseAnimating pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hRagdoll", pEntity.GetEntIndex());
	}
	public float GetDeathTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flDeathTime");
	}
	public void SetDeathTime(const float flDeathTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flDeathTime", flDeathTime);
	}
	public Obs_Mode GetObserverMode()
	{
		return view_as<Obs_Mode>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_iObserverMode"));
	}
	public void SetObserverMode(const Obs_Mode iMode)
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
	public int GetHideHud()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iHideHUD");
	}
	public void SetHideHud(const int iHideHud)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iHideHUD", iHideHud);
	}
	public void ShowCrosshair(const bool bShow)
	{
		int iHideHUD = this.GetHideHud();
		if (bShow)
		{
			this.SetHideHud(iHideHUD & ~HIDEHUD_CROSSHAIR);
		}
		else
		{
			this.SetHideHud(iHideHUD | HIDEHUD_CROSSHAIR);
		}
	}
	public void SetTeam(const int iTeamIndex)
	{
		ChangeClientTeam(this.GetEntIndex(), iTeamIndex);
	}
	public CBaseEntity GetUseEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hUseEntity"));
	}
	public CBaseEntity GetCarriedObject()
	{
		// TODO: HL2DM Needs to call GetPlayerHeldEntity !
		#if defined SRCCOOP_BLACKMESA
		CBaseEntity pUseEntity = this.GetUseEntity();
		if (pUseEntity.IsValid() && pUseEntity.IsClassname("player_pickup"))
		{
			return CBaseEntity(GetEntPropEnt(pUseEntity.GetEntIndex(), Prop_Data, "m_attachedEntity"));
		}
		#endif
		return NULL_CBASEENTITY;
	}
	public bool IsCarryingObject()
	{
		CBaseEntity pUseEntity = this.GetUseEntity();
		return (pUseEntity.IsValid() && pUseEntity.IsClassname("player_pickup"));
	}
	public void ForceDropOfCarriedPhysObjects()
	{
		this.AcceptInputStr("ForceDropPhysObjects");
	}
	public void PlayGameSound(const char[] szSound)
	{
		ClientCommand(this.GetEntIndex(), "playgamesound %s", szSound);
	}
	public void PlaySound(const char[] szSound)
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
	public void SetScore(const int iScore)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iFrags", iScore);
		IServerGameClients().GetPlayerState(this).SetScore(iScore);
	}
	/**
	 * Modifies the player's score by the provided points
	 * 
	 * @param iPoints		The amount of points to add or subtract
	 * @return				The new score for the player
	 */
	public int ModifyScore(const int iPoints)
	{
		int iNewScore = this.GetScore() + iPoints;
		this.SetScore(iNewScore);
		return iNewScore;
	}
	public void Suicide()
	{
		ForcePlayerSuicide(this.GetEntIndex());
	}
	public CFuncLadder GetLadder()
	{
		return CFuncLadder(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLadder"));
	}
	public void SetLadder(CFuncLadder pLadder)
	{
		if (this.GetLadder().IsValid())
		{
			this.GetLadder().FireOutput("OnPlayerGotOffLadder", this.GetEntIndex());
		}
		SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLadder", pLadder.entindex);
	}
	public int GetDefaultFOV()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iDefaultFOV");
	}
	public int GetFOV()
	{
		int iFOV = GetEntProp(this.GetEntIndex(), Prop_Data, "m_iFOV");
		iFOV = iFOV == 0 ? this.GetDefaultFOV() : iFOV;

		// TODO:
		// Implement vehichle FOV logic https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/server/player.cpp#L8395

		float flFOVRate = GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flFOVRate");
		if (flFOVRate == 0.0)
			return iFOV;
		
		float flDeltaTime = (GetGameTime() - GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flFOVTime")) / flFOVRate;
		if (flDeltaTime >= 1.0)
		{
			SetEntProp(this.GetEntIndex(), Prop_Data, "m_iFOVStart", iFOV);
			return iFOV;
		}
		else
		{
			float flFOVStart = float(GetEntProp(this.GetEntIndex(), Prop_Data, "m_iFOVStart"));
			return RoundFloat(SimpleSplineRemapValClamped(flDeltaTime, 0.0, 1.0, flFOVStart, float(iFOV)));
		}
	}
	public bool SetFOV(CBaseEntity pRequester, const int iFOV, const float flZoomRate, const int iZoomStart = 0)
	{
		if (!pRequester.IsValid())
			return false;
		
		CBaseEntity pZoomOwner = CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hZoomOwner"));
		// If we already have an owner, we only allow requests from that owner
		if (pZoomOwner.IsValid() && pZoomOwner != pRequester)
		{
			if (!CanOverrideEnvZoomOwner(pZoomOwner))
				return false;
		}
		else
		{
			if (iFOV == 0)
			{
				pZoomOwner = NULL_CBASEENTITY;
			}
			else
			{
				pZoomOwner = pRequester;
			}
			SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hZoomOwner", pZoomOwner.GetEntIndex());
		}

		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iFOVStart", iZoomStart > 0 ? iZoomStart : this.GetFOV());
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flFOVTime", GetGameTime());
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_iFOV", iFOV);
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flFOVRate", flZoomRate);
		return true;
	}
	public QueryCookie QueryConvar(const char[] szCvar, ConVarQueryFinished fnCallback, any value = 0)
	{
		return QueryClientConVar(this.GetEntIndex(), szCvar, fnCallback, value);
	}
	public bool IsDrawingViewModel()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_bDrawViewmodel") != 0;
	}
	public void DrawViewModel(const bool bDrawViewmodel)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bDrawViewmodel", bDrawViewmodel);
	}
	public void StartAdmireGlovesAnimation()
	{
		// TODO:
		// Create HL2DM implementation.
		#if defined SRCCOOP_BLACKMESA
		static const char g_szModelNameProxy[] = "models/weapons/v_357.mdl";
		static const char g_szModelNameGloves[] = "models/weapons/v_hands.mdl";

		if (this.GetActiveWeapon().IsValid())
			return;

		CPredictedViewModel pViewModel = this.GetViewModel(0);
		if (pViewModel.IsValid())
			pViewModel = this.CreateViewModel(0);
		
		if (!pViewModel.IsValid())
			return;

		// We use a model as a proxy here so that the `SendViewModelMatchingSequence` will
		// send the animation to the gloves. This is done by setting the model then replacing the model index.
		int iModelIndexProxy = PrecacheModel(g_szModelNameProxy);
		int iModelIndex = PrecacheModel(g_szModelNameGloves);
		if (iModelIndexProxy && iModelIndex)
		{
			// TODO:
			// For HL2DM, `ACT_VM_IDLE` might be used instead.
			this.DrawViewModel(true);
			pViewModel.RemoveEffects(EF_NODRAW);
			pViewModel.SetModel(g_szModelNameProxy);
			pViewModel.SetModelIndex(iModelIndex);
			pViewModel.SendViewModelMatchingSequence(pViewModel.SelectWeightedSequence(ACT_VM_DRAW));
		}
		#endif
	}
	public float GetLaggedMovement()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flLaggedMovementValue");
	}
	public void SetLaggedMovement(const float flValue)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flLaggedMovementValue", flValue);
	}
}

methodmap CBeam < CBaseEntity
{
	public CBeam(const int iEntIndex = -1)
	{
		return view_as<CBeam>(CBaseEntity(iEntIndex));
	}
	public static CBeam Create()
	{
		return CBeam(CreateEntityByName("beam"));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetAttachmentCount()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nNumBeamEnts");
	}
	public void SetAttachmentCount(const int iAttachmentCount)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nNumBeamEnts", iAttachmentCount);
	}
	public int GetBeamType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nBeamType");
	}
	public int SetBeamType(const int iBeamType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nBeamType", iBeamType);
	}
	public CBaseEntity GetAttachment(const int iIndex)
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hAttachEntity", iIndex));
	}
	public void SetAttachment(const int iIndex, CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hAttachEntity", pEntity.GetEntIndex(), iIndex);
	}
}

methodmap CEnvBeam < CBeam
{
	public CEnvBeam(const int iEntIndex = -1)
	{
		return view_as<CEnvBeam>(CBaseEntity(iEntIndex));
	}
	public static CEnvBeam Create()
	{
		return CEnvBeam(CreateEntityByName("env_beam"));
	}
	
	public float GetDuration()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_life");
	}
	public void SetDuration(const float flDuration)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_life", flDuration);
	}
	public bool GetStartEntity(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszStartEntity", szBuffer, iMaxLength)); 
	}
	public int SetStartEntity(const char[] szTargetname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszStartEntity", szTargetname); 
	}
	public float GetWidth()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_boltWidth");
	}
	public void SetWidth(const float flWidth)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_boltWidth", flWidth);
	}
	public bool GetTextureName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszSpriteName", szBuffer, iMaxLength));
	}
	public int SetTextureName(const char[] szTexturePath)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszSpriteName", szTexturePath); 
	}
	public float GetNoiseAmplitude()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_noiseAmplitude");
	}
	public void SetNoiseAmplitude(const float flAmplitude)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_noiseAmplitude", flAmplitude);
	}
	public int GetTextureScrollRate()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_speed");
	}
	public void SetTextureScrollRate(const int iTextureScrollRate)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_speed", iTextureScrollRate);
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

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public float GetScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale");
	}
	public void SetScale(const float flScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale", flScale);
	}
	public int GetBrightness()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nBrightness");
	}
	public void SetBrigtness(const int iBrightness)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nBrightness", iBrightness);
	}
	public float GetFrameRate()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate");
	}
	public void SetFrameRate(const float flFrameRate)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate", flFrameRate);
	}
	public float GetFrame()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame");
	}
	public void SetFrame(const float flFrame)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame", flFrame);
	}
	public float Frames()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flMaxFrame");
	}
	public float GetGlowProxySize()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize");
	}
	public void SetGlowProxySize(const float flGlowProxySize)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize", flGlowProxySize);
	}
	public void SetAttachment(CBaseEntity pEntity, const int iAttachment)
	{
		if (pEntity.IsValid())
		{
			SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hAttachedToEntity", pEntity.GetEntIndex());
			SetEntProp(this.GetEntIndex(), Prop_Data, "m_nAttachment", iAttachment);
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

methodmap CSpriteOriented < CSprite
{
	public CSpriteOriented(const int iEntIndex = -1)
	{
		return view_as<CSpriteOriented>(CSprite(iEntIndex));
	}
	public static CSpriteOriented Create()
	{
		return CSpriteOriented(CreateEntityByName("env_sprite_oriented"));
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
		if (pParticle.IsValid())
		{
			pParticle.SetEffectName(szEffectName);
			if (bStartActive)
			{
				pParticle.SetStartActive(bStartActive);
			}
		}
		return pParticle;
	}
	
	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public bool IsActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bActive"));
	}
	public bool GetStartActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive"));
	}
	public void SetStartActive(const bool bStartsActive)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive", bStartsActive);
	}
	public bool IsWeatherEffect()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect"));
	}
	public void SetWeatherEffect(const bool bWeatherEffect)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect", bWeatherEffect);
	}
	public bool GetOrientationFollowsViewer()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer"));
	}
	public void SetOrientationFollowsViewer(const bool bOrientationFollowsViewer)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer", bOrientationFollowsViewer);
	}
	public float GetStartTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime");
	}
	public void SetStartTime(const float flStartTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime", flStartTime);
	}
	public bool GetEffectName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", szBuffer, iMaxLength));
	}
	public int SetEffectName(const char[] szEffectName)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", szEffectName); 
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

methodmap CTeam < CBaseEntity
{
	public CTeam(const int iEntIndex = -1)
	{
		return view_as<CTeam>(CBaseEntity(iEntIndex));
	}
	public static CTeam Get(const int iTeamNum)
	{
		int iEntIndex = -1;
		while ((iEntIndex = FindEntityByClassname(iEntIndex, "team_manager")) != -1)
		{
			CTeam pTeam = CTeam(iEntIndex);
			if (iTeamNum == pTeam.GetTeam())
			{
				return pTeam;
			}
		}
		return NULL_CBASEENTITY;
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetTeam()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iTeamNum");
	}
	public int GetScore()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iScore");
	}
	public void SetScore(const int iScore)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iScore", iScore);
	}
}

methodmap CPlayerResource < CBaseEntity
{
	public CPlayerResource(const int iEntIndex = -1)
	{
		return view_as<CPlayerResource>(CBaseEntity(iEntIndex));
	}
	public static CPlayerResource Get()
	{
		return CPlayerResource(GetPlayerResourceEntity());
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetPing(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iPing", _, pPlayer.entindex);
	}
	public void SetPing(CBasePlayer pPlayer, int iPing)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iPing", iPing, _, pPlayer.entindex);
	}
	public int GetScore(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iScore", _, pPlayer.entindex);
	}
	public void SetScore(CBasePlayer pPlayer, int iScore)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iScore", iScore, _, pPlayer.entindex);
	}
	public int GetDeaths(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iDeaths", _, pPlayer.entindex);
	}
	public void SetDeaths(CBasePlayer pPlayer, int iDeaths)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iDeaths", iDeaths, _, pPlayer.entindex);
	}
	public bool IsConnected(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bConnected", _, pPlayer.entindex));
	}
	public void SetConnected(CBasePlayer pPlayer, bool bConnected)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bConnected", bConnected, _, pPlayer.entindex);
	}
	public int GetTeam(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iTeam", _, pPlayer.entindex);
	}
	public void SetTeam(CBasePlayer pPlayer, int iTeam)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iTeam", iTeam, _, pPlayer.entindex);
	}
	public bool IsAlive(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bAlive", _, pPlayer.entindex));
	}
	public void SetAlive(CBasePlayer pPlayer, bool bAlive)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bAlive", bAlive, _, pPlayer.entindex);
	}
	public int GetHealth(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iHealth", _, pPlayer.entindex);
	}
	public void SetHealth(CBasePlayer pPlayer, int iHealth)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iHealth", iHealth, _, pPlayer.entindex);
	}

	#if defined SRCCOOP_BLACKMESA
	public int GetAccountId(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iAccountID", _, pPlayer.entindex);
	}
	public void SetAccountId(CBasePlayer pPlayer, int iAccountID)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iAccountID", iAccountID, _, pPlayer.entindex);
	}
	public bool IsValidPlayer(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bValid", _, pPlayer.entindex));
	}
	public void SetValidPlayer(CBasePlayer pPlayer, bool bValid)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bValid", bValid, _, pPlayer.entindex);
	}
	public int GetFrags(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iFrags", _, pPlayer.entindex);
	}
	public void SetFrags(CBasePlayer pPlayer, int iFrags)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iFrags", iFrags, _, pPlayer.entindex);
	}
	#endif // SRCCOOP_BLACKMESA
}

methodmap CFuncLadder < CBaseEntity
{
	public CFuncLadder(const int iEntIndex = -1)
	{
		return view_as<CFuncLadder>(CBaseEntity(iEntIndex));
	}
	public static CFuncLadder Create()
	{
		return CFuncLadder(CreateEntityByName("func_ladder"));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public bool IsEnabled()
	{
		return !GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled");
	}
	public void GetTopPosition(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPlayerMountPositionTop", vecBuffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent.IsValid())
		{
			// Technically this should be using m_rgflCoordinateFrame which is a const matrix3x4_t, but if we just add AbsOrigin, it *should* be accurate enough...
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		}
	}
	public void GetBottomPosition(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPlayerMountPositionBottom", vecBuffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent.IsValid())
		{
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		}
	}
}

methodmap CMultiplayRules
{
	public static bool IsTeamplay()
	{
		switch (GetEngineVersion())
		{
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
	public static GLOBALESTATE GetStateByIndex(const int iGlobalIndex)
	{
		return SDKCall(g_pGlobalEntityGetState, iGlobalIndex);
	}
	public static GLOBALESTATE GetState(const char[] szGlobalname)
	{
		return CGlobalState.GetStateByIndex(CGlobalState.GetIndex(szGlobalname));
	}
	public static int GetName(const int iGlobalIndex, const char[] szGlobalName, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetName, szGlobalName, iMaxLength, iGlobalIndex);
	}
	public static bool IsValidIndex(const int iGlobalIndex)
	{
		return CGlobalState.GetName(iGlobalIndex, NULL_STRING, 0) != -1;
	}
	public static int GetCounter(const int iGlobalIndex)
	{
		return SDKCall(g_pGlobalEntityGetCounter, iGlobalIndex);
	}
	public static int GetMap(const int iGlobalIndex, const char[] szGlobalMap, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetMap, szGlobalMap, iMaxLength, iGlobalIndex);
	}
	public static bool SetState(const int iGlobalIndex, GLOBALESTATE state)
	{
		if (CGlobalState.IsValidIndex(iGlobalIndex))
		{
			SDKCall(g_pGlobalEntitySetState, iGlobalIndex, state);
			return true;
		}
		
		return false;
	}
	public static bool SetStateByName(const char[] szGlobalName, const GLOBALESTATE state)
	{
		int iGlobalIndex = CGlobalState.GetIndex(szGlobalName);
		if (iGlobalIndex != -1)
		{
			CGlobalState.SetState(iGlobalIndex, state);
			return true;
		}
		
		return false;
	}
	public static bool SetCounter(const int iGlobalIndex, const int iCounter)
	{
		if (CGlobalState.IsValidIndex(iGlobalIndex))
		{
			SDKCall(g_pGlobalEntitySetCounter, iGlobalIndex, iCounter);
			return true;
		}
		
		return false;
	}
	public static bool SetCounterByName(const char[] szGlobalName, const int iCounter)
	{
		int iGlobalIndex = CGlobalState.GetIndex(szGlobalName);
		if (iGlobalIndex != -1)
		{
			CGlobalState.SetCounter(iGlobalIndex, iCounter);
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

methodmap IServerTools
{
	public IServerTools(Address pAddress)
	{
		return view_as<IServerTools>(pAddress);
	}
	
	public Address GetTempEntsSystem()
	{
		return SDKCall(g_pGetTempEntsSystem, this);
	}
}

/**
 * The player state is an embedded class inside of player that is shared with the engine.
 * The Engine uses some of this data for responding to external server queries.
 */
methodmap CPlayerState
{
	public int GetScore()
	{
		return LoadFromAddress(view_as<Address>(this) + g_iCPlayerState_frags, NumberType_Int32);
	}
	public void SetScore(int iScore)
	{
		StoreToAddress(view_as<Address>(this) + g_iCPlayerState_frags, iScore, NumberType_Int32);
	}
}

methodmap IServerGameClients
{
	public IServerGameClients()
	{
		return g_ServerGameClients;
	}
	
	public CPlayerState GetPlayerState(CBasePlayer pPlayer)
	{
		return SDKCall(g_pGetPlayerState, this, pPlayer.entindex);
	}
}

methodmap CTraceRay < Handle
{
	public CTraceRay(const float vecPosition[3], const float vecRayType[3], const int iTraceMask, const RayType eRayType, const TraceEntityFilter fnFilter, const any pData)
	{
		return view_as<CTraceRay>(TR_TraceRayFilterEx(vecPosition, vecRayType, iTraceMask, eRayType, fnFilter, pData));
	}

	public bool StartedSolid()
	{
		return TR_StartSolid(this);
	}
	public float GetFraction()
	{
		return TR_GetFraction(this);
	}
	public CBaseEntity GetEntity()
	{
		return CBaseEntity(TR_GetEntityIndex(this));
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

public bool TraceEntityFilter_IgnoreData(int iEntIndex, int iMask, any pData)
{
	return (iEntIndex != pData);
}

public bool TraceEntityFilter_IgnorePlayers(int iEntIndex, int iMask, any pData)
{
	CBasePlayer pPlayer = CBasePlayer(iEntIndex);
	if (pPlayer.IsValid())
		return false;
	return (iEntIndex != pData);
}

// This function grabs the member offset within a CSound element from the base of the sound entity.
//
Address GetSoundPoolPropAddress(CSoundEnt pSoundEnt, const int iSoundIndex, const char[] szProperty)
{
	// Obtain the start offset `CSoundEnt::m_SoundPool`
	int iOffsetPoolStart = FindDataMapInfo(pSoundEnt.GetEntIndex(), "m_SoundPool");
	if (iOffsetPoolStart == -1)
		ThrowError("Unable to retrieve offset %s", "m_SoundPool");

	// obtain the offset within `CSound`
	int iOffsetFromStartProperty = FindDataMapInfo(pSoundEnt.GetEntIndex(), szProperty);
	if (iOffsetFromStartProperty == -1)
		ThrowError("Unable to retrieve offset %s", szProperty);
	
	int iOffset = iOffsetFromStartProperty - iOffsetPoolStart;
	if (iOffset < 0)
		ThrowError("Internal Error: Computed offset %s is negative (%i); %s offset: %i, SoundPool offset: %i", szProperty, iOffset, szProperty, iOffsetFromStartProperty, iOffsetPoolStart);
	
	// this + offsetof(m_SoundPool[iSoundIndex]) + offsetof(property)
	return pSoundEnt.GetAddress() + iOffsetPoolStart + iSoundIndex * g_iCSoundSize + iOffset;
}

stock CUtlVector operator+(CUtlVector oper1, int oper2)
{
	return oper1 + view_as<CUtlVector>(oper2);
}

methodmap CUtlVector
{
	public CUtlVector(Address addr)
	{
		return view_as<CUtlVector>(addr);
	}

	// Part of CUtlMemory
	public any m_pMemory()
	{
		return Deref(this);
	}

	public int m_nAllocationCount()
	{
		return Deref(this + 4);
	}

	public int m_nGrowSize()
	{
		return Deref(this + 8);
	}

	// Part of CUtlVector
	public int m_Size()
	{
		return Deref(this + 12);
	}

	public any m_pElements()
	{
		return Deref(this + 16);
	}

	public any GetAt(int i, int elSize = 4)
	{
		return this.m_pElements() + i * elSize;
	}

	public any GetAtPtr(int i)
	{
		return Deref(this.m_pElements() + i * 4);
	}

	/** ToDo: Is this optimal? */
	public void SetAt(int i, any data, int elSize = 4)
	{
		StoreToAddress(this.m_pElements() + i * elSize, data, NumberType_Int32);
	}
}

// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
stock float SimpleSpline(const float value)
{
	float valueSquared = value * value;

	// Nice little ease-in, ease-out spline-like curve
	return (3.0 * valueSquared) - (2.0 * valueSquared * value);
}

stock float SimpleSplineRemapValClamped(const float val, const float A, const float B, const float C, const float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	if (cVal > 1.0)
	{
		cVal = 1.0;
	}
	else if (cVal < 0.0)
	{
		cVal = 0.0;
	}
	return C + (D - C) * SimpleSpline(cVal);
}

stock bool CanOverrideEnvZoomOwner(CBaseEntity pZoomOwner)
{
	char szClassname[MAX_CLASSNAME];
	if (!pZoomOwner.GetClassname(szClassname, sizeof(szClassname)) || strcmp(szClassname, "env_zoom") != 0)
		return false;
	
	return pZoomOwner.HasSpawnFlag(SF_ENV_ZOOM_OVERRIDE);
}

methodmap CPlayerInfoManager
{
	public CPlayerInfoManager()
	{
		return g_pPlayerInfoManager;
	}

	public CGlobalVars GetGlobalVars()
	{
		return SDKCall(g_pGetGlobalVars, view_as<Address>(this));
	}
}

methodmap CVEngineServer
{
	public static int GetClusterForOrigin(const float vec3Origin[3])
	{
		return SDKCall(g_pGetClusterForOrigin, vec3Origin);
	}
	public static MemoryBlock GetPVSForCluster(const int iCluster)
	{
		MemoryBlock pPVS = new MemoryBlock(MAX_MAP_CLUSTERS / 8);
		SDKCall(g_pGetPVSForCluster, iCluster, pPVS.Size, pPVS.Address);
		return pPVS;
	}
	public static bool CheckOriginInPVS(const float vec3Origin[3], const MemoryBlock pCheckPVS)
	{
		return SDKCall(g_pCheckOriginInPVS, vec3Origin, pCheckPVS.Address, pCheckPVS.Size) != 0;
	}
}

methodmap CGlobalVarsBase
{
	public bool IsValid()
	{
		return view_as<int>(this) != 0;
	}

	// struct is almost 1:1 https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/public/globalvars_base.h#L19
	public int GetMaxPlayers()
	{
		return LoadFromAddress(view_as<Address>(this) + 0x14, NumberType_Int32);
	}
	public void SetMaxPlayers(const int iMaxPlayers)
	{
		assert(this.IsValid(), "Invalid state")
		assert1(IsInBounds(iMaxPlayers, 0, MaxClients), "Invalid maxplayers [%d]", iMaxPlayers)
		
		StoreToAddress(view_as<Address>(this) + 0x14, iMaxPlayers, NumberType_Int32);
	}
}

methodmap CGlobalVars < CGlobalVarsBase
{
	
}

methodmap AI_CriteriaSet
{
	public AI_CriteriaSet(const Address pAddress)
	{
		return view_as<AI_CriteriaSet>(pAddress);
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public void AppendCriteria(const char[] szCriteria, const char[] szValue, const float flWeight = 1.0)
	{
		SDKCall(g_pAppendCriteria, this.GetAddress(), szCriteria, szValue, flWeight);
	}
}

methodmap Task_t
{
	public Task_t(const Address pAddress)
	{
		return view_as<Task_t>(pAddress);
	}

	public sharedtasks_e GetTask()
	{
		return view_as<sharedtasks_e>(LoadFromAddress(view_as<Address>(this) + 0x0, NumberType_Int32));
	}
	public void SetTask(const sharedtasks_e iTask)
	{
		StoreToAddress(view_as<Address>(this) + 0x0, iTask, NumberType_Int32);
	}
	public float GetTaskData()
	{
		return LoadFromAddress(view_as<Address>(this) + 0x4, NumberType_Int32); 
	}
	public void SetTaskData(const float flTaskData)
	{
		StoreToAddress(view_as<Address>(this) + 0x4, flTaskData, NumberType_Int32); 
	}
}
