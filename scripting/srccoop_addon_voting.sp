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
#define MENUITEM_RESTARTMAP "RestartMapVote"
#define MENUITEM_MAPVOTE "MapVote"

char INTROMAPS[][] = {"bm_c0a0a", "bm_c0a0b", "bm_c0a0c"};

int nextVoteSkip;
int nextVoteRestart;
int nextVoteMap;

ConVar pReloadMapsOnMapchange;

public void OnPluginStart()
{
	LoadTranslations("common.phrases");
	RegConsoleCmd("sm_skipintro", Command_VoteSkipIntro, "Starts a skip intro vote");
	RegConsoleCmd("sm_restartmap", Command_VoteRestartMap, "Starts a restart map vote");
	RegConsoleCmd("sm_changemap", Command_ChangeMap, "Shows a menu for changing maps");
	RegAdminCmd("sc_reload_maps", Command_ReloadMaps, ADMFLAG_ROOT, "Reloads all entries in the votemap menu from storage");
	pReloadMapsOnMapchange = CreateConVar("sourcecoop_voting_autoreload", "0", "Sets whether to reload all votemap menu entries on mapchange, which can prolong map loading times.", _, true, 0.0, true, 1.0);
	
	InitSourceCoopAddon();
	if (LibraryExists(SRCCOOP_LIBRARY))
	{
		OnSourceCoopStarted();
	}
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			OnClientPutInServer(i);
		}
	}
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		OnSourceCoopStarted();
	}
}

public void OnMapStart()
{
	static bool firstLoad = true;
	if (firstLoad || pReloadMapsOnMapchange.BoolValue)
	{
		firstLoad = false;
		BuildMaps();
	}
}

public Action Command_ReloadMaps(int client, int args)
{
	MsgReply(client, "Reloading maps");
	BuildMaps();
	return Plugin_Handled;
}

void OnSourceCoopStarted()
{
	TopMenu pCoopMenu = GetCoopTopMenu();
	TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_VOTING);
	if (pMenuCategory != INVALID_TOPMENUOBJECT)
	{
		pCoopMenu.AddItem(MENUITEM_SKIPINTRO, MyCoopMenuHandler, pMenuCategory);
		pCoopMenu.AddItem(MENUITEM_RESTARTMAP, MyCoopMenuHandler, pMenuCategory);
		pCoopMenu.AddItem(MENUITEM_MAPVOTE, MyCoopMenuHandler, pMenuCategory);
	}
}

