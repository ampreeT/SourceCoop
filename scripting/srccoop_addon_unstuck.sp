#include <sourcemod>
#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Unstuck",
	author = "Balimbanana, Alienmario",
	description = "Provides unstuck command",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

ConVar g_pConVarNextStuck;
float g_flNextStuck[MAXPLAYERS+1];

public void OnPluginStart()
{
	InitSourceCoopAddon();
	LoadTranslations("srccoop_unstuck.phrases");

	g_pConVarNextStuck = CreateConVar("sourcecoop_next_stuck", "60.0", "Prevents using stuck for this many seconds after using.", _, true, 0.0, false);	
	RegConsoleCmd("stuck", Command_Unstuck);
	RegConsoleCmd("unstuck", Command_Unstuck);
}

public void OnClientDisconnect_Post(int client)
{
	g_flNextStuck[client] = 0.0;
}

public Action Command_Unstuck(int iClient, int iArgs)
{
	if (!SC_IsCurrentMapCoop())
	{
		MsgReply(iClient, "%t", "unavailable");
		return Plugin_Handled;
	}

	CBasePlayer pPlayer = CBasePlayer(iClient);
	if (pPlayer.IsValid() && pPlayer.IsAlive())
	{
		float flGameTime = GetGameTime();
		if (g_flNextStuck[iClient] >= flGameTime)
		{
			MsgReply(iClient, "%t", "cooldown", g_flNextStuck[iClient] - flGameTime);
			return Plugin_Handled;
		}
		
		// Velocity check for if people try to use it to get out of falling to their death
		float flVerticalVelocity = GetEntPropFloat(iClient, Prop_Send, "m_vecVelocity[2]");
		if (flVerticalVelocity < -200.0)
		{
			MsgReply(iClient, "%t", "falling");
			return Plugin_Handled;
		}
		
		if (SC_TeleportToCurrentCheckpoint(pPlayer))
		{
			g_flNextStuck[iClient] = flGameTime + g_pConVarNextStuck.FloatValue;
			MsgReply(iClient, "%t", "success");
		}
		else
		{
			MsgReply(iClient, "%t", "failure");
		}
	}
	
	return Plugin_Handled;
}
