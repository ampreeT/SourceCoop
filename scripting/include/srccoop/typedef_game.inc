#if defined _srccoop_typedef_game_included
 #endinput
#endif
#define _srccoop_typedef_game_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Base definitions
//------------------------------------------------------
#define MAX_WEAPONS 48
#define MAX_AMMO_TYPES 32
#define MAX_VIEWMODELS 2
#define MAX_MAPNAME 32
#define MAX_CLASSNAME 48
#define MAX_MODELNAME 256

#define ACHIEVEMENT_PREFIX "ACHIEVEMENT_EVENT_"

#define vec3_origin {0.0, 0.0, 0.0}

#define VEC_HULL_MIN {-16.0, -16.0, 0.0}
#define VEC_HULL_MAX {16.0, 16.0, 72.0}
#define VEC_VIEW {0.0, 0.0, 64.0}
#define VEC_DUCK_HULL_MIN {-16.0, -16.0, 0.0}
#define VEC_DUCK_HULL_MAX {16.0, 16.0, 36.0}
#define VEC_DUCK_VIEW {0.0, 0.0, 28.0}

#define TICK_INTERVAL			(GetTickInterval())
#define TIME_TO_TICKS(%1)		(RoundToFloor(0.5 + (%1) / TICK_INTERVAL))
#define TICKS_TO_TIME(%1)		( TICK_INTERVAL *(%1) )
#define ROUND_TO_TICKS(%1)		( TICK_INTERVAL * TIME_TO_TICKS(%1) )

#define AI_SLEEP_FLAGS_NONE					0x00000000
#define AI_SLEEP_FLAG_AUTO_PVS				0x00000001
#define AI_SLEEP_FLAG_AUTO_PVS_AFTER_PVS	0x00000002


// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------

#define	CONTENTS_BLOCKLOS		0x40	// block AI line of sight

// everything that blocks line of sight for AI
#define MASK_BLOCKLOS				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
// everything that blocks line of sight for AI plus NPCs
#define MASK_BLOCKLOS_AND_NPCS		(MASK_BLOCKLOS|CONTENTS_MONSTER)

#define MAX_MAP_CLUSTERS 65536

//------------------------------------------------------
// Spawnflags
//------------------------------------------------------
#define SF_SNIPERTARGET_NOINTERRUPT 2

#define SF_INFOPLAYERSTART_MASTER 1

#define SF_AUTO_FIREONCE 0x01
#define SF_REMOVE_ON_FIRE 0x001 // Relay will remove itself after being triggered.

#define SF_CHANGELEVEL_NOTOUCH 0x0002

#define SF_PLAYER_EQUIP_STRIP_WEAPONS 2
#define SF_PLAYER_EQUIP_STRIP_SUIT 4

#define SF_MESSAGE_ONCE			0x0001		// Fade in, not out
#define SF_MESSAGE_ALL 0x0002

// Pickupables spawnflags
#define SF_ITEM_NORESPAWN ( 1 << 30 )	// if set, item wont respawn in MP
#define SF_ITEM_HARDRESPAWN ( 1 << 2 )	// if set in BM, item respawns at regular interval

// item_suit spawnflags
#define SF_BOOTSEQUENCE ( 1 << 17 )

// env_zoom spawnflags
#define SF_ENV_ZOOM_OVERRIDE (1<<0)

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


// Trigger spawnflags
enum
{
	SF_TRIGGER_ALLOW_CLIENTS				= 0x01,		// Players can fire this trigger
	SF_TRIGGER_ALLOW_NPCS					= 0x02,		// NPCS can fire this trigger
	SF_TRIGGER_ALLOW_PUSHABLES				= 0x04,		// Pushables can fire this trigger
	SF_TRIGGER_ALLOW_PHYSICS				= 0x08,		// Physics objects can fire this trigger
	SF_TRIGGER_ONLY_PLAYER_ALLY_NPCS		= 0x10,		// *if* NPCs can fire this trigger, this flag means only player allies do so
	SF_TRIGGER_ONLY_CLIENTS_IN_VEHICLES		= 0x20,		// *if* Players can fire this trigger, this flag means only players inside vehicles can 
	SF_TRIGGER_ALLOW_ALL					= 0x40,		// Everything can fire this trigger EXCEPT DEBRIS!
	SF_TRIGGER_ONLY_CLIENTS_OUT_OF_VEHICLES	= 0x200,	// *if* Players can fire this trigger, this flag means only players outside vehicles can 
	SF_TRIG_PUSH_ONCE						= 0x80,		// trigger_push removes itself after firing once
	SF_TRIG_PUSH_AFFECT_PLAYER_ON_LADDER	= 0x100,	// if pushed object is player on a ladder, then this disengages them from the ladder (HL2only)
	SF_TRIG_TOUCH_DEBRIS 					= 0x400,	// Will touch physics debris objects
	SF_TRIGGER_ONLY_NPCS_IN_VEHICLES		= 0x800,	// *if* NPCs can fire this trigger, only NPCs in vehicles do so (respects player ally flag too)
	SF_TRIGGER_DISALLOW_BOTS                = 0x1000,   // Bots are not allowed to fire this trigger
}

// Sprite spawnflags
#define SF_SPRITE_STARTON		0x0001
#define SF_SPRITE_ONCE			0x0002
#define SF_SPRITE_TEMPORARY		0x8000

// NPC spawnflags
#define SF_NPC_WAIT_TILL_SEEN			( 1 << 0  )	// spawnflag that makes npcs wait until player can see them before attacking.
#define SF_NPC_GAG						( 1 << 1  )	// no idle noises from this npc
#define SF_NPC_FALL_TO_GROUND			( 1 << 2  )	// used my NPC_Maker
#define SF_NPC_DROP_HEALTHKIT			( 1 << 3  )	// Drop a healthkit upon death
#define SF_NPC_START_EFFICIENT			( 1 << 4  ) // Set into efficiency mode from spawn
//										( 1 << 5  )
//										( 1 << 6  )
#define SF_NPC_WAIT_FOR_SCRIPT			( 1 << 7  )	// spawnflag that makes npcs wait to check for attacking until the script is done or they've been attacked
#define SF_NPC_LONG_RANGE				( 1 << 8  )	// makes npcs look far and relaxes weapon range limit 
#define SF_NPC_FADE_CORPSE				( 1 << 9  )	// Fade out corpse after death
#define SF_NPC_ALWAYSTHINK				( 1 << 10 )	// Simulate even when player isn't in PVS.
#define SF_NPC_TEMPLATE					( 1 << 11 )	// This NPC will be used as a template by an npc_maker -- do not spawn.
#define SF_NPC_ALTCOLLISION				( 1 << 12 )
#define SF_NPC_NO_WEAPON_DROP			( 1 << 13 )	// This NPC will not actually drop a weapon that can be picked up
#define SF_NPC_NO_PLAYER_PUSHAWAY		( 1 << 14 )	

//------------------------------------------------------
// Effects flags
//------------------------------------------------------
enum
{
	EF_BONEMERGE			= 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT 			= 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT 			= 0x004,	// player flashlight
	EF_NOINTERP				= 0x008,	// don't interpolate the next frame
	EF_NOSHADOW				= 0x010,	// Don't cast no shadow
	EF_NODRAW				= 0x020,	// don't draw entity
	EF_NORECEIVESHADOW		= 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL	= 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
	EF_ITEM_BLINK			= 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES		= 0x200,	// always assume that the parent entity is animating
}

//------------------------------------------------------
// Settings for m_takedamage
//------------------------------------------------------
#define DAMAGE_NO			0
#define DAMAGE_EVENTS_ONLY	1		// Call damage functions, but don't modify health
#define DAMAGE_YES			2
#define DAMAGE_AIM			3

//------------------------------------------------------
// m_lifeState values
//------------------------------------------------------
#define	LIFE_ALIVE				0 // alive
#define	LIFE_DYING				1 // playing death animation or still falling off of a ledge waiting to hit ground
#define	LIFE_DEAD				2 // dead. lying still.
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4

//-----------------------------------------------------------------------------
// m_nWaterLevel enumeration
// Purpose: Identifies how submerged in water a player is.
//-----------------------------------------------------------------------------
enum WaterLevel
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
}

//------------------------------------------------------
// Screen Fade flags
//------------------------------------------------------
#define FFADE_IN			0x0001		// Just here so we don't pass 0 into the function
#define FFADE_OUT			0x0002		// Fade out (not in)
#define FFADE_MODULATE		0x0004		// Modulate (don't blend)
#define FFADE_STAYOUT		0x0008		// ignores the duration, stays faded out until new ScreenFade message received
#define FFADE_PURGE			0x0010		// Purges all other fades, replacing them with this one

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

// Beam types, encoded as a byte
enum 
{
    BEAM_POINTS = 0,
    BEAM_ENTPOINT,
    BEAM_ENTS,
    BEAM_HOSE,
    BEAM_SPLINE,
    BEAM_LASER,
    NUM_BEAM_TYPES
};

// Entity Dissolve types
enum
{
	ENTITY_DISSOLVE_NORMAL = 0,
	ENTITY_DISSOLVE_ELECTRICAL,
	ENTITY_DISSOLVE_ELECTRICAL_LIGHT,
#if defined SRCCOOP_BLACKMESA
	ENTITY_DISSOLVE_FADE,
	ENTITY_DISSOLVE_REMOVE_NOW,
#endif
	ENTITY_DISSOLVE_CORE
}

enum GLOBALESTATE
{
	GLOBAL_OFF = 0,
	GLOBAL_ON = 1,
	GLOBAL_DEAD = 2
}

enum
{
	TEAM_ANY = -2,
	TEAM_INVALID = -1,
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR = 1,
	TEAM_MARINES = 2,
	TEAM_SCIENTIST = 3,
}

enum
{
	AMMO_INVALID = 0,
	AMMO_9MM = 1,
	AMMO_357 = 2,
	AMMO_BOLTS = 3,
	AMMO_SHELLS = 4,
	AMMO_ENERGY = 5,
	AMMO_MP5NADE = 6,
	AMMO_ROCKETS = 7,
	AMMO_NADES = 8,
	AMMO_SATCHELS = 9,
	AMMO_TRIPMINES = 10,
	AMMO_HIVEGUN = 11,
	AMMO_SNARKS = 12,
	// non-equippable ammo types:
	AMMO_50CAL = 13,
	AMMO_SENTRY = 14,
	AMMO_SNIPERROUND = 15,
	AMMO_SNIPERPENETRATEDROUND = 16
}

// Spectator Movement modes
enum Obs_Mode
{
	OBS_MODE_NONE = 0,	// not in spectator mode
	OBS_MODE_DEATHCAM,	// special mode for death cam animation
	OBS_MODE_FREEZECAM,	// zooms to a target, and freeze-frames on them
	OBS_MODE_FIXED,		// view from a fixed camera position
	OBS_MODE_IN_EYE,	// follow a player in first person view
	OBS_MODE_CHASE,		// follow a player in third person view
	OBS_MODE_POI,		// PASSTIME point of interest - game objective, big fight, anything interesting; added in the middle of the enum due to tons of hard-coded "<ROAMING" enum compares
	OBS_MODE_ROAMING,	// free roaming

	NUM_OBSERVER_MODES
}

enum
{
	USE_OFF,
	USE_ON,
	USE_SET,
	USE_TOGGLE
}

// ToDo: Need to find all values for BMS
enum Class_T
{
	CLASS_PLAYER = 3
}

enum Disposition_t 
{
	D_ER,		// Undefined - error
	D_HT,		// Hate
	D_FR,		// Fear
	D_LI,		// Like
	D_NU		// Neutral
}

enum AI_SleepState_t
{
	AISS_AWAKE,
	AISS_WAITING_FOR_THREAT,
	AISS_WAITING_FOR_PVS,
	AISS_WAITING_FOR_INPUT,
	AISS_AUTO_PVS,
	AISS_AUTO_PVS_AFTER_PVS, // Same as AUTO_PVS, except doesn't activate until/unless the NPC is IN the player's PVS. 
}

enum AI_Efficiency_t
{
	// Run at full tilt
	AIE_NORMAL,

	// Run decision process less often
	AIE_EFFICIENT,

	// Run decision process even less often, ignore other NPCs
	AIE_VERY_EFFICIENT,

	// Run decision process even less often, ignore other NPCs
	AIE_SUPER_EFFICIENT,

	// Don't run at all
	AIE_DORMANT,
}