public void OnCoopMapStart()
{
	char szCurrentMap[MAX_MAPNAME];
	GetCurrentMap(szCurrentMap, sizeof(szCurrentMap));
	if (IsIntroMap(szCurrentMap))
	{
		CreateTimer(15.0, Timer_StartVoteSkipIntro, _, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public void MyCoopMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject topobj_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		char szItem[32];
		topmenu.GetObjName(topobj_id, szItem, sizeof(szItem));
		if (StrEqual(szItem, MENUITEM_SKIPINTRO))
		{
			Format(buffer, maxlength, "Skip intro");
		}
		if (StrEqual(szItem, MENUITEM_RESTARTMAP))
		{
			Format(buffer, maxlength, "Restart current map");
		}
		else if(StrEqual(szItem, MENUITEM_MAPVOTE))
		{
			Format(buffer, maxlength, "Change map");
		}
	}
	else if (action == TopMenuAction_SelectOption)
	{
		char szItem[32];
		topmenu.GetObjName(topobj_id, szItem, sizeof(szItem));
		if (StrEqual(szItem, MENUITEM_SKIPINTRO))
		{
			if (!StartVoteSkipIntro(param))
			{
				topmenu.Display(param, TopMenuPosition_LastCategory);
			}
		}
		else if (StrEqual(szItem, MENUITEM_RESTARTMAP))
		{
			if (!StartVoteRestartMap(param))
			{
				topmenu.Display(param, TopMenuPosition_LastCategory);
			}
		}
		else if (StrEqual(szItem, MENUITEM_MAPVOTE))
		{
			if (!OpenMapSelectMenu(param))
			{
				topmenu.Display(param, TopMenuPosition_LastCategory);
			}
		}
	}
}

//------------------------------------------------------
// Skip intro voting
//------------------------------------------------------

public Action Timer_StartVoteSkipIntro(Handle timer)
{
	StartVoteSkipIntro(-1);
}

public Action Command_VoteSkipIntro(int client, int args)
{
	StartVoteSkipIntro(client);
	return Plugin_Handled;
}

bool StartVoteSkipIntro(int client)
{
	char szCurrentMap[MAX_MAPNAME];
	GetCurrentMap(szCurrentMap, sizeof(szCurrentMap));
	if (!IsIntroMap(szCurrentMap))
	{
		if (client != -1)
			MsgReply(client, "This is not an intro map");
		return false;
	}
	if (IsVoteInProgress())
	{
		if (client != -1)
			MsgReply(client, "Another vote is already in progress");
		return false;
	}
	if (GetTime() < nextVoteSkip && client != -1)
	{
		char sTime[32];
		FormatTimeLengthLong(nextVoteSkip - GetTime(), sTime, sizeof(sTime));
		MsgReply(client, "Skip intro vote is not available for another %s", sTime);
		return false;
	}
	Menu menu = new Menu(VoteSkipHandler);
	menu.SetTitle("Skip intro?");
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	if (client >= 0)
	{
		MsgAll("%N started a skip intro vote!", client);
	}
	else
	{
		MsgAll("Skip intro vote has started!");
	}
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
			MsgAll("Vote successful!");
			ServerCommand("sm_map bm_c1a0a");
		}
		else
		{
			MsgAll("Vote failed!");
		}
	}
	else if (action == MenuAction_VoteCancel)
	{
		MsgAll("Vote cancelled!");
	}
	else if (action == MenuAction_End)
	{
		nextVoteSkip = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}
}

bool IsIntroMap(char szMap[MAX_MAPNAME])
{
	for (int i = 0; i < sizeof(INTROMAPS); i++)
	{
		if (StrEqual(szMap, INTROMAPS[i], false))
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------
// Restart map voting
//------------------------------------------------------

public Action Command_VoteRestartMap(int client, int args)
{
	StartVoteRestartMap(client);
	return Plugin_Handled;
}

bool StartVoteRestartMap(int client)
{
	if (IsVoteInProgress())
	{
		MsgReply(client, "Another vote is already in progress");
		return false;
	}
	if (GetTime() < nextVoteRestart)
	{
		char sTime[32];
		FormatTimeLengthLong(nextVoteRestart - GetTime(), sTime, sizeof(sTime));
		MsgReply(client, "Restart vote is not available for another %s", sTime);
		return false;
	}
	Menu menu = new Menu(VoteRestartHandler);
	menu.SetTitle("Restart current map?");
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%N started a map restart vote!", client);
	return true;
}

public int VoteRestartHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_Select)
	{
		MsgAll("%N voted %s", param1, param2 == 0? "[YES]" : "[NO]");
	}
	if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("Vote successful!");
			char szCurrentMap[MAX_MAPNAME];
			GetCurrentMap(szCurrentMap, sizeof(szCurrentMap));
			ServerCommand("sm_map %s", szCurrentMap);
		}
		else
		{
			MsgAll("Vote failed!");
		}
	}
	else if (action == MenuAction_VoteCancel)
	{
		MsgAll("Vote cancelled!");
	}
	else if (action == MenuAction_End)
	{
		nextVoteRestart = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}
}

//------------------------------------------------------
// Map voting
//------------------------------------------------------

enum struct MapMenuSection
{
	char szName[32];
	ArrayList pSubSections;
	ArrayList pMaps;
}

void MapMenuSection_Init(MapMenuSection _this, const char[] szName)
{
	strcopy(_this.szName, sizeof(_this.szName), szName);
	_this.pSubSections = new ArrayList(sizeof(MapMenuSection));
	_this.pMaps = new ArrayList(MAX_MAPNAME);
}
	
