#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Compile defs
//------------------------------------------------------

#if !defined SRCCOOP_CUSTOM_COMPILE

#if defined SRCCOOP_HL2DM
	
	#define CHECK_ENGINE "HL2DM"; if (GetEngineVersion() != Engine_HL2DM)
	#define OUTPUT_SEPARATOR ","
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
	#define ENTPATCH_NPC_DIALOGUE
	#define ENTPATCH_GOALENTITY_RESOLVENAMES
	#define ENTPATCH_GOAL_LEAD
	#define ENTPATCH_SETPLAYERAVOIDSTATE
	#define ENTPATCH_NPC_SLEEP
	#define ENTPATCH_NPC_THINK_LOCALPLAYER
	#define ENTPATCH_FUNC_TRACKAUTOCHANGE
	#define ENTPATCH_FUNC_TRACKTRAIN
	#define ENTPATCH_NPC_RUNTASK
	#define ENTPATCH_BARNACLE_PREDICTION
	
	#define PLAYERPATCH_SUIT_SOUNDS
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS
	#define PLAYERPATCH_HITREG

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER
	#define GAMEPATCH_PREDICTED_EFFECTS
	//#define GAMEPATCH_UTIL_FINDCLIENT

#else
	
	// SRCCOOP_BLACKMESA is the fallback if no other definition is passed
	#if !defined SRCCOOP_BLACKMESA
		#define SRCCOOP_BLACKMESA
	#endif
	#define CHECK_ENGINE "Black Mesa"; if (GetEngineVersion() != Engine_BlackMesa)
	#define OUTPUT_SEPARATOR "\27"

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
	#define ENTPATCH_PLAYER_LOADSAVED
	#define ENTPATCH_LOGIC_AUTOSAVE_SURVIVAL_RESPAWN
	#define ENTPATCH_GAME_END
	#define ENTPATCH_NPC_DIALOGUE
	#define ENTPATCH_SCRIPTED_SEQUENCE
	#define ENTPATCH_REMOVE_BONE_FOLLOWERS
	#define ENTPATCH_WEAPON_MODELS
	#define ENTPATCH_PLAYER_ALLY
	#define ENTPATCH_GOALENTITY_RESOLVENAMES
	#define ENTPATCH_GOAL_LEAD
	#define ENTPATCH_SETPLAYERAVOIDSTATE
	#define ENTPATCH_NPC_SLEEP
	#define ENTPATCH_NPC_THINK_LOCALPLAYER
	#define ENTPATCH_NPC_ALWAYS_TRANSMIT
	#define ENTPATCH_FUNC_TRACKAUTOCHANGE
	#define ENTPATCH_FUNC_TRACKTRAIN
	#define ENTPATCH_NPC_RUNTASK
	#define ENTPATCH_BARNACLE_PREDICTION
	#define ENTPATCH_BM_XENTURRET
	#define ENTPATCH_BM_ICHTHYOSAUR
	#define ENTPATCH_BM_GARGANTUA
	#define ENTPATCH_BM_PUFFBALLFUNGUS
	#define ENTPATCH_BM_LAV
	#define ENTPATCH_BM_TRIPMINE_COLORS
	#define ENTPATCH_BM_PROP_CHARGERS
	#define ENTPATCH_BM_MISC_MARIONETTIST
	#define ENTPATCH_BM_MUSIC_TRACK
	#define ENTPATCH_BM_SNARK_NEST
	#define ENTPATCH_BM_SP_WEAPONS
	#define ENTPATCH_BM_DISSOLVE
	#define ENTPATCH_BM_XENPORTAL_PUSH_PLAYERS
	#define ENTPATCH_BM_BATTERY_DLIGHT

	#define PLAYERPATCH_SUIT_SOUNDS
	#define PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	#define PLAYERPATCH_SERVERSIDE_RAGDOLLS
	#define PLAYERPATCH_RESTORE_MP_FORCERESPAWN
	#define PLAYERPATCH_OVERRIDE_DEATH_OBSMODE
	#define PLAYERPATCH_FINDLADDER
	#define PLAYERPATCH_HITREG
	#define PLAYERPATCH_BM_CLIENT_PREDICTION

	#define GAMEPATCH_UTIL_GETLOCALPLAYER
	#define GAMEPATCH_IS_MULTIPLAYER
	#define GAMEPATCH_BLOCK_RESTOREWORLD
	#define GAMEPATCH_BLOCK_RESPAWNPLAYERS
	#define GAMEPATCH_BLOCK_ANNOUNCER
	#define GAMEPATCH_ALLOW_FLASHLIGHT
	#define GAMEPATCH_TEAMSELECT_UI
	#define GAMEPATCH_PREDICTED_EFFECTS
	#define GAMEPATCH_BM_GRAVITY
	#define GAMEPATCH_UTIL_FINDCLIENT

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
#include <logdebug>

#include <srccoop/typedef>
#include <srccoop/public>
#include <srccoop_api>
#include <srccoop/config>
#include <srccoop/features>
#include <srccoop/globals>
#include <srccoop/kv_shared>
#include <srccoop/manager_changelevel>
#include <srccoop/manager>
#include <srccoop/manager_survival>
#include <srccoop/manager_persistence>
#include <srccoop/entitypatch>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/bms/entitypatch>
#include <srccoop/bms/deathnotice>
#include <srccoop/bms/idle_anims>
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