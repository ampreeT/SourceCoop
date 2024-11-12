#if defined _srccoop_utils_included
 #endinput
#endif
#define _srccoop_utils_included

#pragma newdecls required
#pragma semicolon 1

#include <srccoop/util/assert>
#include <srccoop/util/stringbuilder>

//------------------------------------------------------
// Gamedata utils
//------------------------------------------------------

/* Macros for loading gamedata key values into configuration structs */

// Prefix for keys in gamedata. Define before including. 
#if !defined CONF_PREFIX
 #define CONF_PREFIX ""
#endif

// Creates a init func for the struct. Pass in a CONF_TYPE macro for each of the struct's variables.
#define CONF_INIT(%1) void Initialize(GameData pGameConfig) {%1}

#define CONF_STR(%1) GetGamedataString(pGameConfig, CONF_PREFIX...#%1, this.%1, sizeof(this.%1));
#define CONF_FLT(%1) this.%1 = GetGamedataFloat(pGameConfig, CONF_PREFIX...#%1);
#define CONF_INT(%1) this.%1 = GetGamedataInt(pGameConfig, CONF_PREFIX...#%1);
#define CONF_CLR(%1) GetGamedataColor(pGameConfig, CONF_PREFIX...#%1, this.%1);


stock void GetGamedataString(GameData pGameConfig, const char[] szKey, char[] szVal, int len)
{
	if (!pGameConfig.GetKeyValue(szKey, szVal, len))
		SetFailState("Missing gamedata key: \"%s\"", szKey);
}

stock float GetGamedataFloat(GameData pGameConfig, const char[] szKey)
{
	char szVal[16]; float flVal;
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));

	if (!StringToFloatEx(szVal, flVal))
		SetFailState("Value for key: \"%s\" should be a float.", szKey);

	return flVal;
}

stock int GetGamedataInt(GameData pGameConfig, const char[] szKey)
{
	char szVal[16]; int iVal;
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));

	if (!StringToIntEx(szVal, iVal))
		SetFailState("Value for key: \"%s\" should be an int.", szKey);

	return iVal;
}

stock void GetGamedataColor(GameData pGameConfig, const char[] szKey, int color[4], const char[] delimiter = " ")
{
	char szVal[16]; char szColor[4][4];
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));
	
	if (3 > ExplodeString(szVal, delimiter, szColor, sizeof(szColor), sizeof(szColor[])))
		SetFailState("Value for key: \"%s\" is not a valid color.", szKey);
	
	color[0] = StringToInt(szColor[0]);
	color[1] = StringToInt(szColor[1]);
	color[2] = StringToInt(szColor[2]);
	color[3] = StringToInt(szColor[3]);
}

stock Address GetInterface(GameData pGameConfig, const char[] szLibrary, const char[] szInterface, bool bExtractIfaceVersion = true)
{
	char szBuffer[64];
	FormatEx(szBuffer, sizeof(szBuffer), "CreateInterface:%s", szLibrary);

	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szBuffer))
		SetFailState("Could not obtain game signature %s", szBuffer);

	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	Handle pCreateIfaceCall = EndPrepSDKCall();
	if (!pCreateIfaceCall)
		SetFailState("Could not prep SDK call %s", szBuffer);

	Address pIfaceAddr;
	if (bExtractIfaceVersion)
	{
		GetGamedataString(pGameConfig, szInterface, szBuffer, sizeof(szBuffer));
		pIfaceAddr = SDKCall(pCreateIfaceCall, szBuffer, 0);
	}
	else
	{
		pIfaceAddr = SDKCall(pCreateIfaceCall, szInterface, 0);
	}
	
	pCreateIfaceCall.Close();

	return pIfaceAddr;
}

//------------------------------------------------------
// DHooks utils
//------------------------------------------------------
#if defined _dhooks_included
stock void LoadDHookDetour(GameData pGameConfig, DynamicDetour& pHandle, const char[] szFuncName, DHookCallback pCallbackPre = null, DHookCallback pCallbackPost = null)
{
	pHandle = DynamicDetour.FromConf(pGameConfig, szFuncName);
	if (!pHandle)
		SetFailState("Couldn't create hook %s", szFuncName);
	if (pCallbackPre && !pHandle.Enable(Hook_Pre, pCallbackPre))
		SetFailState("Couldn't enable pre detour hook %s", szFuncName);
	if (pCallbackPost && !pHandle.Enable(Hook_Post, pCallbackPost))
		SetFailState("Couldn't enable post detour hook %s", szFuncName);
}

