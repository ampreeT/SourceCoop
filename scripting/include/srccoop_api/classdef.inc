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

// Obtains a bitflag from a entity.
//
stock bool IsSpawnFlagSetOnEntity(const int iEntIndex, const int iBitIndex)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is `0..32`", iBitIndex);
	
	return (GetEntProp(iEntIndex, Prop_Data, "m_spawnflags") & (1 << iBitIndex)) != 0;
}

// Sets a bitflag on a entity.
//
stock void SetSpawnFlagOnEntity(const int iEntIndex, const int iBitIndex, const bool bSetSpawnFlag)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is `0..32`", iBitIndex);
	
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
	CBaseEntity.InitClassdef(hGameConfig);
	CBasePlayer.InitClassdef(hGameConfig);
	CGlobalState.InitClassdef(hGameConfig);
	IServerGameDLL.InitClassdef(hGameConfig);
	IServerTools.InitClassdef(hGameConfig);
	CAI_BaseNPC.InitClassdef(hGameConfig);
	CBaseAnimating.InitClassdef(hGameConfig);
	CBaseCombatWeapon.InitClassdef(hGameConfig);
	CBaseCombatCharacter.InitClassdef(hGameConfig);
	CBaseViewModel.InitClassdef(hGameConfig);
	CPlayerInfoManager.InitClassdef(hGameConfig);
	AI_CriteriaSet.InitClassdef(hGameConfig);
	IServerGameClients.InitClassdef(hGameConfig);
	CVEngineServer.InitClassdef(hGameConfig);
	CSoundEnt.InitClassdef(hGameConfig);
	CPlayerState.InitClassdef(hGameConfig);

	// Always last.
	CGlobalVars.InitClassdef(hGameConfig);

	#if defined SRCCOOP_BLACKMESA
	CParamsManager.InitClassdef(hGameConfig);
	#endif
}
