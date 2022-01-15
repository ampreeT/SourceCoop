#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#include <srccoop_api>

#pragma semicolon 1;
#pragma newdecls required;

ConVar g_pConVarReviveTime;
ConVar g_pConVarReviveScore;

CBasePlayer g_pReviveTarget[MAXPLAYERS+1] = {view_as<CBasePlayer>(-1), ...};
float g_flReviveTime[MAXPLAYERS+1];

public Plugin myinfo =
{
	name = "SourceCoop Revive",
	author = "Balimbanana",
	description = "Enables revive by pressing E while looking at player ragdoll",
	version = "1.0.0",
	url = "https://github.com/ampreeT/SourceCoop"
};

public void OnPluginStart()
{
	g_pConVarReviveTime = CreateConVar("sourcecoop_revive_time", "4.0", "Sets time to revive.", _, true, 0.0, false);
	g_pConVarReviveScore = CreateConVar("sourcecoop_revive_score", "1", "Sets score to give for reviving a player.", _, true, 0.0, false);
	
	RegAdminCmd("sourcecoop_revive", Command_ForceRespawn, ADMFLAG_ROOT, "Force respawn by client index.");
}

public void OnMapStart()
{
	PrecacheSound("weapons/tau/gauss_undercharge.wav",true);
	PrecacheSound("items/suitchargeok1.wav",true);
	PrecacheSound("items/suitchargeno1.wav",true);
}

public Action Command_ForceRespawn(int client, int args)
{
	if (args < 1)
	{
		PrintToConsole(client, "You must specify an index to spawn.");
		return Plugin_Handled;
	}
	char szTargIndex[4];
	GetCmdArg(1, szTargIndex, sizeof(szTargIndex));
	CBasePlayer pTarget = CBasePlayer(StringToInt(szTargIndex));
	if (pTarget.IsValid())
	{
		PrintToConsole(client, "Spawned %i", pTarget.GetEntIndex());
		
		pTarget.SetCanSpawn(true);
		pTarget.Spawn();
		pTarget.Activate();
	}
	return Plugin_Handled;
}

public Action OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon)
{
	if (IsPlayerAlive(client))
	{
		if (buttons & IN_USE)
		{
			if (!g_pReviveTarget[client].IsValid()) g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
			CBasePlayer pPlayer = CBasePlayer(client);
			static float vecEyeAngles[3];
			static float vecOrigin[3];
			static float vecRagdollPosition[3];
			pPlayer.GetEyePosition(vecOrigin);
			pPlayer.GetEyeAngles(vecEyeAngles);
			if (g_pReviveTarget[client].IsValid())
			{
				if (g_pReviveTarget[client].IsAlive())
				{
					g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
					g_flReviveTime[client] = 0.0;
				}
				else
				{
					if (g_flReviveTime[client] == 0.0)
					{
						g_flReviveTime[client] = GetGameTime() + g_pConVarReviveTime.FloatValue;
					}
					CBaseEntity pRagdoll = CBaseEntity(GetEntPropEnt(g_pReviveTarget[client].GetEntIndex(), Prop_Send, "m_hRagdoll"));
					if (pRagdoll.IsValid())
					{
						pRagdoll.GetAbsOrigin(vecRagdollPosition);
						if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) > 120.0)
						{
							// Client left range to revive, play deny sound and stop previous start sound
							
							g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
							g_flReviveTime[client] = 0.0;
							
							StopSound(client, SNDCHAN_STATIC, "items/suitchargeok1.wav");
							EmitSoundToAll("items/suitchargeno1.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL);
							
							return Plugin_Continue;
						}
						if (GetGameTime() >= g_flReviveTime[client])
						{
							EmitSoundToAll("weapons/tau/gauss_undercharge.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL, _, _, 150);
							
							g_pReviveTarget[client].SetCanSpawn(true);
							
							g_pReviveTarget[client].Spawn();
							g_pReviveTarget[client].Activate();
							
							// Fix for if player died on a ladder
							SetEntPropEnt(g_pReviveTarget[client].GetEntIndex(), Prop_Data, "m_hLadder", -1);
							
							pPlayer.GetAbsOrigin(vecOrigin);
							
							g_pReviveTarget[client].Teleport(vecOrigin, vecEyeAngles, NULL_VECTOR);
							pRagdoll.Kill();
							
							pPlayer.ModifyScore(g_pConVarReviveScore.IntValue);
							g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
							g_flReviveTime[client] = 0.0;
						}
					}
				}
			}
			else
			{
				for (int i = 1;i<MaxClients+1;i++)
				{
					if (i != client)
					{
						CBasePlayer pTarget = CBasePlayer(i);
						if (pTarget.IsValid())
						{
							if (!pTarget.IsAlive())
							{
								CBaseEntity pRagdoll = CBaseEntity(GetEntPropEnt(i, Prop_Send, "m_hRagdoll"));
								if (pRagdoll.IsValid())
								{
									pRagdoll.GetAbsOrigin(vecRagdollPosition);
									if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) < 100.0)
									{
										TR_TraceRayFilter(vecOrigin, vecEyeAngles, MASK_SOLID, RayType_Infinite, TraceEntityFilter, client);
										TR_GetEndPosition(vecOrigin);
										if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) < 100.0)
										{
											g_pReviveTarget[client] = CBasePlayer(i);
											EmitSoundToAll("items/suitchargeok1.wav", client, SNDCHAN_STATIC, SNDLEVEL_NORMAL);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else if (!(buttons & IN_USE))
		{
			if (g_pReviveTarget[client].IsValid())
			{
				g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
				g_flReviveTime[client] = 0.0;
			}
		}
	}
	return Plugin_Continue;
}

public void OnClientDisconnect_Post(int client)
{
	g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
	g_flReviveTime[client] = 0.0;
}

public bool TraceEntityFilter(int entity, int mask, any data)
{
	if (entity == data) return false;
	return true;
}