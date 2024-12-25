#pragma newdecls required
#pragma semicolon 1

#if defined _srccoop_api_classdef_included
 #endinput
#endif
#define _srccoop_api_classdef_included

#include <sdkhooks>
#include <sdktools>
#include <sourcescramble>

#include <srccoop_api/color>

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

#if defined SRCCOOP_BLACKMESA
int g_iOffset_CParamsManger_bIsMultiplayer = -1;
#endif

stock bool IsSpawnFlagSetOnEntity(const int iEntIndex, const int iBitIndex)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is 0-31", iBitIndex);
	
	return (GetEntProp(iEntIndex, Prop_Data, "m_spawnflags") & (1 << iBitIndex)) != 0;
}

stock void SetSpawnFlagOnEntity(const int iEntIndex, const int iBitIndex, const bool bSetSpawnFlag)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is 0-31", iBitIndex);
	
	int iSpawnFlags = GetEntProp(iEntIndex, Prop_Data, "m_spawnflags");

	if (bSetSpawnFlag)
	{
		iSpawnFlags |= (1 << iBitIndex);
	}
	else
	{
		iSpawnFlags &= ~(1 << iBitIndex);
	}
	
	SetEntProp(iEntIndex, Prop_Data, "m_spawnflags", iSpawnFlags);
}

#include <srccoop_api/classdef/core/CBaseEntity.inc>
#include <srccoop_api/classdef/core/CPointEntity.inc>
#include <srccoop_api/classdef/core/CLogicalEntity.inc>
#include <srccoop_api/classdef/core/CBaseAnimating.inc>
#include <srccoop_api/classdef/core/CBaseAnimatingOverlay.inc>
#include <srccoop_api/classdef/core/CBaseFlex.inc>
#include <srccoop_api/classdef/core/CBaseCombatCharacter.inc>
#include <srccoop_api/classdef/core/AI_CriteriaSet.inc>
#include <srccoop_api/classdef/core/CAI_BaseNPC.inc>
#include <srccoop_api/classdef/core/CAI_ScriptConditions.inc>
#include <srccoop_api/classdef/core/CAI_ScriptedSequence.inc>
#include <srccoop_api/classdef/core/CBaseCombatWeapon.inc>
#include <srccoop_api/classdef/core/CBasePlayer.inc>
#include <srccoop_api/classdef/core/CBaseProp.inc>
#include <srccoop_api/classdef/core/CBaseToggle.inc>
#include <srccoop_api/classdef/core/CBaseTrigger.inc>
#include <srccoop_api/classdef/core/CBaseViewModel.inc>
#include <srccoop_api/classdef/core/CBeam.inc>
#include <srccoop_api/classdef/core/CBreakableProp.inc>
#include <srccoop_api/classdef/core/CChangelevel.inc>
#include <srccoop_api/classdef/core/CDynamicProp.inc>
#include <srccoop_api/classdef/core/CEnvBeam.inc>
#include <srccoop_api/classdef/core/CEnvZoom.inc>
#include <srccoop_api/classdef/core/CFuncLadder.inc>
#include <srccoop_api/classdef/core/CGlobalState.inc>
#include <srccoop_api/classdef/core/CGlobalVars.inc>
#include <srccoop_api/classdef/core/CInfoTarget.inc>
#include <srccoop_api/classdef/core/CItem.inc>
#include <srccoop_api/classdef/core/CItemSuit.inc>
#include <srccoop_api/classdef/core/CLogicAchievement.inc>
#include <srccoop_api/classdef/core/CLogicAuto.inc>
#include <srccoop_api/classdef/core/CLogicRelay.inc>
#include <srccoop_api/classdef/core/CMessage.inc>
#include <srccoop_api/classdef/core/CMultiplayRules.inc>
#include <srccoop_api/classdef/core/CParticleSystem.inc>
#include <srccoop_api/classdef/core/CPlayerInfoManager.inc>
#include <srccoop_api/classdef/core/CPlayerResource.inc>
#include <srccoop_api/classdef/core/CPlayerState.inc>
#include <srccoop_api/classdef/core/CPointHurt.inc>
#include <srccoop_api/classdef/core/CPredictedViewModel.inc>
#include <srccoop_api/classdef/core/CPropVehicle.inc>
#include <srccoop_api/classdef/core/CPropVehicleDriveable.inc>
#include <srccoop_api/classdef/core/CProtoSniper.inc>
#include <srccoop_api/classdef/core/CSceneEntity.inc>
#include <srccoop_api/classdef/core/CSniperTarget.inc>
#include <srccoop_api/classdef/core/CSoundEnt.inc>
#include <srccoop_api/classdef/core/CSprite.inc>
#include <srccoop_api/classdef/core/CSpriteOriented.inc>
#include <srccoop_api/classdef/core/CTeam.inc>
#include <srccoop_api/classdef/core/CTraceRay.inc>
#include <srccoop_api/classdef/core/CTriggerHurt.inc>
#include <srccoop_api/classdef/core/CTriggerPush.inc>
#include <srccoop_api/classdef/core/CUtlVector.inc>
#include <srccoop_api/classdef/core/CVEngineServer.inc>
#include <srccoop_api/classdef/core/CWorld.inc>
#include <srccoop_api/classdef/core/IServerGameClients.inc>
#include <srccoop_api/classdef/core/IServerGameDLL.inc>
#include <srccoop_api/classdef/core/IServerTools.inc>
#include <srccoop_api/classdef/core/Task_t.inc>

