#include <sourcemod>
#include <srccoop/utils.inc>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "BM Coop Skip intro",
	author = "Alienmario",
	description = "Skip campaign intro player-vote",
	version = "1.0.0",
	url = ""
};

#define VOTE_DURATION 20
#define VOTE_COOLDOWN 60

int nextVote;

public void OnPluginStart()
{
	LoadTranslations("common.phrases");
	RegConsoleCmd("sm_skipintro", VoteSkipIntro, "Starts a skip intro vote");
	RegConsoleCmd("sm_introskip", VoteSkipIntro, "Starts a skip intro vote");
}

public Action VoteSkipIntro(int client, int args)
{
	char sCurrentMap[128];
	GetCurrentMap(sCurrentMap, sizeof(sCurrentMap));
	if(!StrEqual(sCurrentMap, "bm_c0a0a", false) && !StrEqual(sCurrentMap, "bm_c0a0b", false) && !StrEqual(sCurrentMap, "bm_c0a0c", false))
	{
		MsgReply(client, "This is not an intro map");
		return Plugin_Handled;
	}
	if (IsVoteInProgress())
	{
		MsgReply(client, "Another vote is already in progress");
		return Plugin_Handled;
	}
	if (GetTime() < nextVote)
	{
		char sTime[32];
		FormatTimeLengthLong(nextVote - GetTime(), sTime, sizeof(sTime));
		MsgReply(client, "Intro skip vote is not available for %s", sTime);
		return Plugin_Handled;
	}
	Menu menu = new Menu(VoteSkipHandler);
	menu.SetTitle("Skip intro?");
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%N started a skip intro vote!", client);
	return Plugin_Handled;
}

public int VoteSkipHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_Select)
	{
		MsgAll("%N voted %s", param1, param2 == 0? "[YES]" : "[NO]");
	}
	if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("Vote successfull!");
			ServerCommand("sm_map bm_c1a0a");
		} else {
			MsgAll("Vote failed!");
		}
	}
	else if(action == MenuAction_VoteCancel)
	{
		MsgAll("Vote cancelled!");
	}
	else if (action == MenuAction_End)
	{
		nextVote = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}
}