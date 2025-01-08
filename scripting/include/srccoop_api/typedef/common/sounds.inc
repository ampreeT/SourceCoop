#pragma newdecls required
#pragma semicolon 1

enum Sound_t
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

	ALL_SOUNDS				= 0x000FFFFF & ~view_as<int>(ALL_SCENTS),
}

enum
{
	MAX_WORLD_SOUNDS_SP	= 64,	// Maximum number of sounds handled by the world at one time in single player.
	// This is also the number of entries saved in a savegame file (for b/w compatibility).

	MAX_WORLD_SOUNDS_MP	= 128	// The sound array size is set this large but we'll only use gpGlobals->maxPlayers+32 entries in mp.
}

enum
{
	SOUNDLIST_EMPTY = -1
}
