#if defined _srccoop_manager_survival_included
 #endinput
#endif
#define _srccoop_manager_survival_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Survival extension of manager
//------------------------------------------------------

enum struct SurvivalManagerData
{
	ArrayList m_pDeadPlayerIDs;
	ConVar m_pConvarMode;
	ConVar m_pConvarRespawn;
	ConVar m_pConvarSpawnTimeout;
	Handle m_pRestartTimer;
	bool m_bRespawning;
}

SurvivalManagerData g_pSurvivalManagerData;

// local shortcut for data:
#define data g_pSurvivalManagerData

methodmap SurvivalManager {

	public static void Initialize()
	{
		data.m_pConvarMode = CreateConVar("sourcecoop_survival_mode", "0", "Sets survival mode. 0 = off. 1 will respawn players if all are dead, 2 will restart the map.", _, true, 0.0, true, 2.0);
		data.m_pConvarRespawn = CreateConVar("sourcecoop_survival_respawn", "1", "Whether to respawn dead players at checkpoints.", _, true, 0.0, true, 1.0);
		data.m_pConvarSpawnTimeout = CreateConVar("sourcecoop_survival_spawn_timeout", "-1", "Number of seconds after the map starts (after initial timer) to allow spawning in, or -1 for no time limit. Do not use too low values so players have some time to join in.", _, true, -1.0);
		data.m_pDeadPlayerIDs = CreateArray();
		
		data.m_pConvarMode.AddChangeHook(ConVarSurvivalModeChanged);
	}
	
	public static void Clear()
	{
		SurvivalManager.ClearDead();
		data.m_pRestartTimer = null;
	}
	
	public static SurvivalMode GetMode()
	{
		return view_as<SurvivalMode>(data.m_pConvarMode.IntValue);
	}
	
	public static bool GetRespawnAtCheckpoints()
	{
		return data.m_pConvarRespawn.BoolValue;
	}
	
	public static float GetSpawnTimeout()
	{
		return data.m_pConvarSpawnTimeout.FloatValue;
	}
	
	public static bool CheckSpawnTimeout()
	{
		float timeout = SurvivalManager.GetSpawnTimeout();
		return timeout == -1
				|| !g_pCoopManagerData.m_bStarted
				|| g_pCoopManagerData.m_flStartTime + timeout > GetGameTime();
	}
	
	public static void ClearDead()
	{
		data.m_pDeadPlayerIDs.Clear();
	}

	public static bool IsDead(CBasePlayer pPlayer)
	{
		int iSteamID = GetSteamAccountID(pPlayer.GetEntIndex());
		return data.m_pDeadPlayerIDs.FindValue(iSteamID) != -1;
	}

	public static void SetDead(CBasePlayer pPlayer, bool bDead)
	{
		int iSteamID = GetSteamAccountID(pPlayer.GetEntIndex());
		int index = data.m_pDeadPlayerIDs.FindValue(iSteamID);

		if ((index == -1) == bDead)
		{
			if (bDead)
			{
				data.m_pDeadPlayerIDs.Push(iSteamID);
			}
			else
			{
				data.m_pDeadPlayerIDs.Erase(index);
			}
		}
	}
	
	public static bool HandlePlayerSpawn(CBasePlayer pPlayer)
	{
		if (SurvivalManager.GetMode())
		{
			if (!data.m_bRespawning && (!SurvivalManager.CheckSpawnTimeout() || SurvivalManager.IsDead(pPlayer)))
			{
				// Enable spectator mode
				pPlayer.SetViewEntity(pPlayer);
				pPlayer.SetObserverMode(OBS_MODE_CHASE);
				// Do not touch things when spectating
				pPlayer.SetSolidFlags(pPlayer.GetSolidFlags() | FSOLID_NOT_SOLID);
				// Do not render by FX, RenderMode is constantly reset
				pPlayer.SetRenderFx(RENDERFX_FADE_FAST);
				// Remove previous fades
				Client_ScreenFade(pPlayer.GetEntIndex(), 1500, FFADE_IN|FFADE_PURGE, 1, 0, 0, 0, 255);
				
				SetHudTextParams(0.016, 0.04, 5.0, 255, 255, 255, 255, 1, 1.0, 1.0, 1.0);
				if (SurvivalManager.GetRespawnAtCheckpoints())
				{
					// ToDo: verify there are any survival CP's left.
					ShowHudText(pPlayer.GetEntIndex(), 1, "You will respawn at the next survival checkpoint");
				}
				else
				{
					ShowHudText(pPlayer.GetEntIndex(), 1, "You will respawn once everyone is dead");
				}
				
				return false;
			}
		}
		return true;
	}
	
	public static void HandlePlayerDeath(CBasePlayer pVictim)
	{
		if (SurvivalManager.GetMode())
		{
			SurvivalManager.SetDead(pVictim, true);
			SurvivalManager.GameOverCheck();
		}
	}
	
	public static void OnLogicAutosave(int iEntIndex, bool bSaveDangerous)
	{
		if (!SurvivalManager.GetMode() || !SurvivalManager.GetRespawnAtCheckpoints() || !g_SpawnSystem.m_bRespawnOnLogicAutosave)
			return;

		if (bSaveDangerous)
		{
			int iMinimumHitpoints = GetEntProp(iEntIndex, Prop_Data, "m_minHitPoints");
			for (int i = 1; i <= MaxClients; i++)
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (pPlayer.IsInGame() && pPlayer.IsAlive())
				{
					if (pPlayer.GetHealth() >= iMinimumHitpoints)
					{
						SurvivalManager.RespawnPlayers();
						break;
					}
				}
			}
		}
		else
		{
			SurvivalManager.RespawnPlayers();
		}
	}
	
	public static void OnCheckpointActivation(CCoopSpawnEntry pCheckpoint)
	{
		if (pCheckpoint.m_bRespawnPlayers && SurvivalManager.GetMode() && SurvivalManager.GetRespawnAtCheckpoints())
		{
			SurvivalManager.RespawnPlayers();
		}
	}
	
	public static void RespawnPlayers(const char[] szHudText = "Checkpoint reached")
	{
		SurvivalManager.ClearDead();
		
		for (int i = 1; i <= MaxClients; i++)
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer.IsInGame())
			{
				if (SurvivalManager.RespawnPlayer(pPlayer))
				{
					if (szHudText[0])
					{
						SetHudTextParams(-1.0, 0.9, 1.5, 200, 200, 200, 255, 0, 1.0, 1.0, 1.0);
						ShowHudText(i, 2, szHudText);
					}
				}
			}
		}
	}
	
	public static bool RespawnPlayer(CBasePlayer pPlayer)
	{
		if (!pPlayer.IsAlive() && pPlayer.GetTeam() > TEAM_SPECTATOR)
		{
			SurvivalManager.SetDead(pPlayer, false);
			data.m_bRespawning = true;
			pPlayer.Spawn();
			data.m_bRespawning = false;
			return true;
		}
		return false;
	}

	public static void GameOverCheck()
	{
		if (SurvivalManager.GetMode() && data.m_pRestartTimer == null && g_pCoopManagerData.m_bStarted && !GetRealClientCount(true, true))
		{
			SurvivalManager.GameOverFade();

			switch (SurvivalManager.GetMode())
			{
				case SURVIVAL_RESPAWN:
				{
					data.m_pRestartTimer = CreateTimer(6.0, Timer_SurvivalRespawnPlayers, _, TIMER_FLAG_NO_MAPCHANGE);
				}
				case SURVIVAL_RESTART:
				{
					data.m_pRestartTimer = CreateTimer(6.0, Timer_SurvivalRestartLevel, _, TIMER_FLAG_NO_MAPCHANGE);
				}
			}
		}
	}

	public static void GameOverFade()
	{
		SetHudTextParams(-1.0, 0.45, 6.0, 200, 200, 200, 255, 0, 0.5, 1.0, 1.0);
		for (int i = 1; i <= MaxClients; i++)
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer.IsInGame() && pPlayer.GetTeam() > TEAM_SPECTATOR)
			{
				Client_ScreenFade(i, 1000, FFADE_OUT|FFADE_STAYOUT, _, 0, 0, 0, 255);
				ShowHudText(i, 2, "#BMS_GameOver_Ally");
			}
		}
	}
}

public Action Timer_SurvivalRespawnPlayers(Handle timer)
{
	data.m_pRestartTimer = null;
	SurvivalManager.RespawnPlayers("Try again! Starting at last checkpoint...");
	return Plugin_Handled;
}

public Action Timer_SurvivalRestartLevel(Handle timer)
{
	data.m_pRestartTimer = null;

	char szMapName[MAX_MAPNAME];
	GetCurrentMap(szMapName, sizeof(szMapName));
	
	strcopy(g_szMapName, sizeof(g_szMapName), g_szPrevMapName);
	ForceChangeLevel(szMapName, SC_SURVIVAL_RESTART_MAPCHANGE);
	
	return Plugin_Handled;
}

public void ConVarSurvivalModeChanged(ConVar hConVar, const char[] szOldValue, const char[] szNewValue)
{
	if (strcmp(szNewValue, "0", false) == 0)
	{
		SurvivalManager.Clear();
	}
}

// end shortcut
#undef data