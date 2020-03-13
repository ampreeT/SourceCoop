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
	Handle m_pStartTimer;
	Array_t m_pDelayedOutputs;
	
	void Initialize()
	{
		this.m_pDelayedOutputs = Array_t(CreateArray(sizeof(FireOutputData)));
	}
	
	void OnMapStart()
	{
		this.m_pStartTimer = null;
		this.m_bStarted = false;
		this.m_bPlayerSpawned = false;
		
		if (this.IsCoopModeEnabled())
		{
			AddWarmupTime(60 * 60 * 24 * 7);
			g_SpawnSystem.ParseConfigFile(g_szMapName);
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
				this.CheckStart();
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
				this.m_pStartTimer = CreateTimer(g_pConvarWaitPeriod.FloatValue, StartTimerCallback, _, TIMER_FLAG_NO_MAPCHANGE);
			}
		}
	}
	
	bool IsInCountDownPeriod()
	{
		return this.m_pStartTimer != null;
	}
	
	bool IsAllowedToStart()
	{
		return (GetRealClientCount(false) - GetRealClientCount(true)) == 0;
	}
	
	void Start()
	{
		this.m_pStartTimer = null;
		if(!GetRealClientCount(true)) {
			return;
		}
		
		this.m_bStarted = true;
		PrintToServer("Co-op map %s has begun", g_szMapName);
		
		// Fire delayed outputs
		for (int i = 0; i < this.m_pDelayedOutputs.Size(); i++)
		{
			FireOutputData pFireOutputData; this.m_pDelayedOutputs.GetArray(i, pFireOutputData);
			FireEntityOutput(pFireOutputData.m_pCaller.GetEntIndex(), pFireOutputData.m_szName, pFireOutputData.m_pActivator.GetEntIndex(), pFireOutputData.m_flDelay);
		}
	}
	
	void AddDelayedOutput(FireOutputData pFireOutputData)
	{
		this.m_pDelayedOutputs.PushArray(pFireOutputData);
	}
	
	// BugPatching = crashfixes or enhancements that don't affect MP gameplay
	// Will currently return true regardless of map or mode
	bool IsBugPatchingEnabled()
	{
		return true;
	}
	
	// Features = changes that reenable SP functionality which affects MP gameplay
	// Will currently return false on non-coop maps
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
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		g_pLevelLump.ParseMapEntities(szMapEntities);
		g_pLevelLump.ParseConfigFile(szMapName);
		g_pLevelLump.ToString(szMapEntities);
	}
	
	return Plugin_Changed;
}

public Action StartTimerCallback(Handle timer)
{
	g_pCoopManager.Start();
}

stock int GetRealClientCount(bool bInGameOnly, bool alive = false)
{
	int clients;
	for(int i = 1; i <= MaxClients; i++)
	{
		if( ((bInGameOnly)? IsClientInGame(i) : IsClientConnected(i)) && !IsFakeClient(i) && (!alive || IsPlayerAlive(i))) {
 			clients++;
 		}
 	}
	return clients;
}

stock bool AddWarmupTime(const int iSeconds)
{
	CBaseEntity pEntity = CreateByClassname("mp_round_time");
	if (pEntity.IsValid() && pEntity.Spawn())
	{
		pEntity.AcceptInputInt("AddWarmupTime", iSeconds);
		pEntity.Kill();
		return true;
	}
	return false;
}