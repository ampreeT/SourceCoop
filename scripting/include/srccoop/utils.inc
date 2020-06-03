#if defined _srccoop_utils_included
 #endinput
#endif
#define _srccoop_utils_included

#include <srccoop/typedef.inc>
#include <srccoop/classdef.inc>

stock void Msg(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 3);
 	PrintToChat(client, "%s%s", CHAT_TAG, formatMsg);
}

stock void MsgReply(int client, const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 3);
 	ReplyToCommand(client, "%s%s", client && GetCmdReplySource() == SM_REPLY_TO_CHAT? CHAT_TAG : "", formatMsg);
}

stock void MsgAll(const char[] msg, any ...)
{
	int len = strlen(msg) + 255;
	char[] formatMsg = new char[len];
	VFormat(formatMsg, len, msg, 2);
	PrintToServer(formatMsg);
 	PrintToChatAll("%s%s", CHAT_TAG, formatMsg);
}

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
		if (pEntity.GetTargetname(szTargetname, sizeof(szTargetname)) && strcmp(szTargetname, targetname) == 0)
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

stock CBaseEntity CreateByClassname(const char[] szClassname)
{
	return CBaseEntity(CreateEntityByName(szClassname));
}

stock CBlackMesaPlayer GetNearestPlayer(CBaseEntity pOriginEntity)
{
	float vecEntityPosition[3];
	pOriginEntity.GetAbsOrigin(vecEntityPosition);

	CBlackMesaPlayer pBestPlayer = CBlackMesaPlayer();
	float flBestDistance = FLT_MAX;
	for (int i = 1; i < (MaxClients + 1); i++)
	{
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
		if (pPlayer.IsValid() && pPlayer.IsAlive())
		{
			float vecPlayerPosition[3];
			pPlayer.GetAbsOrigin(vecPlayerPosition);

			float flDistance = GetVectorDistance(vecEntityPosition, vecPlayerPosition, false);
			if (flDistance < flBestDistance)
			{
				pBestPlayer = pPlayer;
				flBestDistance = flDistance;
			}
		}
	}
	return pBestPlayer;
}

stock int GetRealClientCount(bool bInGameOnly, bool bAlive = false, bool bNoSpectators = false)
{
	int clients;
	for(int i = 1; i <= MaxClients; i++)
	{
		if( (bInGameOnly? IsClientInGame(i) : IsClientConnected(i)) && !IsFakeClient(i)
			&& (!bAlive || IsPlayerAlive(i))
			&& (!bNoSpectators || GetClientTeam(i) != TEAM_SPECTATOR)
		) {
 			clients++;
 		}
 	}
	return clients;
}

enum CanTeleportResult
{
	NO,
	YES,
	YES_MUSTDUCK		//🦆//
}

stock CanTeleportResult CanTeleportPlayer(CBlackMesaPlayer pPlayer, float vecOrigin[3])
{
	bool bDidHit;

	TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	bDidHit = TR_DidHit();
	if(!bDidHit) return YES;
	
	TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	bDidHit = TR_DidHit();
	if(!bDidHit) return YES_MUSTDUCK;
	
	return NO;
}

public bool CanTeleportPlayer_TraceFilter(int entity, int mask, any data) {
	CBaseEntity pEntity = CBaseEntity(entity);
	if(pEntity.IsClassPlayer() || pEntity.IsPickupItem()) {
		return false;
	}
	return true;
}

stock void ScaleVectors(const float vecA[3], const float vecB[3], float vecResult[3])
{
	vecResult[0] = vecA[0] * vecB[0];
	vecResult[1] = vecA[1] * vecB[1];
	vecResult[2] = vecA[2] * vecB[2];
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

	if (userMessage == INVALID_HANDLE) {
		return false;
	}

	if (GetFeatureStatus(FeatureType_Native, "GetUserMessageType") == FeatureStatus_Available &&
		GetUserMessageType() == UM_Protobuf) {

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
	else {
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