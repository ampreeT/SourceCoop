#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#include <srccoop_api>

#pragma semicolon 1;
#pragma newdecls required;

ConVar g_pConVarReviveTime, g_pConVarReviveScore, g_pConVarReviveMessages;

int g_BeamSprite = -1;
int g_HaloSprite = -1;
int g_ColorGreen[4] = {0, 255, 0, 255};

CBasePlayer g_pReviveTarget[MAXPLAYERS+1] = {view_as<CBasePlayer>(-1), ...};
float g_flReviveTime[MAXPLAYERS+1];
float g_flLastSpriteUpdate[MAXPLAYERS+1];

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
	LoadTranslations("common.phrases");
	
	g_pConVarReviveTime = CreateConVar("sourcecoop_revive_time", "4.0", "Sets time that you have to hold E to revive.", _, true, 0.0, false);
	g_pConVarReviveScore = CreateConVar("sourcecoop_revive_score", "1", "Sets score to give for reviving a player.", _, true, 0.0, false);
	g_pConVarReviveMessages = CreateConVar("sourcecoop_revive_messages", "0", "Shows messages such as You have started reviving x.", _, true, 0.0, true, 1.0);
	
	RegAdminCmd("sourcecoop_revive", Command_ForceRespawn, ADMFLAG_ROOT, "Force respawn by client index.");
	
	AutoExecConfig(true, "srccoop_addon_revive");
}

public void OnMapStart()
{
	PrecacheSound("weapons/tau/gauss_undercharge.wav",true);
	PrecacheSound("items/suitchargeok1.wav",true);
	PrecacheSound("items/suitchargeno1.wav",true);
	
	GameData gameConfig = new GameData("funcommands.games");
	if (gameConfig != null)
	{
		static char buffer[PLATFORM_MAX_PATH];
		if (gameConfig.GetKeyValue("SpriteBeam", buffer, sizeof(buffer)) && buffer[0])
		{
			g_BeamSprite = PrecacheModel(buffer);
		}
		if (gameConfig.GetKeyValue("SpriteHalo", buffer, sizeof(buffer)) && buffer[0])
		{
			g_HaloSprite = PrecacheModel(buffer);
		}
	}
}

public Action Command_ForceRespawn(int client, int args)
{
	static char szTarget[64];
	static char szClientName[64];
	
	if (args < 1)
	{
		MsgReply(client, "You must specify a player to spawn.");
		return Plugin_Handled;
	}
	
	GetCmdArg(1, szTarget, sizeof(szTarget));
	
	CBasePlayer pTarget = CBasePlayer(FindTarget(client, szTarget, false));
	if (pTarget.IsValid())
	{
		if (!pTarget.IsAlive())
		{
			pTarget.GetName(szClientName, sizeof(szClientName));
			
			MsgReply(client, "Spawned %i '%s'", pTarget.GetEntIndex(), szClientName);
			SurvivalRespawn(pTarget);

			return Plugin_Handled;
		}
		else
		{
			MsgReply(client, "Player is already alive.");
		}
	}
	
	
	return Plugin_Handled;
}