// entity flags, CBaseEntity::m_iEFlags
enum
{
	EFL_KILLME	=				(1<<0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT	=				(1<<1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE =			(1<<2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES =		(1<<3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1<<4), // This is a special entity that should not be deleted when we restart entities only

	EFL_HAS_PLAYER_CHILD=		(1<<4),	// One of the child entities is a player.

	EFL_DIRTY_SHADOWUPDATE =	(1<<5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY =				(1<<6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT =	(1<<7),

	EFL_BOT_FROZEN =			(1<<8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY =			(1<<9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH =	(1<<10), // Don't attach the edict; we're doing it explicitly
	
	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM =	(1<<11),
	EFL_DIRTY_ABSVELOCITY =		(1<<12),
	EFL_DIRTY_ABSANGVELOCITY =	(1<<13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS	= (1<<14),
	EFL_DIRTY_SPATIAL_PARTITION = (1<<15),
//	UNUSED						= (1<<16),

	EFL_IN_SKYBOX =				(1<<17),	// This is set if the entity detects that it's in the skybox.
											// This forces it to pass the "in PVS" for transmission.
	EFL_USE_PARTITION_WHEN_NOT_SOLID = (1<<18),	// Entities with this flag set show up in the partition even when not solid
	EFL_TOUCHING_FLUID =		(1<<19),	// Used to determine if an entity is floating

	// FIXME: Not really sure where I should add this...
	EFL_IS_BEING_LIFTED_BY_BARNACLE = (1<<20),
	EFL_NO_ROTORWASH_PUSH =		(1<<21),		// I shouldn't be pushed by the rotorwash
	EFL_NO_THINK_FUNCTION =		(1<<22),
	EFL_NO_GAME_PHYSICS_SIMULATION = (1<<23),

	EFL_CHECK_UNTOUCH =			(1<<24),
	EFL_DONTBLOCKLOS =			(1<<25),		// I shouldn't block NPC line-of-sight
	EFL_DONTWALKON =			(1<<26),		// NPC;s should not walk on this entity
	EFL_NO_DISSOLVE =			(1<<27),		// These guys shouldn't dissolve
	EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1<<28),	// Mega physcannon can't ragdoll these guys.
	EFL_NO_WATER_VELOCITY_CHANGE  =	(1<<29),	// Don't adjust this entity's velocity when transitioning into water
	EFL_NO_PHYSCANNON_INTERACTION =	(1<<30),	// Physcannon can't pick these up or punt them
	EFL_NO_DAMAGE_FORCES =		(1<<31),	// Doesn't accept forces from physics damage
}

enum SCOND_t 
{
	COND_NONE,				// A way for a function to return no condition to get
	
	COND_IN_PVS,
	COND_IDLE_INTERRUPT,	// The schedule in question is a low priority idle, and therefore a candidate for translation into something else
	
	COND_LOW_PRIMARY_AMMO,
	COND_NO_PRIMARY_AMMO,
	COND_NO_SECONDARY_AMMO,
	COND_NO_WEAPON,
	COND_SEE_HATE,
	COND_SEE_FEAR,
	COND_SEE_DISLIKE,
	COND_SEE_ENEMY,
	COND_LOST_ENEMY,
	COND_ENEMY_WENT_NULL,	// What most people think COND_LOST_ENEMY is: This condition is set in the edge case where you had an enemy last think, but don't have one this think.
	COND_ENEMY_OCCLUDED,	// Can't see m_hEnemy
	COND_TARGET_OCCLUDED,	// Can't see m_hTargetEnt
	COND_HAVE_ENEMY_LOS,
	COND_HAVE_TARGET_LOS,
	COND_LIGHT_DAMAGE,
	COND_HEAVY_DAMAGE,
	COND_PHYSICS_DAMAGE,
	COND_REPEATED_DAMAGE,	//  Damaged several times in a row

	COND_CAN_RANGE_ATTACK1,	// Hitscan weapon only
	COND_CAN_RANGE_ATTACK2,	// Grenade weapon only
	COND_CAN_MELEE_ATTACK1,
	COND_CAN_MELEE_ATTACK2,

	COND_PROVOKED,
	COND_NEW_ENEMY,

	COND_ENEMY_TOO_FAR,		//	Can we get rid of this one!?!?
	COND_ENEMY_FACING_ME,
	COND_BEHIND_ENEMY,
	COND_ENEMY_DEAD,
	COND_ENEMY_UNREACHABLE,	// Not connected to me via node graph

	COND_SEE_PLAYER,
	COND_LOST_PLAYER,
	COND_SEE_NEMESIS,
	COND_TASK_FAILED,
	COND_SCHEDULE_DONE,
	COND_SMELL,
	COND_TOO_CLOSE_TO_ATTACK, // FIXME: most of this next group are meaningless since they're shared between all attack checks!
	COND_TOO_FAR_TO_ATTACK,
	COND_NOT_FACING_ATTACK,
	COND_WEAPON_HAS_LOS,
	COND_WEAPON_BLOCKED_BY_FRIEND,	// Friend between weapon and target
	COND_WEAPON_PLAYER_IN_SPREAD,	// Player in shooting direction
	COND_WEAPON_PLAYER_NEAR_TARGET,	// Player near shooting position
	COND_WEAPON_SIGHT_OCCLUDED,
	COND_BETTER_WEAPON_AVAILABLE,
	COND_HEALTH_ITEM_AVAILABLE,		// There's a healthkit available.
	COND_GIVE_WAY,					// Another npc requested that I give way
	COND_WAY_CLEAR,					// I no longer have to give way
	COND_HEAR_DANGER,
	COND_HEAR_THUMPER,
	COND_HEAR_BUGBAIT,
	COND_HEAR_COMBAT,
	COND_HEAR_WORLD,
	COND_HEAR_PLAYER,
	COND_HEAR_BULLET_IMPACT,
	COND_HEAR_PHYSICS_DANGER,
	COND_HEAR_MOVE_AWAY,
	COND_HEAR_SPOOKY,				// Zombies make this when Alyx is in darkness mode

	COND_NO_HEAR_DANGER,			// Since we can't use ~CONDITION. Mutually exclusive with COND_HEAR_DANGER

	COND_FLOATING_OFF_GROUND,

	COND_MOBBED_BY_ENEMIES,			// Surrounded by a large number of enemies melee attacking me. (Zombies or Antlions, usually).

	// Commander stuff
	COND_RECEIVED_ORDERS,
	COND_PLAYER_ADDED_TO_SQUAD,
	COND_PLAYER_REMOVED_FROM_SQUAD,

	COND_PLAYER_PUSHING,
	COND_NPC_FREEZE,				// We received an npc_freeze command while we were unfrozen
	COND_NPC_UNFREEZE,				// We received an npc_freeze command while we were frozen

	// This is a talker condition, but done here because we need to handle it in base AI
	// due to it's interaction with behaviors.
	COND_TALKER_RESPOND_TO_QUESTION,
	
	COND_NO_CUSTOM_INTERRUPTS,		// Don't call BuildScheduleTestBits for this schedule. Used for schedules that must strictly control their interruptibility.

	// ======================================
	// IMPORTANT: This must be the last enum
	// ======================================
	LAST_SHARED_CONDITION	
}

enum //Sniper_Conds
{
	COND_SNIPER_CANATTACKDECOY = view_as<int>(LAST_SHARED_CONDITION),
	COND_SNIPER_SUPPRESSED,
	COND_SNIPER_ENABLED,
	COND_SNIPER_DISABLED,
	COND_SNIPER_FRUSTRATED,
	COND_SNIPER_SWEEP_TARGET,
	COND_SNIPER_NO_SHOT,
}

// =======================================
//  Movement goals 
//	Used both to store the current movment goal in m_routeGoalType and to or/and with route
// =======================================
enum //GoalType_t 
{
	GOALTYPE_NONE,
	GOALTYPE_TARGETENT,
	GOALTYPE_ENEMY,
	GOALTYPE_PATHCORNER,
	GOALTYPE_LOCATION,
	GOALTYPE_LOCATION_NEAREST_NODE,
	GOALTYPE_FLANK,
	GOALTYPE_COVER,
	
	GOALTYPE_INVALID
}

//=========================================================
// These are the schedule types
//=========================================================
enum 
{
	SCHED_NONE = 0,
	SCHED_IDLE_STAND,
	SCHED_IDLE_WALK,
	SCHED_IDLE_WANDER,
	SCHED_WAKE_ANGRY,
	SCHED_ALERT_FACE,
	SCHED_ALERT_FACE_BESTSOUND,
	SCHED_ALERT_REACT_TO_COMBAT_SOUND,
	SCHED_ALERT_SCAN,
	SCHED_ALERT_STAND,
	SCHED_ALERT_WALK,
	SCHED_INVESTIGATE_SOUND,
	SCHED_COMBAT_FACE,
	SCHED_COMBAT_SWEEP,
	SCHED_FEAR_FACE,
	SCHED_COMBAT_STAND,
	SCHED_COMBAT_WALK,
	SCHED_CHASE_ENEMY,
	SCHED_CHASE_ENEMY_FAILED,
	SCHED_VICTORY_DANCE,
	SCHED_TARGET_FACE,
	SCHED_TARGET_CHASE,
	SCHED_SMALL_FLINCH,
	SCHED_BIG_FLINCH,
	SCHED_BACK_AWAY_FROM_ENEMY,
	SCHED_MOVE_AWAY_FROM_ENEMY,
	SCHED_BACK_AWAY_FROM_SAVE_POSITION,
	SCHED_TAKE_COVER_FROM_ENEMY,
	SCHED_TAKE_COVER_FROM_BEST_SOUND,
	SCHED_FLEE_FROM_BEST_SOUND,
	SCHED_TAKE_COVER_FROM_ORIGIN,
	SCHED_FAIL_TAKE_COVER,
	SCHED_RUN_FROM_ENEMY,
	SCHED_RUN_FROM_ENEMY_FALLBACK,
	SCHED_MOVE_TO_WEAPON_RANGE,
	SCHED_ESTABLISH_LINE_OF_FIRE,
	SCHED_ESTABLISH_LINE_OF_FIRE_FALLBACK,
	SCHED_PRE_FAIL_ESTABLISH_LINE_OF_FIRE,
	SCHED_FAIL_ESTABLISH_LINE_OF_FIRE,
	SCHED_SHOOT_ENEMY_COVER,
	SCHED_COWER, // usually a last resort!
	SCHED_MELEE_ATTACK1,
	SCHED_MELEE_ATTACK2,
	SCHED_RANGE_ATTACK1,
	SCHED_RANGE_ATTACK2,
	SCHED_SPECIAL_ATTACK1,
	SCHED_SPECIAL_ATTACK2,
	SCHED_STANDOFF,
	SCHED_ARM_WEAPON,
	SCHED_DISARM_WEAPON,
	SCHED_HIDE_AND_RELOAD,
	SCHED_RELOAD,
	SCHED_AMBUSH,
	SCHED_DIE,
	SCHED_DIE_RAGDOLL,
	SCHED_WAIT_FOR_SCRIPT,
	SCHED_AISCRIPT,
	SCHED_SCRIPTED_WALK,
	SCHED_SCRIPTED_RUN,
	SCHED_SCRIPTED_CUSTOM_MOVE,
	SCHED_SCRIPTED_WAIT,
	SCHED_SCRIPTED_FACE,
	SCHED_SCENE_GENERIC,
	SCHED_NEW_WEAPON,
	SCHED_NEW_WEAPON_CHEAT,
	SCHED_SWITCH_TO_PENDING_WEAPON,
	SCHED_GET_HEALTHKIT,
	SCHED_WAIT_FOR_SPEAK_FINISH,

	SCHED_MOVE_AWAY,
	SCHED_MOVE_AWAY_FAIL,
	SCHED_MOVE_AWAY_END,
	SCHED_FORCED_GO,
	SCHED_FORCED_GO_RUN,
	SCHED_NPC_FREEZE,
	SCHED_PATROL_WALK,
	SCHED_COMBAT_PATROL,
	SCHED_PATROL_RUN,
	SCHED_RUN_RANDOM,
	SCHED_FALL_TO_GROUND,
	SCHED_DROPSHIP_DUSTOFF,

	SCHED_FLINCH_PHYSICS,

	SCHED_FAIL,
	SCHED_FAIL_NOSTOP,

	SCHED_RUN_FROM_ENEMY_MOB,

	SCHED_DUCK_DODGE,

	SCHED_INTERACTION_MOVE_TO_PARTNER,
	SCHED_INTERACTION_WAIT_FOR_PARTNER,

	SCHED_SLEEP,

	// ======================================
	// IMPORTANT: This must be the last enum
	// ======================================
	LAST_SHARED_SCHEDULE

}

enum
{
	SCHED_PSNIPER_SCAN = LAST_SHARED_SCHEDULE,
	SCHED_PSNIPER_CAMP,
	SCHED_PSNIPER_ATTACK,
	SCHED_PSNIPER_RELOAD,
	SCHED_PSNIPER_ATTACKDECOY,
	SCHED_PSNIPER_SUPPRESSED,
	SCHED_PSNIPER_DISABLEDWAIT,
	SCHED_PSNIPER_FRUSTRATED_ATTACK,
	SCHED_PSNIPER_SWEEP_TARGET,
	SCHED_PSNIPER_SWEEP_TARGET_NOINTERRUPT,
	SCHED_PSNIPER_SNAPATTACK,
	SCHED_PSNIPER_NO_CLEAR_SHOT,
	SCHED_PSNIPER_PLAYER_DEAD,
}

// edict->movecollide values
enum MoveCollide_t
{
	MOVECOLLIDE_DEFAULT = 0,

	// These ones only work for MOVETYPE_FLY + MOVETYPE_FLYGRAVITY
	MOVECOLLIDE_FLY_BOUNCE,	// bounces, reflects, based on elasticity of surface and object - applies friction (adjust velocity)
	MOVECOLLIDE_FLY_CUSTOM,	// Touch() will modify the velocity however it likes
	MOVECOLLIDE_FLY_SLIDE,  // slides along surfaces (no bounce) - applies friciton (adjusts velocity)

	MOVECOLLIDE_COUNT,		// Number of different movecollides

	// When adding new movecollide types, make sure this is correct
	MOVECOLLIDE_MAX_BITS = 3
}

enum
{
    COLLISION_GROUP_NONE  = 0,
    COLLISION_GROUP_DEBRIS,            // Collides with nothing but world and static stuff
    COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
    COLLISION_GROUP_INTERACTIVE_DEBRIS,    // Collides with everything except other interactive debris or debris
    COLLISION_GROUP_INTERACTIVE,    // Collides with everything except interactive debris or debris
    COLLISION_GROUP_PLAYER,
    COLLISION_GROUP_BREAKABLE_GLASS,
    COLLISION_GROUP_VEHICLE,
    COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player
                                        
    COLLISION_GROUP_NPC,            // Generic NPC group
    COLLISION_GROUP_IN_VEHICLE,        // for any entity inside a vehicle
    COLLISION_GROUP_WEAPON,            // for any weapons that need collision detection
    COLLISION_GROUP_VEHICLE_CLIP,    // vehicle clip brush to restrict vehicle movement
    COLLISION_GROUP_PROJECTILE,        // Projectiles!
    COLLISION_GROUP_DOOR_BLOCKER,    // Blocks entities not permitted to get near moving doors
    COLLISION_GROUP_PASSABLE_DOOR,    // Doors that the player shouldn't collide with
    COLLISION_GROUP_DISSOLVING,        // Things that are dissolving are in this group
    COLLISION_GROUP_PUSHAWAY,        // Nonsolid on client and server, pushaway in player code

    COLLISION_GROUP_NPC_ACTOR,        // Used so NPCs in scripts ignore the player.
}

enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
}

enum //SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST = 0x0001,	// Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST = 0x0002,	// Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID = 0x0004,	// Are we currently not solid?
	FSOLID_TRIGGER = 0x0008,	// This is something may be collideable but fires touch functions
	
	// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE = 0x0010,	// You can't stand on this
	FSOLID_VOLUME_CONTENTS = 0x0020,	// Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED = 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS = 0x0080,	// Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED = 0x0100,	// Collisions are defined in root parent's local coordinate space
	FSOLID_TRIGGER_TOUCH_DEBRIS = 0x0200, // This trigger will touch debris objects
}

enum fieldtype_t
{
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions

	FIELD_CLASSPTR,			// CBaseEntity *
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t *

	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)

	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)

	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)

	// NOTE: Use float arrays for local transformations that don't need to be fixed up.
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

	FIELD_INTERVAL,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)
	
	FIELD_VECTOR2D,			// 2 floats

	FIELD_TYPECOUNT,		// MUST BE LAST
}

//------------------------------------------------------
// BMS specific activity list
//------------------------------------------------------
enum
{
	ACT_INVALID = -1,			// So we have something more succint to check for than '-1'
	ACT_RESET = 0,				// Set m_Activity to this invalid value to force a reset to m_IdealActivity
	ACT_IDLE,
	ACT_TRANSITION,
	ACT_COVER,					// FIXME: obsolete? redundant with ACT_COVER_LOW?
	ACT_COVER_MED,				// FIXME: unsupported?
	ACT_COVER_LOW,				// FIXME: rename ACT_IDLE_CROUCH?
	ACT_WALK,
	ACT_WALK_AIM,
	ACT_WALK_CROUCH,
	ACT_WALK_CROUCH_AIM,
	ACT_RUN,
	ACT_RUN_AIM,
	ACT_RUN_CROUCH,
	ACT_RUN_CROUCH_AIM,
	ACT_RUN_PROTECTED,
	ACT_SCRIPT_CUSTOM_MOVE,
	ACT_RANGE_ATTACK1,
	ACT_RANGE_ATTACK2,
	ACT_RANGE_ATTACK1_LOW,		// FIXME: not used yet, crouched versions of the range attack
	ACT_RANGE_ATTACK2_LOW,		// FIXME: not used yet, crouched versions of the range attack
	ACT_DIESIMPLE,
	ACT_DIEBACKWARD,
	ACT_DIEFORWARD,
	ACT_DIEVIOLENT,
	ACT_DIERAGDOLL,
	ACT_FLY,				// Fly (and flap if appropriate)
	ACT_HOVER,
	ACT_GLIDE,
	ACT_SWIM,
	ACT_JUMP,
	ACT_HOP,				// vertical jump
	ACT_LEAP,				// long forward jump
	ACT_LAND,
	ACT_CLIMB_UP,
	ACT_CLIMB_DOWN,
	ACT_CLIMB_DISMOUNT,
	ACT_SHIPLADDER_UP,
	ACT_SHIPLADDER_DOWN,
	ACT_STRAFE_LEFT,
	ACT_STRAFE_RIGHT,
	ACT_ROLL_LEFT,			// tuck and roll, left
	ACT_ROLL_RIGHT,			// tuck and roll, right
	ACT_TURN_LEFT,			// turn quickly left (stationary)
	ACT_TURN_RIGHT,			// turn quickly right (stationary)
	ACT_CROUCH,				// FIXME: obsolete? only used be soldier (the act of crouching down from a standing position)
	ACT_CROUCHIDLE,			// FIXME: obsolete? only used be soldier (holding body in crouched position (loops))
	ACT_STAND,				// FIXME: obsolete? should be transition (the act of standing from a crouched position)
	ACT_USE,
	ACT_SIGNAL1,
	ACT_SIGNAL2,
	ACT_SIGNAL3,

	ACT_SIGNAL_ADVANCE,		// Squad handsignals, specific.
	ACT_SIGNAL_FORWARD,
	ACT_SIGNAL_GROUP,
	ACT_SIGNAL_HALT,
	ACT_SIGNAL_LEFT,
	ACT_SIGNAL_RIGHT,
	ACT_SIGNAL_TAKECOVER,

	ACT_LOOKBACK_RIGHT,		// look back over shoulder without turning around.
	ACT_LOOKBACK_LEFT,
	ACT_COWER,				// FIXME: unused, should be more extreme version of crouching
	ACT_SMALL_FLINCH,		// FIXME: needed? shouldn't flinching be down with overlays?
	ACT_BIG_FLINCH,		
	ACT_MELEE_ATTACK1,
	ACT_MELEE_ATTACK2,
	ACT_RELOAD,
	ACT_RELOAD_START,
	ACT_RELOAD_FINISH,
	ACT_RELOAD_LOW,
	ACT_ARM,				// pull out gun, for instance
	ACT_DISARM,				// reholster gun
	ACT_DROP_WEAPON,
	ACT_DROP_WEAPON_SHOTGUN,
	ACT_PICKUP_GROUND,		// pick up something in front of you on the ground
	ACT_PICKUP_RACK,		// pick up something from a rack or shelf in front of you.
	ACT_IDLE_ANGRY,			// FIXME: being used as an combat ready idle?  alternate idle animation in which the monster is clearly agitated. (loop)

	ACT_IDLE_RELAXED,
	ACT_IDLE_STIMULATED,
	ACT_IDLE_AGITATED,
	ACT_IDLE_STEALTH,
	ACT_IDLE_HURT,

	ACT_WALK_RELAXED,
	ACT_WALK_STIMULATED,
	ACT_WALK_AGITATED,
	ACT_WALK_STEALTH,

	ACT_RUN_RELAXED,
	ACT_RUN_STIMULATED,
	ACT_RUN_AGITATED,
	ACT_RUN_STEALTH,
		
	ACT_IDLE_AIM_RELAXED,
	ACT_IDLE_AIM_STIMULATED,
	ACT_IDLE_AIM_AGITATED,
	ACT_IDLE_AIM_STEALTH,

	ACT_WALK_AIM_RELAXED,
	ACT_WALK_AIM_STIMULATED,
	ACT_WALK_AIM_AGITATED,
	ACT_WALK_AIM_STEALTH,

	ACT_RUN_AIM_RELAXED,
	ACT_RUN_AIM_STIMULATED,
	ACT_RUN_AIM_AGITATED,
	ACT_RUN_AIM_STEALTH,

	ACT_CROUCHIDLE_STIMULATED,
	ACT_CROUCHIDLE_AIM_STIMULATED,
	ACT_CROUCHIDLE_AGITATED,

	ACT_WALK_HURT,			// limp  (loop)
	ACT_RUN_HURT,			// limp  (loop)
	ACT_SPECIAL_ATTACK1,	// very monster specific special attacks.
	ACT_SPECIAL_ATTACK2,	
	ACT_COMBAT_IDLE,		// FIXME: unused?  agitated idle.
	ACT_WALK_SCARED,
	ACT_RUN_SCARED,
	ACT_VICTORY_DANCE,		// killed a player, do a victory dance.
	ACT_DIE_HEADSHOT,		// die, hit in head. 
	ACT_DIE_CHESTSHOT,		// die, hit in chest
	ACT_DIE_GUTSHOT,		// die, hit in gut
	ACT_DIE_BACKSHOT,		// die, hit in back
	ACT_FLINCH_HEAD,
	ACT_FLINCH_CHEST,
	ACT_FLINCH_STOMACH,
	ACT_FLINCH_LEFTARM,
	ACT_FLINCH_RIGHTARM,
	ACT_FLINCH_LEFTLEG,
	ACT_FLINCH_RIGHTLEG,
	ACT_FLINCH_PHYSICS,

	ACT_IDLE_ON_FIRE,		// ON FIRE animations
	ACT_WALK_ON_FIRE,
	ACT_RUN_ON_FIRE,		

	ACT_RAPPEL_LOOP,		// Rappel down a rope!

	ACT_180_LEFT,			// 180 degree left turn
	ACT_180_RIGHT,

	ACT_90_LEFT,			// 90 degree turns
	ACT_90_RIGHT,

	ACT_STEP_LEFT,			// Single steps
	ACT_STEP_RIGHT,
	ACT_STEP_BACK,
	ACT_STEP_FORE,

	ACT_GESTURE_RANGE_ATTACK1,
	ACT_GESTURE_RANGE_ATTACK2,
	ACT_GESTURE_MELEE_ATTACK1,
	ACT_GESTURE_MELEE_ATTACK2,
	ACT_GESTURE_RANGE_ATTACK1_LOW,	// FIXME: not used yet, crouched versions of the range attack
	ACT_GESTURE_RANGE_ATTACK2_LOW,	// FIXME: not used yet, crouched versions of the range attack

	ACT_MELEE_ATTACK_SWING_GESTURE,

	ACT_GESTURE_SMALL_FLINCH,
	ACT_GESTURE_BIG_FLINCH,
	ACT_GESTURE_FLINCH_BLAST,			// Startled by an explosion
	ACT_GESTURE_FLINCH_BLAST_SHOTGUN,
	ACT_GESTURE_FLINCH_BLAST_DAMAGED,	// Damaged by an explosion
	ACT_GESTURE_FLINCH_BLAST_DAMAGED_SHOTGUN,
	ACT_GESTURE_FLINCH_HEAD,
	ACT_GESTURE_FLINCH_CHEST,
	ACT_GESTURE_FLINCH_STOMACH,
	ACT_GESTURE_FLINCH_LEFTARM,
	ACT_GESTURE_FLINCH_RIGHTARM,
	ACT_GESTURE_FLINCH_LEFTLEG,
	ACT_GESTURE_FLINCH_RIGHTLEG,

	ACT_GESTURE_TURN_LEFT,
	ACT_GESTURE_TURN_RIGHT,
	ACT_GESTURE_TURN_LEFT45,
	ACT_GESTURE_TURN_RIGHT45,
	ACT_GESTURE_TURN_LEFT90,
	ACT_GESTURE_TURN_RIGHT90,
	ACT_GESTURE_TURN_LEFT45_FLAT,
	ACT_GESTURE_TURN_RIGHT45_FLAT,
	ACT_GESTURE_TURN_LEFT90_FLAT,
	ACT_GESTURE_TURN_RIGHT90_FLAT,

	// HALF-LIFE 1 compatability stuff goes here. Temporary!
	ACT_BARNACLE_HIT,		// barnacle tongue hits a monster
	ACT_BARNACLE_PULL,		// barnacle is lifting the monster ( loop )
	ACT_BARNACLE_CHOMP,		// barnacle latches on to the monster
	ACT_BARNACLE_CHEW,		// barnacle is holding the monster in its mouth ( loop )

	// Sometimes, you just want to set an NPC's sequence to a sequence that doesn't actually
	// have an activity. The AI will reset the NPC's sequence to whatever its IDEAL activity
	// is, though. So if you set ideal activity to DO_NOT_DISTURB, the AI will not interfere
	// with the NPC's current sequence. (SJB)
	ACT_DO_NOT_DISTURB,

	// viewmodel (weapon) activities
	// FIXME: move these to the specific viewmodels, no need to make global
	ACT_VM_DRAW,
	ACT_VM_HOLSTER,
	ACT_VM_IDLE,
	ACT_VM_FIDGET,
	ACT_VM_PULLBACK,
	ACT_VM_PULLBACK_HIGH,
	ACT_VM_PULLBACK_LOW,
	ACT_VM_THROW,
	ACT_VM_PULLPIN,
	ACT_VM_PRIMARYATTACK,		// fire
	ACT_VM_SECONDARYATTACK,		// alt. fire
	ACT_VM_RELOAD,			
	ACT_VM_RELOAD_START,			
	ACT_VM_RELOAD_FINISH,			
	ACT_VM_DRYFIRE,				// fire with no ammo loaded.
	ACT_VM_HITLEFT,				// bludgeon, swing to left - hit (primary attk)
	ACT_VM_HITLEFT2,			// bludgeon, swing to left - hit (secondary attk)
	ACT_VM_HITRIGHT,			// bludgeon, swing to right - hit (primary attk)
	ACT_VM_HITRIGHT2,			// bludgeon, swing to right - hit (secondary attk)
	ACT_VM_HITCENTER,			// bludgeon, swing center - hit (primary attk)
	ACT_VM_HITCENTER2,			// bludgeon, swing center - hit (secondary attk)
	ACT_VM_MISSLEFT,			// bludgeon, swing to left - miss (primary attk)
	ACT_VM_MISSLEFT2,			// bludgeon, swing to left - miss (secondary attk)
	ACT_VM_MISSRIGHT,			// bludgeon, swing to right - miss (primary attk)
	ACT_VM_MISSRIGHT2,			// bludgeon, swing to right - miss (secondary attk)
	ACT_VM_MISSCENTER,			// bludgeon, swing center - miss (primary attk)
	ACT_VM_MISSCENTER2,			// bludgeon, swing center - miss (secondary attk)
	ACT_VM_HAULBACK,			// bludgeon, haul the weapon back for a hard strike (secondary attk)
	ACT_VM_SWINGHARD,			// bludgeon, release the hard strike (secondary attk)
	ACT_VM_SWINGMISS,
	ACT_VM_SWINGHIT,
	ACT_VM_IDLE_TO_LOWERED,
	ACT_VM_IDLE_LOWERED,
	ACT_VM_LOWERED_TO_IDLE,
	ACT_VM_RECOIL1,
	ACT_VM_RECOIL2,
	ACT_VM_RECOIL3,
	ACT_VM_PICKUP,
	ACT_VM_RELEASE,
	
	ACT_VM_ATTACH_SILENCER,
	ACT_VM_DETACH_SILENCER,

