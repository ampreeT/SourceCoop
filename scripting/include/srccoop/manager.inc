#if defined _srccoop_manager_included
 #endinput
#endif
#define _srccoop_manager_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Coop manager
//------------------------------------------------------

enum struct CoopManagerData
{	
	KeyValues kv;
	ChangeLevelManager m_pChangeLevelMgr;
	int m_iEnabledFeatures;
	bool m_bIsCoopMap;
	bool m_bStarted;
	float m_flStartTime;
	bool m_bPlayerSpawned;
	int m_iSecondsToStart;
	bool m_bGetReadyTimer;
	Handle m_pStartTimer;
	ArrayList m_pDelayedOutputs;
	int m_iClientChangeLvlIndicies[MAXPLAYERS+1];
	Handle m_pChangeLevelTimer;
	int m_iTicksToEnd;
	CoopConfigLocation m_mapConfigLoc;
}

CoopManagerData g_pCoopManagerData;

// local shortcut for data:
#define data g_pCoopManagerData

methodmap CoopManager
{

	public static void Initialize()
	{
		data.m_pDelayedOutputs = new ArrayList(sizeof(FireOutputData));
		data.m_pChangeLevelMgr = new ChangeLevelManager();
		SurvivalManager.Initialize();
	}

	public static bool OnLevelInit(char szMapEntities[ENTITYSTRING_LENGTH])
	{
		delete data.kv;
		data.m_pDelayedOutputs.Clear();
		data.m_pStartTimer = null;
		data.m_pChangeLevelTimer = null;
		data.m_bGetReadyTimer = false;
		data.m_bStarted = false;
		data.m_bPlayerSpawned = false;
		data.m_pChangeLevelMgr.Clear();
		data.m_iEnabledFeatures = 0;
		for (int i = 0; i <= MaxClients; i++)
		{
			data.m_iClientChangeLvlIndicies[i] = -1;
		}
		CCoopSpawnSystem.Clear();
		g_pLevelLump.Clear();
		SurvivalManager.Clear(true);
		
		data.kv = CoopManager.LoadMapConfig(g_szMapName, data.m_mapConfigLoc);
	
		if ((data.m_bIsCoopMap = data.kv != null))
		{
			if (CoopManager.IsNewMapSeries())
			{
				// reset global states
				g_ServerGameDLL.GameShutdown();
				EquipmentManager.ClearAll();
			}
			g_pLevelLump.ParseMapEntities(szMapEntities);
			g_pLevelLump.ParseConfigFile(data.kv);
			g_pLevelLump.ToString(szMapEntities);
			return true;
		}
		return false;
	}

	public static void OnMapStart()
	{
		if (!data.kv)
			return;
	
		PrecacheSound(Conf.SND_TICK);
		PrecacheSound(Conf.SND_3);
		PrecacheSound(Conf.SND_2);
		PrecacheSound(Conf.SND_1);
		PrecacheSound(Conf.SND_CHANGELEVEL);
		PrecacheSound(Conf.SND_CHANGELEVEL_TICK);

		#if defined SRCCOOP_BLACKMESA
		CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7);
		#endif

		data.kv.Rewind();
		CCoopSpawnSystem.ParseConfigFile(data.kv);
		
		Call_StartForward(g_CoopMapConfigLoadedFwd);
		Call_PushCell(data.kv);
		Call_PushCell(data.m_mapConfigLoc);
		Call_Finish();
		delete data.kv;
	}

	public static void OnClientPutInServer(CBasePlayer pPlayer)
	{
		if (CoopManager.IsCoopModeEnabled())
		{
			CoopManager.CheckStart();
			CCoopSpawnSystem.TeleportPlayer(pPlayer);
		}
	}

	public static void OnPlayerSpawned(CBasePlayer pPlayer)
	{
		bool bDefaultTeleport = g_pSpawnOptions.bUseSpawnSystem;
		bool bDefaultEquipment = g_pSpawnOptions.bUseSpawnEquipment;

		// got saved items from last death?
		if (HasPackedItems(pPlayer))
		{
			if (g_pSpawnOptions.bRevive)
			{
				pPlayer.RemoveAllItems();
				RestorePackedItems(pPlayer);
				bDefaultEquipment = false;
			}
			else
			{
				// no use anymore, dont linger with these ents
				ClearPackedItems(pPlayer);
			}
		}

		if (!VecEqual(g_pSpawnOptions.vecOrigin, vec3_origin))
		{
			CanTeleportResult res;
			if ((res = CanTeleportPlayer(pPlayer, g_pSpawnOptions.vecOrigin, !g_pSpawnOptions.bUnstuck)))
			{
				if (res == CAN_TP_MUSTDUCK)
					pPlayer.StartDucking();
				else
					pPlayer.StopDucking();
			}
			pPlayer.Teleport(g_pSpawnOptions.vecOrigin, g_pSpawnOptions.vecAngles, vec3_origin);
			bDefaultTeleport = false;
		}

		if (bDefaultTeleport)
			CCoopSpawnSystem.TeleportPlayer(pPlayer);
		if (bDefaultEquipment)
			CCoopSpawnSystem.SpawnPlayerEquipment(pPlayer);
		g_pSpawnOptions.Reset();

		ItemInstancingManager.OnPlayerSpawned(pPlayer);
		
		data.m_bPlayerSpawned = true;
		if (!data.m_bStarted)
		{
			if (g_pLevelLump.m_iIntroType == INTRO_FADE)
			{
				Client_ScreenFade(pPlayer.GetEntIndex(), 0, FFADE_STAYOUT);
				pPlayer.SetFlags(pPlayer.GetFlags() | FL_ATCONTROLS);
			}
			else if (g_pLevelLump.m_iIntroType == INTRO_FREEZE)
			{
				pPlayer.SetFlags(pPlayer.GetFlags() | FL_ATCONTROLS);
				Client_ScreenFade(pPlayer.GetEntIndex(), Conf.FROZEN_FADE_DUR_IN, FFADE_OUT|FFADE_STAYOUT, _,
					Conf.FROZEN_FADE_COLOR[0], Conf.FROZEN_FADE_COLOR[1], Conf.FROZEN_FADE_COLOR[2], Conf.FROZEN_FADE_COLOR[3]);
			}
			CoopManager.CheckStart();
		}
	}

	public static void CheckStart()
	{
		if (!data.m_bStarted && data.m_bPlayerSpawned)
		{
			// are we allowed to start or waiting for players?
			if (CoopManager.IsAllowedToStart())
			{
				// is initial timer already running?
				if (CoopManager.IsInCountDownPeriod())
				{
					// can start, but do a 3 sec readyup timer as we cancel the initial player wait timer
					if (!data.m_bGetReadyTimer)
					{
						delete data.m_pStartTimer;
						CoopManager.StartCountdown(true);
					}
				}
				else
				{
					// we can just start if no previous timer is running
					CoopManager.Start();
				}
			}
			else if (!CoopManager.IsInCountDownPeriod())
			{
				// not allowed to start now and no timer is running, start the timer.
				CoopManager.StartCountdown(false);
			}
		}
	}

	public static void StartCountdown(bool bGetReadyTimer)
	{
		if ((data.m_bGetReadyTimer = bGetReadyTimer))
		{
			data.m_iSecondsToStart = 3;
			EmitSoundToAll(Conf.SND_3, .volume = Conf.SND_TICK_VOL);
		}
		else
		{
			data.m_iSecondsToStart = g_pConvarWaitPeriod.IntValue;
		}

		#if defined SRCCOOP_BLACKMESA
		CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + data.m_iSecondsToStart + 1.0);
		#endif

		data.m_pStartTimer = CreateTimer(1.0, StartTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
	}

	public static bool IsInCountDownPeriod()
	{
		return data.m_pStartTimer != null;
	}

	public static bool IsAllowedToStart()
	{
		return (g_pLevelLump.m_iIntroType == INTRO_NONE && data.m_pDelayedOutputs.Length == 0) || (GetRealClientCount(false) - GetRealClientCount(true)) == 0;
	}

	public static void Start()
	{
		if (!GetRealClientCount(true))
		{
			return;
		}
		
		#if defined SRCCOOP_BLACKMESA
		CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, 0.0);
		#endif
		
		data.m_bStarted = true;
		data.m_flStartTime = GetGameTime();
		MsgSrv("%t", "map start", g_szMapName);

		if (g_pLevelLump.m_iIntroType == INTRO_FADE || g_pLevelLump.m_iIntroType == INTRO_FREEZE)
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i) && !IsFakeClient(i))
				{
					CBasePlayer pClient = CBasePlayer(i);
					pClient.SetFlags(pClient.GetFlags() & ~FL_ATCONTROLS);
					if (g_pLevelLump.m_iIntroType == INTRO_FREEZE)
					{
						Client_ScreenFade(pClient.GetEntIndex(), Conf.FROZEN_FADE_DUR_OUT, FFADE_PURGE|FFADE_IN, 1,
							Conf.FROZEN_FADE_COLOR[0], Conf.FROZEN_FADE_COLOR[1], Conf.FROZEN_FADE_COLOR[2], Conf.FROZEN_FADE_COLOR[3]);
					}
				}
			}
		}

		// Fire delayed outputs
		for (int i = 0; i < data.m_pDelayedOutputs.Length; i++)
		{
			FireOutputData pFireOutputData; data.m_pDelayedOutputs.GetArray(i, pFireOutputData);
			if (pFireOutputData.m_pCaller.IsValid())
			{
				FireEntityOutput(pFireOutputData.m_pCaller.GetEntIndex(), pFireOutputData.m_szName, pFireOutputData.m_pActivator.GetEntIndex(), pFireOutputData.m_flDelay);

				if (pFireOutputData.m_pCaller.IsClassname("trigger_once"))
				{
					pFireOutputData.m_pCaller.SetNextThinkTick(GetGameTickCount()+1); // allow to kill itself
				}
			}
		}
		
		// Notify plugins via forward
		Call_StartForward(g_CoopMapStartFwd);
		Call_Finish();
	}

	public static void AddDelayedOutput(const FireOutputData pFireOutputData)
	{
		data.m_pDelayedOutputs.PushArray(pFireOutputData);
	}

	public static bool IsMapAllowedNext(const char[] szNextMap)
	{
		return (g_pLevelLump.m_pNextMapList.Length == 0 || g_pLevelLump.m_pNextMapList.IsInMapList(szNextMap))
				&& IsMapValid(szNextMap);
	}

	public static void OnChangeLevelInput(CChangelevel pChangelevel)
	{
		char szMapName[MAX_MAPNAME];
		pChangelevel.GetMapName(szMapName, sizeof(szMapName));
		
		if (CoopManager.IsMapAllowedNext(szMapName))
		{
			pChangelevel.FireOutput("OnChangeLevel");
			CoopManager.EndMap(szMapName, SC_NORMAL_MAPCHANGE);
		}
	}

	public static void OnChangeLevelTouch(CChangelevel pChangelevel, CBasePlayer pActivator)
	{
		char szMapName[MAX_MAPNAME];
		pChangelevel.GetMapName(szMapName, sizeof(szMapName));
		
		if (!CoopManager.IsMapAllowedNext(szMapName))
		{
			return;
		}

		if (data.m_pChangeLevelTimer == null)
		{
			data.m_iTicksToEnd = g_pConvarEndWaitPeriod.IntValue;
			data.m_pChangeLevelTimer = CreateTimer(CoopManager.GetChangeLevelTimerInterval(), ChangeLevelTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE);
			EmitSoundToAll(Conf.SND_CHANGELEVEL, .volume = Conf.SND_CHANGELEVEL_VOL);
		}
		if (data.m_pChangeLevelMgr.SetPlayerVote(pChangelevel, pActivator, data.m_iClientChangeLvlIndicies))
		{
			data.m_pChangeLevelMgr.UpdateStatus(data.m_iTicksToEnd);
		}
	}
	
	public static float GetChangeLevelTimerInterval()
	{
		int votes = -1; // dont cut time off for first player that finished
		for (int j = 1; j <= MaxClients; j++)
		{
			if (data.m_iClientChangeLvlIndicies[j] != -1)
				votes++;
		}
		
		// Only count alive players in survival mode
		bool bCountLivingOnly = SurvivalManager.GetMode() != SURVIVAL_OFF;
		
		int iPlayerCount = GetRealClientCount(true, bCountLivingOnly, true) - 1; // take off the 1 that we did with votes
		if (iPlayerCount < 1)
		{
			return 0.0;
		}
		if (votes < 1)
		{
			return 1.0;
		}
		
		if (iPlayerCount < votes) iPlayerCount = votes;
		return 1.0 - (1.0 / (iPlayerCount / float(votes))) * g_pConvarEndWaitFactor.FloatValue;
	}

	public static bool IsNewMapSeries()
	{
		// Return false if map was changed naturally, true if externally
		if (GetMapHistorySize() > 0)
		{
			char szMap[MAX_MAPNAME], szReason[32]; int startTime;
			GetMapHistory(0, szMap, sizeof(szMap), szReason, sizeof(szReason), startTime);
		
			if (StrEqual(szReason, SC_NORMAL_MAPCHANGE) || StrEqual(szReason, SC_SURVIVAL_RESTART_MAPCHANGE))
			{
				return false;
			}
		}
		return true;
	}
	
	public static void OnMapSeriesFinished()
	{
		char szNextMap[MAX_MAPNAME];
		g_pConvarHomeMap.GetString(szNextMap, sizeof(szNextMap));
		if (szNextMap[0] != '\0')
		{
			CoopManager.EndMap(szNextMap, SC_FINISHED_HOMEMAP_MAPCHANGE);
		}
		else if (GetNextMap(szNextMap, sizeof(szNextMap)))
		{
			CoopManager.EndMap(szNextMap, SC_FINISHED_NEXTMAP_MAPCHANGE);
		}
		else
		{
			MsgAll("Oh, you're still here? This is the end! Try to start a new map vote or contact the server owner.");
			LogMessage("Tried to change the map, but neither home map or next map is set! Set sourcecoop_homemap, edit the mapcycle or change the map manually.");
		}
	}
	
	public static void EndMap(const char[] szNextMap, const char[] szReason)
	{
		if (StrEqual(szReason, SC_NORMAL_MAPCHANGE))
		{
			EquipmentManager.ClearAll();
			if (CoopManager.IsFeatureEnabled(FT_TRANSFER_PLAYER_STATE))
			{
				EquipmentManager.StoreCurrentEquipmentAll();
			}
		}
		Call_StartForward(g_OnCoopMapEndFwd);
		Call_PushString(szNextMap);
		Call_PushString(szReason);
		Call_PushCell(StrContains(szReason, SC_MAPCHANGE_PREFIX..."-Finished") == 0);
		Action action;
		Call_Finish(action);
		if (action != Plugin_Continue)
		{
			return;
		}
		ForceChangeLevel(szNextMap, szReason);
	}

	public static void EnableFeature(SourceCoopFeature iFeatureBits)
	{
		data.m_iEnabledFeatures |= view_as<int>(iFeatureBits);
	}
	
	public static void DisableFeature(SourceCoopFeature iFeatureBits)
	{
		data.m_iEnabledFeatures &= ~view_as<int>(iFeatureBits);
	}
	
	public static bool IsFeatureEnabled(SourceCoopFeature iFeatureBits)
	{
		return CoopManager.IsCoopModeEnabled() && data.m_iEnabledFeatures & view_as<int>(iFeatureBits);
	}

	public static bool IsCoopModeEnabled()
	{
		return data.m_bIsCoopMap;
	}

	public static KeyValues LoadMapConfig(const char[] szMapName, CoopConfigLocation &src)
	{
		char szConfigPath[PLATFORM_MAX_PATH];
		if (!CoopManager.GetMapConfig(szMapName, szConfigPath, src, true))
			return null;
		
		KeyValues kv = new KeyValues(szMapName);
		kv.SetEscapeSequences(true);
		char szRoot[16];
		if (!(kv.ImportFromFile(szConfigPath) && kv.GetSectionName(szRoot, sizeof(szRoot)) && strcmp(szRoot, "config", false) == 0))
			ThrowError("Couldn't import \"%s\" into KeyValues", szConfigPath);
		
		return kv;
	}

	public static bool GetMapConfig(const char[] szMapName, char szConfigPath[PLATFORM_MAX_PATH], CoopConfigLocation &src, bool bCreateDefault)
	{
		BuildPath(Path_SM, szConfigPath, sizeof(szConfigPath), "data/srccoop/%s.edt", szMapName);
		if (!FileExists(szConfigPath))
		{
			Format(szConfigPath, sizeof(szConfigPath), "maps/%s.edt", szMapName);
			if (!FileExists(szConfigPath, true))
			{
				if (!bCreateDefault
					// attempt to create default
					|| !CoopManager.CreateDefaultConfig(szMapName, szConfigPath)
					// re-run verifying default location and populating src
					|| !CoopManager.GetMapConfig(szMapName, szConfigPath, src, false)) 
				{
					return false;
				}
			}
			else src = CCL_MAPS;
		}
		else src = CCL_SM_DATA;
		return true;
	}

	public static bool CreateDefaultConfig(const char[] szMapName, char szConfigPath[PLATFORM_MAX_PATH])
	{
		g_pConvarDefaultMapConfig.GetString(szConfigPath, sizeof(szConfigPath));
		if (szConfigPath[0] != EOS)
		{
			if (FileExists(szConfigPath))
			{
				char szDest[PLATFORM_MAX_PATH];

				g_pConvarDefaultMapConfigDest.GetString(szDest, sizeof(szDest));
				Format(szDest, sizeof(szDest), "%s/%s.edt", szDest, szMapName);

				if (FileCopy(szConfigPath, szDest))
				{
					LogMessage("Created default map config \"%s\"", szDest);
					szConfigPath = szDest;
					return true;
				}
				else
				{
					LogError("Failed to create default map config \"%s\" !", szDest);
				}
			}
			else
			{
				g_pConvarDefaultMapConfig.GetName(szConfigPath, sizeof(szConfigPath));
				LogError("Default config file is missing. Please fix \"%s\" !", szConfigPath);
			}
		}
		return false;
	}

	public static bool ConfigUsesValveFS(CoopConfigLocation ccl)
	{
		return (ccl == CCL_MAPS);
	}
	
}

