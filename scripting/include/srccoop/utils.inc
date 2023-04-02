#if defined _srccoop_utils_included
 #endinput
#endif
#define _srccoop_utils_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// DHooks utils
//------------------------------------------------------
#if defined _dhooks_included
stock void LoadDHookDetour(const Handle pGameConfig, DynamicDetour& pHandle, const char[] szFuncName, DHookCallback pCallbackPre = INVALID_FUNCTION, DHookCallback pCallbackPost = INVALID_FUNCTION)
{
	pHandle = DynamicDetour.FromConf(pGameConfig, szFuncName);
	if (pHandle == null)
		SetFailState("Couldn't create hook %s", szFuncName);
	if (pCallbackPre != INVALID_FUNCTION && !pHandle.Enable(Hook_Pre, pCallbackPre))
		SetFailState("Couldn't enable pre detour hook %s", szFuncName);
	if (pCallbackPost != INVALID_FUNCTION && !pHandle.Enable(Hook_Post, pCallbackPost))
		SetFailState("Couldn't enable post detour hook %s", szFuncName);
}

stock void LoadDHookVirtual(const Handle pGameConfig, DynamicHook& pHandle, const char[] szFuncName)
{
	pHandle = DynamicHook.FromConf(pGameConfig, szFuncName);
	if (pHandle == null)
		SetFailState("Couldn't create hook %s", szFuncName);
}
#endif

//------------------------------------------------------
// Cookie utils
//------------------------------------------------------
#if defined _clientprefs_included
stock bool GetCookieBool(Cookie cookie, int client)
{
	static char szBuffer[2];
	cookie.Get(client, szBuffer, sizeof(szBuffer));
	return StringToInt(szBuffer) == 1;
}

stock void SetCookieBool(Cookie cookie, int client, bool val)
{
	cookie.Set(client, val? "1" : "0");
}

stock bool IsCookieSet(Cookie cookie, int client)
{
	static char szBuffer[2];
	cookie.Get(client, szBuffer, sizeof(szBuffer));
	return szBuffer[0] != '\0';
}
#endif

//------------------------------------------------------
// Text utils
//------------------------------------------------------
stock void Msg(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 3);
 	PrintToChat(client, "%s%s", SRCCOOP_CHAT_TAG, formatMsg);
}

stock void MsgReply(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 3);
 	ReplyToCommand(client, "%s%s", client && GetCmdReplySource() == SM_REPLY_TO_CHAT? SRCCOOP_CHAT_TAG : SRCCOOP_CONSOLE_TAG, formatMsg);
}

stock void MsgAll(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 2);
	PrintToServer("%s(chat) %s", SRCCOOP_CONSOLE_TAG, formatMsg);
 	PrintToChatAll("%s%s", SRCCOOP_CHAT_TAG, formatMsg);
}

stock void MsgSrv(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 2);
	PrintToServer("%s%s", SRCCOOP_CONSOLE_TAG, formatMsg);
}

#if defined _regex_included
stock bool StrEqualsRegex(const char[] pattern, const char[] str, bool caseSensitive = false)
{
	int patternLength = strlen(pattern);
	if(patternLength > 1 && pattern[0] == '/' && pattern[patternLength - 1] == '/')
	{
		if(str[0] == '\0') {
			// regexing empty string errors out..
			return false;
		}
		char[] patternStripped = new char[--patternLength];
		strcopy(patternStripped, patternLength, pattern[1]);
		int substrings = SimpleRegexMatch(str, patternStripped, caseSensitive? 0 : PCRE_CASELESS);
		return (substrings > 0);
	}
	else return strcmp(pattern, str, caseSensitive) == 0;
}
#endif

stock void FormatTimeLength(int seconds, char[] out, int size)
{
	int hours = seconds / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	if(hours != 0)
	{
		Format(out, size, "%dh ", hours);
	}
	if(mins != 0)
	{
		Format(out, size, "%s%dm ", out, mins);
	}
	if(secs != 0)
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
	if(hours != 0)
	{
		Format(out, size, "%d hour%s", hours, hours == 1? "":"s");
		separator = " ";
	}
	if(mins != 0)
	{
		Format(out, size, "%s%s%d minute%s", out, separator, mins, mins == 1? "":"s");
		separator = " ";
	}
	if(secs != 0)
	{
		Format(out, size, "%s%s%d second%s", out, mins == 0? separator : " and ", secs, secs == 1? "":"s");
	}
}