	// TF2 Scout Pack
	ACT_VM_DRAW_SPECIAL,
	ACT_VM_HOLSTER_SPECIAL,
	ACT_VM_IDLE_SPECIAL,
	ACT_VM_PULLBACK_SPECIAL,
	ACT_VM_PRIMARYATTACK_SPECIAL,
	ACT_VM_SECONDARYATTACK_SPECIAL,
	ACT_VM_HITCENTER_SPECIAL,
	ACT_VM_SWINGHARD_SPECIAL,
	ACT_VM_IDLE_TO_LOWERED_SPECIAL,
	ACT_VM_IDLE_LOWERED_SPECIAL,
	ACT_VM_LOWERED_TO_IDLE_SPECIAL,

	ACT_FISTS_VM_HITLEFT,
	ACT_FISTS_VM_HITRIGHT,
	ACT_FISTS_VM_SWINGHARD,
	ACT_FISTS_VM_IDLE,
	ACT_FISTS_VM_DRAW,

//===========================
// HL2 Specific Activities
//===========================
	// SLAM	Specialty Activities
	ACT_SLAM_STICKWALL_IDLE,
	ACT_SLAM_STICKWALL_ND_IDLE,
	ACT_SLAM_STICKWALL_ATTACH,
	ACT_SLAM_STICKWALL_ATTACH2,
	ACT_SLAM_STICKWALL_ND_ATTACH,
	ACT_SLAM_STICKWALL_ND_ATTACH2,
	ACT_SLAM_STICKWALL_DETONATE,
	ACT_SLAM_STICKWALL_DETONATOR_HOLSTER,
	ACT_SLAM_STICKWALL_DRAW, 
	ACT_SLAM_STICKWALL_ND_DRAW, 
	ACT_SLAM_STICKWALL_TO_THROW, 
	ACT_SLAM_STICKWALL_TO_THROW_ND, 
	ACT_SLAM_STICKWALL_TO_TRIPMINE_ND,
	ACT_SLAM_THROW_IDLE,
	ACT_SLAM_THROW_ND_IDLE,
	ACT_SLAM_THROW_THROW,
	ACT_SLAM_THROW_THROW2,
	ACT_SLAM_THROW_THROW_ND,
	ACT_SLAM_THROW_THROW_ND2,
	ACT_SLAM_THROW_DRAW, 
	ACT_SLAM_THROW_ND_DRAW,
	ACT_SLAM_THROW_TO_STICKWALL, 
	ACT_SLAM_THROW_TO_STICKWALL_ND, 
  	ACT_SLAM_THROW_DETONATE,
  	ACT_SLAM_THROW_DETONATOR_HOLSTER,
	ACT_SLAM_THROW_TO_TRIPMINE_ND,
	ACT_SLAM_TRIPMINE_IDLE,
	ACT_SLAM_TRIPMINE_DRAW, 
	ACT_SLAM_TRIPMINE_ATTACH, 
	ACT_SLAM_TRIPMINE_ATTACH2, 
	ACT_SLAM_TRIPMINE_TO_STICKWALL_ND,
	ACT_SLAM_TRIPMINE_TO_THROW_ND,
	ACT_SLAM_DETONATOR_IDLE, 
	ACT_SLAM_DETONATOR_DRAW, 
	ACT_SLAM_DETONATOR_DETONATE,
	ACT_SLAM_DETONATOR_HOLSTER,
	ACT_SLAM_DETONATOR_STICKWALL_DRAW,
	ACT_SLAM_DETONATOR_THROW_DRAW,

	// Shotgun Specialty Activities
	ACT_SHOTGUN_RELOAD_START,
	ACT_SHOTGUN_RELOAD_FINISH,
	ACT_SHOTGUN_PUMP,

	// SMG2 special activities
	ACT_SMG2_IDLE2,
	ACT_SMG2_FIRE2,
	ACT_SMG2_DRAW2,
	ACT_SMG2_RELOAD2,
	ACT_SMG2_DRYFIRE2,
	ACT_SMG2_TOAUTO,
	ACT_SMG2_TOBURST,
	
	// Physcannon special activities
	ACT_PHYSCANNON_UPGRADE,

	// weapon override activities
	ACT_RANGE_ATTACK_AR1,
	ACT_RANGE_ATTACK_AR2,
	ACT_RANGE_ATTACK_AR2_LOW,
	ACT_RANGE_ATTACK_AR2_GRENADE,
	ACT_RANGE_ATTACK_HMG1,
	ACT_RANGE_ATTACK_ML,
	ACT_RANGE_ATTACK_SMG1,
	ACT_RANGE_ATTACK_SMG1_LOW,
	ACT_RANGE_ATTACK_SMG2,
	ACT_RANGE_ATTACK_SHOTGUN,
	ACT_RANGE_ATTACK_SHOTGUN_LOW,
	ACT_RANGE_ATTACK_PISTOL,
	ACT_RANGE_ATTACK_PISTOL_LOW,
	ACT_RANGE_ATTACK_SLAM,
	ACT_RANGE_ATTACK_TRIPWIRE,
	ACT_RANGE_ATTACK_THROW,
	ACT_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_RANGE_ATTACK_RPG,
	ACT_MELEE_ATTACK_SWING,

	ACT_RANGE_AIM_LOW,
	ACT_RANGE_AIM_SMG1_LOW,
	ACT_RANGE_AIM_PISTOL_LOW,
	ACT_RANGE_AIM_AR2_LOW,

	ACT_COVER_PISTOL_LOW,
	ACT_COVER_SMG1_LOW,

	// weapon override activities
	ACT_GESTURE_RANGE_ATTACK_AR1,
	ACT_GESTURE_RANGE_ATTACK_AR2,
	ACT_GESTURE_RANGE_ATTACK_AR2_GRENADE,
	ACT_GESTURE_RANGE_ATTACK_HMG1,
	ACT_GESTURE_RANGE_ATTACK_ML,
	ACT_GESTURE_RANGE_ATTACK_SMG1,
	ACT_GESTURE_RANGE_ATTACK_SMG1_LOW,
	ACT_GESTURE_RANGE_ATTACK_SMG2,
	ACT_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_GESTURE_RANGE_ATTACK_PISTOL,
	ACT_GESTURE_RANGE_ATTACK_PISTOL_LOW,
	ACT_GESTURE_RANGE_ATTACK_SLAM,
	ACT_GESTURE_RANGE_ATTACK_TRIPWIRE,
	ACT_GESTURE_RANGE_ATTACK_THROW,
	ACT_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,
	ACT_GESTURE_MELEE_ATTACK_SWING,

	ACT_IDLE_RIFLE,
	ACT_IDLE_SMG1,
	ACT_IDLE_ANGRY_SMG1,
	ACT_IDLE_PISTOL,
	ACT_IDLE_ANGRY_PISTOL,
	ACT_IDLE_ANGRY_SHOTGUN,
	ACT_IDLE_STEALTH_PISTOL,

	ACT_IDLE_PACKAGE,
	ACT_WALK_PACKAGE,
	ACT_IDLE_SUITCASE,
	ACT_WALK_SUITCASE,

	ACT_IDLE_SMG1_RELAXED,
	ACT_IDLE_SMG1_STIMULATED,
	ACT_WALK_RIFLE_RELAXED,
	ACT_RUN_RIFLE_RELAXED,
	ACT_WALK_RIFLE_STIMULATED,
	ACT_RUN_RIFLE_STIMULATED,

	ACT_IDLE_AIM_RIFLE_STIMULATED,
	ACT_WALK_AIM_RIFLE_STIMULATED,
	ACT_RUN_AIM_RIFLE_STIMULATED,

	ACT_IDLE_SHOTGUN_RELAXED,
	ACT_IDLE_SHOTGUN_STIMULATED,
	ACT_IDLE_SHOTGUN_AGITATED,

	// Policing activities
	ACT_WALK_ANGRY,
	ACT_POLICE_HARASS1,
	ACT_POLICE_HARASS2,

	// Manned guns
	ACT_IDLE_MANNEDGUN,
		
	// Melee weapon
	ACT_IDLE_MELEE,
	ACT_IDLE_ANGRY_MELEE,

	// RPG activities
	ACT_IDLE_RPG_RELAXED,
	ACT_IDLE_RPG, 
	ACT_IDLE_ANGRY_RPG,
	ACT_COVER_LOW_RPG, 
	ACT_WALK_RPG,
	ACT_RUN_RPG, 
	ACT_WALK_CROUCH_RPG, 
	ACT_RUN_CROUCH_RPG, 
	ACT_WALK_RPG_RELAXED, 
	ACT_RUN_RPG_RELAXED, 

	ACT_WALK_RIFLE,
	ACT_WALK_AIM_RIFLE,
	ACT_WALK_CROUCH_RIFLE,
	ACT_WALK_CROUCH_AIM_RIFLE,
	ACT_RUN_RIFLE,
	ACT_RUN_AIM_RIFLE,
	ACT_RUN_CROUCH_RIFLE,
	ACT_RUN_CROUCH_AIM_RIFLE,
	ACT_RUN_STEALTH_PISTOL,

	ACT_WALK_AIM_SHOTGUN,
	ACT_RUN_AIM_SHOTGUN,

	ACT_WALK_PISTOL,
	ACT_RUN_PISTOL,
	ACT_WALK_AIM_PISTOL,
	ACT_RUN_AIM_PISTOL,
	ACT_WALK_STEALTH_PISTOL,
	ACT_WALK_AIM_STEALTH_PISTOL,
	ACT_RUN_AIM_STEALTH_PISTOL,

	// Reloads
	ACT_RELOAD_PISTOL,
	ACT_RELOAD_PISTOL_LOW,
	ACT_RELOAD_SMG1,
	ACT_RELOAD_SMG1_LOW,
	ACT_RELOAD_SHOTGUN,
	ACT_RELOAD_SHOTGUN_LOW,

	ACT_GESTURE_RELOAD,
	ACT_GESTURE_RELOAD_PISTOL,
	ACT_GESTURE_RELOAD_SMG1,
	ACT_GESTURE_RELOAD_SHOTGUN,

	// Busy animations
	ACT_BUSY_LEAN_LEFT,
	ACT_BUSY_LEAN_LEFT_ENTRY,
	ACT_BUSY_LEAN_LEFT_EXIT,
	ACT_BUSY_LEAN_BACK,
	ACT_BUSY_LEAN_BACK_ENTRY,
	ACT_BUSY_LEAN_BACK_EXIT,
	ACT_BUSY_SIT_GROUND,
	ACT_BUSY_SIT_GROUND_ENTRY,
	ACT_BUSY_SIT_GROUND_EXIT,
	ACT_BUSY_SIT_CHAIR,
	ACT_BUSY_SIT_CHAIR_ENTRY,
	ACT_BUSY_SIT_CHAIR_EXIT,
	ACT_BUSY_STAND,
	ACT_BUSY_QUEUE,

	// Dodge animations
	ACT_DUCK_DODGE,

	// For NPCs being lifted/eaten by barnacles:
	// being swallowed by a barnacle
	ACT_DIE_BARNACLE_SWALLOW,  
	 // being lifted by a barnacle
	ACT_GESTURE_BARNACLE_STRANGLE, 

	ACT_PHYSCANNON_DETACH,	// An activity to be played if we're picking this up with the physcannon
	ACT_PHYSCANNON_ANIMATE, // An activity to be played by an object being picked up with the physcannon, but has different behavior to DETACH
	ACT_PHYSCANNON_ANIMATE_PRE,	// An activity to be played by an object being picked up with the physcannon, before playing the ACT_PHYSCANNON_ANIMATE
	ACT_PHYSCANNON_ANIMATE_POST,// An activity to be played by an object being picked up with the physcannon, after playing the ACT_PHYSCANNON_ANIMATE

	ACT_DIE_FRONTSIDE,
	ACT_DIE_RIGHTSIDE,
	ACT_DIE_BACKSIDE,
	ACT_DIE_LEFTSIDE,

	ACT_OPEN_DOOR,

	// Dynamic interactions
	ACT_DI_ALYX_ZOMBIE_MELEE,
	ACT_DI_ALYX_ZOMBIE_TORSO_MELEE,
	ACT_DI_ALYX_HEADCRAB_MELEE,
	ACT_DI_ALYX_ANTLION,

	ACT_DI_ALYX_ZOMBIE_SHOTGUN64,
	ACT_DI_ALYX_ZOMBIE_SHOTGUN26,

	ACT_READINESS_RELAXED_TO_STIMULATED,
	ACT_READINESS_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_AGITATED_TO_STIMULATED,
	ACT_READINESS_STIMULATED_TO_RELAXED,

	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED,
	ACT_READINESS_PISTOL_RELAXED_TO_STIMULATED_WALK,
	ACT_READINESS_PISTOL_AGITATED_TO_STIMULATED,
	ACT_READINESS_PISTOL_STIMULATED_TO_RELAXED,

	ACT_IDLE_CARRY,
	ACT_WALK_CARRY,

//===========================
// TF2 Specific Activities
//===========================
	ACT_STARTDYING,
	ACT_DYINGLOOP,
	ACT_DYINGTODEAD,

	ACT_RIDE_MANNED_GUN,

	// All viewmodels
	ACT_VM_SPRINT_ENTER,
	ACT_VM_SPRINT_IDLE,
	ACT_VM_SPRINT_LEAVE,

	// Looping weapon firing
	ACT_FIRE_START,
	ACT_FIRE_LOOP,
	ACT_FIRE_END,

	ACT_CROUCHING_GRENADEIDLE,
	ACT_CROUCHING_GRENADEREADY,
	ACT_CROUCHING_PRIMARYATTACK,
	ACT_OVERLAY_GRENADEIDLE,
	ACT_OVERLAY_GRENADEREADY,
	ACT_OVERLAY_PRIMARYATTACK,
	ACT_OVERLAY_SHIELD_UP,
	ACT_OVERLAY_SHIELD_DOWN,
	ACT_OVERLAY_SHIELD_UP_IDLE,
	ACT_OVERLAY_SHIELD_ATTACK,
	ACT_OVERLAY_SHIELD_KNOCKBACK,
	ACT_SHIELD_UP,
	ACT_SHIELD_DOWN,
	ACT_SHIELD_UP_IDLE,
	ACT_SHIELD_ATTACK,
	ACT_SHIELD_KNOCKBACK,
	ACT_CROUCHING_SHIELD_UP,
	ACT_CROUCHING_SHIELD_DOWN,
	ACT_CROUCHING_SHIELD_UP_IDLE,
	ACT_CROUCHING_SHIELD_ATTACK,
	ACT_CROUCHING_SHIELD_KNOCKBACK,

	// turning in place
	ACT_TURNRIGHT45,
	ACT_TURNLEFT45,

	ACT_TURN,

	ACT_OBJ_ASSEMBLING,
	ACT_OBJ_DISMANTLING,
	ACT_OBJ_STARTUP,
	ACT_OBJ_RUNNING,
	ACT_OBJ_IDLE,
	ACT_OBJ_PLACING,
	ACT_OBJ_DETERIORATING,
	ACT_OBJ_UPGRADING,

	// Deploy
	ACT_DEPLOY,
	ACT_DEPLOY_IDLE,
	ACT_UNDEPLOY,

//===========================
// HL1 Specific Activities
//===========================
	// Grenades
	ACT_GRENADE_ROLL,
	ACT_GRENADE_TOSS,

	// Hand grenade
	ACT_HANDGRENADE_THROW1,
	ACT_HANDGRENADE_THROW2,
	ACT_HANDGRENADE_THROW3,

	// Shotgun
	ACT_SHOTGUN_IDLE_DEEP,
	ACT_SHOTGUN_IDLE4,

	// Glock
	ACT_GLOCK_SHOOTEMPTY,
	ACT_GLOCK_SHOOT_RELOAD,

	// RPG
	ACT_RPG_DRAW_UNLOADED,
	ACT_RPG_HOLSTER_UNLOADED,
	ACT_RPG_IDLE_UNLOADED,
	ACT_RPG_FIDGET_UNLOADED,

	// Crossbow
	ACT_CROSSBOW_DRAW_UNLOADED,
	ACT_CROSSBOW_IDLE_UNLOADED,
	ACT_CROSSBOW_FIDGET_UNLOADED,

	// Gauss
	ACT_GAUSS_SPINUP,
	ACT_GAUSS_SPINCYCLE,
	ACT_GAUSS_SPINCYCLE_NOAMMO,

	// Tripmine
	ACT_TRIPMINE_GROUND,
	ACT_TRIPMINE_WORLD,

//===========================
// CSPort Specific Activities
//===========================

	ACT_VM_PRIMARYATTACK_SILENCED,		// fire
	ACT_VM_RELOAD_SILENCED,
	ACT_VM_DRYFIRE_SILENCED,				// fire with no ammo loaded.
	ACT_VM_IDLE_SILENCED,
	ACT_VM_DRAW_SILENCED,
	ACT_VM_IDLE_EMPTY_LEFT,
	ACT_VM_DRYFIRE_LEFT,

	ACT_PLAYER_IDLE_FIRE,
	ACT_PLAYER_CROUCH_FIRE,
	ACT_PLAYER_CROUCH_WALK_FIRE,
	ACT_PLAYER_WALK_FIRE,
	ACT_PLAYER_RUN_FIRE,
	
	ACT_IDLETORUN,
	ACT_RUNTOIDLE,
	

//===========================
// DoD Specific Activities
//===========================
	ACT_SPRINT,
	
	ACT_GET_DOWN_STAND,
	ACT_GET_UP_STAND,
	ACT_GET_DOWN_CROUCH,
	ACT_GET_UP_CROUCH,
	ACT_PRONE_FORWARD,
	ACT_PRONE_IDLE,

	ACT_DEEPIDLE1,
	ACT_DEEPIDLE2,
	ACT_DEEPIDLE3,
	ACT_DEEPIDLE4,

	ACT_VM_RELOAD_DEPLOYED, 
	ACT_VM_RELOAD_IDLE,

	ACT_VM_DRAW_DEPLOYED,

	//Weapon is empty activities
	ACT_VM_DRAW_EMPTY,
	ACT_VM_PRIMARYATTACK_EMPTY,
	ACT_VM_RELOAD_EMPTY,
	ACT_VM_IDLE_EMPTY,
	ACT_VM_IDLE_DEPLOYED_EMPTY,

	ACT_VM_IDLE_8,
	ACT_VM_IDLE_7,
	ACT_VM_IDLE_6,
	ACT_VM_IDLE_5,
	ACT_VM_IDLE_4,
	ACT_VM_IDLE_3,
	ACT_VM_IDLE_2,
	ACT_VM_IDLE_1,

	ACT_VM_IDLE_DEPLOYED,
	ACT_VM_IDLE_DEPLOYED_8,
	ACT_VM_IDLE_DEPLOYED_7,
	ACT_VM_IDLE_DEPLOYED_6,
	ACT_VM_IDLE_DEPLOYED_5,
	ACT_VM_IDLE_DEPLOYED_4,
	ACT_VM_IDLE_DEPLOYED_3,
	ACT_VM_IDLE_DEPLOYED_2,
	ACT_VM_IDLE_DEPLOYED_1,

	// Animation from prone idle to standing/crouch idle. Number designates bullets left
	ACT_VM_UNDEPLOY,
	ACT_VM_UNDEPLOY_8,
	ACT_VM_UNDEPLOY_7,
	ACT_VM_UNDEPLOY_6,
	ACT_VM_UNDEPLOY_5,
	ACT_VM_UNDEPLOY_4,
	ACT_VM_UNDEPLOY_3,
	ACT_VM_UNDEPLOY_2,
	ACT_VM_UNDEPLOY_1,
	ACT_VM_UNDEPLOY_EMPTY,

	// Animation from standing/crouch idle to prone idle. Number designates bullets left
	ACT_VM_DEPLOY,
	ACT_VM_DEPLOY_8,
	ACT_VM_DEPLOY_7,
	ACT_VM_DEPLOY_6,
	ACT_VM_DEPLOY_5,
	ACT_VM_DEPLOY_4,
	ACT_VM_DEPLOY_3,
	ACT_VM_DEPLOY_2,
	ACT_VM_DEPLOY_1,
	ACT_VM_DEPLOY_EMPTY,

	// Shooting animations for standing/crouch position.  Number designates bullets left at START of animation
	ACT_VM_PRIMARYATTACK_8,
	ACT_VM_PRIMARYATTACK_7,
	ACT_VM_PRIMARYATTACK_6,
	ACT_VM_PRIMARYATTACK_5,
	ACT_VM_PRIMARYATTACK_4,
	ACT_VM_PRIMARYATTACK_3,
	ACT_VM_PRIMARYATTACK_2,
	ACT_VM_PRIMARYATTACK_1,