#if defined SRCCOOP_BLACKMESA
#include <srccoop_api/classdef/bms/CBlackMesaBaseCombatWeapon.inc>
#include <srccoop_api/classdef/bms/CBlackMesaBaseCombatWeaponIronsights.inc>
#include <srccoop_api/classdef/bms/CBlackMesaBaseDetonator.inc>
#include <srccoop_api/classdef/bms/CBlackMesaPlayer.inc>
#include <srccoop_api/classdef/bms/CBM_MP_GameRules.inc>
#include <srccoop_api/classdef/bms/CEnvBeamTeam.inc>
#include <srccoop_api/classdef/bms/CParamsManager.inc>
#include <srccoop_api/classdef/bms/CPropChargerBase.inc>
#include <srccoop_api/classdef/bms/CSpriteTeam.inc>
#include <srccoop_api/classdef/bms/CWeapon_Crossbow.inc>
#endif

stock void InitClassdef(const GameData hGameConfig)
{
	ai_los_mode = FindConVar("ai_los_mode");

	g_iCSoundSize = GetGamedataInt(hGameConfig, "CSoundSize");
	if (g_iCSoundSize <= 0)
		SetFailState("Could not obtain CSound struct size");

	g_iCPlayerState_frags = hGameConfig.GetOffset("CPlayerState::frags");
	if (g_iCPlayerState_frags < 0)
		SetFailState("Invalid offset CPlayerState::frags");

	if (!(g_ServerGameClients = view_as<IServerGameClients>(GetInterface(hGameConfig, "server", "IServerGameClients"))))
		SetFailState("Could not get interface for %s", "IServerGameClients");
	
	if (!(g_pPlayerInfoManager = view_as<CPlayerInfoManager>(GetInterface(hGameConfig, "server", "PlayerInfoManager"))))
		SetFailState("Could not get interface for %s", "PlayerInfoManager");
	
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

	char szIsNpc[] = "CBaseEntity::IsNPC";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szIsNpc))
		LogMessage("Could not obtain gamedata offset %s", szIsNpc);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		if (!(g_pIsNPC = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szIsNpc);
	}
	
	char szRemoveAllItems[] = "CBasePlayer::RemoveAllItems";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szRemoveAllItems))
		LogMessage("Could not obtain gamedata offset %s", szRemoveAllItems);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
		if (!(g_pRemoveAllItems = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szRemoveAllItems);
	}
	
	char szCreateViewModel[] = "CBasePlayer::CreateViewModel";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szCreateViewModel))
		LogMessage("Could not obtain gamedata offset %s", szCreateViewModel);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pCreateViewModel = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCreateViewModel);
	}

	char szSetSuitUpdate[] = "CBasePlayer::SetSuitUpdate";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szSetSuitUpdate))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetIndex))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetIndex);
	else
	{
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetIndex = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetIndex);
	}

	char szGlobalEntity_GetState[] = "GlobalEntity_GetState";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetState))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetState);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetState);
	}

	char szGlobalEntity_GetName[] = "GlobalEntity_GetName";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetName))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetName);
	else
	{	
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
		if (!(g_pGlobalEntityGetName = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetName);
	}

	char szGlobalEntity_GetCounter[] = "GlobalEntity_GetCounter";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetCounter))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetCounter);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGlobalEntityGetCounter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetCounter);
	}

	char szGlobalEntity_GetMap[] = "GlobalEntity_GetMap";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetMap))
		LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetMap);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
		if (!(g_pGlobalEntityGetMap = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetMap);
	}

	char szGlobalEntity_SetState[] = "GlobalEntity_SetState";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_SetState))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_SetCounter))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_Add))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGameShutdown))
		LogMessage("Could not obtain gamedata offset %s", szGameShutdown);
	else
	{
		if (!(g_pGameShutdown = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGameShutdown);
	}

	char szGetTempEntsSystem[] = "CServerTools::GetTempEntsSystem";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetTempEntsSystem))
		LogMessage("Could not obtain gamedata offset %s", szGetTempEntsSystem);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetTempEntsSystem = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetTempEntsSystem);
	}

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

	char szGetSequenceLinearMotion[] = "CBaseAnimating::GetSequenceLinearMotion";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGetSequenceLinearMotion))
		LogMessage("Could not obtain gamedata signature %s", szGetSequenceLinearMotion);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer, _, VENCODE_FLAG_COPYBACK);
		if (!(g_pGetSequenceLinearMotion = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetSequenceLinearMotion);
	}

	char szSendWeaponAnim[] = "CBaseCombatWeapon::SendWeaponAnim";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szSendWeaponAnim))
		LogMessage("Could not obtain gamedata offset %s", szSendWeaponAnim);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSendWeaponAnim = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendWeaponAnim);
	}

	char szWeaponSwitch[] = "CBaseCombatCharacter::Weapon_Switch";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szWeaponSwitch))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szWorldSpaceCenter))
		LogMessage("Could not obtain gamedata offset %s", szWorldSpaceCenter);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_Vector, SDKPass_ByRef);
		if (!(g_pWorldSpaceCenter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szWorldSpaceCenter);
	}

	char szGetPlayerState[] = "CServerGameClients::GetPlayerState";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetPlayerState))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szSelectWeightedSequence))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szSendViewModelMatchingSequence))
		LogMessage("Could not obtain gamedata offset %s", szSendViewModelMatchingSequence);
	else
	{
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pSendViewModelMatchingSequence = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendViewModelMatchingSequence);
	}

	char szGetGlobalVars[] = "CPlayerInfoManager::GetGlobalVars";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetGlobalVars))
		LogMessage("Could not obtain gamedata offset %s", szGetGlobalVars);
	else
	{
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetGlobalVars = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetGlobalVars);
	}
	
	char szAppendCriteria[] = "AI_CriteriaSet::AppendCriteria";
	StartPrepSDKCall(SDKCall_Raw);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szAppendCriteria))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetClusterForOrigin))
		LogMessage("Could not obtain gamedata offset %s", szGetClusterForOrigin);
	else
	{
		PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pGetClusterForOrigin = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetClusterForOrigin);
	}
	
	char szGetPVSForCluster[] = "CVEngineServer::GetPVSForCluster";
	StartPrepSDKCall(SDKCall_Engine);
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetPVSForCluster))
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
	if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szCheckOriginInPVS))
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
	g_iOffset_CParamsManger_bIsMultiplayer = GetGamedataInt(hGameConfig, "CParamsManager::m_bIsMultiplayer");
	if (g_iOffset_CParamsManger_bIsMultiplayer < 0)
		SetFailState("Could not obtain member variable offset `CParamsManager::m_bIsMultiplayer`");
	#endif
}
