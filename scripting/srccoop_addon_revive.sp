#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#include <srccoop_api>

#pragma semicolon 1;
#pragma newdecls required;

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
	if ((buttons & IN_USE) && (IsPlayerAlive(client)))
	{
		if (!g_pReviveTarget[client].IsValid()) g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
		CBasePlayer pPlayer = CBasePlayer(client);
		float vecEyeOrigin[3];
		pPlayer.GetEyePosition(vecEyeOrigin);
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
					g_flReviveTime[client] = GetGameTime()+4.0;
				}
				CBaseEntity pRagdoll = CBaseEntity(GetEntPropEnt(g_pReviveTarget[client].GetEntIndex(),Prop_Send,"m_hRagdoll"));
				float vecRagdollPosition[3];
				if (pRagdoll.IsValid())
				{
					pRagdoll.GetAbsOrigin(vecRagdollPosition);
					if (GetVectorDistance(vecEyeOrigin,vecRagdollPosition,false) > 120.0)
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
						float vecRagdollAngle[3];
						
						EmitSoundToAll("weapons/tau/gauss_undercharge.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL, _, _, 150);
						
						g_pReviveTarget[client].SetCanSpawn(true);
						
						g_pReviveTarget[client].Spawn();
						g_pReviveTarget[client].Activate();
						
						// Can't do this because player_ragdoll only has m_angRotation as a Prop_Data
						//pRagdoll.GetAngles(vecRagdollAngle);
						GetEntPropVector(pRagdoll.GetEntIndex(),Prop_Data,"m_angRotation",vecRagdollAngle);
						
						// Ensure pitch and roll are not kept from ragdoll
						vecRagdollAngle[0] = 0.0;
						vecRagdollAngle[2] = 0.0;
						
						g_pReviveTarget[client].Teleport(vecRagdollPosition,vecRagdollAngle,NULL_VECTOR);
						pRagdoll.Kill();
						
						pPlayer.ModifyScore(1);
						g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
					}
				}
			}
		}
		else
		{
			float vecAngles[3];
			float vecEndPosition[3];
			GetClientEyeAngles(client,vecAngles);
			for (int i = 1;i<MaxClients+1;i++)
			{
				if (i != client)
				{
					CBasePlayer pTarget = CBasePlayer(i);
					if (pTarget.IsValid())
					{
						if (!pTarget.IsAlive())
						{
							int iRagdoll = GetEntPropEnt(i,Prop_Send,"m_hRagdoll");
							if (IsValidEntity(iRagdoll))
							{
								float targorgs[3];
								if (HasEntProp(iRagdoll,Prop_Data,"m_vecAbsOrigin")) GetEntPropVector(iRagdoll,Prop_Data,"m_vecAbsOrigin",targorgs);
								else if (HasEntProp(iRagdoll,Prop_Send,"m_vecOrigin")) GetEntPropVector(iRagdoll,Prop_Send,"m_vecOrigin",targorgs);
								float chkdist = GetVectorDistance(vecEyeOrigin,targorgs,false);
								if (chkdist < 100.0)
								{
									TR_TraceRayFilter(vecEyeOrigin,vecAngles,MASK_SOLID,RayType_Infinite,TraceEntityFilter,client);
									TR_GetEndPosition(vecEndPosition);
									chkdist = GetVectorDistance(vecEndPosition,targorgs,false);
									if (chkdist < 100.0)
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