	// Shooting animations for prone position. Number designates bullets left at START of animation
	ACT_VM_PRIMARYATTACK_DEPLOYED,
	ACT_VM_PRIMARYATTACK_DEPLOYED_8,
	ACT_VM_PRIMARYATTACK_DEPLOYED_7,
	ACT_VM_PRIMARYATTACK_DEPLOYED_6, 
	ACT_VM_PRIMARYATTACK_DEPLOYED_5,
	ACT_VM_PRIMARYATTACK_DEPLOYED_4,
	ACT_VM_PRIMARYATTACK_DEPLOYED_3,
	ACT_VM_PRIMARYATTACK_DEPLOYED_2,
	ACT_VM_PRIMARYATTACK_DEPLOYED_1,
	ACT_VM_PRIMARYATTACK_DEPLOYED_EMPTY,

	// Player anim ACTs
	ACT_DOD_DEPLOYED,
	ACT_DOD_PRONE_DEPLOYED,
	ACT_DOD_IDLE_ZOOMED,
	ACT_DOD_WALK_ZOOMED,
	ACT_DOD_CROUCH_ZOOMED,
	ACT_DOD_CROUCHWALK_ZOOMED,
	ACT_DOD_PRONE_ZOOMED,
	ACT_DOD_PRONE_FORWARD_ZOOMED,
	ACT_DOD_PRIMARYATTACK_DEPLOYED,
	ACT_DOD_PRIMARYATTACK_PRONE_DEPLOYED,
	ACT_DOD_RELOAD_DEPLOYED,
	ACT_DOD_RELOAD_PRONE_DEPLOYED,	
	ACT_DOD_PRIMARYATTACK_PRONE,
	ACT_DOD_SECONDARYATTACK_PRONE,
	ACT_DOD_RELOAD_CROUCH,
	ACT_DOD_RELOAD_PRONE,
	ACT_DOD_STAND_IDLE,
	ACT_DOD_STAND_AIM,
	ACT_DOD_CROUCH_IDLE,
	ACT_DOD_CROUCH_AIM,
	ACT_DOD_CROUCHWALK_IDLE,
	ACT_DOD_CROUCHWALK_AIM,
	ACT_DOD_WALK_IDLE,
	ACT_DOD_WALK_AIM,
	ACT_DOD_RUN_IDLE,
	ACT_DOD_RUN_AIM,

	// Positions
	ACT_DOD_STAND_AIM_PISTOL,
	ACT_DOD_CROUCH_AIM_PISTOL,
	ACT_DOD_CROUCHWALK_AIM_PISTOL,
	ACT_DOD_WALK_AIM_PISTOL,
	ACT_DOD_RUN_AIM_PISTOL,
	ACT_DOD_PRONE_AIM_PISTOL,
	ACT_DOD_STAND_IDLE_PISTOL,
	ACT_DOD_CROUCH_IDLE_PISTOL,
	ACT_DOD_CROUCHWALK_IDLE_PISTOL,
	ACT_DOD_WALK_IDLE_PISTOL,
	ACT_DOD_RUN_IDLE_PISTOL,
	ACT_DOD_SPRINT_IDLE_PISTOL,
	ACT_DOD_PRONEWALK_IDLE_PISTOL,

	ACT_DOD_STAND_AIM_C96,
	ACT_DOD_CROUCH_AIM_C96,
	ACT_DOD_CROUCHWALK_AIM_C96,
	ACT_DOD_WALK_AIM_C96,
	ACT_DOD_RUN_AIM_C96,
	ACT_DOD_PRONE_AIM_C96,
	ACT_DOD_STAND_IDLE_C96,
	ACT_DOD_CROUCH_IDLE_C96,
	ACT_DOD_CROUCHWALK_IDLE_C96,
	ACT_DOD_WALK_IDLE_C96,
	ACT_DOD_RUN_IDLE_C96,
	ACT_DOD_SPRINT_IDLE_C96,
	ACT_DOD_PRONEWALK_IDLE_C96,

	ACT_DOD_STAND_AIM_RIFLE,
	ACT_DOD_CROUCH_AIM_RIFLE,
	ACT_DOD_CROUCHWALK_AIM_RIFLE,
	ACT_DOD_WALK_AIM_RIFLE,
	ACT_DOD_RUN_AIM_RIFLE,
	ACT_DOD_PRONE_AIM_RIFLE,
	ACT_DOD_STAND_IDLE_RIFLE,
	ACT_DOD_CROUCH_IDLE_RIFLE,
	ACT_DOD_CROUCHWALK_IDLE_RIFLE,
	ACT_DOD_WALK_IDLE_RIFLE,
	ACT_DOD_RUN_IDLE_RIFLE,
	ACT_DOD_SPRINT_IDLE_RIFLE,
	ACT_DOD_PRONEWALK_IDLE_RIFLE,

	ACT_DOD_STAND_AIM_BOLT,
	ACT_DOD_CROUCH_AIM_BOLT,
	ACT_DOD_CROUCHWALK_AIM_BOLT,
	ACT_DOD_WALK_AIM_BOLT,
	ACT_DOD_RUN_AIM_BOLT,
	ACT_DOD_PRONE_AIM_BOLT,
	ACT_DOD_STAND_IDLE_BOLT,
	ACT_DOD_CROUCH_IDLE_BOLT,
	ACT_DOD_CROUCHWALK_IDLE_BOLT,
	ACT_DOD_WALK_IDLE_BOLT,
	ACT_DOD_RUN_IDLE_BOLT,
	ACT_DOD_SPRINT_IDLE_BOLT,
	ACT_DOD_PRONEWALK_IDLE_BOLT,

	ACT_DOD_STAND_AIM_TOMMY,
	ACT_DOD_CROUCH_AIM_TOMMY,
	ACT_DOD_CROUCHWALK_AIM_TOMMY,
	ACT_DOD_WALK_AIM_TOMMY,
	ACT_DOD_RUN_AIM_TOMMY,
	ACT_DOD_PRONE_AIM_TOMMY,
	ACT_DOD_STAND_IDLE_TOMMY,
	ACT_DOD_CROUCH_IDLE_TOMMY,
	ACT_DOD_CROUCHWALK_IDLE_TOMMY,
	ACT_DOD_WALK_IDLE_TOMMY,
	ACT_DOD_RUN_IDLE_TOMMY,
	ACT_DOD_SPRINT_IDLE_TOMMY,
	ACT_DOD_PRONEWALK_IDLE_TOMMY,

	ACT_DOD_STAND_AIM_MP40,
	ACT_DOD_CROUCH_AIM_MP40,
	ACT_DOD_CROUCHWALK_AIM_MP40,
	ACT_DOD_WALK_AIM_MP40,
	ACT_DOD_RUN_AIM_MP40,
	ACT_DOD_PRONE_AIM_MP40,
	ACT_DOD_STAND_IDLE_MP40,
	ACT_DOD_CROUCH_IDLE_MP40,
	ACT_DOD_CROUCHWALK_IDLE_MP40,
	ACT_DOD_WALK_IDLE_MP40,
	ACT_DOD_RUN_IDLE_MP40,
	ACT_DOD_SPRINT_IDLE_MP40,
	ACT_DOD_PRONEWALK_IDLE_MP40,

	ACT_DOD_STAND_AIM_MP44,
	ACT_DOD_CROUCH_AIM_MP44,
	ACT_DOD_CROUCHWALK_AIM_MP44,
	ACT_DOD_WALK_AIM_MP44,
	ACT_DOD_RUN_AIM_MP44,
	ACT_DOD_PRONE_AIM_MP44,
	ACT_DOD_STAND_IDLE_MP44,
	ACT_DOD_CROUCH_IDLE_MP44,
	ACT_DOD_CROUCHWALK_IDLE_MP44,
	ACT_DOD_WALK_IDLE_MP44,
	ACT_DOD_RUN_IDLE_MP44,
	ACT_DOD_SPRINT_IDLE_MP44,
	ACT_DOD_PRONEWALK_IDLE_MP44,

	ACT_DOD_STAND_AIM_GREASE,
	ACT_DOD_CROUCH_AIM_GREASE,
	ACT_DOD_CROUCHWALK_AIM_GREASE,
	ACT_DOD_WALK_AIM_GREASE,
	ACT_DOD_RUN_AIM_GREASE,
	ACT_DOD_PRONE_AIM_GREASE,
	ACT_DOD_STAND_IDLE_GREASE,
	ACT_DOD_CROUCH_IDLE_GREASE,
	ACT_DOD_CROUCHWALK_IDLE_GREASE,
	ACT_DOD_WALK_IDLE_GREASE,
	ACT_DOD_RUN_IDLE_GREASE,
	ACT_DOD_SPRINT_IDLE_GREASE,
	ACT_DOD_PRONEWALK_IDLE_GREASE,

	ACT_DOD_STAND_AIM_MG,
	ACT_DOD_CROUCH_AIM_MG,
	ACT_DOD_CROUCHWALK_AIM_MG,
	ACT_DOD_WALK_AIM_MG,
	ACT_DOD_RUN_AIM_MG,
	ACT_DOD_PRONE_AIM_MG,
	ACT_DOD_STAND_IDLE_MG,
	ACT_DOD_CROUCH_IDLE_MG,
	ACT_DOD_CROUCHWALK_IDLE_MG,
	ACT_DOD_WALK_IDLE_MG,
	ACT_DOD_RUN_IDLE_MG,
	ACT_DOD_SPRINT_IDLE_MG,
	ACT_DOD_PRONEWALK_IDLE_MG,

	ACT_DOD_STAND_AIM_30CAL,
	ACT_DOD_CROUCH_AIM_30CAL,
	ACT_DOD_CROUCHWALK_AIM_30CAL,
	ACT_DOD_WALK_AIM_30CAL,
	ACT_DOD_RUN_AIM_30CAL,
	ACT_DOD_PRONE_AIM_30CAL,
	ACT_DOD_STAND_IDLE_30CAL,
	ACT_DOD_CROUCH_IDLE_30CAL,
	ACT_DOD_CROUCHWALK_IDLE_30CAL,
	ACT_DOD_WALK_IDLE_30CAL,
	ACT_DOD_RUN_IDLE_30CAL,
	ACT_DOD_SPRINT_IDLE_30CAL,
	ACT_DOD_PRONEWALK_IDLE_30CAL,

	ACT_DOD_STAND_AIM_GREN_FRAG,
	ACT_DOD_CROUCH_AIM_GREN_FRAG,
	ACT_DOD_CROUCHWALK_AIM_GREN_FRAG,
	ACT_DOD_WALK_AIM_GREN_FRAG,
	ACT_DOD_RUN_AIM_GREN_FRAG,
	ACT_DOD_PRONE_AIM_GREN_FRAG,
	ACT_DOD_SPRINT_AIM_GREN_FRAG,
	ACT_DOD_PRONEWALK_AIM_GREN_FRAG,
	ACT_DOD_STAND_AIM_GREN_STICK,
	ACT_DOD_CROUCH_AIM_GREN_STICK,
	ACT_DOD_CROUCHWALK_AIM_GREN_STICK,
	ACT_DOD_WALK_AIM_GREN_STICK,
	ACT_DOD_RUN_AIM_GREN_STICK,
	ACT_DOD_PRONE_AIM_GREN_STICK,
	ACT_DOD_SPRINT_AIM_GREN_STICK,
	ACT_DOD_PRONEWALK_AIM_GREN_STICK,

	ACT_DOD_STAND_AIM_KNIFE,
	ACT_DOD_CROUCH_AIM_KNIFE,
	ACT_DOD_CROUCHWALK_AIM_KNIFE,
	ACT_DOD_WALK_AIM_KNIFE,
	ACT_DOD_RUN_AIM_KNIFE,
	ACT_DOD_PRONE_AIM_KNIFE,
	ACT_DOD_SPRINT_AIM_KNIFE,
	ACT_DOD_PRONEWALK_AIM_KNIFE,

	ACT_DOD_STAND_AIM_SPADE,
	ACT_DOD_CROUCH_AIM_SPADE,
	ACT_DOD_CROUCHWALK_AIM_SPADE,
	ACT_DOD_WALK_AIM_SPADE,
	ACT_DOD_RUN_AIM_SPADE,
	ACT_DOD_PRONE_AIM_SPADE,
	ACT_DOD_SPRINT_AIM_SPADE,
	ACT_DOD_PRONEWALK_AIM_SPADE,

	ACT_DOD_STAND_AIM_BAZOOKA,
	ACT_DOD_CROUCH_AIM_BAZOOKA,
	ACT_DOD_CROUCHWALK_AIM_BAZOOKA,
	ACT_DOD_WALK_AIM_BAZOOKA,
	ACT_DOD_RUN_AIM_BAZOOKA,
	ACT_DOD_PRONE_AIM_BAZOOKA,
	ACT_DOD_STAND_IDLE_BAZOOKA,
	ACT_DOD_CROUCH_IDLE_BAZOOKA,
	ACT_DOD_CROUCHWALK_IDLE_BAZOOKA,
	ACT_DOD_WALK_IDLE_BAZOOKA,
	ACT_DOD_RUN_IDLE_BAZOOKA,
	ACT_DOD_SPRINT_IDLE_BAZOOKA,
	ACT_DOD_PRONEWALK_IDLE_BAZOOKA,

	ACT_DOD_STAND_AIM_PSCHRECK,
	ACT_DOD_CROUCH_AIM_PSCHRECK,
	ACT_DOD_CROUCHWALK_AIM_PSCHRECK,
	ACT_DOD_WALK_AIM_PSCHRECK,
	ACT_DOD_RUN_AIM_PSCHRECK,
	ACT_DOD_PRONE_AIM_PSCHRECK,
	ACT_DOD_STAND_IDLE_PSCHRECK,
	ACT_DOD_CROUCH_IDLE_PSCHRECK,
	ACT_DOD_CROUCHWALK_IDLE_PSCHRECK,
	ACT_DOD_WALK_IDLE_PSCHRECK,
	ACT_DOD_RUN_IDLE_PSCHRECK,
	ACT_DOD_SPRINT_IDLE_PSCHRECK,
	ACT_DOD_PRONEWALK_IDLE_PSCHRECK,

	ACT_DOD_STAND_AIM_BAR,
	ACT_DOD_CROUCH_AIM_BAR,
	ACT_DOD_CROUCHWALK_AIM_BAR,
	ACT_DOD_WALK_AIM_BAR,
	ACT_DOD_RUN_AIM_BAR,
	ACT_DOD_PRONE_AIM_BAR,
	ACT_DOD_STAND_IDLE_BAR,
	ACT_DOD_CROUCH_IDLE_BAR,
	ACT_DOD_CROUCHWALK_IDLE_BAR,
	ACT_DOD_WALK_IDLE_BAR,
	ACT_DOD_RUN_IDLE_BAR,
	ACT_DOD_SPRINT_IDLE_BAR,
	ACT_DOD_PRONEWALK_IDLE_BAR,

	// Zoomed aims
	ACT_DOD_STAND_ZOOM_RIFLE,
	ACT_DOD_CROUCH_ZOOM_RIFLE,
	ACT_DOD_CROUCHWALK_ZOOM_RIFLE,
	ACT_DOD_WALK_ZOOM_RIFLE,
	ACT_DOD_RUN_ZOOM_RIFLE,
	ACT_DOD_PRONE_ZOOM_RIFLE,

	ACT_DOD_STAND_ZOOM_BOLT,
	ACT_DOD_CROUCH_ZOOM_BOLT,
	ACT_DOD_CROUCHWALK_ZOOM_BOLT,
	ACT_DOD_WALK_ZOOM_BOLT,
	ACT_DOD_RUN_ZOOM_BOLT,
	ACT_DOD_PRONE_ZOOM_BOLT,

	ACT_DOD_STAND_ZOOM_BAZOOKA,
	ACT_DOD_CROUCH_ZOOM_BAZOOKA,
	ACT_DOD_CROUCHWALK_ZOOM_BAZOOKA,
	ACT_DOD_WALK_ZOOM_BAZOOKA,
	ACT_DOD_RUN_ZOOM_BAZOOKA,
	ACT_DOD_PRONE_ZOOM_BAZOOKA,

	ACT_DOD_STAND_ZOOM_PSCHRECK,
	ACT_DOD_CROUCH_ZOOM_PSCHRECK,
	ACT_DOD_CROUCHWALK_ZOOM_PSCHRECK,
	ACT_DOD_WALK_ZOOM_PSCHRECK,
	ACT_DOD_RUN_ZOOM_PSCHRECK,
	ACT_DOD_PRONE_ZOOM_PSCHRECK,

	// Deployed Aim
	ACT_DOD_DEPLOY_RIFLE,
	ACT_DOD_DEPLOY_TOMMY,
	ACT_DOD_DEPLOY_MG,
	ACT_DOD_DEPLOY_30CAL,

	// Prone Deployed Aim
	ACT_DOD_PRONE_DEPLOY_RIFLE ,
	ACT_DOD_PRONE_DEPLOY_TOMMY,
	ACT_DOD_PRONE_DEPLOY_MG,
	ACT_DOD_PRONE_DEPLOY_30CAL,

	// Attacks

	// Rifle
	ACT_DOD_PRIMARYATTACK_RIFLE,
	ACT_DOD_SECONDARYATTACK_RIFLE,
	ACT_DOD_PRIMARYATTACK_PRONE_RIFLE,
	ACT_DOD_SECONDARYATTACK_PRONE_RIFLE,
	ACT_DOD_PRIMARYATTACK_PRONE_DEPLOYED_RIFLE,
	ACT_DOD_PRIMARYATTACK_DEPLOYED_RIFLE,

	// Bolt
	ACT_DOD_PRIMARYATTACK_BOLT,
	ACT_DOD_SECONDARYATTACK_BOLT,
	ACT_DOD_PRIMARYATTACK_PRONE_BOLT ,
	ACT_DOD_SECONDARYATTACK_PRONE_BOLT ,

	// Tommy
	ACT_DOD_PRIMARYATTACK_TOMMY,
	ACT_DOD_PRIMARYATTACK_PRONE_TOMMY,
	ACT_DOD_SECONDARYATTACK_TOMMY,
	ACT_DOD_SECONDARYATTACK_PRONE_TOMMY,

	// MP40
	ACT_DOD_PRIMARYATTACK_MP40,
	ACT_DOD_PRIMARYATTACK_PRONE_MP40 ,
	ACT_DOD_SECONDARYATTACK_MP40,
	ACT_DOD_SECONDARYATTACK_PRONE_MP40 ,

	// MP44
	ACT_DOD_PRIMARYATTACK_MP44,
	ACT_DOD_PRIMARYATTACK_PRONE_MP44 ,

	// Greasegun
	ACT_DOD_PRIMARYATTACK_GREASE,
	ACT_DOD_PRIMARYATTACK_PRONE_GREASE ,

	// Pistols (Colt, Luger)
	ACT_DOD_PRIMARYATTACK_PISTOL,
	ACT_DOD_PRIMARYATTACK_PRONE_PISTOL ,
	ACT_DOD_PRIMARYATTACK_C96,
	ACT_DOD_PRIMARYATTACK_PRONE_C96,

	// Mgs (mg42, mg34)
	ACT_DOD_PRIMARYATTACK_MG,
	ACT_DOD_PRIMARYATTACK_PRONE_MG ,
	ACT_DOD_PRIMARYATTACK_PRONE_DEPLOYED_MG ,
	ACT_DOD_PRIMARYATTACK_DEPLOYED_MG ,

	// 30cal
	ACT_DOD_PRIMARYATTACK_30CAL,
	ACT_DOD_PRIMARYATTACK_PRONE_30CAL,
	ACT_DOD_PRIMARYATTACK_DEPLOYED_30CAL,
	ACT_DOD_PRIMARYATTACK_PRONE_DEPLOYED_30CAL ,

	// Grenades
	ACT_DOD_PRIMARYATTACK_GREN_FRAG,
	ACT_DOD_PRIMARYATTACK_PRONE_GREN_FRAG,
	ACT_DOD_PRIMARYATTACK_GREN_STICK,
	ACT_DOD_PRIMARYATTACK_PRONE_GREN_STICK,

	// Knife
	ACT_DOD_PRIMARYATTACK_KNIFE,
	ACT_DOD_PRIMARYATTACK_PRONE_KNIFE,

	// Spade
	ACT_DOD_PRIMARYATTACK_SPADE,
	ACT_DOD_PRIMARYATTACK_PRONE_SPADE,

	// Bazooka
	ACT_DOD_PRIMARYATTACK_BAZOOKA,
	ACT_DOD_PRIMARYATTACK_PRONE_BAZOOKA,

	// Pschreck
	ACT_DOD_PRIMARYATTACK_PSCHRECK,
	ACT_DOD_PRIMARYATTACK_PRONE_PSCHRECK ,

