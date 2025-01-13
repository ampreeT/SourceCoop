#include <sourcemod>
#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Voting",
	author = "Alienmario",
	description = "Provides coop related voting",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

#define VOTE_DURATION 20
#define VOTE_COOLDOWN 60
#define VOTE_SUCCESS_MAPCHANGE_DELAY 4.0
#define VOTE_SKIP_AUTOSTART_DELAY 15.0

#define MENUITEM_SKIPINTRO "SkipIntroVote"
#define MENUITEM_RESTARTMAP "RestartMapVote"
#define MENUITEM_MAPVOTE "MapVote"
#define MENUITEM_SURVIVAL "SurvivalVote"

int nextVoteSkip;
int nextVoteRestart;
int nextVoteMap;
int nextVoteSurvival;
char szSkipTo[MAX_MAPNAME];
bool bAutoVoteSkip;

ConVar cvReloadMapsOnMapchange;
ConVar cvAllowVoteSkipIntro;
ConVar cvAllowVoteRestartMap;
ConVar cvAllowVoteChangeMap;
ConVar cvAllowVoteSurvival;

ConVar cvSurivalMode;

public void OnPluginStart()
{
	InitSourceCoopAddon();
	LoadTranslations("common.phrases"); /* reuse some translations (identified by use of capital letters) */
	LoadTranslations("srccoop_voting.phrases");
	RegConsoleCmd("sm_skipintro", Command_VoteSkipIntro, "Starts a skip intro vote");
	RegConsoleCmd("sm_restartmap", Command_VoteRestartMap, "Starts a restart map vote");
	RegConsoleCmd("sm_changemap", Command_ChangeMap, "Shows a menu for changing maps");
	RegConsoleCmd("sm_survival", Command_VoteSurvival, "Starts a survival vote");
	RegAdminCmd("sc_reload_maps", Command_ReloadMaps, ADMFLAG_ROOT, "Reloads all entries in the votemap menu from storage");
	cvReloadMapsOnMapchange = CreateConVar("sourcecoop_voting_autoreload", "1", "Sets whether to reload all votemap menu entries on mapchange, which can prolong map loading times.", _, true, 0.0, true, 1.0);
	cvAllowVoteSkipIntro = CreateConVar("sourcecoop_voting_skipintro", "1", "Allow skip intro voting?", _, true, 0.0, true, 1.0);
	cvAllowVoteRestartMap = CreateConVar("sourcecoop_voting_restartmap", "1", "Allow restart map voting?", _, true, 0.0, true, 1.0);
	cvAllowVoteChangeMap = CreateConVar("sourcecoop_voting_changemap", "1", "Allow change map voting?", _, true, 0.0, true, 1.0);
	cvAllowVoteSurvival = CreateConVar("sourcecoop_voting_survival", "2", "Allow survival mode voting? Use one of values from sourcecoop_survival_mode to select the mode to vote for.", _, true, 0.0);

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			OnClientPutInServer(i);
		}
	}
}

public void OnAllPluginsLoaded()
{
	if (!(cvSurivalMode = FindConVar("sourcecoop_survival_mode")))
	{
		ThrowError("sourcecoop_survival_mode does not exist!");
	}
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = SC_GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_VOTING);
		if (pMenuCategory != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_SKIPINTRO, MyCoopMenuHandler, pMenuCategory);
			pCoopMenu.AddItem(MENUITEM_RESTARTMAP, MyCoopMenuHandler, pMenuCategory);
			pCoopMenu.AddItem(MENUITEM_MAPVOTE, MyCoopMenuHandler, pMenuCategory);
			pCoopMenu.AddItem(MENUITEM_SURVIVAL, MyCoopMenuHandler, pMenuCategory);
		}
	}
}

public void OnMapStart()
{
	static bool firstLoad = true;
	if (firstLoad || cvReloadMapsOnMapchange.BoolValue)
	{
		firstLoad = false;
		MapParser.BuildMaps();
	}
	if (!SC_IsCurrentMapCoop())
	{
		szSkipTo = "";
	}
}

public void SC_OnCoopMapConfigLoaded(KeyValues kv, CoopConfigLocation location)
{
	kv.Rewind();
	
	kv.GetString("voting_skip_to", szSkipTo, sizeof(szSkipTo));
	bAutoVoteSkip = !!kv.GetNum("voting_skip_autostart");
	
	// Cache the edt file for votemap generation
	// provided that it won't be accessible outside of current map due to being packed inside the bsp.
	
	if (location == CCL_MAPS) // loaded from maps folder
	{
		char szMapFile[PLATFORM_MAX_PATH];
		GetCurrentMap(szMapFile, sizeof(szMapFile));
		Format(szMapFile, sizeof(szMapFile), "maps/%s.edt", szMapFile);
		
		if (!FileExists(szMapFile, true, "MOD")) // not inside the mod's maps
		{
			char szDest[PLATFORM_MAX_PATH];
			Format(szDest, sizeof(szDest), "%s.cached", szMapFile);
			FileCopy(szMapFile, szDest, true);
		}
	}
}

