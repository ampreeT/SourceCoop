enum struct FireOutputData
{
	char m_szName[MAX_KEY];
	CBaseEntity m_pCaller;
	CBaseEntity m_pActivator;
	float m_flDelay;
}

enum GameState
{
	STATE_WARMUP = 0,
	STATE_INTERMISSION,
	STATE_ROUND
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
		
		if (this.IsCoopModeEnabled())
		{
			SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7);
			SetStateNextState(STATE_WARMUP, STATE_WARMUP);
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
				if(!this.m_bStarted)
				{
					if(g_pLevelLump.m_iIntroType == INTRO_FADE)
					{
						Client_ScreenFade(pPlayer.GetEntIndex(), 0, FFADE_STAYOUT);
						pPlayer.SetFlags(pPlayer.GetFlags() | FL_FROZEN);
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
		if(!GetRealClientCount(true)) {
			return;
		}
		SetStateEndTime(STATE_WARMUP, GetGameTime() + 60 * 60 * 24 * 7);
		SetCurrentState(STATE_ROUND); // doesnt do actual transition
		
		//todo: try SetStateEndTime(STATE_ROUND) with mp_timelimit
		//ConVar pTimeLimit = FindConVar("mp_timelimit");
		//int iTimeLimit = pTimeLimit.IntValue;
	
		this.m_bStarted = true;
		PrintToServer("Co-op map %s has begun", g_szMapName);
		
		if(g_pLevelLump.m_iIntroType == INTRO_FADE)
		{
			for(int i = 1; i <= MaxClients; i++)
			{
				if(IsClientInGame(i))
				{
					CBlackMesaPlayer pClient = CBlackMesaPlayer(i);
					pClient.SetFlags(pClient.GetFlags() & ~FL_FROZEN);
				}
			}
		}
		
		// Fire delayed outputs
		for (int i = 0; i < this.m_pDelayedOutputs.Length; i++)
		{
			FireOutputData pFireOutputData; this.m_pDelayedOutputs.GetArray(i, pFireOutputData);
			FireEntityOutput(pFireOutputData.m_pCaller.GetEntIndex(), pFireOutputData.m_szName, pFireOutputData.m_pActivator.GetEntIndex(), pFireOutputData.m_flDelay);
		}
	}
	
	void AddDelayedOutput(FireOutputData pFireOutputData)
	{
		this.m_pDelayedOutputs.PushArray(pFireOutputData);
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
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		g_pLevelLump.ParseMapEntities(szMapEntities);
		g_pLevelLump.ParseConfigFile(szMapName);
		g_pLevelLump.ToString(szMapEntities);
	}
	
	return Plugin_Changed;
}

public Action StartTimerCallback(Handle pTimer)
{
	SetStateEndTime(STATE_WARMUP, GetGameTime() + g_pCoopManager.m_iSecondsToStart + 1);
	if(!g_pCoopManager.m_iSecondsToStart--)
	{
		g_pCoopManager.m_pStartTimer = null;
		g_pCoopManager.Start();
		return Plugin_Stop;
	}
	return Plugin_Continue;
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

stock void SetCurrentState(const GameState iState)
{
	GameRules_SetProp("m_nCurrentStateId", iState);
}

//todo fix
stock void SetStateNextState(const GameState iState, const GameState iNextState)
{
	GameRules_SetProp("m_nNextState", iNextState, _, view_as<int>(iState));
}

//todo fix
stock void SetStateEndTime(const GameState iState, const float flEndTime)
{
	GameRules_SetPropFloat("m_flStateDoneTime", flEndTime, view_as<int>(iState));
}

//todo fix
stock void SetStateIsInIntermission(const GameState iState, const bool bInIntermission)
{
	GameRules_SetProp("m_bIsInIntermission", bInIntermission, _, 0);
}

/**
 * Fades a client's screen to a specified color
 * Your adviced to read the FFADE_ Comments
 *
 * @param client		Player for which to fade the screen
 * @param duration		duration in seconds the effect stays
 * @param mode			fade mode, see FFADE_ defines
 * @param holdtime		holdtime in seconds
 * @param r				red amount
 * @param g				green amount
 * @param b				blue amount
 * @param a				transparency
 * @return				True on success, false otherwise
 */
stock bool Client_ScreenFade(int client, int duration, int mode, int holdtime=-1, int r=0, int g=0, int b=0, int a=255, bool reliable=true)
{
	Handle userMessage = StartMessageOne("Fade", client, (reliable?USERMSG_RELIABLE:0));

	if (userMessage == INVALID_HANDLE) {
		return false;
	}

	if (GetFeatureStatus(FeatureType_Native, "GetUserMessageType") == FeatureStatus_Available &&
		GetUserMessageType() == UM_Protobuf) {

		int color[4];
		color[0] = r;
		color[1] = g;
		color[2] = b;
		color[3] = a;

		PbSetInt(userMessage,   "duration",   duration);
		PbSetInt(userMessage,   "hold_time",  holdtime);
		PbSetInt(userMessage,   "flags",      mode);
		PbSetColor(userMessage, "clr",        color);
	}
	else {
		BfWriteShort(userMessage,	duration);	// Fade duration
		BfWriteShort(userMessage,	holdtime);	// Fade hold time
		BfWriteShort(userMessage,	mode);		// What to do
		BfWriteByte(userMessage,	r);			// Color R
		BfWriteByte(userMessage,	g);			// Color G
		BfWriteByte(userMessage,	b);			// Color B
		BfWriteByte(userMessage,	a);			// Color Alpha
	}
	EndMessage();

	return true;
}