	// Bar
	ACT_DOD_PRIMARYATTACK_BAR,
	ACT_DOD_PRIMARYATTACK_PRONE_BAR,

	// Reloads
	ACT_DOD_RELOAD_GARAND,
	ACT_DOD_RELOAD_K43,
	ACT_DOD_RELOAD_BAR,
	ACT_DOD_RELOAD_MP40,
	ACT_DOD_RELOAD_MP44,
	ACT_DOD_RELOAD_BOLT,
	ACT_DOD_RELOAD_M1CARBINE,
	ACT_DOD_RELOAD_TOMMY,
	ACT_DOD_RELOAD_GREASEGUN,
	ACT_DOD_RELOAD_PISTOL,
	ACT_DOD_RELOAD_FG42,
	ACT_DOD_RELOAD_RIFLE,
	ACT_DOD_RELOAD_RIFLEGRENADE,
	ACT_DOD_RELOAD_C96,

	// Crouch
	ACT_DOD_RELOAD_CROUCH_BAR,
	ACT_DOD_RELOAD_CROUCH_RIFLE,
	ACT_DOD_RELOAD_CROUCH_RIFLEGRENADE,
	ACT_DOD_RELOAD_CROUCH_BOLT,
	ACT_DOD_RELOAD_CROUCH_MP44,
	ACT_DOD_RELOAD_CROUCH_MP40,
	ACT_DOD_RELOAD_CROUCH_TOMMY,
	ACT_DOD_RELOAD_CROUCH_BAZOOKA,
	ACT_DOD_RELOAD_CROUCH_PSCHRECK,
	ACT_DOD_RELOAD_CROUCH_PISTOL,
	ACT_DOD_RELOAD_CROUCH_M1CARBINE,
	ACT_DOD_RELOAD_CROUCH_C96,

	// Bazookas
	ACT_DOD_RELOAD_BAZOOKA,
	ACT_DOD_ZOOMLOAD_BAZOOKA,
	ACT_DOD_RELOAD_PSCHRECK,
	ACT_DOD_ZOOMLOAD_PSCHRECK,

	// Deployed
	ACT_DOD_RELOAD_DEPLOYED_FG42,
	ACT_DOD_RELOAD_DEPLOYED_30CAL,
	ACT_DOD_RELOAD_DEPLOYED_MG,
	ACT_DOD_RELOAD_DEPLOYED_MG34,
	ACT_DOD_RELOAD_DEPLOYED_BAR,

	// Prone
	ACT_DOD_RELOAD_PRONE_PISTOL,
	ACT_DOD_RELOAD_PRONE_GARAND,
	ACT_DOD_RELOAD_PRONE_M1CARBINE,
	ACT_DOD_RELOAD_PRONE_BOLT,
	ACT_DOD_RELOAD_PRONE_K43,
	ACT_DOD_RELOAD_PRONE_MP40,
	ACT_DOD_RELOAD_PRONE_MP44,
	ACT_DOD_RELOAD_PRONE_BAR,
	ACT_DOD_RELOAD_PRONE_GREASEGUN,
	ACT_DOD_RELOAD_PRONE_TOMMY,
	ACT_DOD_RELOAD_PRONE_FG42,
	ACT_DOD_RELOAD_PRONE_RIFLE,
	ACT_DOD_RELOAD_PRONE_RIFLEGRENADE,
	ACT_DOD_RELOAD_PRONE_C96,

	// Prone bazooka
	ACT_DOD_RELOAD_PRONE_BAZOOKA,
	ACT_DOD_ZOOMLOAD_PRONE_BAZOOKA,
	ACT_DOD_RELOAD_PRONE_PSCHRECK,
	ACT_DOD_ZOOMLOAD_PRONE_PSCHRECK,

	// Prone deployed
	ACT_DOD_RELOAD_PRONE_DEPLOYED_BAR,
	ACT_DOD_RELOAD_PRONE_DEPLOYED_FG42,
	ACT_DOD_RELOAD_PRONE_DEPLOYED_30CAL,
	ACT_DOD_RELOAD_PRONE_DEPLOYED_MG,
	ACT_DOD_RELOAD_PRONE_DEPLOYED_MG34,

	// Prone zoomed aim
	ACT_DOD_PRONE_ZOOM_FORWARD_RIFLE,
	ACT_DOD_PRONE_ZOOM_FORWARD_BOLT,
	ACT_DOD_PRONE_ZOOM_FORWARD_BAZOOKA,
	ACT_DOD_PRONE_ZOOM_FORWARD_PSCHRECK,

	// Crouch attack
	ACT_DOD_PRIMARYATTACK_CROUCH,
	ACT_DOD_PRIMARYATTACK_CROUCH_SPADE,
	ACT_DOD_PRIMARYATTACK_CROUCH_KNIFE,
	ACT_DOD_PRIMARYATTACK_CROUCH_GREN_FRAG,
	ACT_DOD_PRIMARYATTACK_CROUCH_GREN_STICK,
	ACT_DOD_SECONDARYATTACK_CROUCH,
	ACT_DOD_SECONDARYATTACK_CROUCH_TOMMY,
	ACT_DOD_SECONDARYATTACK_CROUCH_MP40,

	// Hand Signals
	ACT_DOD_HS_IDLE,
	ACT_DOD_HS_CROUCH,
	ACT_DOD_HS_IDLE_30CAL,
	ACT_DOD_HS_IDLE_BAZOOKA,
	ACT_DOD_HS_IDLE_PSCHRECK,
	ACT_DOD_HS_IDLE_KNIFE,
	ACT_DOD_HS_IDLE_MG42,
	ACT_DOD_HS_IDLE_PISTOL,
	ACT_DOD_HS_IDLE_STICKGRENADE,
	ACT_DOD_HS_IDLE_TOMMY,
	ACT_DOD_HS_IDLE_MP44,
	ACT_DOD_HS_IDLE_K98,
	ACT_DOD_HS_CROUCH_30CAL,
	ACT_DOD_HS_CROUCH_BAZOOKA,
	ACT_DOD_HS_CROUCH_PSCHRECK,
	ACT_DOD_HS_CROUCH_KNIFE,
	ACT_DOD_HS_CROUCH_MG42,
	ACT_DOD_HS_CROUCH_PISTOL,
	ACT_DOD_HS_CROUCH_STICKGRENADE,
	ACT_DOD_HS_CROUCH_TOMMY,
	ACT_DOD_HS_CROUCH_MP44,
	ACT_DOD_HS_CROUCH_K98,

	ACT_DOD_STAND_IDLE_TNT,
	ACT_DOD_CROUCH_IDLE_TNT,
	ACT_DOD_CROUCHWALK_IDLE_TNT,
	ACT_DOD_WALK_IDLE_TNT,
	ACT_DOD_RUN_IDLE_TNT,
	ACT_DOD_SPRINT_IDLE_TNT,
	ACT_DOD_PRONEWALK_IDLE_TNT,

	ACT_DOD_PLANT_TNT,
	ACT_DOD_DEFUSE_TNT,

// HL2MP
	ACT_HL2MP_IDLE,
	ACT_HL2MP_RUN,
	ACT_HL2MP_IDLE_CROUCH,
	ACT_HL2MP_WALK_CROUCH,
	ACT_HL2MP_GESTURE_RANGE_ATTACK,
	ACT_HL2MP_GESTURE_RELOAD,
	ACT_HL2MP_JUMP,
	
	ACT_HL2MP_IDLE_PISTOL,
	ACT_HL2MP_RUN_PISTOL,
	ACT_HL2MP_IDLE_CROUCH_PISTOL,
	ACT_HL2MP_WALK_CROUCH_PISTOL,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,
	ACT_HL2MP_GESTURE_RELOAD_PISTOL,
	ACT_HL2MP_JUMP_PISTOL,

	ACT_HL2MP_IDLE_SMG1,
	ACT_HL2MP_RUN_SMG1,
	ACT_HL2MP_IDLE_CROUCH_SMG1,
	ACT_HL2MP_WALK_CROUCH_SMG1,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,
	ACT_HL2MP_GESTURE_RELOAD_SMG1,
	ACT_HL2MP_JUMP_SMG1,

  	ACT_HL2MP_IDLE_AR2,
	ACT_HL2MP_RUN_AR2,
	ACT_HL2MP_IDLE_CROUCH_AR2,
	ACT_HL2MP_WALK_CROUCH_AR2,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2,
	ACT_HL2MP_GESTURE_RELOAD_AR2,
	ACT_HL2MP_JUMP_AR2,

  	ACT_HL2MP_IDLE_SHOTGUN,
	ACT_HL2MP_RUN_SHOTGUN,
	ACT_HL2MP_IDLE_CROUCH_SHOTGUN,
	ACT_HL2MP_WALK_CROUCH_SHOTGUN,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,
	ACT_HL2MP_JUMP_SHOTGUN,

  	ACT_HL2MP_IDLE_RPG,
	ACT_HL2MP_RUN_RPG,
	ACT_HL2MP_IDLE_CROUCH_RPG,
	ACT_HL2MP_WALK_CROUCH_RPG,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_RPG,
	ACT_HL2MP_GESTURE_RELOAD_RPG,
	ACT_HL2MP_JUMP_RPG,

  	ACT_HL2MP_IDLE_GRENADE,
	ACT_HL2MP_RUN_GRENADE,
	ACT_HL2MP_IDLE_CROUCH_GRENADE,
	ACT_HL2MP_WALK_CROUCH_GRENADE,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,
	ACT_HL2MP_GESTURE_RELOAD_GRENADE,
	ACT_HL2MP_JUMP_GRENADE,

  	ACT_HL2MP_IDLE_PHYSGUN,
	ACT_HL2MP_RUN_PHYSGUN,
	ACT_HL2MP_IDLE_CROUCH_PHYSGUN,
	ACT_HL2MP_WALK_CROUCH_PHYSGUN,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_PHYSGUN,
	ACT_HL2MP_GESTURE_RELOAD_PHYSGUN,
	ACT_HL2MP_JUMP_PHYSGUN,

  	ACT_HL2MP_IDLE_CROSSBOW,
	ACT_HL2MP_RUN_CROSSBOW,
	ACT_HL2MP_IDLE_CROUCH_CROSSBOW,
	ACT_HL2MP_WALK_CROUCH_CROSSBOW,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,
	ACT_HL2MP_GESTURE_RELOAD_CROSSBOW,
	ACT_HL2MP_JUMP_CROSSBOW,

  	ACT_HL2MP_IDLE_MELEE,
	ACT_HL2MP_RUN_MELEE,
	ACT_HL2MP_IDLE_CROUCH_MELEE,
	ACT_HL2MP_WALK_CROUCH_MELEE,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,
	ACT_HL2MP_GESTURE_RELOAD_MELEE,
	ACT_HL2MP_JUMP_MELEE,

	ACT_HL2MP_IDLE_SLAM,
	ACT_HL2MP_RUN_SLAM,
	ACT_HL2MP_IDLE_CROUCH_SLAM,
	ACT_HL2MP_WALK_CROUCH_SLAM,
	ACT_HL2MP_GESTURE_RANGE_ATTACK_SLAM,
	ACT_HL2MP_GESTURE_RELOAD_SLAM,
	ACT_HL2MP_JUMP_SLAM,

// Portal!
	ACT_VM_FIZZLE,

	// Multiplayer
	ACT_MP_STAND_IDLE,
	ACT_MP_CROUCH_IDLE,
	ACT_MP_CROUCH_DEPLOYED_IDLE,
	ACT_MP_CROUCH_DEPLOYED,
	ACT_MP_CROUCHWALK_DEPLOYED,
	ACT_MP_DEPLOYED_IDLE,
	ACT_MP_RUN,
	ACT_MP_WALK,
	ACT_MP_AIRWALK,
	ACT_MP_CROUCHWALK,
	ACT_MP_SPRINT,
	ACT_MP_JUMP,
	ACT_MP_JUMP_START,
	ACT_MP_JUMP_FLOAT,
	ACT_MP_JUMP_LAND,
	ACT_MP_DOUBLEJUMP,
	ACT_MP_SWIM,
	ACT_MP_DEPLOYED,
	ACT_MP_SWIM_DEPLOYED,
	ACT_MP_VCD,

	ACT_MP_ATTACK_STAND_PRIMARYFIRE,
	ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_STAND_SECONDARYFIRE,
	ACT_MP_ATTACK_STAND_GRENADE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,
	ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,
	ACT_MP_ATTACK_CROUCH_GRENADE,
	ACT_MP_ATTACK_SWIM_PRIMARYFIRE,
	ACT_MP_ATTACK_SWIM_SECONDARYFIRE,
	ACT_MP_ATTACK_SWIM_GRENADE,
	ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,
	ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,
	ACT_MP_ATTACK_AIRWALK_GRENADE,
	ACT_MP_RELOAD_STAND,
	ACT_MP_RELOAD_STAND_LOOP,
	ACT_MP_RELOAD_STAND_END,
	ACT_MP_RELOAD_CROUCH,
	ACT_MP_RELOAD_CROUCH_LOOP,
	ACT_MP_RELOAD_CROUCH_END,
	ACT_MP_RELOAD_SWIM,
	ACT_MP_RELOAD_SWIM_LOOP,
	ACT_MP_RELOAD_SWIM_END,
	ACT_MP_RELOAD_AIRWALK,
	ACT_MP_RELOAD_AIRWALK_LOOP,
	ACT_MP_RELOAD_AIRWALK_END,
	ACT_MP_ATTACK_STAND_PREFIRE,
	ACT_MP_ATTACK_STAND_POSTFIRE,
	ACT_MP_ATTACK_STAND_STARTFIRE,
	ACT_MP_ATTACK_CROUCH_PREFIRE,
	ACT_MP_ATTACK_CROUCH_POSTFIRE,
	ACT_MP_ATTACK_SWIM_PREFIRE,
	ACT_MP_ATTACK_SWIM_POSTFIRE,

	// Multiplayer - Primary
	ACT_MP_STAND_PRIMARY,
	ACT_MP_CROUCH_PRIMARY,
	ACT_MP_RUN_PRIMARY,
	ACT_MP_WALK_PRIMARY,
	ACT_MP_AIRWALK_PRIMARY,
	ACT_MP_CROUCHWALK_PRIMARY,
	ACT_MP_JUMP_PRIMARY,
	ACT_MP_JUMP_START_PRIMARY,
	ACT_MP_JUMP_FLOAT_PRIMARY,
	ACT_MP_JUMP_LAND_PRIMARY,
	ACT_MP_SWIM_PRIMARY,
	ACT_MP_DEPLOYED_PRIMARY,
	ACT_MP_SWIM_DEPLOYED_PRIMARY,
	ACT_MP_CROUCHWALK_DEPLOYED_PRIMARY,
	ACT_MP_CROUCH_DEPLOYED_IDLE_PRIMARY,

	ACT_MP_ATTACK_STAND_PRIMARY,		// RUN, WALK
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_CROUCH_PRIMARY,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED,
	ACT_MP_ATTACK_SWIM_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_PRIMARY,

	ACT_MP_RELOAD_STAND_PRIMARY,		// RUN, WALK
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP,
	ACT_MP_RELOAD_STAND_PRIMARY_END,
	ACT_MP_RELOAD_CROUCH_PRIMARY,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END,
	ACT_MP_RELOAD_SWIM_PRIMARY,
	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP,
	ACT_MP_RELOAD_SWIM_PRIMARY_END,
	ACT_MP_RELOAD_AIRWALK_PRIMARY,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END,

	ACT_MP_RELOAD_STAND_PRIMARY_2,
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP_2,
	ACT_MP_RELOAD_STAND_PRIMARY_END_2,	
	ACT_MP_RELOAD_CROUCH_PRIMARY_2,	
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP_2,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END_2,
	ACT_MP_RELOAD_SWIM_PRIMARY_2,
	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP_2,
	ACT_MP_RELOAD_SWIM_PRIMARY_END_2,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_2,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP_2,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END_2,

	// PRIMARY ALT
	ACT_MP_ATTACK_STAND_PRIMARY_ALT,
	ACT_MP_ATTACK_CROUCH_PRIMARY_ALT,
	ACT_MP_ATTACK_SWIM_PRIMARY_ALT,
	ACT_MP_RELOAD_STAND_PRIMARY_ALT,
	ACT_MP_RELOAD_CROUCH_PRIMARY_ALT,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_ALT,
	ACT_MP_RELOAD_STAND_PRIMARY_LOOP_ALT,
	ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP_ALT,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP_ALT,
	ACT_MP_RELOAD_STAND_PRIMARY_END_ALT,
	ACT_MP_RELOAD_CROUCH_PRIMARY_END_ALT,
	ACT_MP_RELOAD_AIRWALK_PRIMARY_END_ALT,
	ACT_MP_RELOAD_SWIM_PRIMARY_ALT,
	ACT_MP_ATTACK_STAND_PRIMARY_SUPER,
	ACT_MP_ATTACK_CROUCH_PRIMARY_SUPER,
	ACT_MP_ATTACK_SWIM_PRIMARY_SUPER,

	ACT_MP_ATTACK_STAND_GRENADE_PRIMARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_PRIMARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_PRIMARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_PRIMARY,

	// Secondary
	ACT_MP_STAND_SECONDARY,
	ACT_MP_CROUCH_SECONDARY,
	ACT_MP_RUN_SECONDARY,
	ACT_MP_WALK_SECONDARY,
	ACT_MP_AIRWALK_SECONDARY,
	ACT_MP_CROUCHWALK_SECONDARY,
	ACT_MP_JUMP_SECONDARY,
	ACT_MP_JUMP_START_SECONDARY,
	ACT_MP_JUMP_FLOAT_SECONDARY,
	ACT_MP_JUMP_LAND_SECONDARY,
	ACT_MP_SWIM_SECONDARY,

	ACT_MP_ATTACK_STAND_SECONDARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_SECONDARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_SECONDARY,

	ACT_MP_RELOAD_STAND_SECONDARY,		// RUN, WALK
	ACT_MP_RELOAD_STAND_SECONDARY_LOOP,
	ACT_MP_RELOAD_STAND_SECONDARY_END,
	ACT_MP_RELOAD_CROUCH_SECONDARY,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP,
	ACT_MP_RELOAD_CROUCH_SECONDARY_END,
	ACT_MP_RELOAD_SWIM_SECONDARY,
	ACT_MP_RELOAD_SWIM_SECONDARY_LOOP,
	ACT_MP_RELOAD_SWIM_SECONDARY_END,
	ACT_MP_RELOAD_AIRWALK_SECONDARY,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_END,

	ACT_MP_RELOAD_STAND_SECONDARY_2,
	ACT_MP_RELOAD_CROUCH_SECONDARY_2,
	ACT_MP_RELOAD_SWIM_SECONDARY_2,
	ACT_MP_RELOAD_AIRWALK_SECONDARY_2,

	ACT_MP_ATTACK_STAND_GRENADE_SECONDARY,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_SECONDARY,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_SECONDARY,
	ACT_MP_ATTACK_AIRWALK_GRENADE_SECONDARY,

	// Secondary2
	ACT_MP_STAND_SECONDARY2,
	ACT_MP_CROUCH_SECONDARY2,
	ACT_MP_RUN_SECONDARY2,
	ACT_MP_WALK_SECONDARY2,
	ACT_MP_AIRWALK_SECONDARY2,
	ACT_MP_CROUCHWALK_SECONDARY2,
	ACT_MP_JUMP_SECONDARY2,
	ACT_MP_JUMP_START_SECONDARY2,
	ACT_MP_JUMP_FLOAT_SECONDARY2,
	ACT_MP_JUMP_LAND_SECONDARY2,
	ACT_MP_SWIM_SECONDARY2,

	ACT_MP_ATTACK_STAND_SECONDARY2,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_SECONDARY2,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_SECONDARY2,
	ACT_MP_ATTACK_AIRWALK_SECONDARY2,

	ACT_MP_RELOAD_STAND_SECONDARY2,		// RUN, WALK
	ACT_MP_RELOAD_STAND_SECONDARY2_LOOP,
	ACT_MP_RELOAD_STAND_SECONDARY2_END,
	ACT_MP_RELOAD_CROUCH_SECONDARY2,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_SECONDARY2_LOOP,
	ACT_MP_RELOAD_CROUCH_SECONDARY2_END,
	ACT_MP_RELOAD_SWIM_SECONDARY2,
	ACT_MP_RELOAD_SWIM_SECONDARY2_LOOP,
	ACT_MP_RELOAD_SWIM_SECONDARY2_END,
	ACT_MP_RELOAD_AIRWALK_SECONDARY2,
	ACT_MP_RELOAD_AIRWALK_SECONDARY2_LOOP,
	ACT_MP_RELOAD_AIRWALK_SECONDARY2_END,