public Action Command_ReloadMaps(int client, int args)
{
	MsgReply(client, "%t", "reloading maps");
	MapParser.BuildMaps();
	return Plugin_Handled;
}

public void SC_OnCoopMapStart()
{
	if (szSkipTo[0] && bAutoVoteSkip)
	{
		CreateTimer(VOTE_SKIP_AUTOSTART_DELAY, Timer_StartVoteSkipIntro, _, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public void MyCoopMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject topobj_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		char szItem[32];
		topmenu.GetObjName(topobj_id, szItem, sizeof(szItem));
		SetGlobalTransTarget(param);

		if (StrEqual(szItem, MENUITEM_SURVIVAL))
		{
			Format(buffer, maxlength, "%t", SC_GetSurvivalMode()? "start disable vote" : "start enable vote", szItem);
		}
		else
		{
			Format(buffer, maxlength, "%t", szItem);
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
		else if (StrEqual(szItem, MENUITEM_SURVIVAL))
		{
			if (!StartVoteSurvival(param))
			{
				topmenu.Display(param, TopMenuPosition_LastCategory);
			}
		}
	}
}

//------------------------------------------------------
// Skip intro voting
//------------------------------------------------------

public void Timer_StartVoteSkipIntro(Handle timer)
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
	if (!cvAllowVoteSkipIntro.BoolValue)
	{
		if (client != -1)
			MsgReply(client, "%t", "vote disabled", MENUITEM_SKIPINTRO);
		return false;
	}
	if (!szSkipTo[0])
	{
		if (client != -1)
			MsgReply(client, "%t", "not intro map");
		return false;
	}
	if (IsVoteInProgress())
	{
		if (client != -1)
			MsgReply(client, "%t", "Vote in Progress");
		return false;
	}
	if (GetTime() < nextVoteSkip && client != -1)
	{
		char szTime[32];
		FormatTimeLength(nextVoteSkip - GetTime(), szTime, sizeof(szTime));
		MsgReply(client, "%t", "vote cooldown", MENUITEM_SKIPINTRO, szTime);
		return false;
	}
	Menu menu = new Menu(VoteSkipHandler, MenuAction_DisplayItem | MenuAction_Display);
	menu.SetTitle(MENUITEM_SKIPINTRO);
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	if (client >= 0)
	{
		MsgAll("%t", "vote started by player", client, MENUITEM_SKIPINTRO);
	}
	else
	{
		MsgAll("%t", "vote started", MENUITEM_SKIPINTRO);
	}
	return true;
}

public int VoteSkipHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_DisplayItem || action == MenuAction_Display || action == MenuAction_Select || action == MenuAction_VoteCancel)
	{
		return MenuTranslationHelper(menu, action, param1, param2);
	}
	else if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("%t", "vote successful");
			MsgAll("%t", "changing map to", szSkipTo);
			StartMapVoteFinishedTimer(szSkipTo, SC_VOTING_SKIP_MAPCHANGE);
		}
		else
		{
			MsgAll("%t", "vote unsuccessful");
		}
	}
	else if (action == MenuAction_End)
	{
		nextVoteSkip = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}

	return -1;
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
	if (!cvAllowVoteRestartMap.BoolValue)
	{
		MsgReply(client, "%t", "vote disabled", MENUITEM_RESTARTMAP);
		return false;
	}
	if (IsVoteInProgress())
	{
		MsgReply(client, "%t", "Vote in Progress");
		return false;
	}
	if (GetTime() < nextVoteRestart)
	{
		char szTime[32];
		FormatTimeLength(nextVoteRestart - GetTime(), szTime, sizeof(szTime));
		MsgReply(client, "%t", "vote cooldown", MENUITEM_RESTARTMAP, szTime);
		return false;
	}
	Menu menu = new Menu(VoteRestartHandler, MenuAction_DisplayItem | MenuAction_Display);
	menu.SetTitle(MENUITEM_RESTARTMAP);
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%t", "vote started by player", client, MENUITEM_RESTARTMAP);
	return true;
}

