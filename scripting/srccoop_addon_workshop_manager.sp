#include <sourcemod>
#include <sdktools>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

#define MAXILESIZE_OVERRIDE 999999

public Plugin myinfo =
{
	name = "SourceCoop Workshop Manager",
	author = "Alienmario",
	description = "Kicks players who are missing workshop maps with a custom message",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

ConVar pWorkshopMsg;
bool bKickEnabled;

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	if (GetEngineVersion() != Engine_BlackMesa)
		return APLRes_SilentFailure;
}

public void OnPluginStart()
{
	InitSourceCoopAddon();
	
	ConVar cvar = FindConVar("net_maxfilesize");
	cvar.SetBounds(ConVarBound_Upper, false);
	cvar.SetInt(MAXILESIZE_OVERRIDE);
	cvar.AddChangeHook(OnMaxFileSizeChanged);
	delete cvar;
	
	pWorkshopMsg = CreateConVar("sourcecoop_workshop_message", "Missing map! Subscribe to SourceCoop workshop collection + restart game", "The message to display to players missing workshop maps. Supported placeholders: {BSPNAME}");
}

public void OnMapStart()
{
	if (!SC_IsCurrentMapCoop())
		bKickEnabled = false;
}

public void SC_OnCoopMapConfigLoaded(KeyValues kv)
{
	kv.Rewind();
	bKickEnabled = !kv.GetNum("allow_server_download");
}

public void OnMaxFileSizeChanged(ConVar convar, char[] oldValue, char[] newValue)
{
	if (StringToInt(newValue) != MAXILESIZE_OVERRIDE)
	{
		convar.IntValue = MAXILESIZE_OVERRIDE;
		PrintToServer("You cannot change net_maxfilesize! It is being managed by SourceCoop Workshop Manager.");
	}
}

public Action OnFileSend(int client, const char[] szFile)
{
	if (bKickEnabled
		&& strncmp(szFile, "maps/", 5, false) == 0
		&& StrEqual(szFile[strlen(szFile)-4], ".bsp", false)
		&& !FileExists(szFile, true, "MOD")) // deduce that it's from WS
	{
		char szMsg[128], szBsp[MAX_MAPNAME];
		pWorkshopMsg.GetString(szMsg, sizeof(szMsg));
		
		GetCurrentMap(szBsp, sizeof(szBsp));
		ReplaceString(szMsg, sizeof(szMsg), "{BSPNAME}", szBsp, false);
		
		KickClient(client, szMsg);
	}
	
	return Plugin_Continue;
}