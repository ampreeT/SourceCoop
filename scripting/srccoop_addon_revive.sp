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
							/*
							// IN VALUES
							CBasePlayer pPlayer
							float flTimeToComplete = 4.0;
							float flLengthBeam = 40.0;
							float flWidthBeam = 0.3;
							int Color[4];
							float flDistFromPly = 10.0;
							*/
							
							Client_ProgressBar(pPlayer, g_pConVarReviveTime.FloatValue, 40.0, 0.3, g_ColorGreen, 10.0);
							
							g_flLastSpriteUpdate[client] = GetGameTime() + g_pConVarReviveTime.FloatValue;
						}
						
						if (GetGameTime() >= g_flReviveTime[client])
						{
							// Sounds/effects
							EmitSoundToAll("weapons/tau/gauss_undercharge.wav", client, SNDCHAN_AUTO, SNDLEVEL_NORMAL, _, _, 150);
							Client_ScreenFade(g_pReviveTarget[client].GetEntIndex(), 256, FFADE_PURGE|FFADE_IN, 1, 0, 0, 200, 255);
							
							SurvivalRespawn(g_pReviveTarget[client]);
							
							g_pReviveTarget[client].Spawn();
							g_pReviveTarget[client].Activate();
							
							// Fix for if player died on a ladder
							SetEntPropEnt(g_pReviveTarget[client].GetEntIndex(), Prop_Data, "m_hLadder", -1);
							
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
	g_flLastSpriteUpdate[client] = 0.0;
	Client_RemoveProgressBar(client);
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
	ResetReviveStatus(client);
}

public bool TraceEntityFilter(int entity, int mask, any data)
{
	if (entity == data) return false;
	return true;
}


//------------------------------------------------------
// Progress bar rendering
//------------------------------------------------------
CBaseEntity g_pProgressBar[MAXPLAYERS+1] = {view_as<CBaseEntity>(-1), ...};
stock void Client_ProgressBar(CBasePlayer pPlayer, float flTime = 4.0, float flBarLength = 40.0, float flBarWidth = 0.3, int Color[4] = {0, 255, 0, 255}, float flDistFromPlayer = 10.0)
{
	// Always remove previous bar if there is one
	Client_RemoveProgressBar(pPlayer.GetEntIndex());
	
	static float vecEyePosition[3];
	
	if (pPlayer.IsValid())
	{
		int iBeam = CreateEntityByName("beam");
		if (IsValidEntity(iBeam))
		{
			pPlayer.GetEyePosition(vecEyePosition);
			
			DispatchKeyValue(iBeam, "model", "sprites/laser.vmt");
			DispatchKeyValue(iBeam, "texture", "sprites/halo01.vmt");
			SetEntProp(iBeam, Prop_Data, "m_nModelIndex", g_BeamSprite);
			SetEntProp(iBeam, Prop_Data, "m_nHaloIndex", 0);
			TeleportEntity(iBeam, vecEyePosition, NULL_VECTOR, NULL_VECTOR);
			DispatchSpawn(iBeam);
			ActivateEntity(iBeam);
			
			SetEntityRenderColor(iBeam, Color[0], Color[1], Color[2], Color[3]);
			SetEntProp(iBeam, Prop_Data, "m_nBeamType", 1);
			SetEntProp(iBeam, Prop_Data, "m_nBeamFlags", 0);
			SetEntProp(iBeam, Prop_Data, "m_nNumBeamEnts", 2);
			SetEntPropVector(iBeam, Prop_Data, "m_vecEndPos", vecEyePosition);
			SetEntPropFloat(iBeam, Prop_Data, "m_fWidth", flBarWidth);
			SetEntPropFloat(iBeam, Prop_Data, "m_fEndWidth", flBarWidth);
			SetEntPropFloat(iBeam, Prop_Data, "m_fSpeed", 0.0);
			SetEntPropFloat(iBeam, Prop_Data,"m_flFrameRate", 0.0);
			SetEntPropFloat(iBeam, Prop_Data,"m_flHDRColorScale", 1.0);
			SetEntProp(iBeam, Prop_Data, "m_nDissolveType", -1);
			SetEntProp(iBeam, Prop_Data, "m_nRenderMode", 2);
			SetEntPropFloat(iBeam, Prop_Data, "m_fHaloScale", 0.0);
			SetEntPropEnt(iBeam, Prop_Data, "m_hEffectEntity", pPlayer.GetEntIndex());
			
			g_pProgressBar[pPlayer.GetEntIndex()] = CBaseEntity(iBeam);
			static float vecStats[3];
			vecStats[0] = flTime;
			vecStats[1] = flBarLength;
			vecStats[2] = flDistFromPlayer;
			SetEntPropVector(iBeam, Prop_Data, "m_vecMaxs", vecStats);
			SetEntPropFloat(iBeam, Prop_Data, "m_flLocalTime", GetGameTime() + flTime);
			
			SDKHookEx(iBeam, SDKHook_SetTransmit, Transmit_ProgressBar);
		}
	}
	
	return;
}

