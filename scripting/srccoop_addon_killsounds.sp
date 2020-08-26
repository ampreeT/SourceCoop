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
	version = "1.0.0",
	url = "https://github.com/ampreeT/SourceCoop"
};

#define SND_KILLNPC "buttons/button10.wav"
#define MENUITEM_TOGGLE "Toggle killsounds"

Cookie pEnabledCookie;
char g_szBuffer[2];

public void OnPluginStart()
{
	HookEvent("entity_killed", Event_EntityKilled, EventHookMode_Post);
	pEnabledCookie = new Cookie("sourcecoop_ks_enabled", "Killsounds", CookieAccess_Protected);
	
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
	TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_SOUND_SETTINGS);
	if(pMenuCategory != INVALID_TOPMENUOBJECT)
	{
		pCoopMenu.AddItem(MENUITEM_TOGGLE, MyMenuHandler, pMenuCategory);
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, MENUITEM_TOGGLE);
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if(AreClientCookiesCached(param))
		{
			pEnabledCookie.Get(param, g_szBuffer, sizeof(g_szBuffer));
			if(StringToInt(g_szBuffer))
			{
				pEnabledCookie.Set(param, "0");
				Msg(param, "NPC killsounds disabled.");
			}
			else
			{
				pEnabledCookie.Set(param, "1");
				Msg(param, "NPC killsounds enabled.");
			}
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void OnConfigsExecuted()
{
	PrecacheSound(SND_KILLNPC);
}

public void OnClientCookiesCached(int client)
{
	pEnabledCookie.Get(client, g_szBuffer, sizeof(g_szBuffer));
	if(!strlen(g_szBuffer))
	{
		// new player - default to ON
		pEnabledCookie.Set(client, "1");
	}
}

public void Event_EntityKilled(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	CBaseEntity pKilled = CBaseEntity(hEvent.GetInt("entindex_killed"));
	CBaseEntity pAttacker = CBaseEntity(hEvent.GetInt("entindex_attacker"));
	
	if(pAttacker.IsClassPlayer() && pKilled.IsClassNPC())
	{
		pEnabledCookie.Get(pAttacker.GetEntIndex(), g_szBuffer, sizeof(g_szBuffer));
		if(StringToInt(g_szBuffer))
		{
			// double the sound, double the fun (actualy just to hear it over gunfire..)
			EmitSoundToClient(pAttacker.GetEntIndex(), SND_KILLNPC);
			EmitSoundToClient(pAttacker.GetEntIndex(), SND_KILLNPC);
		}
	}
}
