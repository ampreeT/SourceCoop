#if defined _srccoop_typedef_included
 #endinput
#endif
#define _srccoop_typedef_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Prefix tags for printing messages
//------------------------------------------------------
#define SRCCOOP_CHAT_COLOR_PRI "\x07e69c12"
#define SRCCOOP_CHAT_COLOR_SEC "\x07eb6f28"
#define SRCCOOP_CHAT_TAG		SRCCOOP_CHAT_COLOR_SEC..."[SRCCOOP]"...SRCCOOP_CHAT_COLOR_PRI..." "
#define SRCCOOP_CONSOLE_TAG "[SRCCOOP] "

//------------------------------------------------------
// Mapchange reasons for SourceMod
// Max length is 31
//------------------------------------------------------

// String which SM uses when mapchange is not triggered externally:
#define SM_NORMAL_MAPCHANGE "Normal level change"

// SourceCoop strings:
#define SC_MAPCHANGE_PREFIX "SourceCoop"

#define SC_NORMAL_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Normal"
#define SC_SURVIVAL_RESTART_MAPCHANGE SC_MAPCHANGE_PREFIX..."-SurvivalRestart"
#define SC_FINISHED_HOMEMAP_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Finished-HomeMap"
#define SC_FINISHED_NEXTMAP_MAPCHANGE SC_MAPCHANGE_PREFIX..."-Finished-NextMap"

#define SC_VOTING_MAPCHANGE_PREFIX SC_MAPCHANGE_PREFIX..."-Voting"
#define SC_VOTING_SKIP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-IntroSkip"
#define SC_VOTING_RESTART_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-RestartMap"
#define SC_VOTING_VOTEMAP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-NewMap"

//------------------------------------------------------
// Other definitions
//------------------------------------------------------

#define SRCCOOP_GAMEDATA_NAME "srccoop.games"

#define CHAR_BIT 8
#define FLT_MAX 2147483647.0
#define INT_MIN -2147483648
#define INT_MAX 2147483647

#define ENTITYSTRING_LENGTH 2097152

#define MAX_FORMAT 256
#define MAX_KEY 32
#define MAX_VALUE 1024

//------------------------------------------------------
// Enums
//------------------------------------------------------

enum OperatingSystem
{
    OS_Unknown = -1,
    OS_Windows,
    OS_Linux,
    OS_Mac
}

enum IntroType
{
	INTRO_NONE = 0,
	INTRO_FADE,
	INTRO_FREEZE
}

enum CoopConfigLocation
{
	CCL_SM_DATA = 0,
	CCL_MAPS
}

enum SurvivalMode
{
	SURVIVAL_OFF = 0,
	SURVIVAL_RESPAWN,
	SURVIVAL_RESTART
}