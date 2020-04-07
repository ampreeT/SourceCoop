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

stock int GetRealClientCount(bool bInGameOnly, bool alive = false)
{
	int clients;
	for(int i = 1; i <= MaxClients; i++)
	{
		if( ((bInGameOnly)? IsClientInGame(i) : IsClientConnected(i)) && !IsFakeClient(i) && (!alive || IsPlayerAlive(i))) {
 			clients++;
 		}
 	}
	return clients;
}

/**
 * Fades a client's screen to a specified color
 * Your adviced to read the FFADE_ Comments
 *
 * @param client		Player for which to fade the screen
 * @param duration		duration in seconds the effect stays
 * @param mode			fade mode, see FFADE_ defines
 * @param holdtime		holdtime in seconds
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