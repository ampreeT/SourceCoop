#if defined _srccoop_public_included
 #endinput
#endif
#define _srccoop_public_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Prefix tags / colors for printing messages
//------------------------------------------------------
#define SRCCOOP_CHAT_COLOR_PRI "\x07e69c12"
#define SRCCOOP_CHAT_COLOR_SEC "\x07eb6f28"
#define SRCCOOP_CHAT_COLOR_TER "\x07e8b454"
#define SRCCOOP_CHAT_TAG		SRCCOOP_CHAT_COLOR_SEC..."[SRCCOOP]"...SRCCOOP_CHAT_COLOR_PRI..." "
#define SRCCOOP_CONSOLE_TAG "[SRCCOOP] "

//------------------------------------------------------
// Mapchange reasons for SourceMod (Max length 31)
//------------------------------------------------------

// String which SM uses when mapchange is not triggered externally:
#define SM_NORMAL_MAPCHANGE "Normal level change"

// SourceCoop strings:
#define SC_MAPCHANGE_PREFIX "SourceCoop"

#define SC_NORMAL_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Normal"
#define SC_SURVIVAL_RESTART_MAPCHANGE SC_MAPCHANGE_PREFIX..."-SurvivalRestart"
#define SC_FINISHED_HOMEMAP_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Finished-HomeMap"
#define SC_FINISHED_NEXTMAP_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Finished-NextMap"
// Voting
#define SC_VOTING_MAPCHANGE_PREFIX SC_MAPCHANGE_PREFIX..."-Voting"
#define SC_VOTING_SKIP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-IntroSkip"
#define SC_VOTING_RESTART_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-RestartMap"
#define SC_VOTING_VOTEMAP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-NewMap"

//------------------------------------------------------
// Menu definitions
//------------------------------------------------------

/* Predefined TopMenu category for player settings */
#define COOPMENU_CATEGORY_PLAYER "PlayerSettings"

/* Predefined TopMenu category for sound settings */
#define COOPMENU_CATEGORY_SOUNDS "SoundSettings"

/* Predefined TopMenu category for voting actions */
#define COOPMENU_CATEGORY_VOTING "Voting"

/* Predefined TopMenu category for other settings */
#define COOPMENU_CATEGORY_OTHER "Other"

//------------------------------------------------------
// Other definitions
//------------------------------------------------------

#define SRCCOOP_VERSION "1.5-dev"
#define SRCCOOP_URL "https://github.com/ampreeT/SourceCoop"
#define SRCCOOP_GAMEDATA_NAME "srccoop.games"
#define SRCCOOP_CONFIG_GAMEDATA_NAME "srccoop_config.games"
#define SRCCOOP_LIBRARY "SRCCOOP" /* The library name registered by the core plugin */

#define CHAR_BIT 8
#define FLT_MAX 2147483647.0
#define INT_MIN -2147483648
#define INT_MAX 2147483647

enum IntroType
{
	INTRO_NONE = 0,
	INTRO_FADE,
	INTRO_FREEZE
}

enum CoopConfigLocation
{
	CCL_SM_DATA = 0, /* "Path_SM/data/srccoop" | Uses default filesystem */
	CCL_MAPS /* "maps" | Uses valve filesystem */
}

enum SurvivalMode
{
	SURVIVAL_OFF = 0,
	SURVIVAL_RESPAWN,
	SURVIVAL_RESTART
}

enum struct SpawnOptions
{
	bool bUseSpawnSystem;		/* Teleport using the spawn system, aka use the active checkpoint */
	bool bUseSpawnEquipment;	/* Spawn with equipment using the spawn system. */
	bool bRevive;				/* Restore pre-death state if possible. Overrides bUseSpawnEquipment. Does not teleport by itself. */
	float vecOrigin[3];			/* New origin or zeroes for null. Overrides other teleport options. */
	float vecAngles[3];			/* New angles to use if vecOrigin is set. */
	bool bUnstuck;				/* Allow checking for empty space around vecOrigin. */

	void Reset()
	{
		this.bUseSpawnSystem = true;
		this.bUseSpawnEquipment = true;
		this.bRevive = false;
		this.vecOrigin = { 0.0, 0.0, 0.0 };
		this.vecAngles = { 0.0, 0.0, 0.0 };
		this.bUnstuck = true;
	}
}
