#if defined _srccoop_typedef_included
 #endinput
#endif
#define _srccoop_typedef_included

#define CHAT_TAG "\x07eb6f28[SRCCOOP]\x07e69c12 "
#define CONSOLE_TAG "[SRCCOOP] "
#define CHAT_COLOR_PRI "\x07e69c12"
#define CHAT_COLOR_SEC "\x07eb6f28"

#define SRCCOOP_GAMEDATA_NAME "srccoop.games"

#define CHAR_BIT 8

// String which SM uses when mapchange is not triggered externally
#define SM_NORMAL_MAPCHANGE "Normal level change"

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