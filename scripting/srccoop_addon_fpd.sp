#include <sdkhooks>
#include <dhooks>
#include <sourcemod>
#include <clientprefs>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name        = "SourceCoop First person death",
	author      = "AmpreeT, Alienmario",
	description = "First person deaths and fade",
	version     = SRCCOOP_VERSION,
	url         = SRCCOOP_URL
};

#define MENUITEM_TOGGLE_FPD "ToggleDeathCamera"

enum
{
	FPD_DEFAULT = 0, // determine from server feature
	FPD_THIRDPERSON, // always thirdperson
	FPD_FIRSTPERSON, // always firstperson
	FPD_COUNT
}

char g_szAttachments[][] = {"eyes", "forward"};

EngineVersion g_Engine;
ConVar        g_pConvarFadeToBlackLength;
ConVar        g_pConvarPlayerToggle;
Cookie        g_pStateCookie;
int           g_iAttacment[MAXPLAYERS + 1];
DynamicHook   hkUpdateOnRemove;

public void OnPluginStart()
{
	g_Engine = GetEngineVersion();
	LoadTranslations("srccoop_fpd.phrases");

	GameData pGameConfig = InitSourceCoopAddon(false);
	LoadDHookVirtual(pGameConfig, hkUpdateOnRemove, "CBaseEntity::UpdateOnRemove");
	pGameConfig.Close();

	g_pStateCookie = new Cookie("sourcecoop_fpd", "First person death", CookieAccess_Protected);
	g_pConvarFadeToBlackLength = CreateConVar("sourcecoop_fpd_fade_ms", "1500", "Duration in milliseconds to fade first-person death screen to black. 0 to disable.", _, true, 0.0);
	g_pConvarPlayerToggle = CreateConVar("sourcecoop_fpd_player_toggle", "1", "Enable players to choose death camera option regardless of server/map settings.", _, true, 0.0, true, 1.0);
	g_pConvarPlayerToggle.AddChangeHook(PlayerToggleCvarChanged);
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		UpdateMenuItems(!g_pConvarPlayerToggle.BoolValue);
	}
}

public void PlayerToggleCvarChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	UpdateMenuItems(!convar.BoolValue);
}

void UpdateMenuItems(bool remove = false)
{
	TopMenu pCoopMenu = SC_GetCoopTopMenu();
	TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_PLAYER);
	if (pMenuCategory != INVALID_TOPMENUOBJECT)
	{
		static TopMenuObject pToggleObj = INVALID_TOPMENUOBJECT;
		if (remove)
		{
			pCoopMenu.Remove(pToggleObj);
		}
		else
		{
			pToggleObj = pCoopMenu.AddItem(MENUITEM_TOGGLE_FPD, MyMenuHandler, pMenuCategory);
		}
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject object_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		SetGlobalTransTarget(param);
		int state = GetFPDState(param);
		switch (state)
		{
			case FPD_DEFAULT:
				Format(buffer, maxlength, "%t: %t", "death cam", "default");
			case FPD_THIRDPERSON:
				Format(buffer, maxlength, "%t: %t", "death cam", "third-person");
			case FPD_FIRSTPERSON:
				Format(buffer, maxlength, "%t: %t", "death cam", "first-person");
		}
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(param))
		{
			int state = GetFPDState(param);
			state = (state + 1) % FPD_COUNT;
			SetFPDState(param, state);
			Msg(param, "%t", "death cam setting updated");
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

int GetFPDState(int client)
{
	char buf[2];
	g_pStateCookie.Get(client, buf, sizeof(buf));
	return StringToInt(buf);
}

void SetFPDState(int client, int state)
{
	char buf[2];
	IntToString(state, buf, sizeof(buf));
	g_pStateCookie.Set(client, buf);
}

bool ShouldUseFPDeathCamera(int client)
{
	int state;
	if (!g_pConvarPlayerToggle.BoolValue || (state = GetFPDState(client)) == FPD_DEFAULT)
	{
		 if (!SC_IsCoopFeatureEnabled(FT_FIRSTPERSON_DEATHCAM))
		 	return false;
	}
	else if (state != FPD_FIRSTPERSON)
	{
		return false;
	}
	for (int i = 0; i < sizeof(g_szAttachments); i++)
	{
		if (LookupEntityAttachment(client, g_szAttachments[i]))
		{
			g_iAttacment[client] = i;
			return true;
		}
	}
	return false;
}

// ------------IMPLEMENTATION BELOW------------ //

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	if (g_Engine == Engine_BlackMesa)
	{
		if (strcmp(szClassname, "camera_death") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_CameraDeathSpawn);
			hkUpdateOnRemove.HookEntity(Hook_Post, iEntIndex, Hook_CameraDeathRemoved);
		}
	}
}

public void Hook_CameraDeathSpawn(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	CBasePlayer pOwner = view_as<CBasePlayer>(pEntity.GetOwner());
	if (pOwner.IsValid())
	{
		if (ShouldUseFPDeathCamera(pOwner.entindex))
		{
			CBaseEntity pViewEntity = pOwner.GetViewEntity();
			if (!pViewEntity.IsValid() || pViewEntity == pOwner)
			{
				pOwner.SetViewEntity(pEntity);
				return;
			}
		}
	}
	AcceptEntityInput(iEntIndex, "Kill");
}

MRESReturn Hook_CameraDeathRemoved(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer.GetViewEntity() == pEntity)
			{
				pPlayer.SetViewEntity(pPlayer);
			}
		}
	}
	return MRES_Ignored;
}

public void SC_OnPlayerRagdollCreated(CBasePlayer pPlayer, CBaseAnimating pRagdoll)
{
	int client = pPlayer.entindex;
	CBaseEntity pViewEnt;
	if (g_Engine == Engine_BlackMesa)
	{
		pViewEnt = pPlayer.GetViewEntity();
		if (!pViewEnt.IsValid() || !pViewEnt.IsClassname("camera_death"))
			return;
	}
	else
	{
		// No camera_death entity outside of BM
		if (!ShouldUseFPDeathCamera(client))
		{
			return;
		}
		pViewEnt = CreateCameraDeath(pPlayer);
	}

	// Parent death camera to ragdoll
	pViewEnt.SetParent(pRagdoll);
	pViewEnt.SetParentAttachment(g_szAttachments[g_iAttacment[client]]);

	int fadeDuration = g_pConvarFadeToBlackLength.IntValue;
	if (fadeDuration)
	{
		pPlayer.ScreenFade(fadeDuration, RGBA(0, 0, 0, 255), FFADE_OUT | FFADE_STAYOUT);
	}
}

public CInfoTarget CreateCameraDeath(const CBasePlayer pPlayer)
{
	CInfoTarget pCamera = CInfoTarget.Create();
	pCamera.SetClassname("camera_death");
	pCamera.Spawn();
	pCamera.m_iEFlags |= EFL_FORCE_CHECK_TRANSMIT;
	pPlayer.SetViewEntity(pCamera);
	return pCamera;
}