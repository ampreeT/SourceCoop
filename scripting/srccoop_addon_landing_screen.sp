#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop landing screen",
	author = "Alienmario",
	description = "SourceCoop landing screen overlay",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

ConVar pCvarLogoMaterial;
bool bEnabled;
bool bHidden[MAXPLAYERS + 1];
bool bReApply[MAXPLAYERS + 1];

public void OnPluginStart()
{
	pCvarLogoMaterial = CreateConVar("sourcecoop_logo_material", "sourcecoop/landing_screen");

	// games that have a transparent team select can close overlay after picking a team 
	#if defined SRCCOOP_BLACKMESA
	HookEvent("player_team", Event_PlayerChangeTeam, EventHookMode_Post);
	#endif
}

public void OnMapStart()
{
	bEnabled = SC_IsCurrentMapCoop();
	if (bEnabled)
	{
		char szBuffer[PLATFORM_MAX_PATH], szVMT[PLATFORM_MAX_PATH], szVTF[PLATFORM_MAX_PATH];
		pCvarLogoMaterial.GetString(szBuffer, sizeof(szBuffer));
		Format(szVMT, sizeof(szVMT), "materials/%s.vmt", szBuffer);
		Format(szVTF, sizeof(szVTF), "materials/%s.vtf", szBuffer);
		AddFileToDownloadsTable(szVMT);
		AddFileToDownloadsTable(szVTF);
		PrecacheModel(szVMT, true);
	}
}

public void OnClientPutInServer(int client)
{
	if (bEnabled)
	{
		bHidden[client] = false;
		bReApply[client] = true;
		ApplyOverlay(client);
	}
	else
	{
		bHidden[client] = true;
	}
}

public void Event_PlayerChangeTeam(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	int client = GetClientOfUserId(hEvent.GetInt("userid"));
	if (!bHidden[client])
	{
		int iTeam = hEvent.GetInt("team");
		if (iTeam > TEAM_SPECTATOR || iTeam == TEAM_UNASSIGNED)
		{
			ClientCommand(client, "r_screenoverlay 0");
			bHidden[client] = true;
		}
	}
}

public void OnPlayerRunCmdPost(int client, int buttons, int impulse, const float vel[3], const float angles[3], int weapon, int subtype, int cmdnum, int tickcount, int seed, const int mouse[2])
{
	if (!bHidden[client])
	{
		if (mouse[0] || mouse[1])
		{
			ClientCommand(client, "r_screenoverlay 0");
			bHidden[client] = true;
		}
		else if (bReApply[client]) // in some cases, the overlay can poof during the spawn-in process, gotta reapply
		{
			bReApply[client] = false;
			ApplyOverlay(client);
		}
	}
}

void ApplyOverlay(int client)
{
	static char szBuffer[PLATFORM_MAX_PATH];
	pCvarLogoMaterial.GetString(szBuffer, sizeof(szBuffer));
	ClientCommand(client, "r_screenoverlay %s", szBuffer);
}