#pragma newdecls required
#pragma semicolon 1

enum CTriggerCoop_CountType
{
	TRIGGER_COOP_RAW        = 0, // Compare as number of players
	TRIGGER_COOP_PERCENTAGE = 1  // Compare as percentage of players
}

enum struct CTriggerCoop_TouchData
{
	int iPlayersTouching;
	int iPlayersRequired;
}

methodmap CTriggerCoop < CBaseTrigger
{
	public CTriggerCoop(const int iEntIndex = -1)
	{
		return view_as<CTriggerCoop>(CBaseTrigger(iEntIndex));
	}
	public static CTriggerCoop Create()
	{
		return CTriggerCoop(CreateEntityByName("trigger_coop"));
	}

	public static bool TranslateOutput(const char[] szOutputName, char[] szResult, int iMaxLength, bool bReverse = false)
	{
		static char szOutputMap[][][] = 
		{
			{"OnPlayersIn", "OnUser1"},
			{"OnPlayersOut", "OnUser2"},
			{"OnAllPlayersIn", "OnUser3"}
		};
		int i = FindStringInArrayDict(szOutputMap, sizeof(szOutputMap), szOutputName, false, bReverse ? 1 : 0);
		if (i != -1)
		{
			strcopy(szResult, iMaxLength, szOutputMap[i][bReverse ? 0 : 1]);
			return true;
		}
		return false;
	}

	public void OnCreated()
	{
		int iEntIndex = this.entindex;
		SDKHook(iEntIndex, SDKHook_StartTouchPost, Hook_StartTouchPost);
		SDKHook(iEntIndex, SDKHook_EndTouchPost, Hook_EndTouchPost);
		DHookEntity(hkKeyValue_char, false, iEntIndex, _, Hook_KeyValue);
		DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_AcceptInput);
		CreateTimer(1.0, Timer_UpdateHud, this, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
	}

	public void SetCountType(const CTriggerCoop_CountType eCountType)
	{
		this.SetUserData("m_nCountType", eCountType);
	}
	public CTriggerCoop_CountType GetCountType()
	{
		return this.GetUserData("m_nCountType", TRIGGER_COOP_PERCENTAGE);
	}

	public void SetPlayerValue(const int iValue)
	{
		this.SetUserData("m_iPlayerValue", iValue);
	}
	public int GetPlayerValue()
	{
		return this.GetUserData("m_iPlayerValue", 50);
	}
	
	public void SetUseHud(const bool bValue)
	{
		this.SetUserData("m_bUseHud", bValue);
	}
	public bool GetUseHud()
	{
		return this.GetUserData("m_bUseHud", true);
	}

	public void SetTouchData(const CTriggerCoop_TouchData pData)
	{
		this.SetUserDataArray("m_pTouchData", pData, sizeof(CTriggerCoop_TouchData));
	}
	public bool GetTouchData(CTriggerCoop_TouchData pData)
	{
		return this.GetUserDataArray("m_pTouchData", pData, sizeof(CTriggerCoop_TouchData));
	}

	public void UpdateTouchingPlayers()
	{
		if (!this.IsEnabled())
			return;
		
		CTriggerCoop_TouchData pData;
		int iPlayersAlive;

		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (this.IsTouching(pPlayer))
				{
					pData.iPlayersTouching++;
				}
				iPlayersAlive++;
			}
		}
		
		if (this.GetCountType() == TRIGGER_COOP_RAW)
		{
			pData.iPlayersRequired = this.GetPlayerValue();
		}
		else // TRIGGER_COOP_PERCENTAGE
		{
			// 0.5 is rounding down, had to add 0.01...
			pData.iPlayersRequired = RoundFloat(iPlayersAlive * (this.GetPlayerValue() * 0.01) + 0.01);
		}
		
		CTriggerCoop_TouchData pOldData;
		if (!this.GetTouchData(pOldData))
		{
			pOldData.iPlayersTouching = -1;
		}

		if (pData.iPlayersRequired == pOldData.iPlayersRequired
			&& pData.iPlayersTouching == pOldData.iPlayersTouching)
		{
			// no change
			return;
		}

		this.SetTouchData(pData);

		if (pData.iPlayersTouching >= pData.iPlayersRequired)
		{
			if (pData.iPlayersTouching > 0 && pOldData.iPlayersTouching < pOldData.iPlayersRequired)
			{
				this.FireOutput("OnUser1"); // OnPlayersIn
			}
		}
		else if (pData.iPlayersTouching < pData.iPlayersRequired)
		{
			if (pOldData.iPlayersTouching > 0 && pOldData.iPlayersTouching >= pOldData.iPlayersRequired)
			{
				this.FireOutput("OnUser2"); // OnPlayersOut
			}
		}

		if (pData.iPlayersTouching == iPlayersAlive && iPlayersAlive > 0)
		{
			this.FireOutput("OnUser3"); // OnAllPlayersIn
		}
	}

	public void UpdateHud()
	{
		if (!this.IsEnabled() || !this.GetUseHud())
			return;
		
		CTriggerCoop_TouchData pData;
		if (!this.GetTouchData(pData))
			return;

		RGBA color = this.GetRenderColor();
		SetHudTextParams(-1.0, 0.9, 0.5, color.r, color.g, color.b, color.a, 1, 1.0, 1.0, 1.0);
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (this.IsTouching(pPlayer))
				{
					ShowHudText(i, 2, "%i / %i %t", pData.iPlayersTouching, pData.iPlayersRequired, "players");
				}
			}
		}
	}
}

