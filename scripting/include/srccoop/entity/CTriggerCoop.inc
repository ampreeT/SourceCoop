#pragma newdecls required
#pragma semicolon 1

enum CTriggerCoop_CountType
{
	TRIGGER_COOP_RAW        = 0, // Compare as number of players
	TRIGGER_COOP_PERCENTAGE = 1  // Compare as percentage of players
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

	public void OnCreated()
	{
		int iEntIndex = this.entindex;
		SDKHook(iEntIndex, SDKHook_TouchPost, Hook_Touch);
		SDKHook(iEntIndex, SDKHook_EndTouchPost, Hook_Touch);
		DHookEntity(hkKeyValue_char, false, iEntIndex, _, Hook_KeyValue);
		DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_AcceptInput);
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

	public void SetFiredPlayersIn(const bool bValue)
	{
		this.SetUserData("m_bFiredPlayersIn", bValue);
	}
	public bool GetFiredPlayersIn()
	{
		return this.GetUserData("m_bFiredPlayersIn", false);
	}
	
	public void SetUseHud(const bool bValue)
	{
		this.SetUserData("m_bUseHud", bValue);
	}
	public bool GetUseHud()
	{
		return this.GetUserData("m_bUseHud", true);
	}
}

//------------------------------------------------------
// Touch function
//------------------------------------------------------
static void Hook_Touch(int _this, int _other)
{
	CTriggerCoop pThis = CTriggerCoop(_this);
	if (!pThis.IsEnabled())
		return;
	
	static char szHudMsg[32];
	ArrayList hPlayersInside = new ArrayList(MaxClients+1);
	int iLivingPlayers;

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pThis.IsTouching(pPlayer))
			{
				hPlayersInside.Push(i);
			}
			iLivingPlayers++;
		}
	}
	
	int iPlayerValue = pThis.GetPlayerValue();
	bool bFiredPlayersIn = pThis.GetFiredPlayersIn();
	
	// CountType 0 - Number of players
	if (pThis.GetCountType() == TRIGGER_COOP_RAW)
	{
		if (pThis.GetUseHud())
		{
			FormatEx(szHudMsg, sizeof(szHudMsg), "%i/%i players required", hPlayersInside.Length, iPlayerValue);
			for (int i = 0; i < hPlayersInside.Length; i++)
			{
				SetHudTextParams(-1.0, 0.9, 0.5, 255, 255, 255, 255, 1, 1.0, 1.0, 1.0);
				ShowHudText(hPlayersInside.Get(i), 2, szHudMsg);
			}
		}
		if (hPlayersInside.Length >= iPlayerValue)
		{
			pThis.SetFiredPlayersIn(true);
			pThis.AcceptInputStr("FireUser1", "", CBaseEntity(_other));
		}
		else if (hPlayersInside.Length < iPlayerValue && bFiredPlayersIn)
		{
			pThis.SetFiredPlayersIn(false);
			pThis.AcceptInputStr("FireUser2", "", CBaseEntity(_other));
		}
		if (hPlayersInside.Length == iLivingPlayers)
		{
			pThis.AcceptInputStr("FireUser3", "", CBaseEntity(_other));
		}
	}
	else
	{
		// CountType 1 - Percentage
		float flTotalPercentage = (hPlayersInside.Length / iLivingPlayers)  * 100.0;
		if (pThis.GetUseHud())
		{
			// Tried view_as<float>() (float) and float(), everything is set to 0.0 on return
			float flPlayerValue = 0.0 + iPlayerValue;
			float flLivingPlayers = 0.0 + iLivingPlayers;
			// 0.5 is rounding down, had to add 0.01...
			Format(szHudMsg, sizeof(szHudMsg), "%i/%i players required", hPlayersInside.Length, RoundFloat(flLivingPlayers * (flPlayerValue * 0.01) + 0.01));
			for (int i = 0; i < hPlayersInside.Length; i++)
			{
				SetHudTextParams(-1.0, 0.9, 0.5, 255, 255, 255, 255, 1, 1.0, 1.0, 1.0);
				ShowHudText(hPlayersInside.Get(i), 2, szHudMsg);
			}
		}
		if (flTotalPercentage >= iPlayerValue)
		{
			pThis.SetFiredPlayersIn(true);
			pThis.AcceptInputStr("FireUser1", "", CBaseEntity(_other));
		}
		else if (flTotalPercentage < iPlayerValue && bFiredPlayersIn)
		{
			pThis.SetFiredPlayersIn(false);
			pThis.AcceptInputStr("FireUser2", "", CBaseEntity(_other));
		}
		if (hPlayersInside.Length == iLivingPlayers)
		{
			pThis.AcceptInputStr("FireUser3", "", CBaseEntity(_other));
		}
	}
	hPlayersInside.Close();
}

//------------------------------------------------------
// KeyValue(char)
// Remapping output keyvalues to ones we can use
//------------------------------------------------------
static MRESReturn Hook_KeyValue(int _this, DHookReturn hReturn, DHookParam hParams)
{
	CTriggerCoop pThis = CTriggerCoop(_this);
	static char szKey[MAX_FORMAT];
	static char szVal[MAX_VALUE];
	hParams.GetString(1, szKey, sizeof(szKey));
	hParams.GetString(2, szVal, sizeof(szVal));
	
	if (StrEqual(szKey, "OnPlayersIn", false))
	{
		hParams.SetString(1, "OnUser1");
	}
	else if (StrEqual(szKey, "OnPlayersOut", false))
	{
		hParams.SetString(1, "OnUser2");
	}
	else if (StrEqual(szKey, "OnAllPlayersIn", false))
	{
		hParams.SetString(1, "OnUser3");
	}
	else if (StrEqual(szKey, "PlayerValue", false))
	{
		pThis.SetPlayerValue(StringToInt(szVal));
	}
	else if (StrEqual(szKey, "CountType", false))
	{
		pThis.SetCountType(view_as<CTriggerCoop_CountType>(StringToInt(szVal)));
	}
	else if (StrEqual(szKey, "UseHud", false))
	{
		pThis.SetUseHud(!!StringToInt(szVal));
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Implementation of inputs to trigger_coop
//------------------------------------------------------
static MRESReturn Hook_AcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (hParams.IsNull(1))
		return MRES_Ignored;
	
	char szInputType[MAX_FORMAT];
	hParams.GetString(1, szInputType, sizeof(szInputType));
	
	if (strcmp(szInputType, "TeleportPlayersNotTouching", false) == 0)
	{
		hReturn.Value = true;
		CTriggerCoop pThis = CTriggerCoop(_this);
		
		float vTargetOrigin[3], vTargetAngles[3];
		char szTarget[64];
		pThis.GetTarget(szTarget, sizeof(szTarget));
		CBaseEntity pTarget = CBaseEntity(FindEntityByTargetname(-1, szTarget));
		if (pTarget == NULL_CBASEENTITY)
		{
			char szTargetname[MAX_VALUE];
			pThis.GetTargetname(szTargetname, sizeof(szTargetname));
			PrintToServer("trigger_coop: '%s' has invalid target: '%s'", szTargetname, szTarget);
			return MRES_Supercede;
		}

		// Should WorldSpaceCenter be used instead?
		pTarget.GetAbsOrigin(vTargetOrigin);
		pTarget.GetAbsAngles(vTargetAngles);
		
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (!pThis.IsTouching(pPlayer))
				{
					pPlayer.Teleport(vTargetOrigin, vTargetAngles, vec3_origin);
				}
			}
		}
		
		return MRES_Supercede;
	}
	
	return MRES_Ignored;
}