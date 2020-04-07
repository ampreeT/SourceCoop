enum struct FireOutputData
{
	char m_szName[MAX_KEY];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
}

enum struct CoopManager
{
	bool m_bIsCoopMap;
	bool m_bStarted;
	bool m_bPlayerSpawned;
	int m_iSecondsToStart;
	Handle m_pStartTimer;
	Array_t m_pDelayedOutputs;
	
	void Initialize()
	{
		this.m_pDelayedOutputs = new Array_t(CreateArray(sizeof(FireOutputData)));
	}
	
	void OnMapStart()
	{
		this.m_pStartTimer = null;
		this.m_bStarted = false;
		this.m_bPlayerSpawned = false;
		
		if (this.m_bIsCoopMap) // cvars are not configured yet
		{
			CBM_MP_GameRules.SetStateIsInIntermission(STATE_WARMUP, false); // false will let players spawn, true holds them waiting till the state changes (and also plays tick sounds last 3 seconds)
			CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7);
			g_SpawnSystem.ParseConfigFile(g_szMapName);
		}
		else
		{
			g_SpawnSystem.Clear();
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
			g_SpawnSystem.SpawnPlayer(pPlayer);
			
			if(pPlayer.IsAlive())
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
				delete this.m_pStartTimer;
				this.Start();
			}
			else if(!this.IsInCountDownPeriod())
			{
				this.m_iSecondsToStart = g_pConvarWaitPeriod.IntValue;
				CBM_MP_GameRules.SetStateEndTime(STATE_WARMUP, GetGameTime() + this.m_iSecondsToStart + 1);
				this.m_pStartTimer = CreateTimer(1.0, StartTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
			}
		}
	}
	
	bool IsInCountDownPeriod()
	{
		return this.m_pStartTimer != null;
	}
	
	bool IsAllowedToStart()
	{
		return this.m_pDelayedOutputs.Length == 0 || (GetRealClientCount(false) - GetRealClientCount(true)) == 0;
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
}

CoopManager g_pCoopManager;

#pragma dynamic 2097152 
public Action OnLevelInit(const char[] szMapName, char szMapEntities[2097152])		// you probably need to incease SlowScriptTimeout in core.cfg
{
	strcopy(g_szMapName, sizeof(g_szMapName), szMapName);
	g_pCoopManager.m_bIsCoopMap = g_pLevelLump.IsCoopMap(szMapName);
	g_pCoopManager.m_pDelayedOutputs.Clear();
	g_pLevelLump.Clear();
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		if(g_pCoopManager.ShouldResetGlobalStates())
			ResetGlobalStates();
		
		g_pLevelLump.ParseMapEntities(szMapEntities);
		g_pLevelLump.ParseConfigFile(szMapName);
		g_pLevelLump.ToString(szMapEntities);
	}
	
	return Plugin_Changed;
}

public Action StartTimerCallback(Handle pTimer)
{
	if(!g_pCoopManager.m_iSecondsToStart--)
	{
		g_pCoopManager.m_pStartTimer = null;
		g_pCoopManager.Start();
		return Plugin_Stop;
	}
	return Plugin_Continue;
}