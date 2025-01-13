#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#define CONF_PREFIX "REVIVE_"

#include <srccoop_addon>

#pragma semicolon 1;
#pragma newdecls required;

public Plugin myinfo =
{
	name = "SourceCoop Revive",
	author = "Balimbanana",
	description = "Enables revive by pressing <USE> while looking at player ragdoll",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

enum struct ReviveConfig
{
	char SND_START[PLATFORM_MAX_PATH];
	char SND_DENY[PLATFORM_MAX_PATH];
	char SND_RESPAWN[PLATFORM_MAX_PATH];
	int SND_RESPAWN_PITCH;
	int SNDLEVEL;
	RGBA BAR_COLOR;
	char BAR_MODEL[PLATFORM_MAX_PATH];
	char RAGDOLL_PARTICLE[128];

	CONF_INIT(
		CONF_STR(SND_START)
		CONF_STR(SND_DENY)
		CONF_STR(SND_RESPAWN)
		CONF_INT(SND_RESPAWN_PITCH)
		CONF_INT(SNDLEVEL)
		CONF_CLR(BAR_COLOR)
		CONF_STR(BAR_MODEL)
		CONF_STR(RAGDOLL_PARTICLE)
	)
}

ReviveConfig Conf;

ConVar g_pConVarReviveTime;
ConVar g_pConVarReviveScore;
ConVar g_pConVarReviveMessages;
ConVar g_pConVarRagdollEffectsTimer;
ConVar g_pConVarRagdollBlink;
ConVar g_pConVarRagdollParticle;
ConVar g_pConvarAllowInClassicMode;

ConVar g_pConVarSurvivalMode;

CBasePlayer g_pReviveTarget[MAXPLAYERS + 1] = {NULL_CBASEENTITY, ...};
float g_flReviveTime[MAXPLAYERS + 1];
float g_flNextSpriteUpdate[MAXPLAYERS + 1];
int g_iBeamSprite = -1;
bool g_bEnabled;
SpawnOptions g_pSpawnOptions;

Handle g_pRagdollEffectsTimer[MAXPLAYERS + 1];

public void OnPluginStart()
{
	LoadTranslations("common.phrases");
	
	InitSourceCoopAddon();
	Conf.Initialize(LoadSourceCoopConfig());
	
	g_pConVarReviveTime = CreateConVar("sourcecoop_revive_time", "4.0", "Sets time that you have to hold E to revive.", _, true, 0.0, false);
	g_pConVarReviveScore = CreateConVar("sourcecoop_revive_score", "1", "Sets score to give for reviving a player.", _, true, 0.0, false);
	g_pConVarReviveMessages = CreateConVar("sourcecoop_revive_messages", "0", "Shows messages such as You have started reviving x.", _, true, 0.0, true, 1.0);
	g_pConVarRagdollEffectsTimer = CreateConVar("sourcecoop_revive_ragdoll_effects_timer", "4.0", "Delay for applying ragdoll highlighting effects. -1 to disable all ragdoll effects.", _, true, -1.0);
	g_pConVarRagdollParticle = CreateConVar("sourcecoop_revive_ragdoll_particle", "1", "Whether to spawn a particle inside player ragdolls to improve their visibility.", _, true, 0.0, true, 1.0);
	g_pConVarRagdollBlink = CreateConVar("sourcecoop_revive_ragdoll_blink", "1", "Whether to blink player ragdolls to improve their visibility.", _, true, 0.0, true, 1.0);
	g_pConvarAllowInClassicMode = CreateConVar("sourcecoop_revive_in_classic_mode", "1", "Whether to allow reviving in non-survival mode.", _, true, 0.0, true, 1.0);
	g_pConvarAllowInClassicMode.AddChangeHook(OnAllowInClassicModeChanged);
	
	RegAdminCmd("sc_revive", Command_Revive, ADMFLAG_SLAY, "Respawns dead players");

	g_pSpawnOptions.bRevive = true;
	g_pSpawnOptions.bUnstuck = true;
}

public void OnAllPluginsLoaded()
{
	if ((g_pConVarSurvivalMode = FindConVar("sourcecoop_survival_mode")) == null)
		SetFailState("ConVar sourcecoop_survival_mode not found.");
	
	g_pConVarSurvivalMode.AddChangeHook(OnSurvivalModeChanged);
}

public void OnConfigsExecuted()
{
	SetEnabledState();
	if (!SC_IsCurrentMapCoop())
		return;
	
	PrecacheSound(Conf.SND_RESPAWN, true);
	PrecacheSound(Conf.SND_START, true);
	PrecacheSound(Conf.SND_DENY, true);
	g_iBeamSprite = PrecacheModel(Conf.BAR_MODEL);
}

public void OnSurvivalModeChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	SetEnabledState();
}

