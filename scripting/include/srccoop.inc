#if defined _srccoop_included
	#endinput
#endif
#define _srccoop_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Compile defs
//------------------------------------------------------

// Uncomment to disable debugging
// #define NO_DEBUG

#if !defined SRCCOOP_CUSTOM_COMPILE

#if defined SRCCOOP_HL2DM
	
	#define CHECK_ENGINE "HL2DM"; if (GetEngineVersion() != Engine_HL2DM)

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
	
	#define PLAYERPATCH_SETSUITUPDATE
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER

#else
	
	// SRCCOOP_BLACKMESA is the fallback if no other definition is passed
	#define SRCCOOP_BLACKMESA
	#define CHECK_ENGINE "Black Mesa"; if (GetEngineVersion() != Engine_BlackMesa)

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
	#define ENTPATCH_ENV_SPRITE
	#define ENTPATCH_AI_SCRIPT_CONDITIONS
	#define ENTPATCH_FUNC_ROTATING
	#define ENTPATCH_PLAYER_LOADSAVED
	#define ENTPATCH_LOGIC_AUTOSAVE_SURVIVAL_RESPAWN
	#define ENTPATCH_GAME_END
	#define ENTPATCH_FIND_NAMED_ENTITY
	#define ENTPATCH_REMOVE_BONE_FOLLOWERS
	#define ENTPATCH_WEAPON_MODELS
	#define ENTPATCH_PLAYER_ALLY
	#define ENTPATCH_RELATION_TYPE
	#define ENTPATCH_GOALENTITY_RESOLVENAMES
	#define ENTPATCH_GOAL_LEAD
	#define ENTPATCH_SETPLAYERAVOIDSTATE
	#define ENTPATCH_BM_XENTURRET
	#define ENTPATCH_BM_ICHTHYOSAUR
	#define ENTPATCH_BM_GARGANTUA
	#define ENTPATCH_BM_HOUNDEYE
	#define ENTPATCH_BM_GONARCH
	#define ENTPATCH_BM_NIHILANTH
	#define ENTPATCH_BM_PUFFBALLFUNGUS
	#define ENTPATCH_BM_PROP_CHARGERS
	#define ENTPATCH_BM_MISC_MARIONETTIST
	#define ENTPATCH_BM_MUSIC_TRACK
	#define ENTPATCH_BM_FUNC_TRACKAUTOCHANGE
	#define ENTPATCH_BM_FUNC_TRACKTRAIN

	#define PLAYERPATCH_SETSUITUPDATE
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER
	#define GAMEPATCH_BLOCK_RESTOREWORLD
	#define GAMEPATCH_BLOCK_RESPAWNPLAYERS
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

#pragma newdecls required
#pragma semicolon 1

#include <logdebug>

#include <srccoop_api>
#include <srccoop/globals>
#include <srccoop/kv_shared>
#include <srccoop/manager_changelevel>
#include <srccoop/manager>
#include <srccoop/manager_survival>
#include <srccoop/manager_persistence>
#include <srccoop/entitypatch>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/blackmesa/entitypatch>
#endif
#include <srccoop/instancing>
#include <srccoop/playerpatch>
#include <srccoop/menu>
#include <srccoop/commands>
#include <srccoop/natives>