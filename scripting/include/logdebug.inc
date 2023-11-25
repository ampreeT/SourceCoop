/*
 * logdebug.inc by Dr. McKay, updated by Alienmario
 * 
 * A simple to use library for debug logging which can be redirected or disabled on-the-fly.
 * Employs transitional syntax - SourceMod 1.7 or newer is required to compile and run.
 * 
 * Define NO_DEBUG before including this file to completely disable all debugging
 */

#if defined _logdebug_included
 #endinput
#endif
#define _logdebug_included

#pragma newdecls required
#pragma semicolon 1

#if defined NO_DEBUG
 stock void InitDebugLog(const char[] convarName, const char[] debugTag, int adminFlags = ADMFLAG_GENERIC, const char timeFormat[64] = "") { }
 stock bool LogDebug(const char[] format, any ...) { return false; }
 #endinput
#endif

#define DEBUG_SERVER_CONSOLE		1		/**< Message will be routed to server console */
#define DEBUG_CLIENT_CONSOLE		2		/**< Message will be routed to all clients' consoles */
#define DEBUG_ADMIN_CONSOLE			4		/**< Message will be routed to consoles of admins with a flag specified by plugin */
#define DEBUG_CLIENT_CHAT			8		/**< Message will be routed to all clients' chat boxes (and consequently consoles) */
#define DEBUG_ADMIN_CHAT			16		/**< Message will be routed to chat boxes of admins with a flag specified by plugin */
#define DEBUG_LOG_FILE				32		/**< Message will be routed to plugin's debug log */

#define DEBUGPART_TIME				1		/**< Message will include current time */
#define DEBUGPART_TICKCOUNT			2		/**< Message will include current tick count */
#define DEBUGPART_LOCATION			4		/**< Message will include full location in the following syntax sourcefile::function:line */

ConVar g_cvarDebugLocation;
ConVar g_cvarDebugParts;
char g_DebugLogFileName[PLATFORM_MAX_PATH];
char g_DebugTag[11];
char g_DebugCvarName[64];
char g_DebugTimeFormat[64];
int g_DebugAdminFlags;

/**
 * Inits debug logging. You must call this in OnPluginStart().
 * 
 * @param convarName		A name to use for the cvar which controls debug log output locations.
 * 							A second convar with "_parts" suffix can be used for controlling which information is included in the messages.
 * 							Also used as filename for logfile.
 * @param debugTag			Tag to prepend to messages, without []. Max 10 characters.
 * @param adminFlag			One or more admin flagbits which define whether a user is an "admin". If you pass multiple flags, users will need ALL flags.
 * @param timeFormat		Time format if messages include time. Empty to use sm_datetime_format. Refer to FormatTime documentation.
 * @noreturn
 */
stock void InitDebugLog(const char[] convarBaseName, const char[] debugTag, int adminFlags = ADMFLAG_GENERIC, const char timeFormat[64] = "")
{
	BuildPath(Path_SM, g_DebugLogFileName, sizeof(g_DebugLogFileName), "logs/%s.log", convarBaseName);
	
	char flagChars[32];
	AdminFlag flags[22];
	int numFlags = FlagBitsToArray(adminFlags, flags, sizeof(flags));
	for (int i = 0; i < numFlags; i++)
	{
		int len = strlen(flagChars);
		if (len > 0)
		{
			len += StrCat(flagChars, sizeof(flagChars), ", ");
		}
		int flagChar;
		FindFlagChar(flags[i], flagChar);
		flagChars[len] = flagChar;
		flagChars[len + 1] = 0;
	}
	
	g_DebugAdminFlags = adminFlags;
	
	int convarNameLen = strlen(convarBaseName) + 10;
	char[] convarName = new char[convarNameLen];
	char convarDescription[512];
	
	FormatEx(convarDescription, sizeof(convarDescription), "Add up values to enable debug logging to those locations\n  1 = server console\n  2 = all clients' consoles\n  4 = consoles of admins with '%s' flag%s or access to '%s' override\n  8 = all clients' chat\n  16 = chat of admins with '%s' flag%s or access to '%s' override\n  32 = debug log file %s", flagChars, flagChars[1] == '\0' ? "" : "s", convarBaseName, flagChars, flagChars[1] == '\0' ? "" : "s", convarBaseName, g_DebugLogFileName);
	g_cvarDebugLocation = CreateConVar(convarBaseName, "0", convarDescription, FCVAR_DONTRECORD, true, 0.0, true, 63.0);
	
	FormatEx(convarName, convarNameLen, "%s_parts", convarBaseName);
	FormatEx(convarDescription, sizeof(convarDescription), "Add up values to set up additional information included in the logs\n  1 = time\n  2 = tick count\n  4 = full location in the following syntax sourcefile::function:line");
	g_cvarDebugParts = CreateConVar(convarName, "6", convarDescription, FCVAR_DONTRECORD, true, 0.0, true, 7.0);
	
	strcopy(g_DebugTag, sizeof(g_DebugTag), debugTag);
	strcopy(g_DebugCvarName, sizeof(g_DebugCvarName), convarBaseName);
	g_DebugTimeFormat = timeFormat;
}

