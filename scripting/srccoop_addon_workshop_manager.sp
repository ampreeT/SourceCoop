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

ConVar g_hWorkshopMsg;
bool g_bKickEnabled;
char g_szMapWorkshopId[32];
char g_szMapTitle[128];

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	if (GetEngineVersion() != Engine_BlackMesa)
		return APLRes_SilentFailure;
}

public void OnPluginStart()
{
	InitSourceCoopAddon();
	
	ConVar net_maxfilesize = FindConVar("net_maxfilesize");
	net_maxfilesize.SetBounds(ConVarBound_Upper, false);
	net_maxfilesize.SetInt(MAXILESIZE_OVERRIDE);
	net_maxfilesize.AddChangeHook(OnMaxFileSizeChanged);
	delete net_maxfilesize;
	
	g_hWorkshopMsg = CreateConVar("sourcecoop_workshop_message", "Missing workshop map \"{TITLE}\"", "The message to display to players missing workshop maps. Supported placeholders: {TITLE} {BSPNAME} {WSID}");
}

public void OnMapStart()
{
	if (!SC_IsCurrentMapCoop())
		g_bKickEnabled = false;
}

public void SC_OnCoopMapConfigLoaded(KeyValues kv)
{
	kv.Rewind();
	kv.GetString("workshop", g_szMapWorkshopId, sizeof(g_szMapWorkshopId));

	// we kick if a map has workshop id filled in and doesn't explicitly allow direct downloads
	g_bKickEnabled = g_szMapWorkshopId[0] != EOS && !kv.GetNum("allow_server_download");

	char szCampaign[64], szChapter[64];
	kv.GetString("campaign", szCampaign, sizeof(szCampaign));
	kv.GetString("chapter", szChapter, sizeof(szChapter));
	if (szCampaign[0] == EOS || StrEqual(szCampaign, "Workshop maps", false))
	{
		// if campaign is not filled in, or a generic name is used, use chapter for the title
		FormatEx(g_szMapTitle, sizeof(g_szMapTitle), szChapter);
	}
	else
	{
		// otherwise use campaign for the title
		FormatEx(g_szMapTitle, sizeof(g_szMapTitle), szCampaign);
	}
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
	if (g_bKickEnabled
		&& strncmp(szFile, "maps/", 5, false) == 0
		&& StrEqual(szFile[strlen(szFile)-4], ".bsp", false))
	{
		char szMsg[128], szBsp[MAX_MAPNAME];
		g_hWorkshopMsg.GetString(szMsg, sizeof(szMsg));
		
		GetCurrentMap(szBsp, sizeof(szBsp));
		ReplaceString(szMsg, sizeof(szMsg), "{TITLE}", g_szMapTitle, false);
		ReplaceString(szMsg, sizeof(szMsg), "{BSPNAME}", szBsp, false);
		ReplaceString(szMsg, sizeof(szMsg), "{WSID}", g_szMapWorkshopId, false);
		
		KickClient(client, szMsg);
	}
	
	return Plugin_Continue;
}