	// Melee
	ACT_MP_STAND_MELEE,
	ACT_MP_CROUCH_MELEE,
	ACT_MP_RUN_MELEE,
	ACT_MP_WALK_MELEE,
	ACT_MP_AIRWALK_MELEE,
	ACT_MP_CROUCHWALK_MELEE,
	ACT_MP_JUMP_MELEE,
	ACT_MP_JUMP_START_MELEE,
	ACT_MP_JUMP_FLOAT_MELEE,
	ACT_MP_JUMP_LAND_MELEE,
	ACT_MP_SWIM_MELEE,

	ACT_MP_ATTACK_STAND_MELEE,		// RUN, WALK
	ACT_MP_ATTACK_STAND_MELEE_SECONDARY,
	ACT_MP_ATTACK_CROUCH_MELEE,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,
	ACT_MP_ATTACK_SWIM_MELEE,
	ACT_MP_ATTACK_AIRWALK_MELEE,

	ACT_MP_ATTACK_STAND_GRENADE_MELEE,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_MELEE,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_MELEE,
	ACT_MP_ATTACK_AIRWALK_GRENADE_MELEE,

	// Item1
	ACT_MP_STAND_ITEM1,
	ACT_MP_CROUCH_ITEM1,
	ACT_MP_RUN_ITEM1,
	ACT_MP_WALK_ITEM1,
	ACT_MP_AIRWALK_ITEM1,
	ACT_MP_CROUCHWALK_ITEM1,
	ACT_MP_JUMP_ITEM1,
	ACT_MP_JUMP_START_ITEM1,
	ACT_MP_JUMP_FLOAT_ITEM1,
	ACT_MP_JUMP_LAND_ITEM1,
	ACT_MP_SWIM_ITEM1,

	ACT_MP_ATTACK_STAND_ITEM1,		// RUN, WALK
	ACT_MP_ATTACK_STAND_ITEM1_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM1,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM1,
	ACT_MP_ATTACK_AIRWALK_ITEM1,

	ACT_MP_DEPLOYED_ITEM1,
	ACT_MP_DEPLOYED_IDLE_ITEM1,
	ACT_MP_CROUCHWALK_DEPLOYED_ITEM1,
	ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM1,
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED_ITEM1,
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED_ITEM1,

	// Item2
	ACT_MP_STAND_ITEM2,
	ACT_MP_CROUCH_ITEM2,
	ACT_MP_RUN_ITEM2,
	ACT_MP_WALK_ITEM2,
	ACT_MP_AIRWALK_ITEM2,
	ACT_MP_CROUCHWALK_ITEM2,
	ACT_MP_JUMP_ITEM2,
	ACT_MP_JUMP_START_ITEM2,
	ACT_MP_JUMP_FLOAT_ITEM2,
	ACT_MP_JUMP_LAND_ITEM2,
	ACT_MP_SWIM_ITEM2,

	ACT_MP_ATTACK_STAND_ITEM2,		// RUN, WALK
	ACT_MP_ATTACK_STAND_ITEM2_SECONDARY,
	ACT_MP_ATTACK_CROUCH_ITEM2,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY,
	ACT_MP_ATTACK_SWIM_ITEM2,
	ACT_MP_ATTACK_AIRWALK_ITEM2,

	ACT_MP_ATTACK_STAND_HARD_ITEM2,	
	ACT_MP_ATTACK_CROUCH_HARD_ITEM2,
	ACT_MP_ATTACK_SWIM_HARD_ITEM2,

	ACT_MP_DEPLOYED_ITEM2,
	ACT_MP_DEPLOYED_IDLE_ITEM2,
	ACT_MP_CROUCHWALK_DEPLOYED_ITEM2,
	ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM2,
	ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED_ITEM2,
	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED_ITEM2,

	ACT_MP_RELOAD_STAND_ITEM2,		// RUN, WALK
	ACT_MP_RELOAD_STAND_ITEM2_LOOP,
	ACT_MP_RELOAD_STAND_ITEM2_END,
	ACT_MP_RELOAD_CROUCH_ITEM2,		// CROUCHWALK
	ACT_MP_RELOAD_CROUCH_ITEM2_LOOP,
	ACT_MP_RELOAD_CROUCH_ITEM2_END,
	ACT_MP_RELOAD_SWIM_ITEM2,
	ACT_MP_RELOAD_SWIM_ITEM2_LOOP,
	ACT_MP_RELOAD_SWIM_ITEM2_END,
	ACT_MP_RELOAD_AIRWALK_ITEM2,
	ACT_MP_RELOAD_AIRWALK_ITEM2_LOOP,
	ACT_MP_RELOAD_AIRWALK_ITEM2_END,
	ACT_MP_RELOAD_NO_AMMO_ITEM2,

	ACT_MP_ATTACK_STAND_GRENADE_ITEM2,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_ITEM2,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_ITEM2,
	ACT_MP_ATTACK_AIRWALK_GRENADE_ITEM2,

	// Passtime
	ACT_MP_STAND_PASSTIME,
	ACT_MP_RUN_PASSTIME,
	ACT_MP_CROUCHWALK_PASSTIME,

	// Flinches
	ACT_MP_GESTURE_FLINCH,
	ACT_MP_GESTURE_FLINCH_PRIMARY,
	ACT_MP_GESTURE_FLINCH_SECONDARY,
	ACT_MP_GESTURE_FLINCH_MELEE,
	ACT_MP_GESTURE_FLINCH_ITEM1,
	ACT_MP_GESTURE_FLINCH_ITEM2,

	ACT_MP_GESTURE_FLINCH_HEAD,
	ACT_MP_GESTURE_FLINCH_CHEST,
	ACT_MP_GESTURE_FLINCH_STOMACH,
	ACT_MP_GESTURE_FLINCH_LEFTARM,
	ACT_MP_GESTURE_FLINCH_RIGHTARM,
	ACT_MP_GESTURE_FLINCH_LEFTLEG,
	ACT_MP_GESTURE_FLINCH_RIGHTLEG,

// Team Fortress specific - medic heal, medic infect, etc.....
	ACT_MP_GRENADE1_DRAW,
	ACT_MP_GRENADE1_IDLE,
	ACT_MP_GRENADE1_ATTACK,
	ACT_MP_GRENADE2_DRAW,
	ACT_MP_GRENADE2_IDLE,
	ACT_MP_GRENADE2_ATTACK,

	ACT_MP_PRIMARY_GRENADE1_DRAW,
	ACT_MP_PRIMARY_GRENADE1_IDLE,
	ACT_MP_PRIMARY_GRENADE1_ATTACK,
	ACT_MP_PRIMARY_GRENADE2_DRAW,
	ACT_MP_PRIMARY_GRENADE2_IDLE,
	ACT_MP_PRIMARY_GRENADE2_ATTACK,

	ACT_MP_SECONDARY_GRENADE1_DRAW,
	ACT_MP_SECONDARY_GRENADE1_IDLE,
	ACT_MP_SECONDARY_GRENADE1_ATTACK,
	ACT_MP_SECONDARY_GRENADE2_DRAW,
	ACT_MP_SECONDARY_GRENADE2_IDLE,
	ACT_MP_SECONDARY_GRENADE2_ATTACK,

	ACT_MP_MELEE_GRENADE1_DRAW,
	ACT_MP_MELEE_GRENADE1_IDLE,
	ACT_MP_MELEE_GRENADE1_ATTACK,
	ACT_MP_MELEE_GRENADE2_DRAW,
	ACT_MP_MELEE_GRENADE2_IDLE,
	ACT_MP_MELEE_GRENADE2_ATTACK,

	ACT_MP_ITEM1_GRENADE1_DRAW,
	ACT_MP_ITEM1_GRENADE1_IDLE,
	ACT_MP_ITEM1_GRENADE1_ATTACK,
	ACT_MP_ITEM1_GRENADE2_DRAW,
	ACT_MP_ITEM1_GRENADE2_IDLE,
	ACT_MP_ITEM1_GRENADE2_ATTACK,

	ACT_MP_ITEM2_GRENADE1_DRAW,
	ACT_MP_ITEM2_GRENADE1_IDLE,
	ACT_MP_ITEM2_GRENADE1_ATTACK,
	ACT_MP_ITEM2_GRENADE2_DRAW,
	ACT_MP_ITEM2_GRENADE2_IDLE,
	ACT_MP_ITEM2_GRENADE2_ATTACK,

	// Building
	ACT_MP_STAND_BUILDING,
	ACT_MP_CROUCH_BUILDING,
	ACT_MP_RUN_BUILDING,
	ACT_MP_WALK_BUILDING,
	ACT_MP_AIRWALK_BUILDING,
	ACT_MP_CROUCHWALK_BUILDING,
	ACT_MP_JUMP_BUILDING,
	ACT_MP_JUMP_START_BUILDING,
	ACT_MP_JUMP_FLOAT_BUILDING,
	ACT_MP_JUMP_LAND_BUILDING,
	ACT_MP_SWIM_BUILDING,

	ACT_MP_ATTACK_STAND_BUILDING,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_BUILDING,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_BUILDING,
	ACT_MP_ATTACK_AIRWALK_BUILDING,

	ACT_MP_ATTACK_STAND_GRENADE_BUILDING,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_BUILDING,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_BUILDING,
	ACT_MP_ATTACK_AIRWALK_GRENADE_BUILDING,

	// Building
	ACT_MP_STAND_BUILDING_DEPLOYED,
	ACT_MP_CROUCH_BUILDING_DEPLOYED,
	ACT_MP_RUN_BUILDING_DEPLOYED,
	ACT_MP_WALK_BUILDING_DEPLOYED,
	ACT_MP_AIRWALK_BUILDING_DEPLOYED,
	ACT_MP_CROUCHWALK_BUILDING_DEPLOYED,
	ACT_MP_JUMP_BUILDING_DEPLOYED,
	ACT_MP_JUMP_START_BUILDING_DEPLOYED,
	ACT_MP_JUMP_FLOAT_BUILDING_DEPLOYED,
	ACT_MP_JUMP_LAND_BUILDING_DEPLOYED,
	ACT_MP_SWIM_BUILDING_DEPLOYED,

	ACT_MP_ATTACK_STAND_BUILDING_DEPLOYED,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_BUILDING_DEPLOYED,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_BUILDING_DEPLOYED,
	ACT_MP_ATTACK_AIRWALK_BUILDING_DEPLOYED,

	ACT_MP_ATTACK_STAND_GRENADE_BUILDING_DEPLOYED,		// RUN, WALK
	ACT_MP_ATTACK_CROUCH_GRENADE_BUILDING_DEPLOYED,		// CROUCHWALK
	ACT_MP_ATTACK_SWIM_GRENADE_BUILDING_DEPLOYED,
	ACT_MP_ATTACK_AIRWALK_GRENADE_BUILDING_DEPLOYED,

	ACT_MP_STAND_PDA,
	ACT_MP_CROUCH_PDA,
	ACT_MP_RUN_PDA,
	ACT_MP_WALK_PDA,
	ACT_MP_AIRWALK_PDA,
	ACT_MP_CROUCHWALK_PDA,
	ACT_MP_JUMP_PDA,
	ACT_MP_JUMP_START_PDA,
	ACT_MP_JUMP_FLOAT_PDA,
	ACT_MP_JUMP_LAND_PDA,
	ACT_MP_SWIM_PDA,

	ACT_MP_ATTACK_STAND_PDA,
	ACT_MP_ATTACK_SWIM_PDA,

	ACT_MP_STAND_LOSERSTATE,
	ACT_MP_CROUCH_LOSERSTATE,
	ACT_MP_RUN_LOSERSTATE,
	ACT_MP_WALK_LOSERSTATE,
	ACT_MP_AIRWALK_LOSERSTATE,
	ACT_MP_CROUCHWALK_LOSERSTATE,
	ACT_MP_JUMP_LOSERSTATE,
	ACT_MP_JUMP_START_LOSERSTATE,
	ACT_MP_JUMP_FLOAT_LOSERSTATE,
	ACT_MP_JUMP_LAND_LOSERSTATE,
	ACT_MP_SWIM_LOSERSTATE,
	ACT_MP_DOUBLEJUMP_LOSERSTATE,

	ACT_MP_DOUBLEJUMP_CROUCH,
	ACT_MP_DOUBLEJUMP_CROUCH_PRIMARY,
	ACT_MP_DOUBLEJUMP_CROUCH_SECONDARY,
	ACT_MP_DOUBLEJUMP_CROUCH_MELEE,
	ACT_MP_DOUBLEJUMP_CROUCH_ITEM1,
	ACT_MP_DOUBLEJUMP_CROUCH_ITEM2,
	ACT_MP_DOUBLEJUMP_CROUCH_LOSERSTATE,
	ACT_MP_DOUBLEJUMP_CROUCH_PASSTIME,

	ACT_MP_GESTURE_VC_HANDMOUTH,
	ACT_MP_GESTURE_VC_FINGERPOINT,
	ACT_MP_GESTURE_VC_FISTPUMP,
	ACT_MP_GESTURE_VC_THUMBSUP,
	ACT_MP_GESTURE_VC_NODYES,
	ACT_MP_GESTURE_VC_NODNO,

	ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY,
	ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY,
	ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY,
	ACT_MP_GESTURE_VC_NODYES_PRIMARY,
	ACT_MP_GESTURE_VC_NODNO_PRIMARY,

	ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY,
	ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY,
	ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY,
	ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY,
	ACT_MP_GESTURE_VC_NODYES_SECONDARY,
	ACT_MP_GESTURE_VC_NODNO_SECONDARY,

	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,
	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,
	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,
	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,
	ACT_MP_GESTURE_VC_NODYES_MELEE,
	ACT_MP_GESTURE_VC_NODNO_MELEE,

	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM1,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM1,
	ACT_MP_GESTURE_VC_NODYES_ITEM1,
	ACT_MP_GESTURE_VC_NODNO_ITEM1,

	ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2,
	ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2,
	ACT_MP_GESTURE_VC_FISTPUMP_ITEM2,
	ACT_MP_GESTURE_VC_THUMBSUP_ITEM2,
	ACT_MP_GESTURE_VC_NODYES_ITEM2,
	ACT_MP_GESTURE_VC_NODNO_ITEM2,

	ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING,
	ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING,
	ACT_MP_GESTURE_VC_FISTPUMP_BUILDING,
	ACT_MP_GESTURE_VC_THUMBSUP_BUILDING,
	ACT_MP_GESTURE_VC_NODYES_BUILDING,
	ACT_MP_GESTURE_VC_NODNO_BUILDING,

	ACT_MP_GESTURE_VC_HANDMOUTH_PDA,
	ACT_MP_GESTURE_VC_FINGERPOINT_PDA,
	ACT_MP_GESTURE_VC_FISTPUMP_PDA,
	ACT_MP_GESTURE_VC_THUMBSUP_PDA,
	ACT_MP_GESTURE_VC_NODYES_PDA,
	ACT_MP_GESTURE_VC_NODNO_PDA,

	ACT_MP_STUN_BEGIN,
	ACT_MP_STUN_MIDDLE,
	ACT_MP_STUN_END,

	ACT_MP_PASSTIME_THROW_BEGIN,
	ACT_MP_PASSTIME_THROW_MIDDLE,
	ACT_MP_PASSTIME_THROW_END,
	ACT_MP_PASSTIME_THROW_CANCEL,

	ACT_VM_UNUSABLE,
	ACT_VM_UNUSABLE_TO_USABLE,
	ACT_VM_USABLE_TO_UNUSABLE,

	// Specific viewmodel activities for weapon roles
	ACT_PRIMARY_VM_DRAW,
	ACT_PRIMARY_VM_HOLSTER,
	ACT_PRIMARY_VM_IDLE,
	ACT_PRIMARY_VM_PULLBACK,
	ACT_PRIMARY_VM_PRIMARYATTACK,
	ACT_PRIMARY_VM_SECONDARYATTACK,
	ACT_PRIMARY_VM_RELOAD,			
	ACT_PRIMARY_RELOAD_START,			
	ACT_PRIMARY_RELOAD_FINISH,			
	ACT_PRIMARY_VM_DRYFIRE,		
	ACT_PRIMARY_VM_IDLE_TO_LOWERED,
	ACT_PRIMARY_VM_IDLE_LOWERED,
	ACT_PRIMARY_VM_LOWERED_TO_IDLE,
	ACT_PRIMARY_VM_RELOAD_2,
	ACT_PRIMARY_RELOAD_START_2,		
	ACT_PRIMARY_RELOAD_FINISH_2,
	ACT_PRIMARY_VM_RELOAD_3,
	ACT_PRIMARY_RELOAD_START_3,		
	ACT_PRIMARY_RELOAD_FINISH_3,
	ACT_PRIMARY_VM_PRIMARYATTACK_3,

	ACT_SECONDARY_VM_DRAW,
	ACT_SECONDARY_VM_HOLSTER,
	ACT_SECONDARY_VM_IDLE,
	ACT_SECONDARY_VM_PULLBACK,
	ACT_SECONDARY_VM_PRIMARYATTACK,
	ACT_SECONDARY_VM_SECONDARYATTACK,
	ACT_SECONDARY_VM_RELOAD,			
	ACT_SECONDARY_RELOAD_START,
	ACT_SECONDARY_RELOAD_FINISH,
	ACT_SECONDARY_VM_RELOAD2,			
	ACT_SECONDARY_VM_DRYFIRE,		
	ACT_SECONDARY_VM_IDLE_TO_LOWERED,
	ACT_SECONDARY_VM_IDLE_LOWERED,
	ACT_SECONDARY_VM_LOWERED_TO_IDLE,

	ACT_SECONDARY_VM_DRAW_2,
	ACT_SECONDARY_VM_IDLE_2,
	ACT_SECONDARY_VM_PRIMARYATTACK_2,
	ACT_SECONDARY_VM_RELOAD_2,

	ACT_MELEE_VM_DRAW,
	ACT_MELEE_VM_HOLSTER,
	ACT_MELEE_VM_IDLE,
	ACT_MELEE_VM_PULLBACK,
	ACT_MELEE_VM_PRIMARYATTACK,
	ACT_MELEE_VM_SECONDARYATTACK,
	ACT_MELEE_VM_RELOAD,			
	ACT_MELEE_VM_DRYFIRE,		
	ACT_MELEE_VM_IDLE_TO_LOWERED,
	ACT_MELEE_VM_IDLE_LOWERED,
	ACT_MELEE_VM_LOWERED_TO_IDLE,
	ACT_MELEE_VM_STUN,
	ACT_MELEE_VM_HITCENTER,		
	ACT_MELEE_VM_SWINGHARD,

	ACT_PDA_VM_DRAW,
	ACT_PDA_VM_HOLSTER,
	ACT_PDA_VM_IDLE,
	ACT_PDA_VM_PULLBACK,
	ACT_PDA_VM_PRIMARYATTACK,
	ACT_PDA_VM_SECONDARYATTACK,
	ACT_PDA_VM_RELOAD,			
	ACT_PDA_VM_DRYFIRE,		
	ACT_PDA_VM_IDLE_TO_LOWERED,
	ACT_PDA_VM_IDLE_LOWERED,
	ACT_PDA_VM_LOWERED_TO_IDLE,

	ACT_ENGINEER_PDA1_VM_DRAW,
	ACT_ENGINEER_PDA2_VM_DRAW,
	ACT_ENGINEER_BLD_VM_DRAW,
	ACT_ENGINEER_PDA1_VM_IDLE,
	ACT_ENGINEER_PDA2_VM_IDLE,
	ACT_ENGINEER_BLD_VM_IDLE,

	ACT_ITEM1_VM_DRAW,
	ACT_ITEM1_VM_HOLSTER,
	ACT_ITEM1_VM_IDLE,
	ACT_ITEM1_VM_IDLE_2,
	ACT_ITEM1_VM_PULLBACK,
	ACT_ITEM1_VM_PRIMARYATTACK,
	ACT_ITEM1_VM_SECONDARYATTACK,
	ACT_ITEM1_VM_RELOAD,			
	ACT_ITEM1_VM_DRYFIRE,		
	ACT_ITEM1_VM_IDLE_TO_LOWERED,
	ACT_ITEM1_VM_IDLE_LOWERED,
	ACT_ITEM1_VM_LOWERED_TO_IDLE,
	ACT_ITEM1_RELOAD_START,			
	ACT_ITEM1_RELOAD_FINISH,			
	ACT_ITEM1_VM_HITCENTER,
	ACT_ITEM1_VM_SWINGHARD,
	ACT_ITEM1_BACKSTAB_VM_UP,
	ACT_ITEM1_BACKSTAB_VM_DOWN,
	ACT_ITEM1_BACKSTAB_VM_IDLE,
	ACT_MELEE_VM_ITEM1_STUN,

