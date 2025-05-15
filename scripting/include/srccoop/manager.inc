#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Coop manager
//------------------------------------------------------

enum struct FireOutputData
{
	char m_szName[32];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
}

enum struct CoopManagerData
{
	KeyValues kv;
	ChangeLevelManager m_pChangeLevelMgr;
	int m_iEnabledFeatures;
	bool m_bIsCoopMap;
	bool m_bStarted;
	char m_szChapterTitle[64];
	bool m_bMapHasDelayedOutput;
	int m_iSpawnedPlayerCountLastMap;
	int m_iSpawnedPlayerCount;
	int m_iSecondsToStart;
	float m_flStartTime;
	Handle m_hStartTimer;
	ArrayList m_pDelayedOutputs;
	int m_iClientChangeLvlIndicies[MAXPLAYERS + 1];
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
		data.m_hStartTimer = null;
		data.m_pChangeLevelTimer = null;
		data.m_bStarted = false;
		data.m_szChapterTitle = "";
		data.m_iSpawnedPlayerCountLastMap = data.m_iSpawnedPlayerCount;
		data.m_iSpawnedPlayerCount = 0;
		data.m_iSecondsToStart = cellmax;
		data.m_bMapHasDelayedOutput = false;
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
				IServerGameDLL.Get().GameShutdown();
				EquipmentManager.ClearAll();
			}
			g_pLevelLump.ParseMapEntities(szMapEntities);
			g_pLevelLump.ParseConfigFile(data.kv);
			g_pLevelLump.ToString(szMapEntities);
			return true;
		}
		return false;
	}

	public static void CheckAndAssignChapterTitle()
	{
		// We need to get the chapter title from `CWorld` when a `env_message` is created
		// as the chapter title gets unassigned after the message is created.
		// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/world.cpp#L681
		char szChapterTitle[64];
		CWorld().GetChapterTitle(szChapterTitle, sizeof(szChapterTitle));
		strcopy(data.m_szChapterTitle, sizeof(data.m_szChapterTitle), szChapterTitle);
	}

	public static void EntitySpawnPost(CBaseEntity pEntity)
	{
		// find and hook output hooks for entity
		if (!data.m_bStarted)
		{
			ArrayList pOutputHookList = g_pLevelLump.GetOutputHooksForEntity(pEntity);
			if (pOutputHookList.Length)
			{
				if (pEntity.IsClassname("logic_auto"))
				{
					// do not let it get killed, so the output can fire later
					view_as<CLogicAuto>(pEntity).m_spawnflags &= ~SF_AUTO_FIREONCE;
				}
				for (int i = 0; i < pOutputHookList.Length; i++)
				{
					CEntityOutputHook pOutputHook; pOutputHookList.GetArray(i, pOutputHook);
					HookSingleEntityOutput(pEntity.entindex, pOutputHook.m_szOutputName, Callback_OutputCallbackForDelay);
				}
				data.m_bMapHasDelayedOutput = true;
			}
			pOutputHookList.Close();
		}
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

		CoopManager.SetIntroTimeOnHud(INT_MAX);

		data.kv.Rewind();
		CCoopSpawnSystem.ParseConfigFile(data.kv);
		
		Call_StartForward(g_CoopMapConfigLoadedFwd);
		Call_PushCell(data.kv);
		Call_PushCell(data.m_mapConfigLoc);
		Call_Finish();
		delete data.kv;
	}

	public static void SetIntroTimeOnHud(const int iSecondsToWait)
	{
		bool bShouldShow = iSecondsToWait > -1;

		if (bShouldShow)
		{
			#if defined INTRO_TIMER_HUDTEXT
			SetHudTextParams(-1.0, 0.05, 1.0, 200, 200, 200, 255, 0, 0.5, 0.1, 0.1);
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i))
				{
					ShowHudText(i, 2, "%d", iSecondsToWait);
				}
			}
			#endif
			
			EmitSoundToAll(Conf.SND_TICK, .volume = Conf.SND_TICK_VOL);

			if (iSecondsToWait == 3)
			{
				EmitSoundToAll(Conf.SND_3, .volume = Conf.SND_TICK_VOL);
			}
			else if (iSecondsToWait == 2)
			{
				EmitSoundToAll(Conf.SND_2, .volume = Conf.SND_TICK_VOL);
			}
			else if (iSecondsToWait == 1)
			{
				EmitSoundToAll(Conf.SND_1, .volume = Conf.SND_TICK_VOL);
			}

			#if defined SRCCOOP_BLACKMESA
			CBM_MP_GameRules.SetCurrentState(STATE_WARMUP);
			CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + iSecondsToWait + 1.0);
			#endif
		}
		else if (!CoopManager.IsFeatureEnabled(FT_ENABLE_TIMELIMIT))
		{
			#if defined SRCCOOP_BLACKMESA
			CBM_MP_GameRules.SetCurrentState(STATE_ROUND);
			CBM_MP_GameRules.SetStateEndTime(STATE_ROUND, view_as<float>(INT_MAX));
			#endif
		}
	}

	public static void StartIntroTimer(const int iSecondsToStart)
	{
		delete data.m_hStartTimer;
		if (iSecondsToStart > 0)
		{
			data.m_iSecondsToStart = iSecondsToStart;
			CoopManager.SetIntroTimeOnHud(data.m_iSecondsToStart);
			data.m_hStartTimer = CreateTimer(1.0, Timer_DecrementUntilMapStart, _, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT);
		}
		else
		{
			data.m_iSecondsToStart = 0;
			Timer_DecrementUntilMapStart(null);
		}
	}
	
	public static void OnPlayerRunCmd(CBasePlayer pPlayer)
	{
		if (!data.m_bStarted)
		{
			CBaseCombatWeapon pWeapon = pPlayer.GetActiveWeapon();
			if (pWeapon != NULL_CBASEENTITY)
			{
				pWeapon.DelayNextAttack(0.25);
			}
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
			CCoopSpawnSystem.TeleportPlayerOnSpawn(pPlayer);
		if (bDefaultEquipment)
			CCoopSpawnSystem.SpawnPlayerEquipment(pPlayer);
		g_pSpawnOptions.Reset();

		ItemInstancingManager.OnPlayerSpawned(pPlayer);

		data.m_iSpawnedPlayerCount = GetRealClientCount(true);
		
		if (!data.m_bStarted)
		{
			if (g_pLevelLump.m_iIntroType == INTRO_FADE)
			{
				pPlayer.m_fFlags |= (FL_ATCONTROLS | FL_FROZEN | FL_NOTARGET);	// Freeze controls and angles.
				pPlayer.ScreenFade(0, RGBA(0, 0, 0, 255), FFADE_STAYOUT);
			}
			else if (g_pLevelLump.m_iIntroType == INTRO_FREEZE)
			{
				pPlayer.m_fFlags |= (FL_ATCONTROLS | FL_NOTARGET);				// Freeze controls.
				pPlayer.ScreenFade(Conf.FROZEN_FADE_DUR_IN, Conf.FROZEN_FADE_COLOR, FFADE_OUT | FFADE_STAYOUT);
			}

			if (g_pLevelLump.m_iIntroType == INTRO_NONE && !data.m_bMapHasDelayedOutput)
			{
				CoopManager.StartIntroTimer(0);
				return;
			}
			
			switch (g_pConvarStartWaitMode.IntValue)
			{
				case 0: // no skip
				{
					if (!data.m_hStartTimer)
					{
						CoopManager.StartIntroTimer(g_pConvarStartWaitPeriod.IntValue);
					}
				}

				case 1: // skip when matched in-game vs connected count
				{
					if (GetRealClientCount(false) - g_iPlayerCount <= 0 && data.m_iSecondsToStart > 3)
					{
						CoopManager.StartIntroTimer(3);
					}
					else if (!data.m_hStartTimer)
					{
						CoopManager.StartIntroTimer(g_pConvarStartWaitPeriod.IntValue);
					}
				}

				case 2: // skip when matched last map's player count
				{
					if ((data.m_iSpawnedPlayerCountLastMap - data.m_iSpawnedPlayerCount) <= 0 && data.m_iSecondsToStart > 3)
					{
						CoopManager.StartIntroTimer(3);
					}
					else if (!data.m_hStartTimer)
					{
						CoopManager.StartIntroTimer(g_pConvarStartWaitPeriod.IntValue);
					}
				}
			}
		}
	}
	
	public static void OnClientDisconnectPost(const int iClient)
	{
		if (!data.m_bStarted && !g_iPlayerCount)
		{
			delete data.m_hStartTimer;
			CoopManager.SetIntroTimeOnHud(INT_MAX);
		}
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
			data.m_pChangeLevelTimer = CreateTimer(CoopManager.GetChangeLevelTimerInterval(), Timer_DecrementChangeLevel, _, TIMER_FLAG_NO_MAPCHANGE);
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

	/**
	 * Finds an index of the team to enforce.
	 * Must be called after teams had been created.
	 * Returns -1 on failure / if teams are not to be enforced.
	 */
	public static int GetEnforcedTeam()
	{
		char szTeam[MAX_TEAMNAME];
		g_pConvarCoopTeam.GetString(szTeam, sizeof(szTeam));

		int iTeam;
		if (!StringToIntEx(szTeam, iTeam))
		{
			iTeam = FindTeamByName(szTeam);
		}

		return (iTeam > TEAM_SPECTATOR) ? iTeam : TEAM_INVALID;
	}
}

// Runs every 1 second.
// 
public Action Timer_DecrementUntilMapStart(Handle hTimer)
{
	--data.m_iSecondsToStart;
	CoopManager.SetIntroTimeOnHud(data.m_iSecondsToStart);

	if (data.m_iSecondsToStart < 0)
	{
		MsgSrv("%t", "map start", g_szMapName);

		data.m_bStarted = true;
		data.m_flStartTime = GetGameTime();

		if (strlen(data.m_szChapterTitle) > 0)
		{
			CreateTimer(1.0, Timer_ShowChapterTitle, _, TIMER_FLAG_NO_MAPCHANGE);
		}

		#if defined SRCCOOP_BLACKMESA
		if (g_pConvarCleanHud.BoolValue)
		{
			SetClientGamerules("CBM_MP_Coop_GameRules"); // hides the top hud
			if (CMultiplayRules.IsTeamplay())
			{
				hkThink.HookEntity(Hook_Post, GetPlayerResourceEntity(), Hook_PlayerResourceFixupCoopTeam);
			}
		}
		#endif

		if (g_pLevelLump.m_iIntroType == INTRO_FADE || g_pLevelLump.m_iIntroType == INTRO_FREEZE)
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i) && !IsFakeClient(i))
				{
					CBasePlayer pPlayer = CBasePlayer(i);
					// TODO: Look into this.
					// `FL_FROZEN` is being removed even without the below line in Black Mesa (i doubt this affects other games).
					// `CBM_MP_GameRules.SetStateEndTime` might be the culprit and might have side effects for other behaviors too.
					pPlayer.m_fFlags &= ~(FL_ATCONTROLS | FL_FROZEN | FL_NOTARGET);
					if (g_pLevelLump.m_iIntroType == INTRO_FREEZE)
					{
						pPlayer.ScreenFade(Conf.FROZEN_FADE_DUR_OUT, Conf.FROZEN_FADE_COLOR, FFADE_PURGE | FFADE_IN, 1);
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
				FireEntityOutput(pFireOutputData.m_pCaller.entindex, pFireOutputData.m_szName, pFireOutputData.m_pActivator.entindex, pFireOutputData.m_flDelay);

				if (pFireOutputData.m_pCaller.IsClassname("trigger_once"))
				{
					pFireOutputData.m_pCaller.SetNextThinkTick(GetGameTickCount() + 1); // allow to kill itself
				}
			}
		}
		
		// Notify plugins via forward
		Call_StartForward(g_CoopMapStartFwd);
		Call_Finish();

		data.m_hStartTimer = null;
		return Plugin_Stop;
	}

	return Plugin_Continue;
}

