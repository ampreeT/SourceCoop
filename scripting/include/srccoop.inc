#if defined _srccoop_included
	#endinput
#endif
#define _srccoop_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Compile defs
//------------------------------------------------------

#if !defined SRCCOOP_CUSTOM_COMPILE

// Uncomment to disable debugging
// #define NO_DEBUG

#if defined SRCCOOP_HL2DM
	
	#define CHECK_ENGINE "HL2DM"; if (GetEngineVersion() != Engine_HL2DM)
	#define OUTPUT_SEPARATOR ','
	#define INTRO_TIMER_HUDTEXT

	#define ENTPATCH_CUSTOM_NPC_MODELS
	#define ENTPATCH_UPDATE_ENEMY_MEMORY
	#define ENTPATCH_TRIGGER_CHANGELEVEL
	#define ENTPATCH_TRIGGER_AUTOSAVE
	#define ENTPATCH_POINT_TELEPORT
	#define ENTPATCH_POINT_VIEWCONTROL
	#define ENTPATCH_PLAYER_SPEEDMOD
	#define ENTPATCH_POINT_CLIENTCOMMAND
	#define ENTPATCH_POINT_SERVERCOMMAND_CHANGELEVEL
	#define ENTPATCH_ENV_ZOOM
	#define ENTPATCH_ENV_CREDITS
	#define ENTPATCH_ENV_SPRITE
	#define ENTPATCH_AI_SCRIPT_CONDITIONS
	#define ENTPATCH_FUNC_ROTATING
	#define ENTPATCH_PLAYER_LOADSAVED
	#define ENTPATCH_LOGIC_AUTOSAVE_SURVIVAL_RESPAWN
	#define ENTPATCH_GAME_END
	#define ENTPATCH_FIND_NAMED_ENTITY
	#define ENTPATCH_GOALENTITY_RESOLVENAMES
	#define ENTPATCH_GOAL_LEAD
	#define ENTPATCH_SETPLAYERAVOIDSTATE
	#define ENTPATCH_NPC_SLEEP
	#define ENTPATCH_FUNC_TRACKAUTOCHANGE
	#define ENTPATCH_FUNC_TRACKTRAIN
	
	#define PLAYERPATCH_SETSUITUPDATE
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER

#else
	
	// SRCCOOP_BLACKMESA is the fallback if no other definition is passed
	#if !defined SRCCOOP_BLACKMESA
		#define SRCCOOP_BLACKMESA
	#endif
	#define CHECK_ENGINE "Black Mesa"; if (GetEngineVersion() != Engine_BlackMesa)
	#define OUTPUT_SEPARATOR '\27'

	#define ENTPATCH_CUSTOM_NPC_MODELS
	#define ENTPATCH_UPDATE_ENEMY_MEMORY
	#define ENTPATCH_SNIPER
	#define ENTPATCH_TRIGGER_CHANGELEVEL
	#define ENTPATCH_TRIGGER_AUTOSAVE
	#define ENTPATCH_POINT_TELEPORT
	#define ENTPATCH_POINT_VIEWCONTROL
	#define ENTPATCH_PLAYER_SPEEDMOD
	#define ENTPATCH_POINT_CLIENTCOMMAND
	#define ENTPATCH_POINT_SERVERCOMMAND_CHANGELEVEL
	#define ENTPATCH_ENV_ZOOM
	#define ENTPATCH_ENV_CREDITS
	#define ENTPATCH_ENV_INTROCREDITS
	#define ENTPATCH_ENV_SPRITE
	#define ENTPATCH_AI_SCRIPT_CONDITIONS
	#define ENTPATCH_LAGCOMP_POSE_PARAMS
	#define ENTPATCH_FUNC_ROTATING
	#define ENTPATCH_PLAYER_LOADSAVED
	#define ENTPATCH_LOGIC_AUTOSAVE_SURVIVAL_RESPAWN
	#define ENTPATCH_GAME_END
	#define ENTPATCH_FIND_NAMED_ENTITY
	#define ENTPATCH_REMOVE_BONE_FOLLOWERS
	#define ENTPATCH_WEAPON_MODELS
	#define ENTPATCH_PLAYER_ALLY
	#define ENTPATCH_RELATION_TYPE
	#define ENTPATCH_PLAYER_COMPANION
	#define ENTPATCH_GOALENTITY_RESOLVENAMES
	#define ENTPATCH_GOAL_LEAD
	#define ENTPATCH_SETPLAYERAVOIDSTATE
	#define ENTPATCH_NPC_SLEEP
	#define ENTPATCH_FUNC_TRACKAUTOCHANGE
	#define ENTPATCH_FUNC_TRACKTRAIN
	#define ENTPATCH_BM_XENTURRET
	#define ENTPATCH_BM_ICHTHYOSAUR
	#define ENTPATCH_BM_GARGANTUA
	#define ENTPATCH_BM_HOUNDEYE
	#define ENTPATCH_BM_GONARCH
	#define ENTPATCH_BM_NIHILANTH
	#define ENTPATCH_BM_PUFFBALLFUNGUS
	#define ENTPATCH_BM_LAV
	#define ENTPATCH_BM_TRIPMINE_COLORS
	#define ENTPATCH_BM_PROP_CHARGERS
	#define ENTPATCH_BM_MISC_MARIONETTIST
	#define ENTPATCH_BM_MUSIC_TRACK
	#define ENTPATCH_BM_SNARK_NEST
	#define ENTPATCH_BM_TAU_VELOCITY

	#define PLAYERPATCH_SETSUITUPDATE
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS
	#define PLAYERPATCH_RESTORE_MP_FORCERESPAWN
	#define PLAYERPATCH_OVERRIDE_DEATH_OBSMODE
	#define PLAYERPATCH_FINDLADDER

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER
	#define GAMEPATCH_BLOCK_RESTOREWORLD
	#define GAMEPATCH_BLOCK_RESPAWNPLAYERS
	#define GAMEPATCH_BLOCK_ANNOUNCER
	#define GAMEPATCH_ALLOW_FLASHLIGHT
	#define GAMEPATCH_TEAMSELECT_UI

#endif // SRCCOOP_BLACKMESA

#endif // !SRCCOOP_CUSTOM_COMPILE


//------------------------------------------------------
// Includes
//------------------------------------------------------


#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <regex>
#include <dhooks>
#include <topmenus>
#include <sourcescramble>

#pragma newdecls required
#pragma semicolon 1

#include <logdebug>

#include <srccoop/typedef>
#include <srccoop/typedef_game>
#include <srccoop/public>
#include <srccoop/classdef>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/blackmesa/typedef>
#include <srccoop/blackmesa/classdef>
#endif
#include <srccoop/config>
#include <srccoop/utils>
#include <srccoop/features>
#include <srccoop/globals>
#include <srccoop/kv_shared>
#include <srccoop/manager_changelevel>
#include <srccoop/manager>
#include <srccoop/manager_survival>
#include <srccoop/manager_persistence>
#include <srccoop/entitypatch>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/blackmesa/entitypatch>
#include <srccoop/blackmesa/deathnotice>
#elseif defined SRCCOOP_HL2DM
#include <srccoop/hl2dm/deathnotice>
#endif
#include <srccoop/deathnotice>
#include <srccoop/instancing>
#include <srccoop/playerpatch>
#include <srccoop/import>
#include <srccoop/menu>
#include <srccoop/commands>
#include <srccoop/natives>