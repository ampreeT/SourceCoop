#if defined _srccoop_api_typedef_included
 #endinput
#endif
#define _srccoop_api_typedef_included

#pragma newdecls required
#pragma semicolon 1

#include <sdktools_trace>

//------------------------------------------------------
// Base definitions
//------------------------------------------------------
#if !defined CHAR_BIT
 #define CHAR_BIT 8
#endif

#if !defined FLT_MAX
 #define FLT_MAX 2147483647.0
#endif

#if !defined INT_MIN
 #define INT_MIN cellmin
#endif

#if !defined INT_MAX
 #define INT_MAX cellmax
#endif

#define MAX_WEAPONS     48
#define MAX_AMMO_TYPES  32
#define MAX_VIEWMODELS  2
#define MAX_MAPNAME     32
#define MAX_NETNAME     32
#define MAX_CLASSNAME   48
#define MAX_MODELNAME   256
#define MAX_TEAMNAME    32
#define ENTITYSTRING_LENGTH 2097152

#define ACHIEVEMENT_PREFIX "ACHIEVEMENT_EVENT_"

#define vec3_origin         { 0.0, 0.0, 0.0 }

#define VEC_HULL_MIN        { -16.0, -16.0, 0.0 }
#define VEC_HULL_MAX        { 16.0, 16.0, 72.0 }
#define VEC_VIEW            { 0.0, 0.0, 64.0 }
#define VEC_DUCK_HULL_MIN   { -16.0, -16.0, 0.0 }
#define VEC_DUCK_HULL_MAX   { 16.0, 16.0, 36.0 }
#define VEC_DUCK_VIEW       { 0.0, 0.0, 28.0 }

#define TICK_INTERVAL		(GetTickInterval())
#define TIME_TO_TICKS(%1)	(RoundToFloor(0.5 + (%1) / TICK_INTERVAL))
#define TICKS_TO_TIME(%1)	(TICK_INTERVAL * (%1))
#define ROUND_TO_TICKS(%1)	(TICK_INTERVAL * TIME_TO_TICKS(%1))

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define	CONTENTS_BLOCKLOS 0x40	// block AI line of sight

// everything that blocks line of sight for AI
#define MASK_BLOCKLOS           (CONTENTS_SOLID | CONTENTS_MOVEABLE | CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS  (MASK_BLOCKLOS | CONTENTS_MONSTER)

#define MAX_MAP_CLUSTERS 65536

//------------------------------------------------------
// These are caps bits to indicate what an object's capabilities (currently used for +USE, save/restore and level transitions)
//------------------------------------------------------
#define		FCAP_MUST_SPAWN				0x00000001		// Spawn after restore
#define		FCAP_ACROSS_TRANSITION		0x00000002		// should transfer between transitions 
// UNDONE: This will ignore transition volumes (trigger_transition), but not the PVS!!!
#define		FCAP_FORCE_TRANSITION		0x00000004		// ALWAYS goes across transitions
#define		FCAP_NOTIFY_ON_TRANSITION	0x00000008		// Entity will receive Inside/Outside transition inputs when a transition occurs

#define		FCAP_IMPULSE_USE			0x00000010		// can be used by the player
#define		FCAP_CONTINUOUS_USE			0x00000020		// can be used by the player
#define		FCAP_ONOFF_USE				0x00000040		// can be used by the player
#define		FCAP_DIRECTIONAL_USE		0x00000080		// Player sends +/- 1 when using (currently only tracktrains)
// NOTE: Normally +USE only works in direct line of sight.  Add these caps for additional searches
#define		FCAP_USE_ONGROUND			0x00000100
#define		FCAP_USE_IN_RADIUS			0x00000200
#define		FCAP_SAVE_NON_NETWORKABLE	0x00000400

#define		FCAP_MASTER					0x10000000		// Can be used to "master" other entities (like multisource)
#define		FCAP_WCEDIT_POSITION		0x40000000		// Can change position and update Hammer in edit mode
#define		FCAP_DONT_SAVE				0x80000000		// Don't save this