void MapMenuSection_Destroy(MapMenuSection _this)
{
	if (_this.pSubSections != null)
	{
		int len = _this.pSubSections.Length;
		MapMenuSection pSubSection;
		for (int i = 0; i < len; i++)
		{
			_this.pSubSections.GetArray(i, pSubSection);
			MapMenuSection_Destroy(pSubSection);
		}
	}
	delete _this.pSubSections;
	delete _this.pMaps;
}

bool MapMenuSection_GetSubSection(MapMenuSection _this, const char[] szSection, MapMenuSection pSubSection, bool add = true)
{
	MapMenuSection pTemp;
	int len = _this.pSubSections.Length;
	for (int i = 0; i < len; i++)
	{
		_this.pSubSections.GetArray(i, pTemp);
		if(StrEqual(pTemp.szName, szSection))
		{
			pSubSection = pTemp;
			return true;
		}
	}
	if (add)
	{
		MapMenuSection_Init(pTemp, szSection);
		_this.pSubSections.PushArray(pTemp);
		pSubSection = pTemp;
		return true;
	}
	return false;
}

MapMenuSection pRootMapSection;
MapMenuSection pMapSection[MAXPLAYERS+1];
ArrayStack pMapMenuNavStack[MAXPLAYERS+1];
char szCurrentMapVote[MAX_MAPNAME];

#define CAMPAIGN_NONE "[Unnamed campaigns]"
#define CHAPTER_NONE "[Unnamed chapters]"
#define MAPMENU_SUBSECTION "0"
#define MAPMENU_MAP "1"

public void OnClientPutInServer(int client)
{
	pMapMenuNavStack[client] = new ArrayStack(sizeof(MapMenuSection));
}

public void OnClientDisconnect(int client)
{
	delete pMapMenuNavStack[client];
}

void BuildMaps()
{
	MapMenuSection_Destroy(pRootMapSection);
	MapMenuSection_Init(pRootMapSection, "");
	
	StringMap duplicityChecker = new StringMap();
	
	char szConfigPath[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, szConfigPath, sizeof(szConfigPath), "data/srccoop");
	LoadMapsInPath(szConfigPath, duplicityChecker);
	LoadMapsInPath("maps", duplicityChecker);
	
	delete duplicityChecker;
}

void LoadMapsInPath(const char szConfigPath[PLATFORM_MAX_PATH], StringMap duplicityChecker)
{
	char szFile[PLATFORM_MAX_PATH], szBuffer[PLATFORM_MAX_PATH];
	FileType fileType;
	DirectoryListing dir = OpenDirectory(szConfigPath, true);
	
	while (dir.GetNext(szFile, sizeof(szFile), fileType))
	{
		if (fileType == FileType_File)
		{
			int len = strlen(szFile);
			if (len >= 4 && strcmp(szFile[len - 4], ".edt", false) == 0)
			{
				FormatEx(szBuffer, sizeof(szBuffer), "%s/%s", szConfigPath, szFile);
				szFile[len - 4] = '\0';
				if(!IsMapValid(szFile))
				{
					continue;
				}
				KeyValues kv = new KeyValues("");
				kv.SetEscapeSequences(true);
				if (kv.ImportFromFile(szBuffer) && kv.GetSectionName(szBuffer, sizeof(szBuffer)) && strcmp(szBuffer, "config", false) == 0)
				{
					if (duplicityChecker.SetString(szFile, "", false))
					{
						LoadMap(szFile, kv);
					}
				}
				delete kv;
			}
		}
	}
	delete dir;
}

