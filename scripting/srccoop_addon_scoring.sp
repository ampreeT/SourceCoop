#include <sourcemod>
#include <sdkhooks>
#include <clientprefs>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Scoring",
	author = "",
	description = "Placeholder plugin",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

/* 
public void OnPluginStart()
{
	InitSourceCoopAddon();
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = SC_GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_OTHER);
		if (pMenuCategory != INVALID_TOPMENUOBJECT)
		{

		}
	}
}
*/