	ACT_ITEM2_VM_DRAW,
	ACT_ITEM2_VM_HOLSTER,
	ACT_ITEM2_VM_IDLE,
	ACT_ITEM2_VM_PULLBACK,
	ACT_ITEM2_VM_PRIMARYATTACK,
	ACT_ITEM2_VM_SECONDARYATTACK,
	ACT_ITEM2_VM_RELOAD,			
	ACT_ITEM2_VM_DRYFIRE,		
	ACT_ITEM2_VM_IDLE_TO_LOWERED,
	ACT_ITEM2_VM_IDLE_LOWERED,
	ACT_ITEM2_VM_LOWERED_TO_IDLE,
	ACT_ITEM2_VM_CHARGE,
	ACT_ITEM2_VM_IDLE_2,
	ACT_ITEM2_VM_IDLE_3,
	ACT_ITEM2_VM_CHARGE_IDLE_3,
	ACT_ITEM2_VM_HITCENTER,
	ACT_ITEM2_VM_SWINGHARD,
	ACT_ITEM2_BACKSTAB_VM_UP,
	ACT_ITEM2_BACKSTAB_VM_DOWN,
	ACT_ITEM2_BACKSTAB_VM_IDLE,
	ACT_MELEE_VM_ITEM2_STUN,

	ACT_ITEM3_VM_DRAW,
	ACT_ITEM3_VM_HOLSTER,
	ACT_ITEM3_VM_IDLE,
	ACT_ITEM3_VM_PULLBACK,
	ACT_ITEM3_VM_PRIMARYATTACK,
	ACT_ITEM3_VM_SECONDARYATTACK,
	ACT_ITEM3_VM_RELOAD,			
	ACT_ITEM3_VM_DRYFIRE,		
	ACT_ITEM3_VM_IDLE_TO_LOWERED,
	ACT_ITEM3_VM_IDLE_LOWERED,
	ACT_ITEM3_VM_LOWERED_TO_IDLE,
	ACT_ITEM3_VM_CHARGE,
	ACT_ITEM3_VM_IDLE_2,
	ACT_ITEM3_VM_IDLE_3,
	ACT_ITEM3_VM_CHARGE_IDLE_3,
	ACT_ITEM3_VM_HITCENTER,
	ACT_ITEM3_VM_SWINGHARD,

	ACT_SECONDARY2_VM_DRAW,
	ACT_SECONDARY2_VM_HOLSTER,
	ACT_SECONDARY2_VM_IDLE,
	ACT_SECONDARY2_VM_PULLBACK,
	ACT_SECONDARY2_VM_PRIMARYATTACK,
	ACT_SECONDARY2_VM_SECONDARY2ATTACK,
	ACT_SECONDARY2_VM_RELOAD,			
	ACT_SECONDARY2_RELOAD_START,
	ACT_SECONDARY2_RELOAD_FINISH,
	ACT_SECONDARY2_VM_RELOAD2,			
	ACT_SECONDARY2_VM_DRYFIRE,		
	ACT_SECONDARY2_VM_IDLE_TO_LOWERED,
	ACT_SECONDARY2_VM_IDLE_LOWERED,
	ACT_SECONDARY2_VM_LOWERED_TO_IDLE,
	
	ACT_BACKSTAB_VM_UP,
	ACT_BACKSTAB_VM_DOWN,
	ACT_BACKSTAB_VM_IDLE,

	ACT_PRIMARY_ATTACK_STAND_PREFIRE,
	ACT_PRIMARY_ATTACK_STAND_POSTFIRE,
	ACT_PRIMARY_ATTACK_STAND_STARTFIRE,
	ACT_PRIMARY_ATTACK_CROUCH_PREFIRE,
	ACT_PRIMARY_ATTACK_CROUCH_POSTFIRE,
	ACT_PRIMARY_ATTACK_SWIM_PREFIRE,
	ACT_PRIMARY_ATTACK_SWIM_POSTFIRE,

	ACT_SECONDARY_ATTACK_STAND_PREFIRE,
	ACT_SECONDARY_ATTACK_STAND_POSTFIRE,
	ACT_SECONDARY_ATTACK_STAND_STARTFIRE,
	ACT_SECONDARY_ATTACK_CROUCH_PREFIRE,
	ACT_SECONDARY_ATTACK_CROUCH_POSTFIRE,
	ACT_SECONDARY_ATTACK_SWIM_PREFIRE,
	ACT_SECONDARY_ATTACK_SWIM_POSTFIRE,

	ACT_MELEE_ATTACK_STAND_PREFIRE,
	ACT_MELEE_ATTACK_STAND_POSTFIRE,
	ACT_MELEE_ATTACK_STAND_STARTFIRE,
	ACT_MELEE_ATTACK_CROUCH_PREFIRE,
	ACT_MELEE_ATTACK_CROUCH_POSTFIRE,
	ACT_MELEE_ATTACK_SWIM_PREFIRE,
	ACT_MELEE_ATTACK_SWIM_POSTFIRE,

	ACT_ITEM1_ATTACK_STAND_PREFIRE,
	ACT_ITEM1_ATTACK_STAND_POSTFIRE,
	ACT_ITEM1_ATTACK_STAND_STARTFIRE,
	ACT_ITEM1_ATTACK_CROUCH_PREFIRE,
	ACT_ITEM1_ATTACK_CROUCH_POSTFIRE,
	ACT_ITEM1_ATTACK_SWIM_PREFIRE,
	ACT_ITEM1_ATTACK_SWIM_POSTFIRE,

	ACT_ITEM2_ATTACK_STAND_PREFIRE,
	ACT_ITEM2_ATTACK_STAND_POSTFIRE,
	ACT_ITEM2_ATTACK_STAND_STARTFIRE,
	ACT_ITEM2_ATTACK_CROUCH_PREFIRE,
	ACT_ITEM2_ATTACK_CROUCH_POSTFIRE,
	ACT_ITEM2_ATTACK_SWIM_PREFIRE,
	ACT_ITEM2_ATTACK_SWIM_POSTFIRE,

	ACT_MP_STAND_MELEE_ALLCLASS,
	ACT_MP_CROUCH_MELEE_ALLCLASS,
	ACT_MP_RUN_MELEE_ALLCLASS,
	ACT_MP_WALK_MELEE_ALLCLASS,
	ACT_MP_AIRWALK_MELEE_ALLCLASS,
	ACT_MP_CROUCHWALK_MELEE_ALLCLASS,
	ACT_MP_JUMP_MELEE_ALLCLASS,
	ACT_MP_JUMP_START_MELEE_ALLCLASS,
	ACT_MP_JUMP_FLOAT_MELEE_ALLCLASS,
	ACT_MP_JUMP_LAND_MELEE_ALLCLASS,
	ACT_MP_SWIM_MELEE_ALLCLASS,

	ACT_MP_ATTACK_STAND_MELEE_ALLCLASS,		// RUN, WALK
	ACT_MP_ATTACK_STAND_MELEE_SECONDARY_ALLCLASS,
	ACT_MP_ATTACK_CROUCH_MELEE_ALLCLASS,		// CROUCHWALK
	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY_ALLCLASS,
	ACT_MP_ATTACK_SWIM_MELEE_ALLCLASS,
	ACT_MP_ATTACK_AIRWALK_MELEE_ALLCLASS,

	ACT_MELEE_ALLCLASS_VM_DRAW,
	ACT_MELEE_ALLCLASS_VM_HOLSTER,
	ACT_MELEE_ALLCLASS_VM_IDLE,
	ACT_MELEE_ALLCLASS_VM_PULLBACK,
	ACT_MELEE_ALLCLASS_VM_PRIMARYATTACK,
	ACT_MELEE_ALLCLASS_VM_SECONDARYATTACK,
	ACT_MELEE_ALLCLASS_VM_RELOAD,			
	ACT_MELEE_ALLCLASS_VM_DRYFIRE,		
	ACT_MELEE_ALLCLASS_VM_IDLE_TO_LOWERED,
	ACT_MELEE_ALLCLASS_VM_IDLE_LOWERED,
	ACT_MELEE_ALLCLASS_VM_LOWERED_TO_IDLE,
	ACT_MELEE_ALLCLASS_VM_STUN,
	ACT_MELEE_ALLCLASS_VM_HITCENTER,		
	ACT_MELEE_ALLCLASS_VM_SWINGHARD,

	// BOMB activities for TD mode.
	ACT_MP_STAND_BOMB,
	ACT_MP_JUMP_START_BOMB,
	ACT_MP_JUMP_FLOAT_BOMB,
	ACT_MP_JUMP_LAND_BOMB,
	ACT_MP_RUN_BOMB,
	ACT_MP_SWIM_BOMB,

	// More Primary VM activities for Soldier Quake RL
	ACT_VM_DRAW_QRL,
	ACT_VM_IDLE_QRL,
	ACT_VM_PULLBACK_QRL,
	ACT_VM_PRIMARYATTACK_QRL,
	ACT_VM_RELOAD_QRL,
	ACT_VM_RELOAD_START_QRL,			
	ACT_VM_RELOAD_FINISH_QRL,	

	// Third person anims for the Soldier Quake RL
	ACT_MP_RELOAD_STAND_PRIMARY3,
	ACT_MP_RELOAD_CROUCH_PRIMARY3,
	ACT_MP_RELOAD_AIRWALK_PRIMARY3,
	ACT_MP_RELOAD_STAND_PRIMARY3_LOOP,
	ACT_MP_RELOAD_CROUCH_PRIMARY3_LOOP,
	ACT_MP_RELOAD_AIRWALK_PRIMARY3_LOOP,
	ACT_MP_RELOAD_STAND_PRIMARY3_END,
	ACT_MP_RELOAD_CROUCH_PRIMARY3_END,
	ACT_MP_RELOAD_AIRWALK_PRIMARY3_END,
	ACT_MP_RELOAD_SWIM_PRIMARY3,

	// Throwable Animations
	ACT_MP_THROW,
	ACT_THROWABLE_VM_DRAW,
	ACT_THROWABLE_VM_IDLE,
	ACT_THROWABLE_VM_FIRE,

	// Spell Animations
	ACT_SPELL_VM_DRAW,
	ACT_SPELL_VM_IDLE, 
	ACT_SPELL_VM_ARM, 
	ACT_SPELL_VM_FIRE,

	// Bread Monster Sapper
	ACT_BREADSAPPER_VM_DRAW,
	ACT_BREADSAPPER_VM_IDLE,

	// Bread Gloves
	ACT_BREADGLOVES_VM_HITLEFT,
	ACT_BREADGLOVES_VM_HITRIGHT,
	ACT_BREADGLOVES_VM_SWINGHARD,
	ACT_BREADGLOVES_VM_IDLE,
	ACT_BREADGLOVES_VM_DRAW,

	ACT_BREADMONSTER_GLOVES_IDLE,
	ACT_BREADMONSTER_GLOVES_HITRIGHT,
	ACT_BREADMONSTER_GLOVES_HITUP,

	ACT_BREADMONSTER_VM_DRAW,
	ACT_BREADMONSTER_VM_IDLE,
	ACT_BREADMONSTER_VM_PRIMARYATTACK,

	ACT_PARACHUTE_DEPLOY,
	ACT_PARACHUTE_DEPLOY_IDLE,
	ACT_PARACHUTE_RETRACT,
	ACT_PARACHUTE_RETRACT_IDLE,

	ACT_BOT_SPAWN,
	ACT_BOT_PANIC,
	ACT_BOT_PRIMARY_MOVEMENT,
	ACT_BOT_GESTURE_FLINCH,
	ACT_BOT_PANIC_START,
	ACT_BOT_PANIC_END,
	
	ACT_VM_RELOAD_LONG,
	ACT_VM_PRIMARYATTACK_LASTROUND,
	ACT_VM_PRIMARYATTACK_MP,
	ACT_VM_IDLE_IS,
	ACT_VM_IDLE_TO_IS,
	ACT_VM_IS_TO_IDLE,
	ACT_VM_PRIMARYATTACK_IS,
	ACT_VM_PRIMARYATTACK_IS_MP,
	ACT_VM_PRIMARYATTACK_LASTROUND_IS_MP,
	ACT_VM_SECONDARYATTACK_IS,
	ACT_CROSSBOW_HOLSTER_UNLOADED,
	ACT_CROSSBOW_BOLT_BACK,
	ACT_CROSSBOW_BOLT_BACK_UNLOADED,
	ACT_CROSSBOW_IDLE_TO_IS_UNLOADED,
	ACT_CROSSBOW_IS_TO_IDLE_UNLOADED,
	ACT_VM_OVERCHARGE,
	ACT_IDLE_SHOTGUN,
	ACT_MP_GESTURE_RANGE_ATTACK,
	ACT_MP_GESTURE_RELOAD,
	ACT_BMMP_JUMP_GLOCK,
	ACT_BMMP_JUMP_357,
	ACT_BMMP_JUMP_SHOTGUN,
	ACT_BMMP_JUMP_MP5,
	ACT_BMMP_JUMP_HIVEHAND,
	ACT_BMMP_JUMP_TAU,
	ACT_BMMP_JUMP_GLUON,
	ACT_BMMP_JUMP_GRENADE,
	ACT_BMMP_JUMP_RPG,
	ACT_BMMP_JUMP_CROSSBOW,
	ACT_BMMP_JUMP_CROWBAR,
	ACT_BMMP_JUMP_SATCHEL,
	ACT_BMMP_JUMP_TRIPMINE,
	ACT_BMMP_JUMP_SNARK,
	ACT_BMMP_JUMP_LAND,
	ACT_BMMP_JUMP_START_GLOCK,
	ACT_BMMP_JUMP_START_357,
	ACT_BMMP_JUMP_START_SHOTGUN,
	ACT_BMMP_JUMP_START_MP5,
	ACT_BMMP_JUMP_START_HIVEHAND,
	ACT_BMMP_JUMP_START_TAU,
	ACT_BMMP_JUMP_START_GLUON,
	ACT_BMMP_JUMP_START_GRENADE,
	ACT_BMMP_JUMP_START_RPG,
	ACT_BMMP_JUMP_START_CROSSBOW,
	ACT_BMMP_JUMP_START_CROWBAR,
	ACT_BMMP_JUMP_START_SATCHEL,
	ACT_BMMP_JUMP_START_TRIPMINE,
	ACT_BMMP_JUMP_START_SNARK,
	ACT_BMMP_JUMP_FLOAT_GLOCK,
	ACT_BMMP_JUMP_FLOAT_357,
	ACT_BMMP_JUMP_FLOAT_SHOTGUN,
	ACT_BMMP_JUMP_FLOAT_MP5,
	ACT_BMMP_JUMP_FLOAT_HIVEHAND,
	ACT_BMMP_JUMP_FLOAT_TAU,
	ACT_BMMP_JUMP_FLOAT_GLUON,
	ACT_BMMP_JUMP_FLOAT_GRENADE,
	ACT_BMMP_JUMP_FLOAT_RPG,
	ACT_BMMP_JUMP_FLOAT_CROSSBOW,
	ACT_BMMP_JUMP_FLOAT_CROWBAR,
	ACT_BMMP_JUMP_FLOAT_SATCHEL,
	ACT_BMMP_JUMP_FLOAT_TRIPMINE,
	ACT_BMMP_JUMP_FLOAT_SNARK,
	ACT_BMMP_SWIM_GLOCK,
	ACT_BMMP_SWIM_357,
	ACT_BMMP_SWIM_SHOTGUN,
	ACT_BMMP_SWIM_MP5,
	ACT_BMMP_SWIM_HIVEHAND,
	ACT_BMMP_SWIM_TAU,
	ACT_BMMP_SWIM_GLUON,
	ACT_BMMP_SWIM_GRENADE,
	ACT_BMMP_SWIM_RPG,
	ACT_BMMP_SWIM_CROSSBOW,
	ACT_BMMP_SWIM_CROWBAR,
	ACT_BMMP_SWIM_SATCHEL,
	ACT_BMMP_SWIM_TRIPMINE,
	ACT_BMMP_SWIM_SNARK,
	ACT_BMMP_IDLE_GLOCK,
	ACT_BMMP_IDLE_357,
	ACT_BMMP_IDLE_SHOTGUN,
	ACT_BMMP_IDLE_MP5,
	ACT_BMMP_IDLE_HIVEHAND,
	ACT_BMMP_IDLE_TAU,
	ACT_BMMP_IDLE_GLUON,
	ACT_BMMP_IDLE_GRENADE,
	ACT_BMMP_IDLE_RPG,
	ACT_BMMP_IDLE_CROSSBOW,
	ACT_BMMP_IDLE_CROWBAR,
	ACT_BMMP_IDLE_SATCHEL,
	ACT_BMMP_IDLE_TRIPMINE,
	ACT_BMMP_IDLE_SNARK,
	ACT_BMMP_IDLE_CROUCH_GLOCK,
	ACT_BMMP_IDLE_CROUCH_357,
	ACT_BMMP_IDLE_CROUCH_SHOTGUN,
	ACT_BMMP_IDLE_CROUCH_MP5,
	ACT_BMMP_IDLE_CROUCH_HIVEHAND,
	ACT_BMMP_IDLE_CROUCH_TAU,
	ACT_BMMP_IDLE_CROUCH_GLUON,
	ACT_BMMP_IDLE_CROUCH_GRENADE,
	ACT_BMMP_IDLE_CROUCH_RPG,
	ACT_BMMP_IDLE_CROUCH_CROSSBOW,
	ACT_BMMP_IDLE_CROUCH_CROWBAR,
	ACT_BMMP_IDLE_CROUCH_SATCHEL,
	ACT_BMMP_IDLE_CROUCH_TRIPMINE,
	ACT_BMMP_IDLE_CROUCH_SNARK,
	ACT_BMMP_WALK_CROUCH_GLOCK,
	ACT_BMMP_WALK_CROUCH_357,
	ACT_BMMP_WALK_CROUCH_SHOTGUN,
	ACT_BMMP_WALK_CROUCH_MP5,
	ACT_BMMP_WALK_CROUCH_HIVEHAND,
	ACT_BMMP_WALK_CROUCH_TAU,
	ACT_BMMP_WALK_CROUCH_GLUON,
	ACT_BMMP_WALK_CROUCH_GRENADE,
	ACT_BMMP_WALK_CROUCH_RPG,
	ACT_BMMP_WALK_CROUCH_CROSSBOW,
	ACT_BMMP_WALK_CROUCH_CROWBAR,
	ACT_BMMP_WALK_CROUCH_SATCHEL,
	ACT_BMMP_WALK_CROUCH_TRIPMINE,
	ACT_BMMP_WALK_CROUCH_SNARK,
	ACT_BMMP_RUN_GLOCK,
	ACT_BMMP_RUN_357,
	ACT_BMMP_RUN_SHOTGUN,
	ACT_BMMP_RUN_MP5,
	ACT_BMMP_RUN_HIVEHAND,
	ACT_BMMP_RUN_TAU,
	ACT_BMMP_RUN_GLUON,
	ACT_BMMP_RUN_GRENADE,
	ACT_BMMP_RUN_RPG,
	ACT_BMMP_RUN_CROSSBOW,
	ACT_BMMP_RUN_CROWBAR,
	ACT_BMMP_RUN_SATCHEL,
	ACT_BMMP_RUN_TRIPMINE,
	ACT_BMMP_RUN_SNARK,
	ACT_BMMP_GESTURE_RANGE_ATTACK_GLOCK,
	ACT_BMMP_GESTURE_RANGE_ATTACK_357,
	ACT_BMMP_GESTURE_RANGE_ATTACK_SHOTGUN,
	ACT_BMMP_GESTURE_RANGE_ATTACK_MP5,
	ACT_BMMP_GESTURE_RANGE_ATTACK_HIVEHAND,
	ACT_BMMP_GESTURE_RANGE_ATTACK_TAU,
	ACT_BMMP_GESTURE_RANGE_ATTACK_GLUON,
	ACT_BMMP_GESTURE_RANGE_ATTACK_GRENADE,
	ACT_BMMP_GESTURE_RANGE_ATTACK_RPG,
	ACT_BMMP_GESTURE_RANGE_ATTACK_CROSSBOW,
	ACT_BMMP_GESTURE_RANGE_ATTACK_CROWBAR,
	ACT_BMMP_GESTURE_RANGE_ATTACK_SATCHEL,
	ACT_BMMP_GESTURE_RANGE_ATTACK_SNARK,
	ACT_BMMP_GESTURE_RELOAD_GLOCK,
	ACT_BMMP_GESTURE_RELOAD_357,
	ACT_BMMP_GESTURE_RELOAD_SHOTGUN,
	ACT_BMMP_GESTURE_RELOAD_MP5,
	ACT_BMMP_GESTURE_RELOAD_CROSSBOW,
	ACT_BMMP_GESTURE_RELOAD_RPG,
	ACT_ENGINEER_REVOLVER_DRAW,
	ACT_ENGINEER_REVOLVER_IDLE,
	ACT_ENGINEER_REVOLVER_PRIMARYATTACK,
	ACT_ENGINEER_REVOLVER_RELOAD,
	ACT_KART_IDLE,
	ACT_KART_ACTION_SHOOT,
	ACT_KART_ACTION_DASH,
	ACT_KART_JUMP_START,
	ACT_KART_JUMP_FLOAT,
	ACT_KART_JUMP_LAND,
	ACT_KART_IMPACT,
	ACT_KART_IMPACT_BIG,
	ACT_KART_GESTURE_POSITIVE,
	ACT_KART_GESTURE_NEGATIVE,
	ACT_GRAPPLE_DRAW,
	ACT_GRAPPLE_IDLE,
	ACT_GRAPPLE_FIRE_START,
	ACT_GRAPPLE_FIRE_IDLE,
	ACT_GRAPPLE_PULL_START,
	ACT_GRAPPLE_PULL_IDLE,
	ACT_GRAPPLE_PULL_END,
	ACT_PRIMARY_VM_INSPECT_START,
	ACT_PRIMARY_VM_INSPECT_IDLE,
	ACT_PRIMARY_VM_INSPECT_END,
	ACT_SECONDARY_VM_INSPECT_START,
	ACT_SECONDARY_VM_INSPECT_IDLE,
	ACT_SECONDARY_VM_INSPECT_END,
	ACT_MELEE_VM_INSPECT_START,
	ACT_MELEE_VM_INSPECT_IDLE,
	ACT_MELEE_VM_INSPECT_END,
	ACT_BALL_VM_CATCH,
	ACT_BALL_VM_PICKUP,
	ACT_BALL_VM_IDLE,
	ACT_BALL_VM_THROW_START,
	ACT_BALL_VM_THROW_LOOP,
	ACT_BALL_VM_THROW_END,
	ACT_MP_COMPETITIVE_LOSERSTATE,
	ACT_MP_COMPETITIVE_WINNERSTATE,
	ACT_SECONDARY_VM_ALTATTACK,
	ACT_MP_PUSH_STAND_SECONDARY,
	ACT_MP_PUSH_CROUCH_SECONDARY,
	ACT_MP_PUSH_SWIM_SECONDARY,
	ACT_CACTUS_FLINCH
}

enum
{
	MAX_WORLD_SOUNDS_SP	= 64,	// Maximum number of sounds handled by the world at one time in single player.
	// This is also the number of entries saved in a savegame file (for b/w compatibility).

