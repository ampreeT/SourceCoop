#include <sourcemod>
#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Thirdperson",
	author = "Alienmario (Original thirdperson code by Dr. McKay)",
	description = "Thirdperson view switcher",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

#define MENUITEM_TOGGLE "ToggleThirdperson"

ConVar pEnabledCvar;
ConVar pCheatsCvar;
bool bIsInThirdperson[MAXPLAYERS+1];

public void OnPluginStart()
{
	InitSourceCoopAddon();
	LoadTranslations("srccoop_thirdperson.phrases");
	
	RegConsoleCmd("sm_thirdperson", Command_Thirdperson, "Type !thirdperson to go into thirdperson mode");
	RegConsoleCmd("sm_firstperson", Command_Firstperson, "Type !firstperson to exit thirdperson mode");
	pEnabledCvar = CreateConVar("sourcecoop_thirdperson_enabled", "1", "Is thirdperson enabled?");
	pCheatsCvar = FindConVar("sv_cheats");
	HookConVarChange(pEnabledCvar, EnabledCvarChanged);
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = SC_GetCoopTopMenu();
		TopMenuObject pSettingsMenu = pCoopMenu.FindCategory(COOPMENU_CATEGORY_PLAYER);
		if (pSettingsMenu != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_TOGGLE, ThirdPersonMenuHandler, pSettingsMenu);
		}
	}
}

public void ThirdPersonMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, "%T", "toggle thirdperson", param);
	}
	else if (action == TopMenuAction_SelectOption)
	{
		FakeClientCommand(param, bIsInThirdperson[param]? "sm_firstperson" : "sm_thirdperson");
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void EnabledCvarChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	bool enable = pEnabledCvar.BoolValue && SC_IsCurrentMapCoop();
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			if (!IsFakeClient(i))
			{
				if (enable)
				{
					SendConVarValue(i, pCheatsCvar, "1");
				}
				else
				{
					ClientCommand(i, "firstperson");
					SendConVarValue(i, pCheatsCvar, "0");
					bIsInThirdperson[i] = false;
				}
			}
		}
	}
}

public void OnClientPutInServer(int client)
{
	bIsInThirdperson[client] = false;
	if (pEnabledCvar.BoolValue && SC_IsCurrentMapCoop() && !IsFakeClient(client))
	{
		SendConVarValue(client, pCheatsCvar, "1");
	}
}

public Action Command_Thirdperson(int client, int args)
{
	if (!pEnabledCvar.BoolValue || !SC_IsCurrentMapCoop())
	{
		MsgReply(client, "%t", "thirdperson blocked");
		return Plugin_Handled;
	}
	SetThirdperson(client, true);
	MsgReply(client, "%t", "thirdperson enter");
	return Plugin_Handled;
}

public Action Command_Firstperson(int client, int args)
{
	if (!pEnabledCvar.BoolValue || !SC_IsCurrentMapCoop())
	{
		MsgReply(client, "%t", "thirdperson blocked");
		return Plugin_Handled;
	}
	SetThirdperson(client, false);
	MsgReply(client, "%t", "thirdperson exit");
	return Plugin_Handled;
}

void SetThirdperson(int client, bool enable)
{
	if (enable)
	{
		SendConVarValue(client, pCheatsCvar, "1");
		ClientCommand(client, "thirdperson");
		bIsInThirdperson[client] = true;
	}
	else
	{
		ClientCommand(client, "firstperson");

		if (IsPlayerAlive(client))
		{
			SC_FixWeaponPrediction(CBasePlayer(client));
		}

		bIsInThirdperson[client] = false;
	}
}