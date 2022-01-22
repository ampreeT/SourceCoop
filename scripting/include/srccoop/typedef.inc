#if defined _srccoop_typedef_included
 #endinput
#endif
#define _srccoop_typedef_included

#define CHAT_COLOR_PRI "\x07e69c12"
#define CHAT_COLOR_SEC "\x07eb6f28"
#define CHAT_TAG		CHAT_COLOR_SEC..."[SRCCOOP]"...CHAT_COLOR_PRI..." "
#define CONSOLE_TAG "[SRCCOOP] "

#define SRCCOOP_GAMEDATA_NAME "srccoop.games"

#define CHAR_BIT 8

//------------------------------------------------------
// Mapchange reasons for SourceMod
// Max length is 31
//------------------------------------------------------

// String which SM uses when mapchange is not triggered externally
#define SM_NORMAL_MAPCHANGE "Normal level change"

#define SC_VOTING_MAPCHANGE_PREFIX "SourceCoop-Voting"
#define SC_VOTING_SKIP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-IntroSkip"
#define SC_VOTING_RESTART_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-RestartMap"
#define SC_VOTING_VOTEMAP_MAPCHANGE SC_VOTING_MAPCHANGE_PREFIX..."-NewMap"

#define FLT_MAX 2147483647.0
#define INT_MIN -2147483648
#define INT_MAX 2147483647

#define ENTITYSTRING_LENGTH 2097152

#define MAX_FORMAT 256

#define MAX_KEY 32
#define MAX_VALUE 1024

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