public Action StartTimerCallback(Handle pTimer)
{
	if (!--data.m_iSecondsToStart)
	{
		data.m_pStartTimer = null;
		CoopManager.Start();
		return Plugin_Stop;
	}
	
	if (data.m_iSecondsToStart == 3)
	{
		EmitSoundToAll(Conf.SND_3, .volume = Conf.SND_TICK_VOL);
	}
	else if (data.m_iSecondsToStart == 2)
	{
		EmitSoundToAll(Conf.SND_2, .volume = Conf.SND_TICK_VOL);
	}
	else if (data.m_iSecondsToStart == 1)
	{
		EmitSoundToAll(Conf.SND_1, .volume = Conf.SND_TICK_VOL);
	}
	else
	{
		EmitSoundToAll(Conf.SND_TICK, .volume = Conf.SND_TICK_VOL);
	}

	#if defined INTRO_TIMER_HUDTEXT
	
	SetHudTextParams(-1.0, 0.05, 1.0, 200, 200, 200, 255, 0, 0.5, 0.1, 0.1);
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			ShowHudText(i, 2, "%d", data.m_iSecondsToStart);
		}
	}

	#endif
	
	return Plugin_Continue;
}

public Action ChangeLevelTimerCallback(Handle pTimer)
{
	if (!data.m_iTicksToEnd--)
	{
		CChangelevel pChangelevel = data.m_pChangeLevelMgr.GetWinner();
		char szNextMap[MAX_MAPNAME];
		pChangelevel.GetMapName(szNextMap, sizeof(szNextMap));
		pChangelevel.FireOutput("OnChangeLevel");
		CoopManager.EndMap(szNextMap, SC_NORMAL_MAPCHANGE);
	}
	else
	{
		EmitSoundToAll(Conf.SND_CHANGELEVEL_TICK, .volume = Conf.SND_CHANGELEVEL_TICK_VOL);
		data.m_pChangeLevelMgr.UpdateStatus(data.m_iTicksToEnd);
		data.m_pChangeLevelTimer = CreateTimer(CoopManager.GetChangeLevelTimerInterval(), ChangeLevelTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE);
	}
	return Plugin_Handled;
}

// end shortcut
#undef data