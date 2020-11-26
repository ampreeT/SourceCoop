#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Voting",
	author = "Alienmario",
	description = "Provides coop related voting",
	version = "1.0.0",
	url = "https://github.com/ampreeT/SourceCoop"
};

#define VOTE_DURATION 20
#define VOTE_COOLDOWN 60

#define MENUITEM_SKIPINTRO "SkipIntroVote"

char INTROMAPS[][] = {"bm_c0a0a", "bm_c0a0b", "bm_c0a0c"};

int nextVote;

public void OnPluginStart()
{
	LoadTranslations("common.phrases");
	RegConsoleCmd("sm_skipintro", VoteSkipIntroCmd, "Starts a skip intro vote");
	RegConsoleCmd("sm_introskip", VoteSkipIntroCmd, "Starts a skip intro vote");
	
	if (LibraryExists(SRCCOOP_LIBRARY))
	{
		OnSourceCoopStarted();
	}
}

public void OnLibraryAdded(const char[] name)
{
	if(StrEqual(name, SRCCOOP_LIBRARY))
	{
		OnSourceCoopStarted();
	}
}

void OnSourceCoopStarted()
{
	TopMenu pCoopMenu = GetCoopTopMenu();
	TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_VOTING);
	if(pMenuCategory != INVALID_TOPMENUOBJECT)
	{
		pCoopMenu.AddItem(MENUITEM_SKIPINTRO, MyMenuHandler, pMenuCategory);
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject topobj_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, "Skip intro");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if(!StartVoteSkipIntro(param))
		{
			topmenu.Display(param, TopMenuPosition_LastCategory);
		}
	}
}

public Action VoteSkipIntroCmd(int client, int args)
{
	StartVoteSkipIntro(client);
	return Plugin_Handled;
}

bool StartVoteSkipIntro(int client)
{
	char sCurrentMap[128];
	bool found;
	GetCurrentMap(sCurrentMap, sizeof(sCurrentMap));
	for (int i = 0; i < sizeof(INTROMAPS); i++)
	{
		if(StrEqual(sCurrentMap, INTROMAPS[i], false))
		{
			found = true; break;
		}
	}
	if (!found)
	{
		MsgReply(client, "This is not an intro map");
		return false;
	}
	if (IsVoteInProgress())
	{
		MsgReply(client, "Another vote is already in progress");
		return false;
	}
	if (GetTime() < nextVote)
	{
		char sTime[32];
		FormatTimeLengthLong(nextVote - GetTime(), sTime, sizeof(sTime));
		MsgReply(client, "Skip intro vote is not available for %s", sTime);
		return false;
	}
	Menu menu = new Menu(VoteSkipHandler);
	menu.SetTitle("Skip intro?");
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%N started a skip intro vote!", client);
	return true;
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