public Action OnPlayerRunCmd(int client, int &buttons, int &impulse, float vel[3], float angles[3], int &weapon)
{
	if (IsPlayerAlive(client))
	{
		if (buttons & IN_USE)
		{
			CBasePlayer pPlayer = CBasePlayer(client);
			static float vecEyeAngles[3];
			static float vecOrigin[3];
			static float vecRagdollPosition[3];
			static float vecStart[3];
			static float vecEnd[3];
			static float angAdjust;
			
			pPlayer.GetEyePosition(vecOrigin);
			pPlayer.GetEyeAngles(vecEyeAngles);
			
			if (g_pReviveTarget[client].IsValid())
			{
				if (g_pReviveTarget[client].IsAlive())
				{
					ResetReviveStatus(client);
					if (g_pConVarReviveMessages.BoolValue) Msg(client, "Your revive target respawned...");
				}
				else
				{
					if (g_flReviveTime[client] == 0.0)
					{
						g_flReviveTime[client] = GetGameTime() + g_pConVarReviveTime.FloatValue;
					}
					
					if (g_pReviveTarget[client].GetRagdoll().IsValid())
					{
						g_pReviveTarget[client].GetRagdoll().GetAbsOrigin(vecRagdollPosition);
						
						if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) > 120.0)
						{
							// Client left range to revive, play deny sound and stop previous start sound
							ResetReviveStatus(client);
							
							StopSound(client, SNDCHAN_STATIC, "items/suitchargeok1.wav");
							EmitSoundToAll("items/suitchargeno1.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL);
							
							if (g_pConVarReviveMessages.BoolValue) Msg(client, "You have canceled reviving...");
							
							return Plugin_Continue;
						}
						
						// This delay is only here to prevent massive spam of tempents
						if (g_flLastSpriteUpdate[client] <= GetGameTime())
						{
							vecStart[0] = (vecOrigin[0] + (8 * Cosine(DegToRad(vecEyeAngles[1]+20.0))));
							vecStart[1] = (vecOrigin[1] + (8 * Sine(DegToRad(vecEyeAngles[1]+20.0))));
							vecStart[2] = (vecOrigin[2] - (8 * Tangent(DegToRad(vecEyeAngles[0]))));
							
							angAdjust = (40.0 * (g_flReviveTime[client] - GetGameTime()) / g_pConVarReviveTime.FloatValue) - 20.0;
							
							vecEnd[0] = (vecOrigin[0] + (8 * Cosine(DegToRad(vecEyeAngles[1]+angAdjust))));
							vecEnd[1] = (vecOrigin[1] + (8 * Sine(DegToRad(vecEyeAngles[1]+angAdjust))));
							vecEnd[2] = vecStart[2];
							
							TE_SetupBeamPoints(vecStart, vecEnd, g_BeamSprite, g_HaloSprite, 0, 0, 0.1, 0.2, 0.2, 0, 0.0, g_ColorGreen, 0);
							TE_SendToClient(client, 0.0);
							
							g_flLastSpriteUpdate[client] = GetGameTime() + 0.02;
						}
						
						if (GetGameTime() >= g_flReviveTime[client])
						{
							// Sounds/effects
							EmitSoundToAll("weapons/tau/gauss_undercharge.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL, _, _, 150);
							Client_ScreenFade(g_pReviveTarget[client].GetEntIndex(), 256, FFADE_PURGE|FFADE_IN, 1, 0, 0, 200, 255);
							
							SurvivalRespawn(g_pReviveTarget[client]);
							
							g_pReviveTarget[client].Spawn();
							g_pReviveTarget[client].Activate();
							
							g_pReviveTarget[client].GetRagdoll().Kill();
							
							// Delay to allow equip time
							Handle dp = CreateDataPack();
							CreateDataTimer(0.1, DelayRespawn, dp, TIMER_FLAG_NO_MAPCHANGE);
							WritePackCell(dp, pPlayer);
							WritePackCell(dp, g_pReviveTarget[client]);
							
							// Give score to reviver
							pPlayer.ModifyScore(g_pConVarReviveScore.IntValue);
							if (g_pConVarReviveMessages.BoolValue) Msg(client, "You have revived '%N' and have gained %i score!", g_pReviveTarget[client].GetEntIndex(), g_pConVarReviveScore.IntValue);
							
							ResetReviveStatus(client);
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
								if (pTarget.GetRagdoll().IsValid())
								{
									pTarget.GetRagdoll().GetAbsOrigin(vecRagdollPosition);
									
									if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) < 100.0)
									{
										TR_TraceRayFilter(vecOrigin, vecEyeAngles, MASK_SOLID, RayType_Infinite, TraceEntityFilter, client);
										TR_GetEndPosition(vecOrigin);
										if (GetVectorDistance(vecOrigin, vecRagdollPosition, false) < 100.0)
										{
											g_pReviveTarget[client] = CBasePlayer(i);
											EmitSoundToAll("items/suitchargeok1.wav", client, SNDCHAN_STATIC, SNDLEVEL_NORMAL);
											if (g_pConVarReviveMessages.BoolValue) Msg(client, "You have started reviving '%N'", i);
											
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
				ResetReviveStatus(client);
				if (g_pConVarReviveMessages.BoolValue) Msg(client, "You have canceled reviving...");
			}
		}
	}
	return Plugin_Continue;
}

void ResetReviveStatus(int client)
{
	g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
	g_flReviveTime[client] = 0.0;
}

public Action DelayRespawn(Handle timer, Handle dp)
{
	if (dp != INVALID_HANDLE)
	{
		ResetPack(dp);
		CBasePlayer pPlayer = ReadPackCell(dp);
		CBasePlayer pTarget = ReadPackCell(dp);
		
		if ((pTarget.IsValid()) && (pPlayer.IsValid()))
		{
			float vecOrigin[3];
			float vecEyeAngles[3];
			
			pPlayer.GetAbsOrigin(vecOrigin);
			pPlayer.GetEyeAngles(vecEyeAngles);
			
			pTarget.Teleport(vecOrigin, vecEyeAngles, NULL_VECTOR);
		}
	}
	return Plugin_Handled;
}

public void OnClientDisconnect_Post(int client)
{
	g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
	g_flReviveTime[client] = 0.0;
	g_flLastSpriteUpdate[client] = 0.0;
}

public bool TraceEntityFilter(int entity, int mask, any data)
{
	if (entity == data) return false;
	return true;
}