//------------------------------------------------------
// Performs remapping of outputs and sets entity properties
//------------------------------------------------------
static MRESReturn Hook_KeyValue(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static char szKey[MAX_KEY];
	static char szVal[MAX_VALUE];
	hParams.GetString(1, szKey, sizeof(szKey));
	hParams.GetString(2, szVal, sizeof(szVal));

	if (strncmp(szKey, "OnUser", 6, false) == 0 && IsCharNumeric(szKey[6]))
	{
		MsgSrv("ERROR: OnUser* outputs are not supported for trigger_coop!");
		hReturn.Value = false;
		return MRES_Supercede;
	}

	if (CTriggerCoop.TranslateOutput(szKey, szKey, sizeof(szKey)))
	{
		hParams.SetString(1, szKey);
		return MRES_ChangedHandled;
	}
	else if (StrEqual(szKey, "PlayerValue", false))
	{
		CTriggerCoop(_this).SetPlayerValue(StringToInt(szVal));
	}
	else if (StrEqual(szKey, "CountType", false))
	{
		CTriggerCoop(_this).SetCountType(view_as<CTriggerCoop_CountType>(StringToInt(szVal)));
	}
	else if (StrEqual(szKey, "UseHud", false))
	{
		CTriggerCoop(_this).SetUseHud(!!StringToInt(szVal));
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Touch hooks
//------------------------------------------------------
static void Hook_StartTouchPost(int _this, int _other)
{
	CTriggerCoop pThis = CTriggerCoop(_this);
	pThis.UpdateTouchingPlayers();
}

static void Hook_EndTouchPost(int _this, int _other)
{
	CTriggerCoop pThis = CTriggerCoop(_this);
	pThis.UpdateTouchingPlayers();
}

//------------------------------------------------------
// Hud update timer
//------------------------------------------------------
static Action Timer_UpdateHud(Handle timer, CTriggerCoop pThis)
{
	if (pThis.IsValid())
	{
		pThis.UpdateHud();
		return Plugin_Continue;
	}
	return Plugin_Stop;
}

//------------------------------------------------------
// Implementation of inputs to trigger_coop
//------------------------------------------------------
static MRESReturn Hook_AcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (hParams.IsNull(1))
		return MRES_Ignored;
	
	char szInputType[MAX_KEY];
	hParams.GetString(1, szInputType, sizeof(szInputType));
	
	if (StrEqual(szInputType, "TeleportPlayersNotTouching", false))
	{
		hReturn.Value = true;
		CTriggerCoop pThis = CTriggerCoop(_this);
		
		char szTarget[MAX_VALUE];
		pThis.GetTarget(szTarget, sizeof(szTarget));
		CBaseEntity pTarget = CBaseEntity(FindEntityByTargetname(-1, szTarget));
		if (pTarget == NULL_CBASEENTITY)
		{
			char szTargetname[MAX_VALUE];
			pThis.GetTargetname(szTargetname, sizeof(szTargetname));
			MsgSrv("WARN: trigger_coop '%s' has invalid target: '%s'", szTargetname, szTarget);
		}
		else
		{
			float vecTargetOrigin[3], vecTargetAngles[3];
			pTarget.GetAbsOrigin(vecTargetOrigin);
			pTarget.GetAbsAngles(vecTargetAngles);
			
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
				{
					CBasePlayer pPlayer = CBasePlayer(i);
					if (!pThis.IsTouching(pPlayer))
					{
						pPlayer.Teleport(vecTargetOrigin, vecTargetAngles, vec3_origin);
					}
				}
			}
		}
		
		return MRES_Supercede;
	}
	
	return MRES_Ignored;
}