public int VoteRestartHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_DisplayItem || action == MenuAction_Display || action == MenuAction_Select || action == MenuAction_VoteCancel)
	{
		return MenuTranslationHelper(menu, action, param1, param2);
	}
	else if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("%t", "vote successful");
			MsgAll("%t", "restarting map");
			char szCurrentMap[MAX_MAPNAME];
			GetCurrentMap(szCurrentMap, sizeof(szCurrentMap));
			StartMapVoteFinishedTimer(szCurrentMap, SC_VOTING_RESTART_MAPCHANGE);
		}
		else
		{
			MsgAll("%t", "vote unsuccessful");
		}
	}
	else if (action == MenuAction_End)
	{
		nextVoteRestart = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}

	return -1;
}

//------------------------------------------------------
// Survival voting
//------------------------------------------------------

public Action Command_VoteSurvival(int client, int args)
{
	StartVoteSurvival(client);
	return Plugin_Handled;
}

bool StartVoteSurvival(int client)
{
	if (!cvAllowVoteSurvival.IntValue)
	{
		MsgReply(client, "%t", "vote disabled", MENUITEM_SURVIVAL);
		return false;
	}
	if (IsVoteInProgress())
	{
		MsgReply(client, "%t", "Vote in Progress");
		return false;
	}
	if (GetTime() < nextVoteSurvival)
	{
		char szTime[32];
		FormatTimeLength(nextVoteSurvival - GetTime(), szTime, sizeof(szTime));
		MsgReply(client, "%t", "vote cooldown", MENUITEM_SURVIVAL, szTime);
		return false;
	}
	Menu menu = new Menu(VoteSurvivalHandler, MenuAction_DisplayItem | MenuAction_Display);
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	cvSurivalMode.AddChangeHook(OnSurvivalModeChangedWhileVoting);
	MsgAll("%t", "vote started by player", client, MENUITEM_SURVIVAL);
	return true;
}

public int VoteSurvivalHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_DisplayItem || action == MenuAction_Select)
	{
		return MenuTranslationHelper(menu, action, param1, param2);
	}
	else if (action == MenuAction_Display)
	{
		char buffer[256];
		Format(buffer, sizeof(buffer), "%T?",
			cvSurivalMode.IntValue ? "start disable vote" : "start enable vote",
			param1, MENUITEM_SURVIVAL
		);
		view_as<Panel>(param2).SetTitle(buffer);
	}
	else if (action == MenuAction_VoteEnd)
	{
		cvSurivalMode.RemoveChangeHook(OnSurvivalModeChangedWhileVoting);
		if (param1 == 0)
		{
			MsgAll("%t", "vote successful");
			int mode = cvAllowVoteSurvival.IntValue;
			if (mode)
			{
				if (cvSurivalMode.IntValue)
				{
					cvSurivalMode.IntValue = 0;
					MsgAll("%t", "disable vote successful", MENUITEM_SURVIVAL);
				}
				else
				{
					cvSurivalMode.IntValue = mode;
					MsgAll("%t", "enable vote successful", MENUITEM_SURVIVAL);
				}
			}
		}
		else
		{
			MsgAll("%t", "vote unsuccessful");
		}
	}
	else if (action == MenuAction_VoteCancel)
	{
		cvSurivalMode.RemoveChangeHook(OnSurvivalModeChangedWhileVoting);
		return MenuTranslationHelper(menu, action, param1, param2);
	}
	else if (action == MenuAction_End)
	{
		nextVoteSurvival = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}

	return -1;
}

void OnSurvivalModeChangedWhileVoting(ConVar convar, const char[] oldValue, const char[] newValue)
{
	CancelVote();
}

//------------------------------------------------------
// Votemap - Structure
//------------------------------------------------------

enum struct MapMenuSection
{
	char szName[32];
	ArrayList pSubSections;
	ArrayList pMaps;

	void Init(const char[] szName)
	{
		strcopy(this.szName, sizeof(this.szName), szName);
		this.pSubSections = new ArrayList(sizeof(MapMenuSection));
		this.pMaps = new ArrayList(MAX_MAPNAME);
	}

	void Close()
	{
		if (this.pSubSections != null)
		{
			int len = this.pSubSections.Length;
			MapMenuSection pSubSection;
			for (int i = 0; i < len; i++)
			{
				this.pSubSections.GetArray(i, pSubSection);
				pSubSection.Close();
			}
		}
		delete this.pSubSections;
		delete this.pMaps;
	}

