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
	bool m_bSpecInit[MAXPLAYERS + 1];
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
	
	public static void Clear(bool bMapChange)
	{
		SurvivalManager.ClearDead();
		if (bMapChange)
		{
			data.m_pRestartTimer = null;
		}
		else
		{
			delete data.m_pRestartTimer;
		}

		for (int i = 1; i <= MaxClients; i++)
		{
			data.m_bSpecInit[i] = false;
		}
	}

	public static SurvivalMode GetMode()
	{
		return view_as<SurvivalMode>(data.m_pConvarMode.IntValue);
	}

	/**
	 * Is currently respawning a player?
	 */
	public static bool IsRespawning()
	{
		return data.m_bRespawning;
	}
	
	public static bool GetRespawnAtCheckpoints()
	{
		return data.m_pConvarRespawn.BoolValue;
	}
	
	public static float GetSpawnTimeout()
	{
		return data.m_pConvarSpawnTimeout.FloatValue;
	}
	
	/**
	 * Returns true if players can spawn, false if spawn timeout is up.
	 */
	public static bool CheckSpawnTimeout()
	{
		float timeout = SurvivalManager.GetSpawnTimeout();
		return timeout == -1
			|| !g_pCoopManagerData.m_bStarted
			|| g_pCoopManagerData.m_flStartTime + timeout > GetGameTime();
	}
	
	/**
	 * Clears saved dead players
	 */
	public static void ClearDead()
	{
		data.m_pDeadPlayerIDs.Clear();
	}

	public static bool IsDead(CBasePlayer pPlayer)
	{
		int iSteamID = GetSteamAccountID(pPlayer.entindex, g_pConvarValidateSteamIds.BoolValue);
		return data.m_pDeadPlayerIDs.FindValue(iSteamID) != -1;
	}

	public static void SetDead(CBasePlayer pPlayer, bool bDead)
	{
		int iSteamID = GetSteamAccountID(pPlayer.entindex, g_pConvarValidateSteamIds.BoolValue);
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
	
	/**
	 * Returns false if player spawn should be blocked.
	 */
	public static bool HandlePlayerSpawn(CBasePlayer pPlayer)
	{
		if (SurvivalManager.GetMode())
		{
			if (!data.m_bRespawning && (!SurvivalManager.CheckSpawnTimeout() || SurvivalManager.IsDead(pPlayer)))
			{
				int client = pPlayer.entindex;
				
				if (!data.m_bSpecInit[client] && data.m_pRestartTimer == null)
				{
					// Enable spectator mode
					data.m_bSpecInit[client] = true;
					pPlayer.SetViewEntity(pPlayer);
					pPlayer.SetObserverMode(OBS_MODE_CHASE);
					pPlayer.SetMoveType(MOVETYPE_OBSERVER);
					pPlayer.m_usSolidFlags |= FSOLID_NOT_SOLID;
					pPlayer.m_fEffects |= EF_NODRAW;
					
					// Show spectate gui
					ShowVGUIPanel(client, "specgui", null);
					
					// Remove previous fades
					pPlayer.ScreenFade(1500, RGBA(0, 0, 0, 255), FFADE_IN | FFADE_PURGE, 1);
					
					SetHudTextParams(0.016, 0.04, 5.0, 255, 255, 255, 255, 1, 1.0, 1.0, 1.0);
					if (SurvivalManager.GetRespawnAtCheckpoints() && CCoopSpawnSystem.CountCheckpoints(true, true))
					{
						ShowHudText(client, 1, "%t", "survival respawn hint checkpoint");
					}
					else
					{
						ShowHudText(client, 1, "%t", "survival respawn hint no checkpoint");
					}
				}
				
				return false;
			}
		}
		return true;
	}
	
	public static void OnPlayerChangeTeam(const CBasePlayer pPlayer, const int iTeam)
	{
		if (SurvivalManager.GetMode())
		{
			data.m_bSpecInit[pPlayer.entindex] = false;
			SurvivalManager.GameOverCheck();
		}
	}

	public static void OnPlayerDeath(CBasePlayer pPlayer)
	{
		if (SurvivalManager.GetMode())
		{
			data.m_bSpecInit[pPlayer.entindex] = false;
			SurvivalManager.SetDead(pPlayer, true);
			SurvivalManager.GameOverCheck();
		}
	}
	
	public static void OnClientDisconnectPost(int client)
	{
		if (SurvivalManager.GetMode())
		{
			data.m_bSpecInit[client] = false;
			SurvivalManager.GameOverCheck();
		}
	}
	
	/**
	 * Respawn logic for logic_autosave entities
	 */
	public static void OnLogicAutosave(int iEntIndex, bool bSaveDangerous)
	{
		if (!SurvivalManager.GetMode() || !SurvivalManager.GetRespawnAtCheckpoints() || !g_pSpawnSystemData.m_bRespawnOnLogicAutosave)
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
	
	/**
	 * Checkpoint activation respawn handler
	 */
	public static void OnCheckpointActivation(CCoopSpawnEntry pCheckpoint)
	{
		if (pCheckpoint.m_bRespawnPlayers && SurvivalManager.GetMode() && SurvivalManager.GetRespawnAtCheckpoints())
		{
			SurvivalManager.RespawnPlayers();
		}
	}
	
	public static void RespawnPlayers(const char[] szHudText = "checkpoint reached")
	{
		SurvivalManager.ClearDead();
		
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (SurvivalManager.RespawnPlayer(pPlayer))
				{
					if (szHudText[0])
					{
						SetHudTextParams(-1.0, 0.9, 1.5, 200, 200, 200, 255, 0, 1.0, 1.0, 1.0);
						ShowHudText(i, 2, "%t", szHudText);
					}
				}
			}
		}
	}
	
	public static bool RespawnPlayer(CBasePlayer pPlayer)
	{
		if (!pPlayer.IsAlive() && pPlayer.GetTeam() != TEAM_SPECTATOR)
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
			if (IsClientInGame(i) && !IsFakeClient(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (pPlayer.GetTeam() != TEAM_SPECTATOR)
				{
					pPlayer.ScreenFade(1000, RGBA(0, 0, 0, 255), FFADE_OUT | FFADE_STAYOUT);
				}
				
				#if defined SRCCOOP_BLACKMESA
				if (!IsTranslatedForLanguage("survival game over", GetClientLanguage(i)))
				{
					ShowHudText(i, 2, "#BMS_GameOver_Ally");
					continue;
				}
				#endif
				ShowHudText(i, 2, "%t", "survival game over");
			}
		}
	}
}

public void Timer_SurvivalRespawnPlayers(Handle timer)
{
	data.m_pRestartTimer = null;
	SurvivalManager.RespawnPlayers("survival restart last checkpoint");
}

public void Timer_SurvivalRestartLevel(Handle timer)
{
	data.m_pRestartTimer = null;

	char szMapName[MAX_MAPNAME];
	GetCurrentMap(szMapName, sizeof(szMapName));
	
	strcopy(g_szMapName, sizeof(g_szMapName), g_szPrevMapName);
	ForceChangeLevel(szMapName, SC_SURVIVAL_RESTART_MAPCHANGE);
}

public void ConVarSurvivalModeChanged(ConVar hConVar, const char[] szOldValue, const char[] szNewValue)
{
	if (strcmp(szNewValue, "0", false) == 0)
	{
		SurvivalManager.Clear(false);
	}
}

// end shortcut
#undef data