public void Timer_DecrementChangeLevel(Handle pTimer)
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
		data.m_pChangeLevelTimer = CreateTimer(CoopManager.GetChangeLevelTimerInterval(), Timer_DecrementChangeLevel, _, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Action Callback_OutputCallbackForDelay(const char[] szOutput, int iCaller, int iActivator, float flDelay)
{
	if (data.m_bStarted)
		return Plugin_Continue;
	
	FireOutputData pFireOutputData;
	strcopy(pFireOutputData.m_szName, sizeof(pFireOutputData.m_szName), szOutput);
	pFireOutputData.m_pCaller = CBaseEntity(iCaller);
	pFireOutputData.m_pActivator = CBaseEntity(iActivator);
	pFireOutputData.m_flDelay = flDelay;
	CoopManager.AddDelayedOutput(pFireOutputData);
	
	if (pFireOutputData.m_pCaller.IsValid())
	{
		// stop from deleting itself
		if (pFireOutputData.m_pCaller.IsClassname("trigger_once"))
		{
			RequestFrame(RequestStopThink, pFireOutputData.m_pCaller);
		}
		else if (pFireOutputData.m_pCaller.IsClassname("logic_relay"))
		{
			CLogicRelay pCaller = view_as<CLogicRelay>(pFireOutputData.m_pCaller);
			if (pCaller.m_spawnflags & SF_REMOVE_ON_FIRE)
			{
				pCaller.m_spawnflags &= ~SF_REMOVE_ON_FIRE;
				UnhookSingleEntityOutput(iCaller, szOutput, Callback_OutputCallbackForDelay);
				HookSingleEntityOutput(iCaller, szOutput, Callback_DeleteEntOnDelayedOutputFire);
			}
		}
	}

	return Plugin_Stop;
}

public Action Callback_DeleteEntOnDelayedOutputFire(const char[] szOutput, int iCaller, int iActivator, float flDelay)
{
	if (data.m_bStarted)
	{
		RemoveEntity(iCaller);
		return Plugin_Continue;
	}
	else
	{
		return Plugin_Stop;
	}
}

public void Timer_ShowChapterTitle(Handle hTimer)
{
	CMessage pMessage = CMessage.Create();
	if (pMessage != NULL_CBASEENTITY)
	{
		pMessage.SetMessage(data.m_szChapterTitle);
		pMessage.m_spawnflags |= SF_MESSAGE_ONCE | SF_MESSAGE_ALL;
		pMessage.ShowMessage();
	}
}

static void RequestStopThink(CBaseEntity pEntity)
{
	if (pEntity.IsValid())
	{
		pEntity.SetNextThinkTick(0);
	}
}

/**
 * When we set the CBM_MP_Coop_GameRules on client, it expects to display a single team (0).
 * For teamplay mode, we need to fixup the sent team value to the expected team.
 */
public MRESReturn Hook_PlayerResourceFixupCoopTeam(int _this)
{
	CPlayerResource pPlayerResource = CPlayerResource(_this);
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer.GetTeam() > TEAM_SPECTATOR)
			{
				pPlayerResource.SetTeam(pPlayer, 0);
			}
		}
	}
	return MRES_Ignored;
}

// end shortcut
#undef data