	bool GetSubSection(const char[] szSection, MapMenuSection pSubSection, bool add = true)
	{
		MapMenuSection pTemp;
		int len = this.pSubSections.Length;
		for (int i = 0; i < len; i++)
		{
			this.pSubSections.GetArray(i, pTemp);
			if (StrEqual(pTemp.szName, szSection))
			{
				pSubSection = pTemp;
				return true;
			}
		}
		if (add)
		{
			pTemp.Init(szSection);
			this.pSubSections.PushArray(pTemp);
			pSubSection = pTemp;
			return true;
		}
		return false;
	}
}

//------------------------------------------------------
// Votemap - Parser
//------------------------------------------------------

#define CAMPAIGN_NONE "[Unnamed campaigns]"
#define CHAPTER_NONE "[Unnamed chapters]"

MapMenuSection pRootMapSection;

methodmap MapParser
{
	public static void BuildMaps()
	{
		float flStartTime = GetEngineTime();

		pRootMapSection.Close();
		pRootMapSection.Init("");
		
		StringMap duplicityChecker = new StringMap();
		
		char szConfigPath[PLATFORM_MAX_PATH];
		BuildPath(Path_SM, szConfigPath, sizeof(szConfigPath), "data/srccoop");
		MapParser.LoadMapsInPath(szConfigPath, duplicityChecker);
		MapParser.LoadMapsInPath("maps", duplicityChecker, true);
		
		MsgSrv("%t", "scanned maps", duplicityChecker.Size, GetEngineTime() - flStartTime);
		delete duplicityChecker;
	}

	public static void LoadMapsInPath(const char szConfigPath[PLATFORM_MAX_PATH], StringMap duplicityChecker, bool bLoadCached = false)
	{
		char szFile[PLATFORM_MAX_PATH], szBuffer[PLATFORM_MAX_PATH];
		FileType fileType;
		DirectoryListing dir = OpenDirectory(szConfigPath, true);
		
		while (dir.GetNext(szFile, sizeof(szFile), fileType))
		{
			if (fileType == FileType_File)
			{
				int len = strlen(szFile);
				int extLen;
				if (len > 4 && strcmp(szFile[len - 4], ".edt", false) == 0)
				{
					extLen = 4;
				}
				else if (bLoadCached && len > 11 && strcmp(szFile[len - 11], ".edt.cached", false) == 0)
				{
					extLen = 11;
				}
				else continue;
				
				FormatEx(szBuffer, sizeof(szBuffer), "%s/%s", szConfigPath, szFile);
				szFile[len - extLen] = '\0';
				if (!IsMapValid(szFile))
				{
					continue;
				}
				KeyValues kv = new KeyValues("");
				kv.SetEscapeSequences(true);
				if (kv.ImportFromFile(szBuffer) && kv.GetSectionName(szBuffer, sizeof(szBuffer)) && strcmp(szBuffer, "config", false) == 0)
				{
					if (duplicityChecker.SetString(szFile, "", false))
					{
						MapParser.LoadMap(szFile, kv);
					}
				}
				delete kv;
			}
		}
		delete dir;
	}

	public static void LoadMap(const char[] szMap, KeyValues kv)
	{
		char szCampaign[64];
		char szChapter[64];
		kv.GetString("campaign", szCampaign, sizeof(szCampaign), CAMPAIGN_NONE);
		kv.GetString("chapter", szChapter, sizeof(szChapter), CHAPTER_NONE);

		MapMenuSection pSection;
		pRootMapSection.GetSubSection(szCampaign, pSection);
		pSection.GetSubSection(szChapter, pSection);
		pSection.pMaps.PushString(szMap);
	}
}

//------------------------------------------------------
// Votemap - Client menu handling
//------------------------------------------------------

MapMenuSection pMapSection[MAXPLAYERS+1];
ArrayStack pMapMenuNavStack[MAXPLAYERS+1];
char szCurrentMapVote[MAX_MAPNAME];

public void OnClientPutInServer(int client)
{
	pMapMenuNavStack[client] = new ArrayStack(sizeof(MapMenuSection));
}

public void OnClientDisconnect(int client)
{
	delete pMapMenuNavStack[client];
}

public Action Command_ChangeMap(int client, int args)
{
	OpenMapSelectMenu(client);
	return Plugin_Handled;
}

bool OpenMapSelectMenu(int client)
{
	if (!cvAllowVoteChangeMap.BoolValue)
	{
		MsgReply(client, "%t", "vote disabled", MENUITEM_MAPVOTE);
		return false;
	}
	if (IsVoteInProgress())
	{
		MsgReply(client, "%t", "Vote in Progress");
		return false;
	}
	if (GetTime() < nextVoteMap)
	{
		char szTime[32];
		FormatTimeLength(nextVoteMap - GetTime(), szTime, sizeof(szTime));
		MsgReply(client, "%t", "vote cooldown", MENUITEM_MAPVOTE, szTime);
		return false;
	}
	pMapSection[client] = pRootMapSection;
	ShowMapSelectMenu(client);
	return true;
}

