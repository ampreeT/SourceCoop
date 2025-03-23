#if defined _srccoop_api_classdef_included
 #endinput
#endif
#define _srccoop_api_classdef_included

#pragma newdecls required
#pragma semicolon 1

#include <srccoop_api/classdef/common/CBaseEntity.inc>
#include <srccoop_api/classdef/common/CPointEntity.inc>
#include <srccoop_api/classdef/common/CLogicalEntity.inc>
#include <srccoop_api/classdef/common/CBaseAnimating.inc>
#include <srccoop_api/classdef/common/CBaseAnimatingOverlay.inc>
#include <srccoop_api/classdef/common/CBaseFlex.inc>
#include <srccoop_api/classdef/common/CBaseCombatCharacter.inc>
#include <srccoop_api/classdef/common/AI_CriteriaSet.inc>
#include <srccoop_api/classdef/common/CAI_BaseNPC.inc>
#include <srccoop_api/classdef/common/CAI_ScriptConditions.inc>
#include <srccoop_api/classdef/common/CAI_ScriptedSequence.inc>
#include <srccoop_api/classdef/common/CBaseCombatWeapon.inc>
#include <srccoop_api/classdef/common/CBasePlayer.inc>
#include <srccoop_api/classdef/common/CBaseProp.inc>
#include <srccoop_api/classdef/common/CBaseToggle.inc>
#include <srccoop_api/classdef/common/CBaseTrigger.inc>
#include <srccoop_api/classdef/common/CBaseViewModel.inc>
#include <srccoop_api/classdef/common/CBeam.inc>
#include <srccoop_api/classdef/common/CBreakableProp.inc>
#include <srccoop_api/classdef/common/CChangelevel.inc>
#include <srccoop_api/classdef/common/CDynamicProp.inc>
#include <srccoop_api/classdef/common/CEnvBeam.inc>
#include <srccoop_api/classdef/common/CEnvZoom.inc>
#include <srccoop_api/classdef/common/CFuncLadder.inc>
#include <srccoop_api/classdef/common/CGameMovement.inc>
#include <srccoop_api/classdef/common/CGlobalState.inc>
#include <srccoop_api/classdef/common/CGlobalVars.inc>
#include <srccoop_api/classdef/common/CInfoTarget.inc>
#include <srccoop_api/classdef/common/CItem.inc>
#include <srccoop_api/classdef/common/CItemSuit.inc>
#include <srccoop_api/classdef/common/CLogicAchievement.inc>
#include <srccoop_api/classdef/common/CLogicAuto.inc>
#include <srccoop_api/classdef/common/CLogicRelay.inc>
#include <srccoop_api/classdef/common/CMessage.inc>
#include <srccoop_api/classdef/common/CMultiplayRules.inc>
#include <srccoop_api/classdef/common/CNPC_Barnacle.inc>
#include <srccoop_api/classdef/common/CParticleSystem.inc>
#include <srccoop_api/classdef/common/CPlayerInfoManager.inc>
#include <srccoop_api/classdef/common/CPlayerResource.inc>
#include <srccoop_api/classdef/common/CPlayerState.inc>
#include <srccoop_api/classdef/common/CPointHurt.inc>
#include <srccoop_api/classdef/common/CPredictedViewModel.inc>
#include <srccoop_api/classdef/common/CPropVehicle.inc>
#include <srccoop_api/classdef/common/CPropVehicleDriveable.inc>
#include <srccoop_api/classdef/common/CRagdollProp.inc>
#include <srccoop_api/classdef/common/CRevertSaved.inc>
#include <srccoop_api/classdef/common/CProtoSniper.inc>
#include <srccoop_api/classdef/common/CSceneEntity.inc>
#include <srccoop_api/classdef/common/CSniperTarget.inc>
#include <srccoop_api/classdef/common/CSoundEnt.inc>
#include <srccoop_api/classdef/common/CSprite.inc>
#include <srccoop_api/classdef/common/CSpriteOriented.inc>
#include <srccoop_api/classdef/common/CTakeDamageInfo.inc>
#include <srccoop_api/classdef/common/CTeam.inc>
#include <srccoop_api/classdef/common/CTraceRay.inc>
#include <srccoop_api/classdef/common/CTriggerHurt.inc>
#include <srccoop_api/classdef/common/CTriggerPush.inc>
#include <srccoop_api/classdef/common/CUtlVector.inc>
#include <srccoop_api/classdef/common/CVEngineServer.inc>
#include <srccoop_api/classdef/common/CWorld.inc>
#include <srccoop_api/classdef/common/IServerGameClients.inc>
#include <srccoop_api/classdef/common/IServerGameDLL.inc>
#include <srccoop_api/classdef/common/IServerTools.inc>
#include <srccoop_api/classdef/common/Task_t.inc>

#if defined SRCCOOP_BLACKMESA
#include <srccoop_api/classdef/bms/CBlackMesaBaseCombatWeapon.inc>
#include <srccoop_api/classdef/bms/CBlackMesaBaseCombatWeaponIronsights.inc>
#include <srccoop_api/classdef/bms/CBlackMesaBaseDetonator.inc>
#include <srccoop_api/classdef/bms/CBlackMesaPlayer.inc>
#include <srccoop_api/classdef/bms/CBM_MP_GameRules.inc>
#include <srccoop_api/classdef/bms/CEnv_XenPortalEffect.inc>
#include <srccoop_api/classdef/bms/CEnvBeamTeam.inc>
#include <srccoop_api/classdef/bms/CItem_HealthKit.inc>
#include <srccoop_api/classdef/bms/CParamsManager.inc>
#include <srccoop_api/classdef/bms/CPropChargerBase.inc>
#include <srccoop_api/classdef/bms/CSpriteTeam.inc>
#include <srccoop_api/classdef/bms/CWeapon_Crossbow.inc>
#endif

stock void InitClassdef(const GameData hGameConfig)
{
	CBaseEntity.InitClassdef(hGameConfig);
	CBasePlayer.InitClassdef(hGameConfig);
	CGlobalState.InitClassdef(hGameConfig);
	IServerGameDLL.InitClassdef(hGameConfig);
	IServerTools.InitClassdef(hGameConfig);
	CAI_BaseNPC.InitClassdef(hGameConfig);
	CBaseAnimating.InitClassdef(hGameConfig);
	CBaseAnimatingOverlay.InitClassdef(hGameConfig);
	CBaseCombatWeapon.InitClassdef(hGameConfig);
	CBaseCombatCharacter.InitClassdef(hGameConfig);
	CBaseViewModel.InitClassdef(hGameConfig);
	CRagdollProp.InitClassdef(hGameConfig);
	CPlayerInfoManager.InitClassdef(hGameConfig);
	AI_CriteriaSet.InitClassdef(hGameConfig);
	IServerGameClients.InitClassdef(hGameConfig);
	CVEngineServer.InitClassdef(hGameConfig);
	CSoundEnt.InitClassdef(hGameConfig);
	CPlayerState.InitClassdef(hGameConfig);
	CNPC_Barnacle.InitClassdef(hGameConfig);

	// Always last.
	CGlobalVars.InitClassdef(hGameConfig);

	#if defined SRCCOOP_BLACKMESA
	CParamsManager.InitClassdef(hGameConfig);
	#endif
}
