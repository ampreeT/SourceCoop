#if defined _srccoop_manager_included
 #endinput
#endif
#define _srccoop_manager_included

#define FROZEN_FADE_R 15
#define FROZEN_FADE_G 15
#define FROZEN_FADE_B 15
#define FROZEN_FADE_A 245
#define FROZEN_FADE_DUR_IN 700
#define FROZEN_FADE_DUR_OUT 1500
#define SND_TICK "bms_objects/clickbeep/click1.wav"


enum struct ChangeLevelVoteLump
{
	CChangelevel m_pChangeLevel;
	int votes;
}

methodmap ChangeLevelManager < Array_t
{
	public ChangeLevelManager()
	{
		return view_as<ChangeLevelManager>(CreateArray(sizeof(ChangeLevelVoteLump)));
	}

	public void UpdateVotes(int[] iClientVoteIndicies)
	{
		ChangeLevelVoteLump pChangeLvlLump;
		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pChangeLvlLump);
			pChangeLvlLump.votes = 0;
			for(int j = 1; j <= MaxClients; j++)
			{
				if(iClientVoteIndicies[j] == i) pChangeLvlLump.votes++;
			}
			this.SetArray(i, pChangeLvlLump);
		}
	}

	public ChangeLevelManager Sorted()
	{
		ChangeLevelManager pClone = view_as<ChangeLevelManager>(this.Clone());
		pClone.SortCustom(view_as<SortFuncADTArray>(ChangeLvlLumpSortFunc));
		return pClone;
	}

	public void UpdateStatus(int iSecondsLeft)
	{
		char szMapName[MAX_MAPNAME], szDisplay[128];
		Panel panel = new Panel();
		
		Format(szDisplay, sizeof(szDisplay), "Changing map.. (%d)", iSecondsLeft);
		panel.SetTitle(szDisplay);

		ChangeLevelManager pSorted = this.Sorted();
		ChangeLevelVoteLump pChangeLvlLump;
		
		for (int i = 0; i < pSorted.Length; i++)
		{
			pSorted.GetArray(i, pChangeLvlLump);
			if(pChangeLvlLump.m_pChangeLevel.IsValid())
			{
		 		pChangeLvlLump.m_pChangeLevel.GetMapName(szMapName, sizeof(szMapName));
				Format(szDisplay, sizeof(szDisplay), "%s (%d players)", szMapName, pChangeLvlLump.votes);
				panel.DrawItem(szDisplay);
			}
		}
		
		for(int i = 1; i <= MaxClients; i++)
		{
			if(IsClientInGame(i))
				panel.Send(i, ChangeLevelStatusPanelCallback, 1);
		}
		
		delete panel;
		delete pSorted;
	}

	public bool SetPlayerVote(CChangelevel pChangelevel, CBlackMesaPlayer pPlayer, int[] iClientVoteIndicies)
	{
		int iPlayer = pPlayer.GetEntIndex();
		ChangeLevelVoteLump pChangeLvlLump;
		for (int i = 0; i < this.Length; i++)
		{
			if(this.GetArray(i, pChangeLvlLump) && pChangeLvlLump.m_pChangeLevel == pChangelevel)
			{
				if(iClientVoteIndicies[iPlayer] != i)
				{
					iClientVoteIndicies[iPlayer] = i;
					this.UpdateVotes(iClientVoteIndicies);
					return true;
				}
				return false;
			}
		}
		pChangeLvlLump.m_pChangeLevel = pChangelevel;
		iClientVoteIndicies[iPlayer] = this.PushArray(pChangeLvlLump);
		this.UpdateVotes(iClientVoteIndicies);
		return true;
	}
	
	public void End()
	{
		ChangeLevelManager pSorted = this.Sorted();
		ChangeLevelVoteLump pChangeLvlLump;
		pSorted.GetArray(0, pChangeLvlLump);
		pChangeLvlLump.m_pChangeLevel.ChangeLevelNow();
		delete pSorted;
	}
}

