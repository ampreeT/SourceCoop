#include <sourcemod>
#include <clientprefs>

#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop KillSounds",
	author = "Alienmario",
	description = "",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

#define MENUITEM_TOGGLE_KILLSOUNDS "ToggleKillsounds"

Cookie pEnabledCookie;
ConVar pConvarDefault;
char szSoundPath[PLATFORM_MAX_PATH];
float flSoundVol;

public void OnPluginStart()
{
	LoadTranslations("srccoop_killsounds.phrases");
	InitSourceCoopAddon();
	
	GameData pConfig = LoadSourceCoopConfig();
	GetGamedataString(pConfig, "KILLSOUNDS_SND", szSoundPath, sizeof(szSoundPath));
	flSoundVol = GetGamedataFloat(pConfig, "KILLSOUNDS_SND_VOL");
	pConfig.Close();

	HookEvent("entity_killed", Event_EntityKilled, EventHookMode_Post);
	pEnabledCookie = new Cookie("sourcecoop_ks_enabled", "Killsounds", CookieAccess_Protected);
	pConvarDefault = CreateConVar("sourcecoop_ks_default", "0", "Sets the default setting of the killsounds player preference.", _, true, 0.0, true, 1.0);
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = SC_GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_SOUNDS);
		if (pMenuCategory != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_TOGGLE_KILLSOUNDS, MyMenuHandler, pMenuCategory);
		}
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, "%T", GetCookieBool(pEnabledCookie, param) ? "disable killsounds" : "enable killsounds", param);
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(param))
		{
			if (GetCookieBool(pEnabledCookie, param))
			{
				SetCookieBool(pEnabledCookie, param, false);
				Msg(param, "%t", "killsounds disabled");
			}
			else
			{
				SetCookieBool(pEnabledCookie, param, true);
				Msg(param, "%t", "killsounds enabled");
			}
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void OnConfigsExecuted()
{
	PrecacheSound(szSoundPath, true);
}

public void OnClientCookiesCached(int client)
{
	if (!IsCookieSet(pEnabledCookie, client))
	{
		// new player - set the default
		SetCookieBool(pEnabledCookie, client, pConvarDefault.BoolValue);
	}
}

public void Event_EntityKilled(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	CBaseEntity pKilled = CBaseEntity(hEvent.GetInt("entindex_killed"));
	CBaseEntity pAttacker = CBaseEntity(hEvent.GetInt("entindex_attacker"));
	
	if (pAttacker.IsClassPlayer() && pKilled.IsClassNPC())
	{
		if (GetCookieBool(pEnabledCookie, pAttacker.entindex))
		{
			EmitSoundToClient(pAttacker.entindex, szSoundPath, .level = SNDLEVEL_NONE, .volume = flSoundVol);
		}
	}
}