/**
 * Logs a message to all enabled debug output points
 * 
 * @param format		Message text with formatting tokens
 * @param ...			Variable number of format parameters
 * @return				true if message was output to at least one place
 */
stock bool LogDebug(const char[] format, any ...)
{
	if (g_cvarDebugLocation == null)
	{
		ThrowError("InitDebugLog must be called before LogDebug");
	}
	
	int output = g_cvarDebugLocation.IntValue;
	if (!output)
	{
		return false;
	}
	
	static char buffer[512];

	int debugParts = g_cvarDebugParts.IntValue;
	if (debugParts)
	{
		char sDebugParts[256];
		if (debugParts & DEBUGPART_TIME)
		{
			FormatTime(sDebugParts, sizeof(sDebugParts), g_DebugTimeFormat[0] == EOS? NULL_STRING : g_DebugTimeFormat);
			StrCat(sDebugParts, sizeof(sDebugParts), " ");
		}
		if (debugParts & DEBUGPART_TICKCOUNT)
		{
			Format(sDebugParts, sizeof(sDebugParts), "%s<%d> ", sDebugParts, GetGameTickCount());
		}
		if (debugParts & DEBUGPART_LOCATION)
		{
			FrameIterator frames = new FrameIterator();
			frames.Next();
			frames.Next();
			frames.GetFilePath(buffer, sizeof(sDebugParts));
			int separatorIndex = FindCharInString(buffer, '\\', true);
			if (separatorIndex == -1)
			{
				separatorIndex = FindCharInString(buffer, '/', true);
			}
			Format(sDebugParts, sizeof(sDebugParts), "%s(%s::", sDebugParts, buffer[separatorIndex + 1]);
			frames.GetFunctionName(buffer, sizeof(sDebugParts));
			Format(sDebugParts, sizeof(sDebugParts), "%s%s:%d) ", sDebugParts, buffer, frames.LineNumber);
			frames.Close();
		}
		VFormat(buffer, sizeof(buffer), format, 2);
		Format(buffer, sizeof(buffer), "[%s] %s- %s", g_DebugTag, sDebugParts, buffer);
	}
	else
	{
		VFormat(buffer, sizeof(buffer), format, 2);
		Format(buffer, sizeof(buffer), "[%s] %s", g_DebugTag, buffer);
	}
	
	if (output & DEBUG_SERVER_CONSOLE)
	{
		PrintToServer(buffer);
	}
	
	if (output & DEBUG_CLIENT_CONSOLE)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i))
			{
				PrintToConsole(i, buffer);
			}
		}
	}
	
	if (output & DEBUG_ADMIN_CONSOLE)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i) && CheckCommandAccess(i, g_DebugCvarName, g_DebugAdminFlags, true))
			{
				PrintToConsole(i, buffer);
			}
		}
	}
	
	if (output & DEBUG_CLIENT_CHAT)
	{
		PrintToChatAll(buffer);
	}
	
	if (output & DEBUG_ADMIN_CHAT)
	{
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i) && CheckCommandAccess(i, g_DebugCvarName, g_DebugAdminFlags, true))
			{
				PrintToChat(i, buffer);
			}
		}
	}
	
	if (output & DEBUG_LOG_FILE)
	{
		LogToFileEx(g_DebugLogFileName, buffer);
	}

	return true;
}

/**
 * Returns a bitstring containing enabled output locations (see DEBUG_ constants)
 * 
 * @return				bitstring for enabled outputs
 */
stock int GetDebugOutputs()
{
	return g_cvarDebugLocation.IntValue;
}

/**
 * Returns a bitstring containing enabled debug parts (see DEBUGPART_ constants)
 * 
 * @return				bitstring for enabled debug parts
 */
stock int GetDebugParts()
{
	return g_cvarDebugParts.IntValue;
}