stock void LoadDHookVirtual(GameData pGameConfig, DynamicHook& pHandle, const char[] szFuncName)
{
	pHandle = DynamicHook.FromConf(pGameConfig, szFuncName);
	if (pHandle == null)
		SetFailState("Couldn't create hook %s", szFuncName);
}
#endif

//------------------------------------------------------
// Memory utils
//------------------------------------------------------
#if defined __sourcescramble_ext_included
stock MemoryPatch LoadMemPatch(GameData pGameConfig, const char[] szName, bool bEnable = true, bool bRequired = true)
{
	MemoryPatch patch = MemoryPatch.CreateFromConf(pGameConfig, szName);
	if (!patch.Validate())
	{
		if (bRequired)
			SetFailState("Failed to verify mempatch \"%s\"", szName);
		LogError("Failed to verify mempatch \"%s\"", szName);
	}
	else if (bEnable)
	{
		patch.Enable();
	}
	return patch;
}
#endif

stock any Deref(any addr, NumberType type = NumberType_Int32)
{
	return LoadFromAddress(view_as<Address>(addr), type);
}

// Address arithmetics operators (why is this not included)!

stock Address operator+(Address oper1, int oper2)
{
	return oper1 + view_as<Address>(oper2);
}

stock Address operator+(int oper1, Address oper2)
{
	return view_as<Address>(oper1) + oper2;
}

stock Address operator-(Address oper1, int oper2)
{
	return oper1 - view_as<Address>(oper2);
}

stock Address operator-(int oper1, Address oper2)
{
	return view_as<Address>(oper1) - oper2;
}

// The following are attributed to github.com/nosoop/stocksoup

/**
 * Loads a null-terminated string from the given address.
 * 
 * The function will return an empty string if the address corresponds to a nullptr.  This
 * functionality is present as a workaround for when an SDKCall that expects a char pointer
 * receives a nullptr and attempts to dereference it (see alliedmodders/sourcemod/issues/874).
 * 
 * If it is necessary to differentiate between an empty string and a null pointer, check if addr
 * is null before calling the function, or pass a reference to bIsNullPointer.
 * 
 * @return Number of bytes written.  0 is returned on an empty string or a null pointer.
 */
stock int LoadStringFromAddress(Address addr, char[] buffer, int maxlen, bool &bIsNullPointer = false)
{
	if (!addr)
	{
		bIsNullPointer = true;
		return 0;
	}
	
	int c;
	char ch;
	do
	{
		ch = view_as<int>(Deref(addr + c, NumberType_Int8));
		buffer[c] = ch;
	} while (ch && ++c < maxlen - 1);
	return c;
}

/**
 * Returns an entity index from its address by attempting to read the
 * CBaseEntity::m_RefEHandle member.  This assumes the address of a CBaseEntity is
 * passed in.
 * 
 * @param pEntity		Address of an entity.
 * @return				Entity index, or -1 if not valid.
 */
stock int GetEntityFromAddress(Address pEntity)
{
	static int offs_RefEHandle;
	if (offs_RefEHandle)
		return LoadEntityFromHandleAddress(pEntity + offs_RefEHandle);
	
	// if we don't have it already, attempt to lookup offset based on SDK information
	// CWorld is derived from CBaseEntity so it should have both offsets
	int offs_angRotation = FindDataMapInfo(0, "m_angRotation"),
			offs_vecViewOffset = FindDataMapInfo(0, "m_vecViewOffset");
	
	if (offs_angRotation == -1)
	{
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_angRotation");
	}
	else if (offs_vecViewOffset == -1)
	{
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_vecViewOffset");
	}
	else if ((offs_angRotation + 0x0C) != (offs_vecViewOffset - 0x04))
	{
		char game[32];
		GetGameFolderName(game, sizeof(game));
		ThrowError("Could not confirm offset of CBaseEntity::m_RefEHandle "
				... "(incorrect assumption for game '%s'?)", game);
	}
	
	// offset seems right, cache it for the next call
	offs_RefEHandle = offs_angRotation + 0x0C;
	return GetEntityFromAddress(pEntity);
}

//------------------------------------------------------
// Text utils
//------------------------------------------------------
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

