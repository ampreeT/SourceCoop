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
ConVar pSoundEffectPath;

public void OnPluginStart()
{
	HookEvent("entity_killed", Event_EntityKilled, EventHookMode_Post);
	pEnabledCookie = new Cookie("sourcecoop_ks_enabled", "Killsounds", CookieAccess_Protected);
	pConvarDefault = CreateConVar("sourcecoop_ks_default", "0", "Sets the default setting of the killsounds player preference.", _, true, 0.0, true, 1.0);
	pSoundEffectPath = CreateConVar("sourcecoop_ks_path", "buttons/button10.wav", "Sets the path to the kill sound effect.");
	pSoundEffectPath.AddChangeHook(OnSndPathChange);

	InitSourceCoopAddon();
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_SOUNDS);
		if(pMenuCategory != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_TOGGLE_KILLSOUNDS, MyMenuHandler, pMenuCategory);
		}
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, GetCookieBool(pEnabledCookie, param) ? "Disable killsounds" : "Enable killsounds");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if(AreClientCookiesCached(param))
		{
			if(GetCookieBool(pEnabledCookie, param))
			{
				SetCookieBool(pEnabledCookie, param, false);
				Msg(param, "Kill confirm sounds disabled.");
			}
			else
			{
				SetCookieBool(pEnabledCookie, param, true);
				Msg(param, "Kill confirm sounds enabled.");
			}
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void OnConfigsExecuted()
{
	char szKillSnd[PLATFORM_MAX_PATH];
	pSoundEffectPath.GetString(szKillSnd, sizeof(szKillSnd));
	PrecacheSound(szKillSnd);
}

public void OnSndPathChange(ConVar pConvar, const char[] szOldPath, const char[] szNewPath)
{
	PrecacheSound(szNewPath);
}

public void OnClientCookiesCached(int client)
{
	if(!IsCookieSet(pEnabledCookie, client))
	{
		// new player - set the default
		SetCookieBool(pEnabledCookie, client, pConvarDefault.BoolValue);
	}
}

public void Event_EntityKilled(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	CBaseEntity pKilled = CBaseEntity(hEvent.GetInt("entindex_killed"));
	CBaseEntity pAttacker = CBaseEntity(hEvent.GetInt("entindex_attacker"));
	
	if(pAttacker.IsClassPlayer() && pKilled.IsClassNPC())
	{
		if(GetCookieBool(pEnabledCookie, pAttacker.GetEntIndex()))
		{
			char szKillSnd[PLATFORM_MAX_PATH];
			pSoundEffectPath.GetString(szKillSnd, sizeof(szKillSnd));
			// double the sound, double the fun (actualy just to hear it over gunfire..)
			EmitSoundToClient(pAttacker.GetEntIndex(), szKillSnd);
			EmitSoundToClient(pAttacker.GetEntIndex(), szKillSnd);
		}
	}
}
