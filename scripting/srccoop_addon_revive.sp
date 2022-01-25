#include <sourcemod>
#include <sdktools>
#include <sdkhooks>

#include <srccoop_api>

#pragma semicolon 1;
#pragma newdecls required;

ConVar g_pConVarReviveTime;
ConVar g_pConVarReviveScore;
ConVar g_pConVarSpriteMaterial, g_pConVarSpriteScale, g_pConVarSpriteColor, g_pConVarSpriteFloatDistance;

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
	g_pConVarReviveTime = CreateConVar("sourcecoop_revive_time", "4.0", "Sets time that you have to hold E to revive.", _, true, 0.0, false);
	g_pConVarReviveScore = CreateConVar("sourcecoop_revive_score", "1", "Sets score to give for reviving a player.", _, true, 0.0, false);
	
	// Sprite ConVars
	g_pConVarSpriteMaterial = CreateConVar("sourcecoop_revive_sprite_material", "vgui/hud/hud_health.vmt", "Sets material of sprite used when player dies. Must be relative to materials/ directory");
	g_pConVarSpriteScale = CreateConVar("sourcecoop_revive_sprite_scale", "0.2", "Sets the scale size of the revive sprite.", _, true, 0.1, false);
	g_pConVarSpriteColor = CreateConVar("sourcecoop_revive_sprite_color", "255 0 0", "Sets color of the revive sprite.");
	g_pConVarSpriteFloatDistance = CreateConVar("sourcecoop_revive_sprite_vertical", "8.0", "Sets distance above ragdoll to spawn in at. Mainly used for custom material types that might clip through the floor.");
	
	RegAdminCmd("sourcecoop_revive", Command_ForceRespawn, ADMFLAG_ROOT, "Force respawn by client index.");
	
	HookEventEx("entity_killed", Event_EntityKilled, EventHookMode_Post);
	
	AutoExecConfig(true, "srccoop_addon_revive");
}

public void OnMapStart()
{
	PrecacheSound("weapons/tau/gauss_undercharge.wav",true);
	PrecacheSound("items/suitchargeok1.wav",true);
	PrecacheSound("items/suitchargeno1.wav",true);
}

public Action Event_EntityKilled(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	int iVictimCheck = GetEventInt(hEvent, "entindex_killed");
	if ((iVictimCheck > 0) && (iVictimCheck <= MaxClients))
	{
		CreateTimer(0.1, GetCLRagdoll, iVictimCheck, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Action GetCLRagdoll(Handle timer, int client)
{
	if (IsValidEntity(client))
	{
		char szSpecifications[128];
		g_pConVarSpriteMaterial.GetString(szSpecifications, sizeof(szSpecifications));
		Format(szSpecifications, sizeof(szSpecifications), "materials/%s", szSpecifications);
		if (!FileExists(szSpecifications, true, NULL_STRING)) return Plugin_Handled;
		g_pConVarSpriteMaterial.GetString(szSpecifications, sizeof(szSpecifications));
		
		int iRagdoll = GetEntPropEnt(client, Prop_Send, "m_hRagdoll");
		if (IsValidEntity(iRagdoll))
		{
			float vecOrigin[3];
			
			GetEntPropVector(iRagdoll, Prop_Data, "m_vecAbsOrigin", vecOrigin);
			// Bring up, then trace down to floor
			vecOrigin[2] += 20.0;
			TR_TraceRayFilter(vecOrigin, {90.0, 0.0, 0.0}, MASK_SOLID, RayType_Infinite, TraceEntityFilter, client);
			TR_GetEndPosition(vecOrigin);
			vecOrigin[2] += g_pConVarSpriteFloatDistance.FloatValue;
			
			int iSprite = CreateEntityByName("env_sprite");
			if (IsValidEntity(iSprite))
			{
				DispatchKeyValue(iSprite, "model", szSpecifications);
				DispatchKeyValue(iSprite, "spawnflags", "1");
				DispatchKeyValueFloat(iSprite, "scale", g_pConVarSpriteScale.FloatValue);
				DispatchKeyValue(iSprite, "rendermode", "9");
				
				g_pConVarSpriteColor.GetString(szSpecifications, sizeof(szSpecifications));
				DispatchKeyValue(iSprite, "rendercolor", szSpecifications);
				
				TeleportEntity(iSprite, vecOrigin, NULL_VECTOR, NULL_VECTOR);
				
				DispatchSpawn(iSprite);
				ActivateEntity(iSprite);
				
				SetEntPropEnt(iSprite, Prop_Data, "m_hEffectEntity", iRagdoll);
				SetEntPropEnt(iRagdoll, Prop_Data, "m_hEffectEntity", iSprite);
				
				CreateTimer(0.1, SpriteTimerTick, CBaseEntity(iSprite), TIMER_FLAG_NO_MAPCHANGE);
			}
		}
	}
	return Plugin_Handled;
}

public Action SpriteTimerTick(Handle timer, CBaseEntity pSprite)
{
	if (pSprite.IsValid())
	{
		CBaseEntity pRagdoll = CBaseEntity(GetEntPropEnt(pSprite.GetEntIndex(), Prop_Data, "m_hEffectEntity"));
		if ((!pRagdoll.IsValid()) || (pRagdoll.GetEntIndex() <= MaxClients))
		{
			pSprite.Kill();
		}
		else
		{
			CreateTimer(0.1, SpriteTimerTick, pSprite, TIMER_FLAG_NO_MAPCHANGE);
		}
	}
	
	return Plugin_Handled;
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
		
		SetPlayerCanSpawn(pTarget, true);
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
						CBaseEntity pSprite = CBaseEntity(GetEntPropEnt(pRagdoll.GetEntIndex(), Prop_Data, "m_hEffectEntity"));
						if (pSprite.IsValid())
						{
							pSprite.GetAbsOrigin(vecRagdollPosition);
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
								
								SetPlayerCanSpawn(g_pReviveTarget[client], true);
								
								g_pReviveTarget[client].Spawn();
								g_pReviveTarget[client].Activate();
								
								// Fix for if player died on a ladder
								SetEntPropEnt(g_pReviveTarget[client].GetEntIndex(), Prop_Data, "m_hLadder", -1);
								
								pSprite.Kill();
								pRagdoll.Kill();
								
								// Delay to allow equip time
								Handle dp = CreateDataPack();
								WritePackCell(dp, pPlayer);
								WritePackCell(dp, g_pReviveTarget[client]);
								// Tested this as a CreateDataTimer, but the datapack is always empty for some reason
								// Not going to put a TIMER_FLAG_NO_MAPCHANGE here so it can close the handle if the map changes
								CreateTimer(0.1, DelayRespawn, dp);
								
								pPlayer.ModifyScore(g_pConVarReviveScore.IntValue);
								g_pReviveTarget[client] = view_as<CBasePlayer>(-1);
								g_flReviveTime[client] = 0.0;
							}
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
									CBaseEntity pSprite = CBaseEntity(GetEntPropEnt(pRagdoll.GetEntIndex(), Prop_Data, "m_hEffectEntity"));
									if (pSprite.IsValid())
									{
										pSprite.GetAbsOrigin(vecRagdollPosition);
										
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

public Action DelayRespawn(Handle timer, Handle dp)
{
	if (dp != INVALID_HANDLE)
	{
		ResetPack(dp);
		CBasePlayer pPlayer = ReadPackCell(dp);
		CBasePlayer pTarget = ReadPackCell(dp);
		CloseHandle(dp);
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
}

public bool TraceEntityFilter(int entity, int mask, any data)
{
	if (entity == data) return false;
	return true;
}