// Like SplitString but more efficient
stock int SplitByChar(const char[] szSource, char c, char[] szBuffer, int nSize)
{
	int i = 0;
	while (szSource[i] != c)
	{
		if(szSource[i] == '\0')
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

stock bool IsReplaceSynonym(const char[] str)
{
	return (strcmp(str, "replace", false) == 0 || strcmp(str, "set", false) == 0);
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
		if(pEntity.GetOwner() == pOwnerEntity)
		{
			return startEnt;
		}
	}
	return -1;
}

//------------------------------------------------------
// Player utils
//------------------------------------------------------
stock CBasePlayer GetNearestPlayer(CBaseEntity pOriginEntity, bool bOnlyAlive = true)
{
	float vecEntityPosition[3];
	pOriginEntity.GetAbsOrigin(vecEntityPosition);

	CBasePlayer pBestPlayer = CBasePlayer();
	float flBestDistance = FLT_MAX;
	for (int i = 1; i < (MaxClients + 1); i++)
	{
		CBasePlayer pPlayer = CBasePlayer(i);
		if (pPlayer.IsValid() && (!bOnlyAlive || pPlayer.IsAlive()))
		{
			float vecPlayerPosition[3];
			pPlayer.GetAbsOrigin(vecPlayerPosition);

			float flDistance = GetVectorDistance(vecEntityPosition, vecPlayerPosition, true);
			if (flDistance < flBestDistance)
			{
				pBestPlayer = pPlayer;
				flBestDistance = flDistance;
			}
		}
	}
	return pBestPlayer;
}

stock int GetRealClientCount(bool bInGameOnly, bool bAliveOnly = false, bool bNoSpectators = false)
{
	int clients;
	for(int i = 1; i <= MaxClients; i++)
	{
		if((bInGameOnly? IsClientInGame(i) : IsClientConnected(i))
			&& !IsFakeClient(i)
			&& (!bAliveOnly || IsPlayerAlive(i))
			&& (!bNoSpectators || GetClientTeam(i) != TEAM_SPECTATOR)
		) {
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

stock CanTeleportResult CanTeleportPlayer(CBasePlayer pPlayer, float vecOrigin[3], bool bExact = false)
{
	bool bDidHit;

	TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	bDidHit = TR_DidHit();
	if(!bDidHit) return CAN_TP_YES;
	
	TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	bDidHit = TR_DidHit();
	if(!bDidHit) return CAN_TP_MUSTDUCK;
	
	if(!bExact)
	{
		// try varying the positions in a radius
		static const float yaw[] = {0.0, 45.0, 90.0, 135.0, 180.0, 225.0, 270.0, 315.0};
		static const float distance[] = {25.0, 50.0};
		static const float zDistance[] = {0.0, 25.0, 50.0};
		
		for(int z = 0; z < sizeof(zDistance); z++)
		{
			for(int d = 0; d < sizeof(distance); d++)
			{
				for(int y = 0; y < sizeof(yaw); y++)
				{
					if(CanTeleportPlayer_Offset(vecOrigin, yaw[y], distance[d], zDistance[z]))
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
	if(!TR_DidHit())
	{
		vecOrigin = newPos;
		return true;
	}

  	return false;
}

public bool CanTeleportPlayer_TraceFilter(int entity, int mask, any data) {
	CBaseEntity pEntity = CBaseEntity(entity);
	if(pEntity.IsClassPlayer() || pEntity.IsPickupItem()) {
		return false;
	}
	return true;
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
	if ( (boxMin1[0] > boxMax2[0]) || (boxMax1[0] < boxMin2[0]) )
		return false;
	if ( (boxMin1[1] > boxMax2[1]) || (boxMax1[1] < boxMin2[1]) )
		return false;
	if ( (boxMin1[2] > boxMax2[2]) || (boxMax1[2] < boxMin2[2]) )
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

	if (file_source == null) {
		return false;
	}

	File file_destination = OpenFile(destination, "wb", use_valve_fs, valve_path_id);

	if (file_destination == null) {
		delete file_source;
		return false;
	}

	int buffer[32];
	int cache;

	while (!IsEndOfFile(file_source)) {
		cache = ReadFile(file_source, buffer, sizeof(buffer), 1);
		WriteFile(file_destination, buffer, cache, 1);
	}

	delete file_source;
	delete file_destination;

	return true;
}