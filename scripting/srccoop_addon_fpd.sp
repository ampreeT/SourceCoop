#include <sdkhooks>
#include <sourcemod>
#include <clientprefs>

#include <srccoop_api>

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

EngineVersion g_Engine;
ConVar        g_pConvarFadeToBlackLength;
ConVar        g_pConvarPlayerToggle;
Cookie        pStateCookie;

public void OnPluginStart()
{
	g_Engine = GetEngineVersion();
	InitSourceCoopAddon();

	pStateCookie = new Cookie("sourcecoop_fpd", "First person death", CookieAccess_Protected);
	g_pConvarFadeToBlackLength = CreateConVar("sourcecoop_fpd_fade_ms", "1500", "Duration in milliseconds to fade first-person death screen to black. 0 to disable.", _, true, 0.0);
	g_pConvarPlayerToggle = CreateConVar("sourcecoop_fpd_player_toggle", "1", "Enable players to choose death camera option regardless of server/map settings.", _, true, 0.0, true, 1.0);
	g_pConvarPlayerToggle.AddChangeHook(PlayerToggleCvarChanged);
}

public void OnLibraryAdded(const char[] name)
{
	if(StrEqual(name, SRCCOOP_LIBRARY))
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
	TopMenu pCoopMenu = GetCoopTopMenu();
	TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_PLAYER);
	if(pMenuCategory != INVALID_TOPMENUOBJECT)
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
		int state = GetFPDState(param);
		switch (state) {
			case FPD_DEFAULT:
				Format(buffer, maxlength, "Death cam: Default");
			case FPD_THIRDPERSON:
				Format(buffer, maxlength, "Death cam: Third-person");
			case FPD_FIRSTPERSON:
				Format(buffer, maxlength, "Death cam: First-person");
		}
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if(AreClientCookiesCached(param))
		{
			int state = GetFPDState(param);
			state = (state + 1) % FPD_COUNT;
			SetFPDState(param, state);
			Msg(param, "Death animation setting updated.");
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

int GetFPDState(int client)
{
	char buf[2];
	pStateCookie.Get(client, buf, sizeof(buf));
	return StringToInt(buf);
}

void SetFPDState(int client, int state)
{
	char buf[2];
	IntToString(state, buf, sizeof(buf));
	pStateCookie.Set(client, buf);
}

bool ShouldUseFPDeathCamera(int client)
{
	int state;
	if (!g_pConvarPlayerToggle.BoolValue || (state = GetFPDState(client)) == FPD_DEFAULT)
	{
		return IsCoopFeatureEnabled(FT_FIRSTPERSON_DEATHCAM);
	}
	else
	{
		return state == FPD_FIRSTPERSON;
	}
}

// ------------IMPLEMENTATION BELOW------------ //

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	if (g_Engine == Engine_BlackMesa)
	{
		if (strcmp(szClassname, "camera_death") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_CameraDeathSpawn);
		}
	}
}

public void OnEntityDestroyed(int iEntIndex)
{
	if (g_Engine == Engine_BlackMesa)
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		if (pEntity.IsValid())
		{
			if (pEntity.IsClassname("camera_death"))
			{
				Hook_CameraDeathDestroyed(pEntity);
			}
		}
	}
}

public void Hook_CameraDeathSpawn(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	CBasePlayer pOwner = view_as<CBasePlayer>(pEntity.GetOwner());
	if (pOwner.IsValid())
	{
		if (ShouldUseFPDeathCamera(pOwner.GetEntIndex()))
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

void Hook_CameraDeathDestroyed(CBaseEntity pEntity)
{
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
}

public void OnPlayerRagdollCreated(CBasePlayer pPlayer, CBaseAnimating pRagdoll)
{
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
		if (!ShouldUseFPDeathCamera(pPlayer.GetEntIndex()))
		{
			return;
		}
		pViewEnt = CreateCameraDeath(pPlayer);
	}

	// Parent death camera to ragdoll
	pViewEnt.SetParent(pRagdoll);
	pViewEnt.SetParentAttachment("eyes");

	int fadeDuration = g_pConvarFadeToBlackLength.IntValue;
	if (fadeDuration)
	{
		Client_ScreenFade(pPlayer.GetEntIndex(), fadeDuration, FFADE_OUT | FFADE_STAYOUT);
	}
}

public CBaseEntity CreateCameraDeath(CBasePlayer pPlayer)
{
	CBaseEntity pCamera = CBaseEntity.Create("info_target");
	pCamera.SetClassname("camera_death");
	pCamera.SetSpawnFlags(1);
	pCamera.Spawn();
	pPlayer.SetViewEntity(pCamera);
	return pCamera;
}