public void OnAllowInClassicModeChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	SetEnabledState();
}

void SetEnabledState()
{
	if (SC_IsCurrentMapCoop() && (g_pConVarSurvivalMode.IntValue || g_pConvarAllowInClassicMode.BoolValue))
	{
		g_bEnabled = true;
	}
	else
	{
		g_bEnabled = false;
		for (int i = 1; i <= MaxClients; i++)
		{
			ResetReviveStatus(i);
		}
	}
}

public Action Command_Revive(int client, int args)
{
	if (!g_bEnabled)
	{
		MsgReply(client, "Revive is disabled.");
		return Plugin_Handled;
	}
	if (args != 1)
	{
		MsgReply(client, "Usage: sc_revive <target>");
		return Plugin_Handled;
	}
	
	char szTarget[65];
	GetCmdArg(1, szTarget, sizeof(szTarget));

	char szTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS], iTargetCount;
	bool bIsML;
	
	if ((iTargetCount = ProcessTargetString(
			szTarget,
			client,
			iTargets,
			sizeof(iTargets),
			COMMAND_FILTER_NO_IMMUNITY|COMMAND_FILTER_DEAD|COMMAND_FILTER_NO_BOTS,
			szTargetName,
			sizeof(szTargetName),
			bIsML)) <= 0)
	{
		ReplyToTargetError(client, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTarget = iTargets[i];
		CBasePlayer pTarget = CBasePlayer(iTarget);
		if (pTarget.GetRagdoll().IsValid())
		{
			pTarget.GetRagdoll().GetAbsOrigin(g_pSpawnOptions.vecOrigin);
			pTarget.GetEyeAngles(g_pSpawnOptions.vecAngles);
		}
		else
		{
			g_pSpawnOptions.vecOrigin = vec3_origin;
		}

		if (SC_Respawn(pTarget, g_pSpawnOptions))
		{
			pTarget.ScreenFade(512, RGBA(0, 0, 200, 255), FFADE_PURGE | FFADE_IN, 1);
			EmitSoundToClient(iTarget, Conf.SND_RESPAWN, iTarget, SNDCHAN_STATIC, SNDLEVEL_NONE, .pitch = Conf.SND_RESPAWN_PITCH);
			MsgReply(client, "Respawned '%N'.", iTarget);
			if (client != iTarget)
			{
				Msg(iTarget, "%N respawned you!", client);
			}
		}
		else
		{
			MsgReply(client, "Player '%N' could not be respawned.", iTarget);
		}
	}
	
	return Plugin_Handled;
}

