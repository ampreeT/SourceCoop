#if defined _srccoop_menu_included
 #endinput
#endif
#define _srccoop_menu_included

TopMenu g_pCoopTopMenu;
TopMenuObject g_pCoopPlayerSettingsMenu;

public any Native_GetCoopTopMenu(Handle plugin, int numParams)
{
	return g_pCoopTopMenu;
}

void InitializeMenus()
{
	RegConsoleCmd("sm_coopmenu", Command_CoopMenu, "Displays the coop menu");
	// The sizeup command is bound by default to equals, but doesn't actually exist, we can take advantake of this.
	RegConsoleCmd("sizeup", Command_CoopMenu, "Displays the coop menu");
	
	g_pCoopTopMenu = new TopMenu(CoopTopMenuHandler);
	g_pCoopPlayerSettingsMenu = g_pCoopTopMenu.AddCategory(COOPMENU_CATEGORY_SETTINGS, CoopTopMenuHandler);
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
			Format(buffer, maxlength, COOPMENU_CATEGORY_SETTINGS);
		}
	}
}

public Action Command_CoopMenu(int client, int args)
{
	if (!client)
	{
		return Plugin_Handled;
	}
	g_pCoopTopMenu.Display(client, TopMenuPosition_Start);
	return Plugin_Handled;
}