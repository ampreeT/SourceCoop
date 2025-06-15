#pragma newdecls required
#pragma semicolon 1

// Purpose:
// SourceCoop definitions and functionality shared with its addons


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

#define SRCCOOP_VERSION "1.5-beta2"
#define SRCCOOP_URL "https://github.com/ampreeT/SourceCoop"
#define SRCCOOP_GAMEDATA_NAME "srccoop.games"
#define SRCCOOP_CONFIG_GAMEDATA_NAME "srccoop_config.games"
#define SRCCOOP_LIBRARY "SRCCOOP" /* The library name registered by the core plugin */

enum IntroType
{
	INTRO_NONE = 0,
	INTRO_FADE,
	INTRO_FREEZE
}

enum CoopConfigLocation
{
	CCL_SM_DATA = 0,       /* "Path_SM/data/srccoop" | Uses default filesystem */
	CCL_MAPS               /* "maps" | Uses valve filesystem */
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

//------------------------------------------------------
// Stocks
//------------------------------------------------------

stock GameData LoadSourceCoopConfig()
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_CONFIG_GAMEDATA_NAME);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config: \"%s\"", SRCCOOP_CONFIG_GAMEDATA_NAME);
	return pGameConfig;
}

stock void Msg(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	SetGlobalTransTarget(client);
	VFormat(formatMsg, len, msg, 3);
	ReplaceMsgColors(formatMsg, len);
 	PrintToChat(client, "%s%s", SRCCOOP_CHAT_TAG, formatMsg);
}

stock void MsgReply(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	SetGlobalTransTarget(client);
	VFormat(formatMsg, len, msg, 3);
	if (client && GetCmdReplySource() == SM_REPLY_TO_CHAT)
	{
		ReplaceMsgColors(formatMsg, len);
		ReplyToCommand(client, "%s%s", SRCCOOP_CHAT_TAG, formatMsg);
	}
	else
	{
		PurgeMsgColors(formatMsg, len);
		ReplyToCommand(client, "%s%s", SRCCOOP_CONSOLE_TAG, formatMsg);
	}
}

stock void MsgAll(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	SetGlobalTransTarget(LANG_SERVER);
	VFormat(formatMsg, len, msg, 2);
	PurgeMsgColors(formatMsg, len);
	PrintToServer("%s(chat) %s", SRCCOOP_CONSOLE_TAG, formatMsg);
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			SetGlobalTransTarget(i);
			VFormat(formatMsg, len, msg, 2);
			ReplaceMsgColors(formatMsg, len);
 			PrintToChat(i, "%s%s", SRCCOOP_CHAT_TAG, formatMsg);
		}
	}
}

stock void MsgAllNoSrv(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			SetGlobalTransTarget(i);
			VFormat(formatMsg, len, msg, 2);
			ReplaceMsgColors(formatMsg, len);
 			PrintToChat(i, "%s%s", SRCCOOP_CHAT_TAG, formatMsg);
		}
	}
}

stock void MsgSrv(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	SetGlobalTransTarget(LANG_SERVER);
	VFormat(formatMsg, len, msg, 2);
	PurgeMsgColors(formatMsg, len);
	PrintToServer("%s%s", SRCCOOP_CONSOLE_TAG, formatMsg);
}

stock void ReplaceMsgColors(char[] msg, int maxLen)
{
	ReplaceString(msg, maxLen, "{pri}", SRCCOOP_CHAT_COLOR_PRI, false);
	ReplaceString(msg, maxLen, "{sec}", SRCCOOP_CHAT_COLOR_SEC, false);
	ReplaceString(msg, maxLen, "{ter}", SRCCOOP_CHAT_COLOR_TER, false);
}

stock void PurgeMsgColors(char[] msg, int maxLen)
{
	ReplaceString(msg, maxLen, "{pri}", "", false);
	ReplaceString(msg, maxLen, "{sec}", "", false);
	ReplaceString(msg, maxLen, "{ter}", "", false);
}