#if defined _regex_included

/**
 * Matches a regex pattern against a string if pattern is enclosed by forward slashes "/".
 * Otherwise acts as a regular equals check.
 */
stock bool StrEqualsRegex(const char[] pattern, const char[] str, bool caseSensitive = false)
{
	int patternLength = strlen(pattern);
	if (patternLength > 1 && pattern[0] == '/' && pattern[patternLength - 1] == '/')
	{
		if (str[0] == '\0')
		{
			// regexing empty string errors out..
			return false;
		}
		char[] patternStripped = new char[--patternLength];
		strcopy(patternStripped, patternLength, pattern[1]);
		int substrings = SimpleRegexMatch(str, patternStripped, caseSensitive? 0 : PCRE_CASELESS);
		return (substrings > 0);
	}
	return strcmp(pattern, str, caseSensitive) == 0;
}

/**
 * Simple * wildcard pattern matching. Best for comparing map names (think maps command), file paths (think gitignore), etc.
 * 
 * Example pattern: *pine*
 * Matched strings: pine, pineapple, spines ...
 * 
 */
stock bool StrEqualsWildcard(const char[] pattern, const char[] str, bool caseSensitive = false)
{
	int regexLen = strlen(pattern) * 2 + 3;
	char[] regex = new char[regexLen];

	EscapeRegex(pattern, regex, regexLen, "*");
	ReplaceString(regex, regexLen, "*", ".*");
	Format(regex, regexLen, "^%s$", regex);

	int substrings = SimpleRegexMatch(str, regex, caseSensitive? 0 : PCRE_CASELESS);
	return (substrings > 0);
}

/**
 * Quotes / escapes all special regex characters in a string.
 * @param allowedChars Characters that should not be escaped.
 * @return Characters written
 */
stock int EscapeRegex(const char[] str, char[] dest, int destLen, const char[] allowedChars = "")
{
	static const char specialChars[] = ".\\+*?[^]$(){}=!<>|:-#";
	int i, j;
	while (str[i] != EOS && (j + 1) < destLen)
	{
		if (FindCharInString(specialChars, str[i]) != -1 && FindCharInString(allowedChars, str[i]) == -1)
		{
			if (j + 2 >= destLen)
				break;
			dest[j++] = '\\';
		}
		dest[j++] = str[i++];
	}
	dest[j] = EOS;
	return j;
}

#endif // defined _regex_included

stock void FormatTimeLength(int seconds, char[] out, int size)
{
	int hours = seconds / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	if (hours != 0)
	{
		Format(out, size, "%dh ", hours);
	}
	if (mins != 0)
	{
		Format(out, size, "%s%dm ", out, mins);
	}
	if (secs != 0)
	{
		Format(out, size, "%s%ds", out, secs);
	}
}

stock void FormatTimeLengthLong(int seconds, char[] out, int size)
{
	int hours = seconds / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	char separator[10];
	if (hours != 0)
	{
		Format(out, size, "%d hour%s", hours, hours == 1? "" : "s");
		separator = " ";
	}
	if (mins != 0)
	{
		Format(out, size, "%s%s%d minute%s", out, separator, mins, mins == 1? "" : "s");
		separator = " ";
	}
	if (secs != 0)
	{
		Format(out, size, "%s%s%d second%s", out, mins == 0? separator : " and ", secs, secs == 1? "" : "s");
	}
}

// Like SplitString but more efficient
stock int SplitByChar(const char[] szSource, char c, char[] szBuffer, int nSize)
{
	int i = 0;
	while (szSource[i] != c)
	{
		if (szSource[i] == '\0')
		{
			return -1;
		}
		i++;
	}
	i++;
	strcopy(szBuffer, i < nSize ? i : nSize, szSource);
	return i;
}

stock void LowerCaseString(const char[] szInput, char[] szOutput, int iSize)
{
	iSize--;
	int x = 0;
	while (szInput[x] != '\0' && x < iSize)
	{
		szOutput[x] = CharToLower(szInput[x]);
		x++;
	}
	szOutput[x] = '\0';
}

stock void UpperCaseString(const char[] szInput, char[] szOutput, int iSize)
{
	iSize--;
	int x = 0;
	while (szInput[x] != '\0' && x < iSize)
	{
		szOutput[x] = CharToUpper(szInput[x]);
		x++;
	}
	szOutput[x] = '\0';
}

