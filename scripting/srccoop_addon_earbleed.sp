#include <sourcemod>
#include <clientprefs>
#include <dhooks>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Earbleed toggle",
	author = "Alienmario",
	description = "Adds a toggle for the ringing sound effect from explosions",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

#define MENUITEM_TOGGLE_EARBLEED "ToggleEarBleed"

Cookie pEnabledCookie;
ConVar pConvarDefault;

void LoadGameData()
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_GAMEDATA_NAME);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config: \"%s\"", SRCCOOP_GAMEDATA_NAME);

	Address pEngineSoundServer = GetInterface(pGameConfig, "engine", "IEngineSoundServer");
	if (!pEngineSoundServer)
		SetFailState("Could not get interface for IEngineSoundServer");

	DynamicHook hkSetPlayerDSP;
	LoadDHookVirtual(pGameConfig, hkSetPlayerDSP, "CEngineSoundServer::SetPlayerDSP");
	if (hkSetPlayerDSP.HookRaw(Hook_Pre, pEngineSoundServer, Hook_SetPlayerDSP) == INVALID_HOOK_ID)
		SetFailState("Could not hook CEngineSoundServer::SetPlayerDSP");
}

public void OnPluginStart()
{
	LoadTranslations("srccoop_earbleed.phrases");
	InitSourceCoopAddon();
	LoadGameData();

	pEnabledCookie = new Cookie("sourcecoop_earbleed_enabled", "Earbleed toggle", CookieAccess_Protected);
	pConvarDefault = CreateConVar("sourcecoop_earbleed_default", "0", "Sets the default setting of the earbleed player preference.", _, true, 0.0, true, 1.0);
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = SC_GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_SOUNDS);
		if (pMenuCategory != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_TOGGLE_EARBLEED, MyMenuHandler, pMenuCategory);
		}
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, "%T", pEnabledCookie.GetInt(param, pConvarDefault.IntValue) ? "disable earbleed" : "enable earbleed", param);
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(param))
		{
			if (pEnabledCookie.GetInt(param, pConvarDefault.IntValue))
			{
				pEnabledCookie.SetInt(param, 0);
				Msg(param, "%t", "earbleed disabled");
			}
			else
			{
				pEnabledCookie.SetInt(param, 1);
				Msg(param, "%t", "earbleed enabled");
			}
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public MRESReturn Hook_SetPlayerDSP(DHookParam hParams)
{
	Address recipientsFilter = hParams.GetAddress(1);
	CUtlVector pRecipients = CUtlVector(recipientsFilter + 8);

	int dspType = hParams.Get(2);
	if (35 <= dspType <= 37)
	{
		int len = pRecipients.Count();
		for (int i = 0; i < len; i++)
		{
			int client = pRecipients.GetPtr(i);
			if ((0 < client <= MaxClients) && IsClientInGame(client)
				&& !IsFakeClient(client) && ShouldRing(client))
			{
				ClientCommand(client, "dsp_player %d\n", dspType);
			}
		}
		return MRES_Supercede;
	}
	
	return MRES_Ignored;
}

bool ShouldRing(int client)
{
	return view_as<bool>(AreClientCookiesCached(client)? pEnabledCookie.GetInt(client, pConvarDefault.IntValue) : pConvarDefault.IntValue);
}