stock void Client_RemoveProgressBar(int iPlayer)
{
	if (g_pProgressBar[iPlayer].IsValid())
	{
		g_pProgressBar[iPlayer].Kill();
		g_pProgressBar[iPlayer] = CBaseEntity(-1);
	}
	
	return;
}

public Action Transmit_ProgressBar(int entity, int client)
{
	if (GetEntPropEnt(entity, Prop_Data, "m_hEffectEntity") == client)
	{
		static float vecStats[3];
		static float vecStart[3], vecEnd[3], vecBeamOrigin[3];
		static float vecEyeAngles[3], vecEyePosition[3];
		static float flAdjustPosition, angAdjust, flEndTime;
		
		if (IsValidEntity(entity))
		{
			CBasePlayer pPlayer = CBasePlayer(client);
			if (pPlayer.IsValid())
			{
				flEndTime = GetEntPropFloat(entity, Prop_Data, "m_flLocalTime");
				if (GetGameTime() >= flEndTime) AcceptEntityInput(entity, "Kill");
				
				pPlayer.GetEyePosition(vecEyePosition);
				pPlayer.GetEyeAngles(vecEyeAngles);
				GetEntPropVector(entity, Prop_Data, "m_vecAbsOrigin", vecBeamOrigin);
				
				GetEntPropVector(entity, Prop_Data, "m_vecMaxs", vecStats);
				
				vecStart[0] = (vecEyePosition[0] + (vecStats[2] * Cosine(DegToRad(vecEyeAngles[1]+vecStats[1]))));
				vecStart[1] = (vecEyePosition[1] + (vecStats[2] * Sine(DegToRad(vecEyeAngles[1]+vecStats[1]))));
				vecStart[2] = (vecEyePosition[2] - (vecStats[2] * Tangent(DegToRad(vecEyeAngles[0]))));
				
				vecEnd[0] = (vecEyePosition[0] + (vecStats[2] * Cosine(DegToRad(vecEyeAngles[1]-vecStats[1]))));
				vecEnd[1] = (vecEyePosition[1] + (vecStats[2] * Sine(DegToRad(vecEyeAngles[1]-vecStats[1]))));
				vecEnd[2] = vecStart[2];
				
				flAdjustPosition = GetVectorDistance(vecStart, vecEnd, false);
				angAdjust = (flAdjustPosition * (flEndTime - GetGameTime()) / vecStats[0]) - flAdjustPosition;
				
				vecEnd[0] = (vecStart[0] + (angAdjust * Cosine(DegToRad(vecEyeAngles[1]+90.0))));
				vecEnd[1] = (vecStart[1] + (angAdjust * Sine(DegToRad(vecEyeAngles[1]+90.0))));
				vecEnd[2] = vecStart[2];
				
				TeleportEntity(entity, vecStart, NULL_VECTOR, NULL_VECTOR);
				SetEntPropVector(entity, Prop_Data, "m_vecEndPos", vecEnd);
				
				return Plugin_Continue;
			}
			else AcceptEntityInput(entity, "Kill");
		}
	}
	return Plugin_Handled;
}