stock bool StrEndsWith(const char[] str, const char[] suffix, bool caseSensitive = true)
{
	return StrEndsWithEx(str, strlen(str), suffix, caseSensitive);
}

stock bool StrEndsWithEx(const char[] str, int realLen, const char[] suffix, bool caseSensitive = true)
{
	int suffixLen = strlen(suffix);
	return (realLen >= suffixLen && StrEqual(str[realLen - suffixLen], suffix, caseSensitive));
}

stock bool IsAddSynonym(const char[] str)
{
	return (strcmp(str, "add", false) == 0 || strcmp(str, "create", false) == 0);
}

stock bool IsRemoveSynonym(const char[] str)
{
	return (strcmp(str, "remove", false) == 0 || strcmp(str, "delete", false) == 0);
}

stock bool IsModifySynonym(const char[] str)
{
	return (strcmp(str, "modify", false) == 0 || strcmp(str, "edit", false) == 0);
}

stock bool IsEnableSynonym(const char[] str)
{
	return (strcmp(str, "enable", false) == 0 || strcmp(str, "turnon", false) == 0 || strcmp(str, "on", false) == 0);
}

stock bool IsDisableSynonym(const char[] str)
{
	return (strcmp(str, "disable", false) == 0 || strcmp(str, "turnoff", false) == 0 || strcmp(str, "off", false) == 0);
}

//------------------------------------------------------
// Array utils
//------------------------------------------------------
stock int FindStringInArrayEx(const char[][] array, int len, const char[] str, bool caseSensitive = true)
{
	for (int i = 0; i < len; i++)
	{
		if (StrEqual(str, array[i], caseSensitive))
		{
			return i;
		}
	}
	return -1;
}

stock int FindValueInArrayEx(const any[] array, int len, const any val)
{
	for (int i = 0; i < len; i++)
	{
		if (array[i] == val)
		{
			return i;
		}
	}
	return -1;
}

stock int FindCharInArray(const any[] array, int len, const int val, bool caseSensitive = true)
{
	int i = FindValueInArrayEx(array, len, val);
	if (!caseSensitive && i == -1)
	{
		int val2 = IsCharLower(val) ? CharToUpper(val) : CharToLower(val);
		if (val2 != val)
		{
			i = FindValueInArrayEx(array, len, val2);
		}
	}
	return i;
}

//------------------------------------------------------
// Entity utils
//------------------------------------------------------
stock int FindEntityByHammerID(int iHammerID, const char[] szClassname = "*")
{
	int startEnt = -1;
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		if (pEntity.GetHammerID() == iHammerID)
		{
			return startEnt;
		}
	}
	return -1;
}

stock int FindEntityByTargetname(int startEnt, const char[] targetname, const char[] szClassname = "*")
{
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		char szTargetname[MAX_CLASSNAME];
		if (pEntity.GetTargetname(szTargetname, sizeof(szTargetname)) && strcmp(szTargetname, targetname, false) == 0)
		{
			return startEnt;
		}
	}
	return -1;
}

stock int FindEntityByOwner(int startEnt, CBaseEntity pOwnerEntity, const char[] szClassname = "*")
{
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		if (pEntity.GetOwner() == pOwnerEntity)
		{
			return startEnt;
		}
	}
	return -1;
}

//------------------------------------------------------
// Player utils
//------------------------------------------------------

/**
 * Return nearest alive player if exists, otherwise return nearest player that passes requirements.
 */
stock CBasePlayer GetNearestPlayerPreferAlive(CBaseEntity pOriginEntity, bool bNoSpectators = false)
{
	return GetNearestPlayer(pOriginEntity, false, bNoSpectators, true);
}

stock CBasePlayer GetNearestPlayerPreferAliveEx(float vecPos[3], bool bNoSpectators = false)
{
	return GetNearestPlayerEx(vecPos, false, bNoSpectators, true);
}

/**
 * Return nearest alive player.
 */
stock CBasePlayer GetNearestPlayerAlive(CBaseEntity pOriginEntity)
{
	return GetNearestPlayer(pOriginEntity, true, true, false);
}

stock CBasePlayer GetNearestPlayerAliveEx(float vecPos[3])
{
	return GetNearestPlayerEx(vecPos, true, true, false);
}