#define MAPMENU_SUBSECTION "0"
#define MAPMENU_MAP "1"

Menu ShowMapSelectMenu(int client)
{
	Menu menu = new Menu(MapSelectMenuHandler, MenuAction_Display);
	menu.ExitBackButton = true;
	menu.SetTitle("select a map");
	
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
	if (action == MenuAction_Display)
	{
		return MenuTranslationHelper(menu, action, client, param2, false);
	}
	else if (action == MenuAction_Select)
	{
		char szInfo[10], szDisp[MAX_MAPNAME];
		menu.GetItem(param2, szInfo, sizeof(szInfo), _, szDisp, sizeof(szDisp));
		
		if (StrEqual(szInfo, MAPMENU_SUBSECTION))
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
				TopMenu pCoopMenu = SC_GetCoopTopMenu();
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
	
	return -1;
}

void StartVoteMap(int client, const char szMap[MAX_MAPNAME])
{
	szCurrentMapVote = szMap;
	Menu menu = new Menu(VoteMapHandler, MenuAction_DisplayItem | MenuAction_Display);
	menu.AddItem("0", "Yes");
	menu.AddItem("1", "No");
	menu.ExitButton = false;
	menu.DisplayVoteToAll(VOTE_DURATION);
	MsgAll("%t", "vote started by player", client, MENUITEM_MAPVOTE);
}

public int VoteMapHandler(Menu menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_DisplayItem || action == MenuAction_Select || action == MenuAction_VoteCancel)
	{
		return MenuTranslationHelper(menu, action, param1, param2);
	}
	else if (action == MenuAction_Display)
	{
		char buffer[256];
		Format(buffer, sizeof(buffer), "%T", "change map to?", param1, szCurrentMapVote);
		view_as<Panel>(param2).SetTitle(buffer);
	}
	else if (action == MenuAction_VoteEnd)
	{
		if (param1 == 0)
		{
			MsgAll("%t", "vote successful");
			MsgAll("%t", "changing map to", szCurrentMapVote);
			StartMapVoteFinishedTimer(szCurrentMapVote, SC_VOTING_VOTEMAP_MAPCHANGE);
		}
		else
		{
			MsgAll("%t", "vote unsuccessful");
		}
	}
	else if (action == MenuAction_End)
	{
		nextVoteMap = GetTime() + VOTE_COOLDOWN;
		delete menu;
	}
	
	return -1;
}

//------------------------------------------------------
// Helpers
//------------------------------------------------------

void StartMapVoteFinishedTimer(const char szMap[MAX_MAPNAME], const char szReason[32])
{
	DataPack dp; CreateDataTimer(VOTE_SUCCESS_MAPCHANGE_DELAY, Timer_ChangeMap, dp, TIMER_FLAG_NO_MAPCHANGE);
	dp.WriteString(szMap);
	dp.WriteString(szReason);
}

public void Timer_ChangeMap(Handle timer, DataPack dp)
{
	char szMap[MAX_MAPNAME], szReason[32];
	dp.Reset();
	dp.ReadString(szMap, sizeof(szMap));
	dp.ReadString(szReason, sizeof(szReason));
	ForceChangeLevel(szMap, szReason);
}

int MenuTranslationHelper(Menu menu, MenuAction action, int param1, int param2, bool appendQuestionMark = true)
{
	char buffer[256];
	if (action == MenuAction_DisplayItem)
	{
		/* Get the display string, we'll use it as a translation phrase */
		menu.GetItem(param2, "", 0, _, buffer, sizeof(buffer));
		Format(buffer, sizeof(buffer), "%T", buffer, param1);
		return RedrawMenuItem(buffer);
	}
	else if (action == MenuAction_Display)
	{
		/* Get the menu title, we'll use it as a translation phrase */
		menu.GetTitle(buffer, sizeof(buffer));
		Format(buffer, sizeof(buffer), "%T%s", buffer, param1, appendQuestionMark? "?" : "");
		view_as<Panel>(param2).SetTitle(buffer);
	}
	else if (action == MenuAction_Select)
	{
		/* Get the display string, we'll use it as a translation phrase */
		menu.GetItem(param2, "", 0, _, buffer, sizeof(buffer));
		MsgAll("%t [%t]", "player voted", param1, buffer);
	}
	else if (action == MenuAction_VoteCancel)
	{
		MsgAll("%t", "Cancelled Vote");
	}

	return -1;
}