public int ChangeLvlLumpSortFunc(int index1, int index2, ArrayList array, Handle hndl)
{
	ChangeLevelVoteLump item1, item2; array.GetArray(index1, item1); array.GetArray(index2, item2);
	if(item1.votes < item2.votes)
		return 1;
	if(item2.votes < item1.votes)
		return -1;
	return 0;
}

public int ChangeLevelStatusPanelCallback(Menu menu, MenuAction action, int param1, int param2) {}

enum struct CoopManager
{
	bool m_bIsCoopMap;
	bool m_bStarted;
	bool m_bPlayerSpawned;
	int m_iSecondsToStart;
	bool m_bGetReadyTimer;
	Handle m_pStartTimer;
	Array_t m_pDelayedOutputs;
	ChangeLevelManager m_pChangeLevelMgr;
	int m_iClientChangeLvlIndicies[MAXPLAYERS+1];
	Handle m_pChangeLevelTimer;
	int m_iTicksToEnd;

	void Initialize()
	{
		this.m_pDelayedOutputs = new Array_t(CreateArray(sizeof(FireOutputData)));
		this.m_pChangeLevelMgr = new ChangeLevelManager();
	}

	void OnLevelInit(const char[] szMapName, char szMapEntities[2097152])
	{
		this.m_pDelayedOutputs.Clear();
		this.m_pStartTimer = null;
		this.m_pChangeLevelTimer = null;
		this.m_bGetReadyTimer = false;
		this.m_bStarted = false;
		this.m_bPlayerSpawned = false;
		this.m_pChangeLevelMgr.Clear();
		for(int i = 0; i <= MaxClients; i++)
		{
			this.m_iClientChangeLvlIndicies[i] = -1;
		}
		g_pLevelLump.Clear();
		
		KeyValues kv = this.LoadMapConfig(szMapName);
		this.m_bIsCoopMap = kv != null;
		
		if(this.m_bIsCoopMap)
		{
			if(this.ShouldResetGlobalStates())
			{
				g_ServerGameDLL.GameShutdown();
			}
			g_pLevelLump.ParseMapEntities(szMapEntities);
			g_pLevelLump.ParseConfigFile(kv);
			g_pLevelLump.ToString(szMapEntities);
			delete kv;
		}
	}