bool LoadMap(const char[] szMap, KeyValues kv)
{
	char szCampaign[64];
	char szChapter[64];
	kv.GetString("campaign", szCampaign, sizeof(szCampaign), CAMPAIGN_NONE);
	kv.GetString("chapter", szChapter, sizeof(szChapter), CHAPTER_NONE);

	MapMenuSection pSection;
	MapMenuSection_GetSubSection(pRootMapSection, szCampaign, pSection);
	MapMenuSection_GetSubSection(pSection, szChapter, pSection);
	pSection.pMaps.PushString(szMap);
}

public Action Command_ChangeMap(int client, int args)
{
	OpenMapSelectMenu(client);
	return Plugin_Handled;
}

bool OpenMapSelectMenu(int client)
{
	if (IsVoteInProgress())
	{
		MsgReply(client, "Another vote is already in progress");
		return false;
	}
	if (GetTime() < nextVoteMap)
	{
		char sTime[32];
		FormatTimeLengthLong(nextVoteMap - GetTime(), sTime, sizeof(sTime));
		MsgReply(client, "Votemap is not available for another %s", sTime);
		return false;
	}
	pMapSection[client] = pRootMapSection;
	ShowMapSelectMenu(client);
	return true;
}

Menu ShowMapSelectMenu(int client)
{
	Menu menu = new Menu(MapSelectMenuHandler);
	menu.ExitBackButton = true;
	menu.SetTitle("Select a map:");
	
	ArrayList pSubs = pMapSection[client].pSubSections;
	MapMenuSection pSub;
	for (int i = 0; i < pSubs.Length; i++)
	{
		pSubs.GetArray(i, pSub);
		menu.AddItem(MAPMENU_SUBSECTION, pSub.szName);
	}
	
	ArrayList pMaps = pMapSection[client].pMaps;
	char szMap[MAX_MAPNAME];
	for (int i = 0; i < pMaps.Length; i++)
	{
		pMaps.GetString(i, szMap, sizeof(szMap));
		menu.AddItem(MAPMENU_MAP, szMap);
	}
	
	menu.Display(client, MENU_TIME_FOREVER);
	return menu;
}

public int MapSelectMenuHandler(Menu menu, MenuAction action, int client, int param2)
{
	if (action == MenuAction_Select)
	{
		char szInfo[10], szDisp[MAX_MAPNAME];
		menu.GetItem(param2, szInfo, sizeof(szInfo), _, szDisp, sizeof(szDisp));
		
		if(StrEqual(szInfo, MAPMENU_SUBSECTION))
		{
			pMapMenuNavStack[client].PushArray(pMapSection[client]);
			pMapSection[client].pSubSections.GetArray(param2, pMapSection[client]);
			ShowMapSelectMenu(client);
		}
		else if (StrEqual(szInfo, MAPMENU_MAP))
		{
			StartVoteMap(client, szDisp);
		}
	}
	else if (action == MenuAction_Cancel)
	{
		if (param2 == MenuCancel_ExitBack)
		{
			if (pMapMenuNavStack[client].Empty)
			{
				TopMenu pCoopMenu = GetCoopTopMenu();
				pCoopMenu.Display(client, TopMenuPosition_LastCategory);
			}
			else
			{
				pMapMenuNavStack[client].PopArray(pMapSection[client]);
				ShowMapSelectMenu(client);
			}
		}
	}
	else if (action == MenuAction_End)
	{
		delete menu;
	}
}

void StartVoteMap(int client, const char szMap[MAX_MAPNAME])
{
	szCurrentMapVote = szMap;
	Menu menu = new Menu(VoteMapHandler);
	menu.SetTitle("Change map to %s?", szCurrentMapVote);
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%N started a new map vote!", client);
}

public int VoteMapHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_Select)
	{
		MsgAll("%N voted %s", param1, param2 == 0? "[YES]" : "[NO]");
	}
	if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("Vote successful!");
			ServerCommand("sm_map %s", szCurrentMapVote);
		}
		else
		{
			MsgAll("Vote failed!");
		}
	}
	else if(action == MenuAction_VoteCancel)
	{
		MsgAll("Vote cancelled!");
	}
	else if (action == MenuAction_End)
	{
		nextVoteMap = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}
}