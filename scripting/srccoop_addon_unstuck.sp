#include <sourcemod>
#include <srccoop_api>

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
	if (!IsCurrentMapCoop())
	{
		MsgReply(iClient, "Unstuck is currently unavailable.");
		return Plugin_Handled;
	}

	CBasePlayer pPlayer = CBasePlayer(iClient);
	if (pPlayer.IsValid() && pPlayer.IsAlive())
	{
		float flGameTime = GetGameTime();
		if (g_flNextStuck[iClient] >= flGameTime)
		{
			MsgReply(iClient, "You cannot unstuck for another %1.1f seconds!", g_flNextStuck[iClient] - flGameTime);
			return Plugin_Handled;
		}
		
		// Velocity check for if people try to use it to get out of falling to their death
		float flVerticalVelocity = GetEntPropFloat(iClient, Prop_Send, "m_vecVelocity[2]");
		if (flVerticalVelocity < -200.0)
		{
			MsgReply(iClient, "Can't unstuck while falling.");
			return Plugin_Handled;
		}
		
		if (TeleportToCurrentCheckpoint(pPlayer))
		{
			g_flNextStuck[iClient] = flGameTime + g_pConVarNextStuck.FloatValue;
			MsgReply(iClient, "Moved to the active checkpoint.");
		}
		else
		{
			MsgReply(iClient, "Unable to find a place to put you.");
		}
	}
	
	return Plugin_Handled;
}