public void OnPlayerRunCmdPost(int client, int buttons)
{
	if (g_bEnabled && IsPlayerAlive(client))
	{
		if (buttons & IN_USE)
		{
			CBasePlayer pPlayer = CBasePlayer(client);
			CBasePlayer pTarget = g_pReviveTarget[client];
			static float vecEyeAngles[3];
			static float vecEyeOrigin[3];
			static float vecRagdollPosition[3];
			
			pPlayer.GetEyePosition(vecEyeOrigin);
			pPlayer.GetEyeAngles(vecEyeAngles);
			
			if (pTarget.IsValid())
			{
				if (pTarget.IsAlive())
				{
					ResetReviveStatus(client);
					if (g_pConVarReviveMessages.BoolValue)
						Msg(client, "Your revive target respawned...");
				}
				else
				{
					if (g_flReviveTime[client] == 0.0)
					{
						g_flReviveTime[client] = GetGameTime() + g_pConVarReviveTime.FloatValue;
					}

					CBaseEntity pRagdoll = pTarget.GetRagdoll();
					if (pRagdoll.IsValid())
					{
						pRagdoll.GetAbsOrigin(vecRagdollPosition);
						
						if (GetVectorDistance(vecEyeOrigin, vecRagdollPosition, false) > 120.0)
						{
							// Client left range to revive, play deny sound and stop previous start sound
							ResetReviveStatus(client);
							
							StopSound(pRagdoll.entindex, SNDCHAN_STATIC, Conf.SND_START);
							EmitSoundToAll(Conf.SND_DENY, pRagdoll.entindex, SNDCHAN_STATIC, Conf.SNDLEVEL);
							
							if (g_pConVarReviveMessages.BoolValue)
								Msg(client, "You have canceled reviving...");
							
							return;
						}
						
						// This delay is only here to prevent massive spam of tempents
						if (g_flNextSpriteUpdate[client] <= GetGameTime())
						{
							Client_ProgressBar(pPlayer,
								.color = Conf.BAR_COLOR,
								.flTime = g_pConVarReviveTime.FloatValue,
								.flBarLength = 30.0,
								.flBarWidth = 0.3,
								.flDistFromPlayer = 20.0);
							
							g_flNextSpriteUpdate[client] = GetGameTime() + g_pConVarReviveTime.FloatValue;
						}
						
						if (GetGameTime() >= g_flReviveTime[client])
						{
							pPlayer.GetAbsOrigin(g_pSpawnOptions.vecOrigin);
							pPlayer.GetEyeAngles(g_pSpawnOptions.vecAngles);

							if (SC_Respawn(pTarget, g_pSpawnOptions))
							{
								pTarget.ScreenFade(512, RGBA(0, 0, 200, 255), FFADE_PURGE | FFADE_IN, 1);
								EmitAmbientSound(Conf.SND_RESPAWN, g_pSpawnOptions.vecOrigin, .level = Conf.SNDLEVEL, .pitch = Conf.SND_RESPAWN_PITCH);
								
								// Give score to reviver
								pPlayer.ModifyScore(g_pConVarReviveScore.IntValue);
								if (g_pConVarReviveMessages.BoolValue)
									Msg(client, "You have revived '%N' and gained %i score!", pTarget.entindex, g_pConVarReviveScore.IntValue);
								
								ResetReviveStatus(client);
							}
						}
					}
				}
			}
			else
			{
				for (int i = 1; i <= MaxClients; i++)
				{
					if (i == client || !IsClientInGame(i) || IsFakeClient(i))
						continue;
					
					pTarget = CBasePlayer(i);
					CBaseEntity pRagdoll = pTarget.GetRagdoll();
					if (pRagdoll.IsValid())
					{
						pRagdoll.GetAbsOrigin(vecRagdollPosition);
						
						if (GetVectorDistance(vecEyeOrigin, vecRagdollPosition, false) < 100.0)
						{
							TR_TraceRayFilter(vecEyeOrigin, vecEyeAngles, MASK_SOLID, RayType_Infinite, TraceEntityFilter_IgnoreData, client);
							TR_GetEndPosition(vecEyeOrigin);
							if (GetVectorDistance(vecEyeOrigin, vecRagdollPosition, false) < 100.0)
							{
								g_pReviveTarget[client] = pTarget;
								
								// Prevent multiple sounds if player is spamming E
								StopSound(pRagdoll.entindex, SNDCHAN_STATIC, Conf.SND_START);
								EmitSoundToAll(Conf.SND_START, pRagdoll.entindex, SNDCHAN_STATIC, Conf.SNDLEVEL);
								
								if (g_pConVarReviveMessages.BoolValue)
									Msg(client, "You have started reviving '%N'", i);
								
								break;
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
				if (g_pConVarReviveMessages.BoolValue)
					Msg(client, "You have canceled reviving...");
			}
		}
	}
}

void ResetReviveStatus(int client)
{
	g_pReviveTarget[client] = NULL_CBASEENTITY;
	g_flReviveTime[client] = 0.0;
	g_flNextSpriteUpdate[client] = 0.0;
	Client_RemoveProgressBar(client);
}

public void OnClientDisconnect_Post(int client)
{
	ResetReviveStatus(client);
	g_pRagdollEffectsTimer[client] = null;
}

public void SC_OnPlayerRagdollCreated(CBasePlayer pPlayer, CBaseAnimating pRagdoll)
{
	int client = pPlayer.entindex;
	delete g_pRagdollEffectsTimer[client];
	if (g_bEnabled)
	{
		float delay = g_pConVarRagdollEffectsTimer.FloatValue;
		if (delay >= 0.0)
		{
			g_pRagdollEffectsTimer[client] = CreateTimer(delay, Timer_SetRagdollEffects, pPlayer, TIMER_FLAG_NO_MAPCHANGE);
		}
	}
}

public void Timer_SetRagdollEffects(Handle timer, CBasePlayer pPlayer)
{
	if (pPlayer.IsInGame())
	{
		g_pRagdollEffectsTimer[pPlayer.entindex] = null;
		CBaseEntity pRagdoll = pPlayer.GetRagdoll();
		if (pRagdoll.IsValid())
		{
			if (g_pConVarRagdollBlink.BoolValue)
				pRagdoll.m_fEffects |= EF_ITEM_BLINK;

			if (g_pConVarRagdollParticle.BoolValue && Conf.RAGDOLL_PARTICLE[0] != EOS)
			{
				CParticleSystem pParticle = CParticleSystem.Create(Conf.RAGDOLL_PARTICLE);
				if (pParticle.IsValid())
				{
					pParticle.SetParent(pRagdoll);
					pParticle.Teleport(vec3_origin);
					pParticle.Spawn();
					pParticle.Activate();
				}
			}
		}
	}
}

//------------------------------------------------------
// Progress bar rendering
//------------------------------------------------------
CBaseEntity g_pProgressBar[MAXPLAYERS + 1] = {NULL_CBASEENTITY, ...};

stock void Client_ProgressBar(CBasePlayer pPlayer, RGBA color, float flTime = 4.0, float flBarLength = 30.0, float flBarWidth = 0.3, float flDistFromPlayer = 20.0)
{
	// Always remove previous bar if there is one
	Client_RemoveProgressBar(pPlayer.entindex);
	
	static float vecEyePosition[3];
	
	if (pPlayer.IsValid())
	{
		pPlayer.GetEyePosition(vecEyePosition);
		
		CBeam pBeam = SetupBeamBar(flBarWidth, vecEyePosition, color, 2);
		CBeam pBarBox = SetupBeamBar(flBarWidth * 1.2, vecEyePosition, RGBA(255, 255, 255, 80), 5);
		
		pBeam.SetEffectEntity(pBarBox);
		pBarBox.SetEffectEntity(pPlayer);
		g_pProgressBar[pPlayer.entindex] = pBeam;
			
		float vecStats[3];
		vecStats[0] = flTime;
		vecStats[1] = flBarLength;
		vecStats[2] = flDistFromPlayer;
		pBeam.SetMaxs(vecStats);
		pBeam.SetLocalTime(GetGameTime() + flTime);

		SDKHook(pBeam.entindex, SDKHook_SetTransmit, Transmit_ProgressBar);
		SDKHook(pBarBox.entindex, SDKHook_SetTransmit, Transmit_ProgressBarBox);
	}
	
	return;
}

stock CBeam SetupBeamBar(float flBarWidth, float vecInitOrigin[3], RGBA color, int iRenderMode)
{
	CBeam pBeam = CBeam.Create();
	int iBeam = pBeam.entindex;
	DispatchKeyValue(iBeam, "model", Conf.BAR_MODEL);
	DispatchKeyValue(iBeam, "texture", "sprites/halo01.vmt");
	SetEntProp(iBeam, Prop_Data, "m_nModelIndex", g_iBeamSprite);
	SetEntProp(iBeam, Prop_Data, "m_nHaloIndex", 0);
	TeleportEntity(iBeam, vecInitOrigin, NULL_VECTOR, NULL_VECTOR);
	DispatchSpawn(iBeam);
	ActivateEntity(iBeam);
	
	pBeam.SetRenderColor(color);
	SetEntProp(iBeam, Prop_Data, "m_nBeamType", 1);
	SetEntProp(iBeam, Prop_Data, "m_nBeamFlags", 0);
	SetEntProp(iBeam, Prop_Data, "m_nNumBeamEnts", 2);
	SetEntPropVector(iBeam, Prop_Data, "m_vecEndPos", vecInitOrigin);
	SetEntPropFloat(iBeam, Prop_Data, "m_fWidth", flBarWidth);
	SetEntPropFloat(iBeam, Prop_Data, "m_fEndWidth", flBarWidth);
	SetEntPropFloat(iBeam, Prop_Data, "m_fSpeed", 0.0);
	SetEntPropFloat(iBeam, Prop_Data,"m_flFrameRate", 0.0);
	SetEntPropFloat(iBeam, Prop_Data,"m_flHDRColorScale", 1.0);
	SetEntProp(iBeam, Prop_Data, "m_nDissolveType", -1);
	SetEntProp(iBeam, Prop_Data, "m_nRenderMode", iRenderMode);
	SetEntPropFloat(iBeam, Prop_Data, "m_fHaloScale", 0.0);
	
	return pBeam;
}

stock void Client_RemoveProgressBar(int iPlayer)
{
	if (g_pProgressBar[iPlayer].IsValid())
	{
		CBaseEntity pBarBox = CBaseEntity(GetEntPropEnt(g_pProgressBar[iPlayer].entindex, Prop_Data, "m_hEffectEntity"));
		if (pBarBox.IsValid())
		{
			pBarBox.Kill();
		}
		
		g_pProgressBar[iPlayer].Kill();
		g_pProgressBar[iPlayer] = NULL_CBASEENTITY;
	}
	
	return;
}

public Action Transmit_ProgressBarBox(int entity, int client)
{
	if (GetEntPropEnt(entity, Prop_Data, "m_hEffectEntity") == client)
	{
		return Plugin_Continue;
	}
	return Plugin_Handled;
}

public Action Transmit_ProgressBar(int entity, int client)
{
	if (g_pProgressBar[client] == CBaseEntity(entity))
	{
		static int iBarBox;
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
				
				angAdjust = vecEyeAngles[0];
				if (angAdjust < 0.0) angAdjust = -1.0*angAdjust;
				if (angAdjust > 64.0) angAdjust = 64.0;
				vecStats[2] = (-1.0 * (vecStats[2] * angAdjust) / 90.0) + vecStats[2];
				vecStats[1] = (vecStats[1] * angAdjust) / 180.0 + vecStats[1];
				
				vecStart[0] = (vecEyePosition[0] + (vecStats[2] * Cosine(DegToRad(vecEyeAngles[1]+vecStats[1]))));
				vecStart[1] = (vecEyePosition[1] + (vecStats[2] * Sine(DegToRad(vecEyeAngles[1]+vecStats[1]))));
				// Clamp vertical position
				angAdjust = vecEyeAngles[0];
				if (angAdjust > 64.0) angAdjust = 64.0;
				else if (angAdjust < -64.0) angAdjust = -64.0;
				vecStart[2] = (vecEyePosition[2] - (vecStats[2] * Tangent(DegToRad(angAdjust))));
				
				vecEnd[0] = (vecEyePosition[0] + (vecStats[2] * Cosine(DegToRad(vecEyeAngles[1]-vecStats[1]))));
				vecEnd[1] = (vecEyePosition[1] + (vecStats[2] * Sine(DegToRad(vecEyeAngles[1]-vecStats[1]))));
				vecEnd[2] = vecStart[2];
				
				
				// Background box
				iBarBox = GetEntPropEnt(entity, Prop_Data, "m_hEffectEntity");
				if (IsValidEntity(iBarBox))
				{
					TeleportEntity(iBarBox, vecStart, NULL_VECTOR, NULL_VECTOR);
					SetEntPropVector(iBarBox, Prop_Data, "m_vecEndPos", vecEnd);
				}
				// End background box
				
				
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