// If these spawnflags need to be used, turn these into enums and move them into the respective classdef file.
/*
//------------------------------------------------------
// Spawnflags
//------------------------------------------------------
#define SF_INFOPLAYERSTART_MASTER 1

#define SF_PLAYER_EQUIP_STRIP_WEAPONS 2
#define SF_PLAYER_EQUIP_STRIP_SUIT 4

// Phys prop spawnflags
#define SF_PHYSPROP_START_ASLEEP				0x000001
#define SF_PHYSPROP_DONT_TAKE_PHYSICS_DAMAGE	0x000002		// this prop can't be damaged by physics collisions
#define SF_PHYSPROP_DEBRIS						0x000004
#define SF_PHYSPROP_MOTIONDISABLED				0x000008		// motion disabled at startup (flag only valid in spawn - motion can be enabled via input)
#define	SF_PHYSPROP_TOUCH						0x000010		// can be 'crashed through' by running player (plate glass)
#define SF_PHYSPROP_PRESSURE					0x000020		// can be broken by a player standing on it
#define SF_PHYSPROP_ENABLE_ON_PHYSCANNON		0x000040		// enable motion only if the player grabs it with the physcannon
#define SF_PHYSPROP_NO_ROTORWASH_PUSH			0x000080		// The rotorwash doesn't push these
#define SF_PHYSPROP_ENABLE_PICKUP_OUTPUT		0x000100		// If set, allow the player to +USE this for the purposes of generating an output
#define SF_PHYSPROP_PREVENT_PICKUP				0x000200		// If set, prevent +USE/Physcannon pickup of this prop
#define SF_PHYSPROP_PREVENT_PLAYER_TOUCH_ENABLE	0x000400		// If set, the player will not cause the object to enable its motion when bumped into
#define SF_PHYSPROP_HAS_ATTACHED_RAGDOLLS		0x000800		// Need to remove attached ragdolls on enable motion/etc
#define SF_PHYSPROP_FORCE_TOUCH_TRIGGERS		0x001000		// Override normal debris behavior and respond to triggers anyway
#define SF_PHYSPROP_FORCE_SERVER_SIDE			0x002000		// Force multiplayer physics object to be serverside
#define SF_PHYSPROP_RADIUS_PICKUP				0x004000		// For Xbox, makes small objects easier to pick up by allowing them to be found 
#define SF_PHYSPROP_ALWAYS_PICK_UP				0x100000		// Physcannon can always pick this up, no matter what mass or constraints may apply.
#define SF_PHYSPROP_NO_COLLISIONS				0x200000		// Don't enable collisions on spawn
#define SF_PHYSPROP_IS_GIB						0x400000		// Limit # of active gibs

// Physbox spawnflags
#define SF_PHYSBOX_ASLEEP					0x01000
#define SF_PHYSBOX_IGNOREUSE				0x02000
#define SF_PHYSBOX_DEBRIS					0x04000
#define SF_PHYSBOX_MOTIONDISABLED			0x08000
#define SF_PHYSBOX_USEPREFERRED				0x10000
#define SF_PHYSBOX_ENABLE_ON_PHYSCANNON		0x20000
#define SF_PHYSBOX_NO_ROTORWASH_PUSH		0x40000		// The rotorwash doesn't push these
#define SF_PHYSBOX_ENABLE_PICKUP_OUTPUT		0x80000
#define SF_PHYSBOX_ALWAYS_PICK_UP		    0x100000		// Physcannon can always pick this up, no matter what mass or constraints may apply.
#define SF_PHYSBOX_NEVER_PICK_UP			0x200000		// Physcannon will never be able to pick this up.
#define SF_PHYSBOX_NEVER_PUNT				0x400000		// Physcannon will never be able to punt this object.
#define SF_PHYSBOX_PREVENT_PLAYER_TOUCH_ENABLE 0x800000		// If set, the player will not cause the object to enable its motion when bumped into
*/

#include <srccoop_api/typedef/common/AI_Efficiency_t.inc>
#include <srccoop_api/typedef/common/AI_Schedule_t.inc>
#include <srccoop_api/typedef/common/AI_SleepFlags_t.inc>
#include <srccoop_api/typedef/common/AI_SleepState_t.inc>
#include <srccoop_api/typedef/common/AmmoType_t.inc>
#include <srccoop_api/typedef/common/BeamType_t.inc>
#include <srccoop_api/typedef/common/Collision_Group_t.inc>
#include <srccoop_api/typedef/common/Disposition_t.inc>
#include <srccoop_api/typedef/common/EntityDissolveType_t.inc>
#include <srccoop_api/typedef/common/fieldtype_t.inc>
#include <srccoop_api/typedef/common/GLOBALESTATE.inc> 
#include <srccoop_api/typedef/common/GoalType_t.inc>
#include <srccoop_api/typedef/common/HideHudFlags_t.inc>
#include <srccoop_api/typedef/common/LifeState_t.inc>
#include <srccoop_api/typedef/common/MoveCollide_t.inc>
#include <srccoop_api/typedef/common/NPC_STATE.inc>
#include <srccoop_api/typedef/common/Obs_Mode.inc>
#include <srccoop_api/typedef/common/SCOND_t.inc>
#include <srccoop_api/typedef/common/ScreenFadeFlags_t.inc>
#include <srccoop_api/typedef/common/sharedtasks_e.inc>
#include <srccoop_api/typedef/common/SolidType_t.inc>
#include <srccoop_api/typedef/common/sounds.inc>
#include <srccoop_api/typedef/common/TakeDamage_t.inc>
#include <srccoop_api/typedef/common/Team_t.inc>
#include <srccoop_api/typedef/common/USE_TYPE.inc>
#include <srccoop_api/typedef/common/WaterLevel_t.inc>

#if defined SRCCOOP_BLACKMESA
#include <srccoop_api/typedef/bms/Class_T.inc>
#include <srccoop_api/typedef/bms/gamerules.inc>
#include <srccoop_api/typedef/bms/Activity.inc>
#endif

#if defined SRCCOOP_HL2DM
#include <srccoop_api/typedef/hl2dm/Class_T.inc>
#include <srccoop_api/typedef/hl2dm/Activity.inc>
#endif