	MAX_WORLD_SOUNDS_MP	= 128	// The sound array size is set this large but we'll only use gpGlobals->maxPlayers+32 entries in mp.
}

enum
{
	SOUND_NONE				= 0,
	SOUND_COMBAT			= 0x00000001,
	SOUND_WORLD				= 0x00000002,
	SOUND_PLAYER			= 0x00000004,
	SOUND_DANGER			= 0x00000008,
	SOUND_BULLET_IMPACT		= 0x00000010,
	SOUND_CARCASS			= 0x00000020,
	SOUND_MEAT				= 0x00000040,
	SOUND_GARBAGE			= 0x00000080,
	SOUND_THUMPER			= 0x00000100, // keeps certain creatures at bay
	SOUND_BUGBAIT			= 0x00000200, // gets the antlion's attention
	SOUND_PHYSICS_DANGER	= 0x00000400,
	SOUND_DANGER_SNIPERONLY	= 0x00000800, // only scares the sniper NPC.
	SOUND_MOVE_AWAY			= 0x00001000,
	SOUND_PLAYER_VEHICLE	= 0x00002000,
	SOUND_READINESS_LOW		= 0x00004000, // Changes listener's readiness (Player Companion only)
	SOUND_READINESS_MEDIUM	= 0x00008000,
	SOUND_READINESS_HIGH	= 0x00010000,

	// Contexts begin here.
	SOUND_CONTEXT_FROM_SNIPER		= 0x00100000, // additional context for SOUND_DANGER
	SOUND_CONTEXT_GUNFIRE			= 0x00200000, // Added to SOUND_COMBAT
	SOUND_CONTEXT_MORTAR			= 0x00400000, // Explosion going to happen here.
	SOUND_CONTEXT_COMBINE_ONLY		= 0x00800000, // Only combine can hear sounds marked this way
	SOUND_CONTEXT_REACT_TO_SOURCE	= 0x01000000, // React to sound source's origin, not sound's location
	SOUND_CONTEXT_EXPLOSION			= 0x02000000, // Context added to SOUND_COMBAT, usually.
	SOUND_CONTEXT_EXCLUDE_COMBINE	= 0x04000000, // Combine do NOT hear this
	SOUND_CONTEXT_DANGER_APPROACH   = 0x08000000, // Treat as a normal danger sound if you see the source, otherwise turn to face source.
	SOUND_CONTEXT_ALLIES_ONLY		= 0x10000000, // Only player allies can hear this sound
	SOUND_CONTEXT_PLAYER_VEHICLE	= 0x20000000, // HACK: need this because we're not treating the SOUND_xxx values as true bit values! See switch in OnListened.

	ALL_CONTEXTS			= 0xFFF00000,

	ALL_SCENTS				= SOUND_CARCASS | SOUND_MEAT | SOUND_GARBAGE,

	ALL_SOUNDS				= 0x000FFFFF & ~ALL_SCENTS,
}

enum
{
	SOUNDLIST_EMPTY = -1
}

enum NPC_STATE
{
	NPC_STATE_INVALID = -1,
	NPC_STATE_NONE = 0,
	NPC_STATE_IDLE,
	NPC_STATE_ALERT,
	NPC_STATE_COMBAT,
	NPC_STATE_SCRIPT,
	NPC_STATE_PLAYDEAD,
	NPC_STATE_PRONE,				// When in clutches of barnacle
	NPC_STATE_DEAD
}

//===================================================================================================================
// Hud Element hiding flags
#define	HIDEHUD_WEAPONSELECTION		( 1<<0 )	// Hide ammo count & weapon selection
#define	HIDEHUD_FLASHLIGHT			( 1<<1 )
#define	HIDEHUD_ALL					( 1<<2 )
#define HIDEHUD_HEALTH				( 1<<3 )	// Hide health & armor / suit battery
#define HIDEHUD_PLAYERDEAD			( 1<<4 )	// Hide when local player's dead
#define HIDEHUD_NEEDSUIT			( 1<<5 )	// Hide when the local player doesn't have the HEV suit
#define HIDEHUD_MISCSTATUS			( 1<<6 )	// Hide miscellaneous status elements (trains, pickup history, death notices, etc)
#define HIDEHUD_CHAT				( 1<<7 )	// Hide all communication elements (saytext, voice icon, etc)
#define	HIDEHUD_CROSSHAIR			( 1<<8 )	// Hide crosshairs
#define	HIDEHUD_VEHICLE_CROSSHAIR	( 1<<9 )	// Hide vehicle crosshair
#define HIDEHUD_INVEHICLE			( 1<<10 )
#define HIDEHUD_BONUS_PROGRESS		( 1<<11 )	// Hide bonus progress display (for bonus map challenges)

//=========================================================
// These are the shared tasks
//=========================================================
enum sharedtasks_e
{
		TASK_INVALID = 0,
		
		// Forces the activity to reset.
		TASK_RESET_ACTIVITY,

		// Waits for the specified number of seconds.
		TASK_WAIT,					

		// Make announce attack sound
		TASK_ANNOUNCE_ATTACK,

		// Waits for the specified number of seconds. Will constantly turn to 
		// face the enemy while waiting. 
		TASK_WAIT_FACE_ENEMY,

		// Waits up to the specified number of seconds. Will constantly turn to 
		// face the enemy while waiting. 
		TASK_WAIT_FACE_ENEMY_RANDOM,

		// Wait until the player enters the same PVS as this character.
		TASK_WAIT_PVS,

		// DON'T use this, it needs to go away. 
		TASK_SUGGEST_STATE,

		// Set m_hTargetEnt to nearest player
		TASK_TARGET_PLAYER,

		// Walk to m_hTargetEnt's location
		TASK_SCRIPT_WALK_TO_TARGET,

		// Run to m_hTargetEnt's location
		TASK_SCRIPT_RUN_TO_TARGET,

		// Move to m_hTargetEnt's location using the activity specified by m_hCine->m_iszCustomMove.
		TASK_SCRIPT_CUSTOM_MOVE_TO_TARGET,

		// Move to within specified range of m_hTargetEnt
		TASK_MOVE_TO_TARGET_RANGE,

		// Move to within specified range of our nav goal
		TASK_MOVE_TO_GOAL_RANGE,

		// Path that moves the character a few steps forward of where it is.
		TASK_MOVE_AWAY_PATH,

		TASK_GET_PATH_AWAY_FROM_BEST_SOUND,

		// Set the implied goal for TASK_GET_PATH_TO_GOAL
		TASK_SET_GOAL,

		// Get the path to the goal specified by TASK_SET_GOAL
		TASK_GET_PATH_TO_GOAL,

		// Path to the enemy's location. Even if the enemy is unseen!
		TASK_GET_PATH_TO_ENEMY,
		
		// Path to the last place this character saw the enemy
		TASK_GET_PATH_TO_ENEMY_LKP,

		// Path to the enemy's location or path to a LOS with the enemy's last known position, depending on range
		TASK_GET_CHASE_PATH_TO_ENEMY,

		// Path to a LOS with the enemy's last known position
		TASK_GET_PATH_TO_ENEMY_LKP_LOS,

		// Path to the dead enemy's carcass.
		TASK_GET_PATH_TO_ENEMY_CORPSE,

		// Path to the player's origin
		TASK_GET_PATH_TO_PLAYER,

		// Path to node with line of sight to enemy
		TASK_GET_PATH_TO_ENEMY_LOS,

		// Path to node with line of sight to enemy, at least flTaskData units away from m_vSavePosition
		TASK_GET_FLANK_RADIUS_PATH_TO_ENEMY_LOS,

		// Path to node with line of sight to enemy, at least flTaskData degrees away from m_vSavePosition from the enemy's POV
		TASK_GET_FLANK_ARC_PATH_TO_ENEMY_LOS,

		// Path to the within shot range of last place this character saw the enemy
		TASK_GET_PATH_TO_RANGE_ENEMY_LKP_LOS,

		// Build a path to m_hTargetEnt
		TASK_GET_PATH_TO_TARGET,

		// Allow a little slop, and allow for some Z offset (like the target is a gun on a table).
		TASK_GET_PATH_TO_TARGET_WEAPON,

		TASK_CREATE_PENDING_WEAPON,

		// Path to nodes[ m_pHintNode ]
		TASK_GET_PATH_TO_HINTNODE,

		// Store current position for later reference
		TASK_STORE_LASTPOSITION,

		// Clear stored position
		TASK_CLEAR_LASTPOSITION,

		// Store current position for later reference
		TASK_STORE_POSITION_IN_SAVEPOSITION,

		// Store best sound position for later reference
		TASK_STORE_BESTSOUND_IN_SAVEPOSITION,
		TASK_STORE_BESTSOUND_REACTORIGIN_IN_SAVEPOSITION,

		TASK_REACT_TO_COMBAT_SOUND,

		// Store current enemy position in saveposition
		TASK_STORE_ENEMY_POSITION_IN_SAVEPOSITION,

		// Move to the goal specified by the player in command mode.
		TASK_GET_PATH_TO_COMMAND_GOAL,

		TASK_MARK_COMMAND_GOAL_POS,

		TASK_CLEAR_COMMAND_GOAL,

		// Path to last position (Last position must be stored with TASK_STORE_LAST_POSITION)
		TASK_GET_PATH_TO_LASTPOSITION,

		// Path to saved position (Save position must by set in code or by a task)
		TASK_GET_PATH_TO_SAVEPOSITION,

		// Path to location that has line of sight to saved position (Save position must by set in code or by a task)
		TASK_GET_PATH_TO_SAVEPOSITION_LOS,

		// Path to random node
		TASK_GET_PATH_TO_RANDOM_NODE,

		// Path to source of loudest heard sound that I care about
		TASK_GET_PATH_TO_BESTSOUND,

		// Path to source of the strongest scend that I care about
		TASK_GET_PATH_TO_BESTSCENT,

		// Run the current path
		TASK_RUN_PATH,	

		// Walk the current path
		TASK_WALK_PATH,	

		// Walk the current path for a specified number of seconds
		TASK_WALK_PATH_TIMED,

		// Walk the current path until you are x units from the goal.
		TASK_WALK_PATH_WITHIN_DIST,

		// Walk the current path until for x units
		TASK_WALK_PATH_FOR_UNITS,

		// Rung the current path until you are x units from the goal.
		TASK_RUN_PATH_FLEE,

		// Run the current path for a specified number of seconds
		TASK_RUN_PATH_TIMED,

		// Run the current path until for x units
		TASK_RUN_PATH_FOR_UNITS,

		// Run the current path until you are x units from the goal.
		TASK_RUN_PATH_WITHIN_DIST,

		// Walk the current path sideways (must be supported by animation)
		TASK_STRAFE_PATH,

		// Clear m_flMoveWaitFinished (timer that inhibits movement)
		TASK_CLEAR_MOVE_WAIT,

		// Decide on the appropriate small flinch animation, and play it. 
		TASK_SMALL_FLINCH,

		// Decide on the appropriate big flinch animation, and play it. 
		TASK_BIG_FLINCH,

		// Prevent dodging for a certain amount of time.
		TASK_DEFER_DODGE,

		// Turn to face ideal yaw
		TASK_FACE_IDEAL,

		// Find an interesting direction to face. Don't face into walls, corners if you can help it.
		TASK_FACE_REASONABLE,

		// Turn to face the way I should walk or run
		TASK_FACE_PATH,

		// Turn to face a player
		TASK_FACE_PLAYER,

		// Turn to face the enemy
		TASK_FACE_ENEMY,

		// Turn to face nodes[ m_pHintNode ]
		TASK_FACE_HINTNODE,

		// Play activity associate with the current hint
		TASK_PLAY_HINT_ACTIVITY,

		// Turn to face m_hTargetEnt
		TASK_FACE_TARGET,

		// Turn to face stored last position (last position must be stored first!)
		TASK_FACE_LASTPOSITION,

		// Turn to face stored save position (save position must be stored first!)
		TASK_FACE_SAVEPOSITION,

		// Turn to face directly away from stored save position (save position must be stored first!)
		TASK_FACE_AWAY_FROM_SAVEPOSITION,

		// Set the current facing to be the ideal
		TASK_SET_IDEAL_YAW_TO_CURRENT,

		// Attack the enemy (should be facing the enemy)
		TASK_RANGE_ATTACK1,
		TASK_RANGE_ATTACK2,		
		TASK_MELEE_ATTACK1,		
		TASK_MELEE_ATTACK2,		

		// Reload weapon
		TASK_RELOAD,

		// Execute special attack (user-defined)
		TASK_SPECIAL_ATTACK1,
		TASK_SPECIAL_ATTACK2,

		TASK_FIND_HINTNODE,
		TASK_FIND_LOCK_HINTNODE,

		TASK_CLEAR_HINTNODE,

		// Claim m_pHintNode exclusively for this NPC.
		TASK_LOCK_HINTNODE,

		// Emit an angry sound
		TASK_SOUND_ANGRY,

		// Emit a dying sound
		TASK_SOUND_DEATH,

		// Emit an idle sound
		TASK_SOUND_IDLE,

		// Emit a sound because you are pissed off because you just saw someone you don't like
		TASK_SOUND_WAKE,

		// Emit a pain sound
		TASK_SOUND_PAIN,

		// Emit a death sound
		TASK_SOUND_DIE,

		// Speak a sentence
		TASK_SPEAK_SENTENCE,

		// Wait for the current sentence I'm speaking to finish
		TASK_WAIT_FOR_SPEAK_FINISH,

		// Set current animation activity to the specified activity
		TASK_SET_ACTIVITY,

		// Adjust the framerate to plus/minus N%
		TASK_RANDOMIZE_FRAMERATE,

		// Immediately change to a schedule of the specified type
		TASK_SET_SCHEDULE,

		// Set the specified schedule to execute if the current schedule fails.
		TASK_SET_FAIL_SCHEDULE,

		// How close to route goal do I need to get
		TASK_SET_TOLERANCE_DISTANCE,

		// How many seconds should I spend search for a route
		TASK_SET_ROUTE_SEARCH_TIME,

		// Return to use of default fail schedule
		TASK_CLEAR_FAIL_SCHEDULE,

		// Play the specified animation sequence before continuing
		TASK_PLAY_SEQUENCE,

		// Play the specified private animation sequence before continuing
		TASK_PLAY_PRIVATE_SEQUENCE,

		// Turn to face the enemy while playing specified animation sequence
		TASK_PLAY_PRIVATE_SEQUENCE_FACE_ENEMY,
		TASK_PLAY_SEQUENCE_FACE_ENEMY,
		TASK_PLAY_SEQUENCE_FACE_TARGET,

		// tries lateral cover first, then node cover
		TASK_FIND_COVER_FROM_BEST_SOUND,

		// tries lateral cover first, then node cover
		TASK_FIND_COVER_FROM_ENEMY,

		// Find a place to hide from the enemy, somewhere on either side of me
		TASK_FIND_LATERAL_COVER_FROM_ENEMY,

		// Find a place further from the saved position
		TASK_FIND_BACKAWAY_FROM_SAVEPOSITION,

		// Fine a place to hide from the enemy, anywhere. Use the node system.
		TASK_FIND_NODE_COVER_FROM_ENEMY,

		// Find a place to hide from the enemy that's within the specified distance
		TASK_FIND_NEAR_NODE_COVER_FROM_ENEMY,

		// data for this one is there MINIMUM aceptable distance to the cover.
		TASK_FIND_FAR_NODE_COVER_FROM_ENEMY,

		// Find a place to go that can't see to where I am now.
		TASK_FIND_COVER_FROM_ORIGIN,

		// Unhook from the AI system.
		TASK_DIE,

		// Wait until scripted sequence plays
		TASK_WAIT_FOR_SCRIPT,

		// Play scripted sequence animation
		TASK_PUSH_SCRIPT_ARRIVAL_ACTIVITY,
		TASK_PLAY_SCRIPT,
		TASK_PLAY_SCRIPT_POST_IDLE,
		TASK_ENABLE_SCRIPT,
		TASK_PLANT_ON_SCRIPT,
		TASK_FACE_SCRIPT,

		// Wait for scene to complete
		TASK_PLAY_SCENE,

		// Wait for 0 to specified number of seconds
		TASK_WAIT_RANDOM,

		// Wait forever (until this schedule is interrupted)
		TASK_WAIT_INDEFINITE,

		TASK_STOP_MOVING,
		
		// Turn left the specified number of degrees
		TASK_TURN_LEFT,

		// Turn right the specified number of degrees
		TASK_TURN_RIGHT,

		// Remember the specified piece of data
		TASK_REMEMBER,

		// Forget the specified piece of data
		TASK_FORGET,
		
		// Wait until current movement is complete. 
		TASK_WAIT_FOR_MOVEMENT,

		// Wait until a single-step movement is complete.
		TASK_WAIT_FOR_MOVEMENT_STEP,

		// Wait until I can't hear any danger sound.
		TASK_WAIT_UNTIL_NO_DANGER_SOUND,

		// Pick up new weapons:
		TASK_WEAPON_FIND,
		TASK_WEAPON_PICKUP,
		TASK_WEAPON_RUN_PATH,	// run to weapon but break if someone else picks it up
		TASK_WEAPON_CREATE,

		TASK_ITEM_PICKUP,
		TASK_ITEM_RUN_PATH,

		// Use small hull for tight navigation
		TASK_USE_SMALL_HULL,

		// wait until you are on ground
		TASK_FALL_TO_GROUND,

		// Wander for a specfied amound of time
		TASK_WANDER,

		TASK_FREEZE,

		// regather conditions at the start of a schedule (all conditions are cleared between schedules)
		TASK_GATHER_CONDITIONS,

		// Require an enemy be seen after the task is run to be considered a candidate enemy
		TASK_IGNORE_OLD_ENEMIES,
		
		TASK_DEBUG_BREAK,

		// Add a specified amount of health to this NPC
		TASK_ADD_HEALTH,

		// Add a gesture layer and wait until it's finished
		TASK_ADD_GESTURE_WAIT,

		// Add a gesture layer
		TASK_ADD_GESTURE,

		// Get a path to my forced interaction partner
		TASK_GET_PATH_TO_INTERACTION_PARTNER,
		
		// First task of all schedules for playing back scripted sequences
		TASK_PRE_SCRIPT,

		// ======================================
		// IMPORTANT: This must be the last enum
		// ======================================
		LAST_SHARED_TASK
}