/**
 * Return nearest player that passes requirements.
 */
stock CBasePlayer GetNearestPlayer(CBaseEntity pOriginEntity, bool bAliveOnly = true, bool bNoSpectators = true, bool bPreferAlive = false)
{
	float vecEntityPosition[3];
	pOriginEntity.GetAbsOrigin(vecEntityPosition);
	return GetNearestPlayerEx(vecEntityPosition, bAliveOnly, bNoSpectators, bPreferAlive);
}

stock CBasePlayer GetNearestPlayerEx(float vecPos[3], bool bAliveOnly = true, bool bNoSpectators = true, bool bPreferAlive = false)
{
	CBasePlayer pBestPlayer = NULL_CBASEENTITY;
	float flBestDistance = FLT_MAX;
	bool bFoundAlive;
	
	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || IsFakeClient(i))
			continue;

		bool bAlive = IsPlayerAlive(i);
		if (bAliveOnly)
		{
			if (!bAlive)
				continue;
		}
		else if (bNoSpectators && GetClientTeam(i) == TEAM_SPECTATOR)
		{
			continue;
		}
		
		if (bPreferAlive)
		{
			if (bFoundAlive)
			{
				if (!bAlive)
					continue;
			}
			else if (bAlive)
			{
				bFoundAlive = true;
				flBestDistance = FLT_MAX;
			}
		}
		
		CBasePlayer pPlayer = CBasePlayer(i);
		float vecPlayerPosition[3];
		pPlayer.GetAbsOrigin(vecPlayerPosition);

		float flDistance = GetVectorDistance(vecPos, vecPlayerPosition, true);
		if (flDistance < flBestDistance)
		{
			pBestPlayer = pPlayer;
			flBestDistance = flDistance;
		}
	}
	return pBestPlayer;
}

stock int GetRealClientCount(bool bInGameOnly, bool bAliveOnly = false, bool bNoSpectators = false)
{
	int clients;
	for (int i = 1; i <= MaxClients; i++)
	{
		if ((bInGameOnly? IsClientInGame(i) : IsClientConnected(i))
			&& !IsFakeClient(i)
			&& (!bAliveOnly || IsPlayerAlive(i))
			&& (!bNoSpectators || GetClientTeam(i) != TEAM_SPECTATOR))
		{
 			clients++;
 		}
 	}
	return clients;
}

enum CanTeleportResult
{
	CAN_TP_NO,
	CAN_TP_YES,
	CAN_TP_MUSTDUCK		//🦆//
}

stock CanTeleportResult CanTeleportPlayer(CBasePlayer pPlayer, float vecOrigin[3], bool bExact = false, bool bDuckCheck = true, bool bOnlyVaried = false)
{
	if (!bOnlyVaried)
	{
		bool bDidHit;

		TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
		bDidHit = TR_DidHit();
		if (!bDidHit) return CAN_TP_YES;
		
		if (bDuckCheck)
		{
			TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
			bDidHit = TR_DidHit();
			if (!bDidHit) return CAN_TP_MUSTDUCK;
		}
	}
	
	if (!bExact)
	{
		// try varying the positions in a radius
		static const float yaw[] = {0.0, 45.0, 90.0, 135.0, 180.0, 225.0, 270.0, 315.0};
		static const float distance[] = {25.0, 50.0};
		static const float zDistance[] = {0.0, 25.0, 50.0};
		
		for (int z = 0; z < sizeof(zDistance); z++)
		{
			for (int d = 0; d < sizeof(distance); d++)
			{
				for (int y = 0; y < sizeof(yaw); y++)
				{
					if (CanTeleportPlayer_Offset(vecOrigin, yaw[y], distance[d], zDistance[z]))
					{
						return CAN_TP_YES;
					}
				}
			}
		}
	}
	
	return CAN_TP_NO;
}

