#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Thirdperson",
	author = "Alienmario (Original thirdperson code by Dr. McKay)",
	description = "Thirdperson view switcher",
	version = "1.0.0",
	url = "https://github.com/ampreeT/SourceCoop"
};

ConVar enabledCvar;
ConVar sv_cheats;
bool isInThirdperson[MAXPLAYERS+1];

#define MENUITEM_TOGGLE "Toggle thirdperson"

public void OnPluginStart()
{
	RegConsoleCmd("sm_thirdperson", Command_Thirdperson, "Type !thirdperson to go into thirdperson mode");
	RegConsoleCmd("sm_firstperson", Command_Firstperson, "Type !firstperson to exit thirdperson mode");
	enabledCvar = CreateConVar("sourcecoop_thirdperson_enabled", "1", "Is thirdperson enabled?");
	sv_cheats = FindConVar("sv_cheats");
	HookConVarChange(enabledCvar, CvarChanged);
}

public void OnAllPluginsLoaded()
{
	TopMenu coopMenu;
	if (LibraryExists(SRCCOOP_LIBRARY) && (coopMenu = GetCoopTopMenu()) != null)
	{
		TopMenuObject playerSettings = coopMenu.FindCategory(COOPMENU_CATEGORY_SETTINGS);
		if(playerSettings == INVALID_TOPMENUOBJECT)
		{
			return;
		}
		coopMenu.AddItem(MENUITEM_TOGGLE, ThirdPersonMenuHandler, playerSettings);
	}
}

public void ThirdPersonMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, MENUITEM_TOGGLE);
	}
	else if (action == TopMenuAction_SelectOption)
	{
		FakeClientCommand(param, isInThirdperson[param]? "sm_firstperson" : "sm_thirdperson");
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void CvarChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	for(int i = 1; i <= MaxClients; i++)
	{
		if(IsClientInGame(i))
		{
			if(enabledCvar.BoolValue)
			{
				SendConVarValue(i, sv_cheats, "1");
			}
			else
			{
				ClientCommand(i, "firstperson");
				SendConVarValue(i, sv_cheats, "0");
				isInThirdperson[i] = false;
			}
		}
	}
}

public void OnClientPutInServer(int client)
{
	isInThirdperson[client] = false;
	if(enabledCvar.BoolValue) {
		SendConVarValue(client, sv_cheats, "1");
	}
}

public Action Command_Thirdperson(int client, int args)
{
	if(!enabledCvar.BoolValue)
	{
		MsgReply(client, "Thirdperson mode is currently disabled");
		return Plugin_Handled;
	}
	SetThirdperson(client, true);
	MsgReply(client, "You are now in thirdperson mode.");
	return Plugin_Handled;
}

public Action Command_Firstperson(int client, int args)
{
	if(!enabledCvar.BoolValue)
	{
		MsgReply(client, "Thirdperson mode is currently disabled");
		return Plugin_Handled;
	}
	SetThirdperson(client, false);
	MsgReply(client, "You are no longer in thirdperson mode.");
	return Plugin_Handled;
}

void SetThirdperson(int client, bool enable)
{
	if(enable)
	{
		SendConVarValue(client, sv_cheats, "1");
		ClientCommand(client, "thirdperson");
		isInThirdperson[client] = true;
	}
	else
	{
		ClientCommand(client, "firstperson");
		isInThirdperson[client] = false;
	}
}