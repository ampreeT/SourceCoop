#if defined _srccoop_menu_included
 #endinput
#endif
#define _srccoop_menu_included

#pragma newdecls required
#pragma semicolon 1

TopMenu g_pCoopTopMenu;
TopMenuObject g_pCoopPlayerSettingsMenu;
TopMenuObject g_pCoopSoundSettingsMenu;
TopMenuObject g_pCoopVotingMenu;
TopMenuObject g_pCoopOthersMenu;

void InitializeMenus()
{
	RegConsoleCmd("sm_coopmenu", Command_CoopMenu, "Displays the coop menu");
	// The sizeup command is bound by default to equals, but doesn't actually exist, we can take advantake of this.
	RegConsoleCmd("sizeup", Command_CoopMenu, "Displays the coop menu");
	
	g_pCoopTopMenu = new TopMenu(CoopTopMenuHandler);
	g_pCoopPlayerSettingsMenu = g_pCoopTopMenu.AddCategory(COOPMENU_CATEGORY_PLAYER, CoopTopMenuHandler);
	g_pCoopSoundSettingsMenu = g_pCoopTopMenu.AddCategory(COOPMENU_CATEGORY_SOUNDS, CoopTopMenuHandler);
	g_pCoopVotingMenu = g_pCoopTopMenu.AddCategory(COOPMENU_CATEGORY_VOTING, CoopTopMenuHandler);
	g_pCoopOthersMenu = g_pCoopTopMenu.AddCategory(COOPMENU_CATEGORY_OTHER, CoopTopMenuHandler);
	
	char path[PLATFORM_MAX_PATH];
	char error[256];
	
	BuildPath(Path_SM, path, sizeof(path), "configs/srccoop/menu_sorting.txt");
	
	if (!g_pCoopTopMenu.LoadConfig(path, error, sizeof(error)))
	{
		LogError("Could not load SourceCoop menu config (file \"%s\": %s)", path, error);
	}
}

public void CoopTopMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject topobj_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayTitle && topobj_id == INVALID_TOPMENUOBJECT)
	{
		Format(buffer, maxlength, "SourceCoop Menu");
	}
	else if (action == TopMenuAction_DisplayOption || action == TopMenuAction_DisplayTitle)
	{
		if (topobj_id == g_pCoopPlayerSettingsMenu)
		{
			Format(buffer, maxlength, "Player settings");
		}
		else if (topobj_id == g_pCoopSoundSettingsMenu)
		{
			Format(buffer, maxlength, "Sound settings");
		}
		else if (topobj_id == g_pCoopVotingMenu)
		{
			Format(buffer, maxlength, "Votes");
		}
		else if (topobj_id == g_pCoopOthersMenu)
		{
			Format(buffer, maxlength, "Other");
		}
	}
}

public Action Command_CoopMenu(int client, int args)
{
	if (!client)
	{
		return Plugin_Handled;
	}
	if(!CoopManager.IsCoopModeEnabled())
	{
		MsgReply(client, "Coop menu is currently unavailable.");
		return Plugin_Handled;
	}
	g_pCoopTopMenu.Display(client, TopMenuPosition_Start);
	return Plugin_Handled;
}