	void OnMapStart()
	{
		PrecacheSound(SND_TICK);

		g_SpawnSystem.Clear();
		if (this.m_bIsCoopMap) // cvars are not configured yet
		{
			CBM_MP_GameRules.SetStateIsInIntermission(STATE_WARMUP, false);
			CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7);
			KeyValues kv = this.LoadMapConfig(g_szMapName);
			if(kv != null)
			{
				g_SpawnSystem.ParseConfigFile(kv);
				delete kv;
			}
		}
	}

	void OnClientPutInServer(int client)
	{
		if (this.IsCoopModeEnabled())
		{
			this.CheckStart();
		}
	}

	void OnPlayerSpawned(CBlackMesaPlayer pPlayer)
	{
		if(this.IsCoopModeEnabled())
		{
			bool bActuallySpawned = pPlayer.IsAlive();
			g_SpawnSystem.SpawnPlayer(pPlayer, bActuallySpawned);

			if(bActuallySpawned)
			{
				this.m_bPlayerSpawned = true;
				if(!this.m_bStarted)
				{
					if(g_pLevelLump.m_iIntroType == INTRO_FADE)
					{
						Client_ScreenFade(pPlayer.GetEntIndex(), 0, FFADE_STAYOUT);
						pPlayer.SetFlags(pPlayer.GetFlags() | FL_ATCONTROLS);
					}
					else if (g_pLevelLump.m_iIntroType == INTRO_FREEZE)
					{
						pPlayer.SetFlags(pPlayer.GetFlags() | FL_ATCONTROLS);
						Client_ScreenFade(pPlayer.GetEntIndex(), FROZEN_FADE_DUR_IN, FFADE_OUT|FFADE_STAYOUT, _, FROZEN_FADE_R, FROZEN_FADE_G, FROZEN_FADE_B, FROZEN_FADE_A);
					}
					this.CheckStart();
				}
			}
		}
	}

	void CheckStart()
	{
		if(!this.m_bStarted && this.m_bPlayerSpawned)
		{
			if(this.IsAllowedToStart())
			{
				if(this.IsInCountDownPeriod())
				{
					if(!this.m_bGetReadyTimer)
					{
						delete this.m_pStartTimer;
						this.StartCountdown(true);
					}
				}
				else
				{
					this.Start();
				}
			}
			else if(!this.IsInCountDownPeriod())
			{
				this.StartCountdown(false);
			}
		}
	}

	void StartCountdown(bool bGetReadyTimer)
	{
		this.m_bGetReadyTimer = bGetReadyTimer;
		this.m_iSecondsToStart = bGetReadyTimer? 3 : g_pConvarWaitPeriod.IntValue;
		CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + this.m_iSecondsToStart + 1.1);
		this.m_pStartTimer = CreateTimer(1.0, StartTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
	}

	bool IsInCountDownPeriod()
	{
		return this.m_pStartTimer != null;
	}

	bool IsAllowedToStart()
	{
		return (g_pLevelLump.m_iIntroType == INTRO_NONE && this.m_pDelayedOutputs.Length == 0) || (GetRealClientCount(false) - GetRealClientCount(true)) == 0;
	}

	void Start()
	{
		CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7); // STATE_WARMUP is what the server still counts with!

		if(!GetRealClientCount(true)) {
			return;
		}

		ConVar timelimit = FindConVar("mp_timelimit");
		CBM_MP_GameRules.SetCurrentState(STATE_ROUND); // doesnt actually restart, just switches the client hud
		CBM_MP_GameRules.SetStateEndTime(STATE_ROUND, GetGameTime() + timelimit.FloatValue); // this is what is displayed on client, nothing happens when it runs out, we may have to handle that
		delete timelimit;

		this.m_bStarted = true;
		PrintToServer("Co-op map %s has begun", g_szMapName);

		if(g_pLevelLump.m_iIntroType == INTRO_FADE || g_pLevelLump.m_iIntroType == INTRO_FREEZE)
		{
			for(int i = 1; i <= MaxClients; i++)
			{
				if(IsClientInGame(i))
				{
					CBlackMesaPlayer pClient = CBlackMesaPlayer(i);
					pClient.SetFlags(pClient.GetFlags() & ~FL_ATCONTROLS);
					if(g_pLevelLump.m_iIntroType == INTRO_FREEZE)
					{
						Client_ScreenFade(pClient.GetEntIndex(), FROZEN_FADE_DUR_OUT, FFADE_PURGE|FFADE_IN, 1, FROZEN_FADE_R, FROZEN_FADE_G, FROZEN_FADE_B, FROZEN_FADE_A);
					}
				}
			}
		}

		// Fire delayed outputs
		for (int i = 0; i < this.m_pDelayedOutputs.Length; i++)
		{
			FireOutputData pFireOutputData; this.m_pDelayedOutputs.GetArray(i, pFireOutputData);
			if(pFireOutputData.m_pCaller.IsValid())
			{
				FireEntityOutput(pFireOutputData.m_pCaller.GetEntIndex(), pFireOutputData.m_szName, pFireOutputData.m_pActivator.GetEntIndex(), pFireOutputData.m_flDelay);

				if(pFireOutputData.m_pCaller.IsClassname("trigger_once"))
				{
					pFireOutputData.m_pCaller.SetNextThinkTick(GetGameTickCount()+1); // allow to kill itself
				}
			}
		}
	}

	void OnTouchChangeLevel(CChangelevel pChangelevel, CBlackMesaPlayer pActivator)
	{
		if(this.m_pChangeLevelTimer == null)
		{
			this.m_iTicksToEnd = g_pConvarEndWaitPeriod.IntValue;
			this.m_pChangeLevelTimer = CreateTimer(this.GetChangeLevelTimerInterval(), ChangeLevelTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE);
		}
		if(this.m_pChangeLevelMgr.SetPlayerVote(pChangelevel, pActivator, this.m_iClientChangeLvlIndicies))
		{
			this.m_pChangeLevelMgr.UpdateStatus(this.m_iTicksToEnd);
		}
	}
	
	float GetChangeLevelTimerInterval()
	{
		int votes = -1; // dont cut time off for first player that finished
		for(int j = 1; j <= MaxClients; j++)
		{
			if(this.m_iClientChangeLvlIndicies[j] != -1) votes++;
		}
		
		int iPlayerCount = GetRealClientCount(true, false, true) - 1; // take off the 1 that we did with votes
		if(iPlayerCount < 1)
		{
			return 0.0;
		}
		if(votes < 1)
		{
			return 1.0;
		}
		
		if(iPlayerCount < votes) iPlayerCount = votes;
		return 1.0 - (1.0 / (iPlayerCount / float(votes))) * g_pConvarEndWaitFactor.FloatValue;
	}

	void AddDelayedOutput(FireOutputData pFireOutputData)
	{
		this.m_pDelayedOutputs.PushArray(pFireOutputData);
	}

	bool ShouldResetGlobalStates()
	{
		// Return false if map was changed naturally, true if externally
		if(GetMapHistorySize() > 0)
		{
			char map[MAX_MAPNAME], reason[32]; int startTime;
			GetMapHistory(0, map, sizeof(map), reason, sizeof(reason), startTime);
			if(strcmp(reason, SM_NORMAL_MAPCHANGE) == 0)
			{
				return false;
			}
		}
		return true;
	}

	/**
	* BugPatching = crashfixes or enhancements that don't affect MP gameplay
	* Will currently return true regardless of map or mode
	*/
	bool IsBugPatchingEnabled()
	{
		return true;
	}

	/**
	* Features = changes that reenable SP functionality which affects MP gameplay
	* Will currently return false on non-coop maps
	*/
	bool IsFeaturePatchingEnabled()
	{
		return this.IsCoopModeEnabled();
	}

	bool IsCoopModeEnabled()
	{
		return this.m_bIsCoopMap && this.IsPluginEnabled();
	}

	bool IsPluginEnabled()
	{
		return g_pConvarCoopEnabled.BoolValue;
	}
	
	KeyValues LoadMapConfig(const char[] szMapName)
	{
		char szConfigPath[MAX_FORMAT];
		BuildPath(Path_SM, szConfigPath, sizeof(szConfigPath), "data/srccoop/%s.edt", szMapName);
		if (!FileExists(szConfigPath))
		{
			Format(szConfigPath, sizeof(szConfigPath), "maps/%s.edt", szMapName);
			if(!FileExists(szConfigPath, true))
			{
				return null;
			}
		}
		
		KeyValues kv = new KeyValues(szMapName);
		char szRoot[16];
		if (!(kv.ImportFromFile(szConfigPath) && kv.GetSectionName(szRoot, sizeof(szRoot)) && strcmp(szRoot, "config", false) == 0))
			ThrowError("Couldn't import %s into KeyValues", szMapName);
		return kv;
	}
}

CoopManager g_pCoopManager;

public Action StartTimerCallback(Handle pTimer)
{
	if(!g_pCoopManager.m_iSecondsToStart--)
	{
		if(g_pCoopManager.m_bGetReadyTimer)
		{
			g_pCoopManager.m_pStartTimer = null;
			g_pCoopManager.Start();
		}
		else
		{
			g_pCoopManager.StartCountdown(true);
		}
		return Plugin_Stop;
	}
	EmitSoundToAll(SND_TICK);
	return Plugin_Continue;
}

public Action ChangeLevelTimerCallback(Handle pTimer)
{
	if(!g_pCoopManager.m_iTicksToEnd--)
	{
		g_pCoopManager.m_pChangeLevelMgr.End();
	}
	else
	{
		g_pCoopManager.m_pChangeLevelMgr.UpdateStatus(g_pCoopManager.m_iTicksToEnd);
		g_pCoopManager.m_pChangeLevelTimer = CreateTimer(g_pCoopManager.GetChangeLevelTimerInterval(), ChangeLevelTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE);
	}
}