stock bool CanTeleportPlayer_Offset(float vecOrigin[3], float yaw, float distance, float z)
{
	float newPos[3], angles[3];
	newPos = vecOrigin; angles[1] = yaw;
	
	GetAngleVectors(angles, angles, NULL_VECTOR, NULL_VECTOR);
	newPos[0] += angles[0] * distance;
	newPos[1] += angles[1] * distance;
	newPos[2] += z;
	
	//TE_SetupGlowSprite(newPos, PrecacheModel("sprites/orangeglow1.vmt"), 60.0, 0.1, 255); TE_SendToAll();
	
	TR_TraceHullFilter(newPos, newPos, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	if (!TR_DidHit())
	{
		vecOrigin = newPos;
		return true;
	}

  	return false;
}

public bool CanTeleportPlayer_TraceFilter(int entity, int mask, any data)
{
	CBaseEntity pEntity = CBaseEntity(entity);
	if (pEntity.IsClassPlayer() || pEntity.IsPickupItem())
	{
		return false;
	}
	return true;
}

stock float PlayerDotProductToEntity(CBasePlayer pPlayer, CBaseEntity pEnt)
{
	float entOrigin[3];
	pEnt.GetAbsOrigin(entOrigin);
	return PlayerDotProductToPoint(pPlayer, entOrigin);
}

stock float PlayerDotProductToEntityCenter(CBasePlayer pPlayer, CBaseEntity pEnt)
{
	float vecCenter[3];
	pEnt.WorldSpaceCenter(vecCenter);
	return PlayerDotProductToPoint(pPlayer, vecCenter);
}

stock float PlayerDotProductToPoint(CBasePlayer pPlayer, const float vecPoint[3])
{
	float angles[3], origin[3], fwd[3], targetDir[3];
	pPlayer.GetEyeAngles(angles);
	GetAngleVectors(angles, fwd, NULL_VECTOR, NULL_VECTOR);

	pPlayer.GetEyePosition(origin);
	MakeVectorFromPoints(origin, vecPoint, targetDir);
	NormalizeVector(targetDir, targetDir);

	return GetVectorDotProduct(fwd, targetDir);
}

/**
 * Fades a client's screen to a specified color
 * Your adviced to read the FFADE_ Comments
 *
 * @param client		Player for which to fade the screen
 * @param duration		duration in milliseconds the effect stays
 * @param mode			fade mode, see FFADE_ defines
 * @param holdtime		holdtime in milliseconds
 * @param r				red amount
 * @param g				green amount
 * @param b				blue amount
 * @param a				transparency
 * @return				True on success, false otherwise
 */
stock bool Client_ScreenFade(int client, int duration, int mode, int holdtime=-1, int r=0, int g=0, int b=0, int a=255, bool reliable=true)
{
	Handle userMessage = StartMessageOne("Fade", client, (reliable?USERMSG_RELIABLE:0));

	if (userMessage == INVALID_HANDLE)
	{
		return false;
	}

	if (GetFeatureStatus(FeatureType_Native, "GetUserMessageType") == FeatureStatus_Available &&
		GetUserMessageType() == UM_Protobuf)
	{

		int color[4];
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;

		PbSetInt(userMessage,   "duration",   duration);
		PbSetInt(userMessage,   "hold_time",  holdtime);
		PbSetInt(userMessage,   "flags",      mode);
		PbSetColor(userMessage, "clr",        color);
	}
	else
	{
		BfWriteShort(userMessage,	duration);	// Fade duration
		BfWriteShort(userMessage,	holdtime);	// Fade hold time
		BfWriteShort(userMessage,	mode);		// What to do
		BfWriteByte(userMessage,	r);			// Color R
		BfWriteByte(userMessage,	g);			// Color G
		BfWriteByte(userMessage,	b);			// Color B
		BfWriteByte(userMessage,	a);			// Color Alpha
	}
	EndMessage();

	return true;
}

//------------------------------------------------------
// Weapon utils
//------------------------------------------------------
stock void PreventWeaponFiring(CBaseCombatWeapon pWeapon, float flDelayTime)
{
	pWeapon.SetNextPrimaryAttack(GetGameTime() + flDelayTime);
	pWeapon.SetNextSecondaryAttack(GetGameTime() + flDelayTime);
}

//------------------------------------------------------
// Math utils
//------------------------------------------------------
stock void ScaleVectors(const float vecA[3], const float vecB[3], float vecResult[3])
{
	vecResult[0] = vecA[0] * vecB[0];
	vecResult[1] = vecA[1] * vecB[1];
	vecResult[2] = vecA[2] * vecB[2];
}

stock void MoveVector(const float vecStart[3], const float vecEnd[3], float scale, float vecResult[3])
{
	SubtractVectors(vecEnd, vecStart, vecResult);
	ScaleVector(vecResult, scale);
	AddVectors(vecStart, vecResult, vecResult);
}

stock void GetVectorDistanceVector(const float vecA[3], const float vecB[3], float vecResult[3])
{
	SubtractVectors(vecA, vecB, vecResult);
	vecResult[0] = FloatAbs(vecResult[0]);
	vecResult[1] = FloatAbs(vecResult[1]);
	vecResult[2] = FloatAbs(vecResult[2]);
}

stock bool VecEqual(const float vecA[3], const float vecB[3])
{
	return (vecA[0] == vecB[0] && vecA[1] == vecB[1] && vecA[2] == vecB[2]);
}

// returns true if there's an intersection between two boxes
// borrowed from public/collisionutils.cpp
stock bool IsBoxIntersectingBox(const float boxMin1[3], const float boxMax1[3], const float boxMin2[3], const float boxMax2[3])
{
	if ((boxMin1[0] > boxMax2[0]) || (boxMax1[0] < boxMin2[0]))
		return false;
	if ((boxMin1[1] > boxMax2[1]) || (boxMax1[1] < boxMin2[1]))
		return false;
	if ((boxMin1[2] > boxMax2[2]) || (boxMax1[2] < boxMin2[2]))
		return false;
	return true;
}

stock any Min(any val1, any val2)
{
	return val1 < val2 ? val1 : val2;
}

stock any Max(any val1, any val2)
{
	return val1 > val2 ? val1 : val2;
}

stock any Clamp(any val, any min, any max)
{
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
	return val;
}

stock bool IsInBounds(any value, any min, any max)
{
	if (value < min || value > max)
		return false;
	
	return true;
}

//------------------------------------------------------
// File utils
//------------------------------------------------------

/* File copy from SMLIB
 * @param source		Input file
 * @param destination	Output file
 * @return				True on success, false otherwise
 */
stock bool FileCopy(const char[] source, const char[] destination, bool use_valve_fs=false, const char[] valve_path_id="GAME")
{
	File file_source = OpenFile(source, "rb", use_valve_fs, valve_path_id);

	if (file_source == null)
	{
		return false;
	}

	File file_destination = OpenFile(destination, "wb", use_valve_fs, valve_path_id);

	if (file_destination == null)
	{
		file_source.Close();
		return false;
	}

	int buffer[32];
	int cache;

	while (!file_source.EndOfFile())
	{
		cache = file_source.Read(buffer, sizeof(buffer), 1);
		file_destination.Write(buffer, cache, 1);
	}

	delete file_source;
	delete file_destination;

	return true;
}

/**
 * This is a workaround for opening existing files, potentially located in the custom folder, with the Valve filesystem, for both reading and writing.
 * Normally, opening an existing file with Valve FS using any writable modes ("r+", "a", ..) will only look for files in root game folder.
 * This function currently accounts for the custom folder only, other additional Valve search paths are ignored.
 * 
 * https://github.com/alliedmodders/sourcemod/issues/2175
 */
stock File OpenFileEx(const char[] file, const char[] mode, bool use_valve_fs=false, const char[] valve_path_id="GAME")
{
	if (use_valve_fs && !StrEqual(mode, "r", false) && !StrEqual(mode, "rb", false) && !StrEqual(mode, "rt", false))
	{
		char szFullPath[PLATFORM_MAX_PATH];
		if (ResolveValveFile(file, szFullPath, sizeof(szFullPath)))
		{
			return OpenFile(szFullPath, mode);
		}
	}
	return OpenFile(file, mode, use_valve_fs, valve_path_id);
}

/**
 * Should only be used as last resort.
 * See OpenFileEx
 */
stock bool ResolveValveFile(const char[] szFile, char[] szFullPath, int maxLen)
{
	DirectoryListing dir = OpenDirectory("custom");
	if (dir)
	{
		char buffer[PLATFORM_MAX_PATH];
		FileType ft;
		
		while (dir.GetNext(buffer, sizeof(buffer), ft))
		{
			if (ft == FileType_Directory && !StrEqual(buffer, ".") && !StrEqual(buffer, ".."))
			{
				Format(buffer, sizeof(buffer), "custom/%s/%s", buffer, szFile);
				if (FileExists(buffer))
				{
					dir.Close();
					Format(szFullPath, maxLen, buffer);
					return true;
				}
			}
		}

		dir.Close();
	}
	return false;
}
