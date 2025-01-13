#pragma newdecls required
#pragma semicolon 1

static Handle g_pRemoveAllItems;
static Handle g_pCreateViewModel;
static Handle g_pSetSuitUpdate;

methodmap CBasePlayer < CBaseCombatCharacter
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szRemoveAllItems[] = "CBasePlayer::RemoveAllItems";
		StartPrepSDKCall(SDKCall_Player);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szRemoveAllItems))
			LogMessage("Could not obtain gamedata offset %s", szRemoveAllItems);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
			if (!(g_pRemoveAllItems = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szRemoveAllItems);
		}

		char szCreateViewModel[] = "CBasePlayer::CreateViewModel";
		StartPrepSDKCall(SDKCall_Player);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szCreateViewModel))
			LogMessage("Could not obtain gamedata offset %s", szCreateViewModel);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pCreateViewModel = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCreateViewModel);
		}

		char szSetSuitUpdate[] = "CBasePlayer::SetSuitUpdate";
		StartPrepSDKCall(SDKCall_Player);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szSetSuitUpdate))
			LogMessage("Could not obtain gamedata signature %s", szSetSuitUpdate);
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pSetSuitUpdate = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSetSuitUpdate);
		}
	}
	public CBasePlayer(const int iEntIndex = -1)
	{
		return view_as<CBasePlayer>(CBaseCombatCharacter(iEntIndex));
	}

	public bool IsValid()
	{
		return this.IsPlayer() && this.IsInGame();
	}
	public bool IsInGame()
	{
		int iEntIndex = this.entindex;
		return iEntIndex != -1 && IsClientInGame(iEntIndex);
	}
	public bool IsBot()
	{
		return IsFakeClient(this.entindex);
	}
	public bool IsAlive()
	{
		return IsPlayerAlive(this.entindex);
	}
	public bool GetName(char[] szBuffer, const int iMaxLength)
	{
		return GetClientName(this.entindex, szBuffer, iMaxLength);
	}
	public void SetName(const char[] szPlayerName)
	{
		SetClientName(this.entindex, szPlayerName);
	}
	public int GetUserId()
	{
		return GetClientUserId(this.entindex);
	}
	public CPropVehicleDriveable GetVehicle()
	{
		return CPropVehicleDriveable(GetEntPropEnt(this.entindex, Prop_Send, "m_hVehicle"));
	}
	public int GetArmor()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_ArmorValue", 1);
	}
	public void SetArmor(const int iArmor)
	{
		SetEntProp(this.entindex, Prop_Data, "m_ArmorValue", iArmor, 1);
	}
	public bool HasSuit()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bWearingSuit") != 0;
	}
	public void SetSuit(const bool bShouldHaveSuit)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bWearingSuit", bShouldHaveSuit);
	}
	public void SetSuitUpdate(const char[] szName, int fgroup, int iNoRepeat)
	{
		SDKCall(g_pSetSuitUpdate, this.entindex, szName, fgroup, iNoRepeat);
	}
	public void ClearSuitQueue()
	{
		int iSize;
		
		iSize = GetEntPropArraySize(this.entindex, Prop_Data, "m_rgSuitPlayList");
		for (int i = 0; i < iSize; i++)
		{
			SetEntProp(this.entindex, Prop_Data, "m_rgSuitPlayList", 0, 4, i);
		}

		iSize = GetEntPropArraySize(this.entindex, Prop_Data, "m_rgiSuitNoRepeat");
		for (int i = 0; i < iSize; i++)
		{
			SetEntProp(this.entindex, Prop_Data, "m_rgiSuitNoRepeat", 0, 4, i);
			SetEntPropFloat(this.entindex, Prop_Data, "m_rgflSuitNoRepeatTime", 0.0, i);
		}
	}
	public bool IsSprinting()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bIsSprinting") != 0;
	}
	public void SetIsSprinting(const bool bIsSprinting)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bIsSprinting", bIsSprinting);
	}
	public bool IsSprintEnabled()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bSprintEnabled") != 0;
	}
	public void SetSprintEnabled(const bool bSprintEnabled)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bSprintEnabled", bSprintEnabled);
	}
	public void Kick()
	{
		KickClient(this.entindex);
	}
	public CBaseAnimating GiveItem(const char[] szItemName)
	{
		if (strcmp(szItemName, "item_suit", false) == 0)
		{
			this.SetSuit(true);
			return NULL_CBASEENTITY;
		}
		
		CBaseAnimating pItem = CBaseAnimating(GivePlayerItem(this.entindex, szItemName));
		if (pItem != NULL_CBASEENTITY && pItem.GetOwner() == NULL_CBASEENTITY)
		{
			// this would drop on the ground if player cant pick up any more
			pItem.KillDelayed(0.1);
		}
		return pItem;
	}
	public CBaseCombatWeapon GiveWeapon(const char[] szWeaponName)
	{
		return view_as<CBaseCombatWeapon>(this.GiveItem(szWeaponName));
	}
	public void RemoveWeapon(const CBaseCombatWeapon pWeapon)
	{
		RemovePlayerItem(this.entindex, pWeapon.entindex);
	}
	public void RemoveAllItems(const bool bRemoveSuit = true)
	{
		SDKCall(g_pRemoveAllItems, this.entindex, bRemoveSuit);
	}
	public bool EquipWeapon(const CBaseCombatWeapon pWeapon)
	{
		if (pWeapon != NULL_CBASEENTITY)
		{
			EquipPlayerWeapon(this.entindex, pWeapon.entindex);
			return true;
		}
		
		// still make this possible to set -1
		EquipPlayerWeapon(this.entindex, -1);
		return false;
	}
	public int GetOldButtons()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_nOldButtons");
	}
	public int GetButtons()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_nButtons");
	}
	public void SetButtons(int iButtons)
	{
		SetEntProp(this.entindex, Prop_Data, "m_nButtons", iButtons);
	}
	public int GetPressedButtons()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_afButtonPressed");
	}
	public int GetReleasedButtons()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_afButtonReleased");
	}
	public int GetLastButtons()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_afButtonLast");
	}
	public bool WasPressingButton(const int iButton)
	{
		return (this.GetOldButtons() & iButton) == iButton;
	}
	public bool IsPressingButton(const int iButton)
	{
		return (this.GetButtons() & iButton) == iButton;
	}
	public bool IsDucking()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bDucking") != 0;
	}
	public bool IsDucked()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bDucked") != 0;
	}
	public void StartDucking() // (taken from point_teleport code)
	{
		this.SetButtons(this.GetButtons() | IN_DUCK);
		this.m_fFlags |= FL_DUCKING;
		int iClient = this.entindex;
		SetEntProp(iClient, Prop_Send, "m_bDucked", true);
		SetEntProp(iClient, Prop_Send, "m_bDucking", true);
		SetEntPropFloat(iClient, Prop_Send, "m_flDucktime", 0.0);
		float vecViewOffset[3] = VEC_DUCK_VIEW;
		ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX));
	}
	public void StopDucking()
	{
		this.SetButtons(this.GetButtons() & ~IN_DUCK);
		this.m_fFlags &= ~FL_DUCKING;
		int iClient = this.entindex;
		SetEntProp(iClient, Prop_Send, "m_bDucked", false);
		SetEntProp(iClient, Prop_Send, "m_bDucking", false);
		float vecViewOffset[3] = VEC_VIEW;
		ScaleVector(vecViewOffset, this.GetModelScale());
		this.SetViewOffset(vecViewOffset);
		this.SetCollisionBounds(view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX));
	}
	public void GetEyeAngles(float vec3Buffer[3])
	{
		GetClientEyeAngles(this.entindex, vec3Buffer);
	}
	public CBaseEntity GetAimTarget()
	{
		float vecEyePos[3], vecEyeAngles[3];
		this.GetEyePosition(vecEyePos);
		this.GetEyeAngles(vecEyeAngles);
		
		CTraceRay pTraceRay = new CTraceRay(vecEyePos, vecEyeAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilter_IgnoreData, this.entindex);
		CBaseEntity pEntity = pTraceRay.GetEntity();
		pTraceRay.Close();
		return pEntity;
	}
	public void SetMaxSpeed(const float flMaxSpeed)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flMaxspeed", flMaxSpeed);
	}
	public float GetMaxSpeed()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flMaxspeed");
	}
	public CBaseEntity GetViewEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hViewEntity"));
	}
	public void SetViewEntity(const CBaseEntity pEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hViewEntity", (this == pEntity) ? -1 : pEntity.entindex);
		SetClientViewEntity(this.entindex, pEntity.entindex);
	}
	public CPredictedViewModel GetViewModel(const int iIndex = 0)
	{
		return CPredictedViewModel(GetEntPropEnt(this.entindex, Prop_Send, "m_hViewModel", iIndex));
	}
	public void SetViewModel(const CBaseEntity pEntity, const int iIndex = 0)
	{
		SetEntPropEnt(this.entindex, Prop_Send, "m_hViewModel", pEntity.entindex, iIndex);
	}
	public CPredictedViewModel CreateViewModel(const int iIndex = 0)
	{
		SDKCall(g_pCreateViewModel, this.entindex, iIndex);
		return this.GetViewModel(iIndex);
	}
	public CBaseEntity GetRagdoll()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Send, "m_hRagdoll"));
	}
	public void SetRagdoll(const CBaseEntity pEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Send, "m_hRagdoll", pEntity.entindex);
	}
	public float GetDeathTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flDeathTime");
	}
	public void SetDeathTime(const float flDeathTime)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flDeathTime", flDeathTime);
	}
	public Obs_Mode GetObserverMode()
	{
		return view_as<Obs_Mode>(GetEntProp(this.entindex, Prop_Data, "m_iObserverMode"));
	}
	public void SetObserverMode(const Obs_Mode eMode)
	{
		SetEntProp(this.entindex, Prop_Data, "m_iObserverMode", eMode);
	}
	public CBaseEntity GetObserverTarget()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hObserverTarget"));
	}
	public void SetObserverTarget(const CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hObserverTarget", pTarget.entindex);
	}
	public HideHudFlags_t GetHideHud()
	{
		return view_as<HideHudFlags_t>(GetEntProp(this.entindex, Prop_Send, "m_iHideHUD"));
	}
	public void SetHideHud(const HideHudFlags_t eHideHud)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iHideHUD", eHideHud);
	}
	public void ShowCrosshair(const bool bShow)
	{
		HideHudFlags_t eHideHUD = this.GetHideHud();
		if (bShow)
		{
			this.SetHideHud(eHideHUD & ~HIDEHUD_CROSSHAIR);
		}
		else
		{
			this.SetHideHud(eHideHUD | HIDEHUD_CROSSHAIR);
		}
	}
	public void SetTeam(const int iTeam)
	{
		ChangeClientTeam(this.entindex, iTeam);
	}
	public CBaseEntity GetUseEntity()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hUseEntity"));
	}
	public CBaseEntity GetCarriedObject()
	{
		// TODO: HL2DM Needs to call GetPlayerHeldEntity !
		#if defined SRCCOOP_BLACKMESA
		CBaseEntity pUseEntity = this.GetUseEntity();
		if (pUseEntity != NULL_CBASEENTITY && pUseEntity.IsClassname("player_pickup"))
		{
			return CBaseEntity(GetEntPropEnt(pUseEntity.entindex, Prop_Data, "m_attachedEntity"));
		}
		#endif
		return NULL_CBASEENTITY;
	}
	public bool IsCarryingObject()
	{
		CBaseEntity pUseEntity = this.GetUseEntity();
		return (pUseEntity != NULL_CBASEENTITY && pUseEntity.IsClassname("player_pickup"));
	}
	public void ForceDropOfCarriedPhysObjects()
	{
		this.AcceptInputStr("ForceDropPhysObjects");
	}
	public void SendCommand(const char[] szCommand)
	{
		ClientCommand(this.entindex, szCommand);
	}
	public bool SendConVarValueStr(const ConVar hConVar, const char[] szValue)
	{
		if (this.IsBot())
		{
			// TODO: Make the array length a constant.
			char szConVar[48];
			hConVar.GetName(szConVar, sizeof(szConVar));
			SetFakeClientConVar(this.entindex, szConVar, szValue);
			return true;
		}
		else
		{
			return SendConVarValue(this.entindex, hConVar, szValue);
		}
	}
	public bool SendConVarValueInt(const ConVar hConVar, const int iValue)
	{
		char szValue[MAX_CHARS_INT(INT_MIN)];
		IntToString(iValue, szValue, sizeof(szValue));
		return this.SendConVarValueStr(hConVar, szValue);
	}
	public void PlayGameSound(const char[] szSound)
	{
		ClientCommand(this.entindex, "playgamesound %s", szSound);
	}
	public void PlaySound(const char[] szSound)
	{
		EmitSoundToClient(this.entindex, szSound);
	}
	/**
	 * Returns how many times the given player has died
	 * 
	 * @return				The number of deaths
	 */
	public int GetDeaths()
	{
		return GetClientDeaths(this.entindex);
	}
	/**
	 * Gets this player's score
	 *
	 * @return				The player's score
	 */
	public int GetScore()
	{
		return GetClientFrags(this.entindex);
	}
	/**
	 * Sets this player's score to the given value
	 *
	 * @param iScore		Value to set as the score for the player
	 */
	public void SetScore(const int iScore)
	{
		SetEntProp(this.entindex, Prop_Data, "m_iFrags", iScore);
		IServerGameClients.Get().GetPlayerState(this).SetScore(iScore);
	}
	/**
	 * Modifies the player's score by the provided points
	 * 
	 * @param iPoints		The amount of points to add or subtract
	 * @return				The new score for the player
	 */
	public int ModifyScore(const int iPoints)
	{
		int iNewScore = this.GetScore() + iPoints;
		this.SetScore(iNewScore);
		return iNewScore;
	}
	public void Suicide()
	{
		ForcePlayerSuicide(this.entindex);
	}
	public CFuncLadder GetLadder()
	{
		return CFuncLadder(GetEntPropEnt(this.entindex, Prop_Data, "m_hLadder"));
	}
	public void SetLadder(const CFuncLadder pLadder)
	{
		if (this.GetLadder() != NULL_CBASEENTITY)
		{
			this.GetLadder().FireOutput("OnPlayerGotOffLadder", this.entindex);
		}
		SetEntPropEnt(this.entindex, Prop_Data, "m_hLadder", pLadder.entindex);
	}
	public int GetDefaultFOV()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iDefaultFOV");
	}
	public int GetFOV()
	{
		int iFOV = GetEntProp(this.entindex, Prop_Data, "m_iFOV");
		iFOV = iFOV == 0 ? this.GetDefaultFOV() : iFOV;

		// TODO:
		// Implement vehichle FOV logic https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/server/player.cpp#L8395

		float flFOVRate = GetEntPropFloat(this.entindex, Prop_Data, "m_flFOVRate");
		if (flFOVRate == 0.0)
			return iFOV;
		
		float flDeltaTime = (GetGameTime() - GetEntPropFloat(this.entindex, Prop_Data, "m_flFOVTime")) / flFOVRate;
		if (flDeltaTime >= 1.0)
		{
			SetEntProp(this.entindex, Prop_Data, "m_iFOVStart", iFOV);
			return iFOV;
		}
		else
		{
			float flFOVStart = float(GetEntProp(this.entindex, Prop_Data, "m_iFOVStart"));
			return RoundFloat(SimpleSplineRemapValClamped(flDeltaTime, 0.0, 1.0, flFOVStart, float(iFOV)));
		}
	}
	public bool SetFOV(const CBaseEntity pRequester, const int iFOV, const float flZoomRate, const int iZoomStart = 0)
	{
		if (pRequester == NULL_CBASEENTITY)
			return false;
		
		CBaseEntity pZoomOwner = CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hZoomOwner"));
		// If we already have an owner, we only allow requests from that owner
		if (pZoomOwner != NULL_CBASEENTITY && pZoomOwner != pRequester)
		{
			if (!CanOverrideEnvZoomOwner(pZoomOwner))
				return false;
		}
		else
		{
			if (iFOV == 0)
			{
				pZoomOwner = NULL_CBASEENTITY;
			}
			else
			{
				pZoomOwner = pRequester;
			}
			SetEntPropEnt(this.entindex, Prop_Data, "m_hZoomOwner", pZoomOwner.entindex);
		}

		SetEntProp(this.entindex, Prop_Data, "m_iFOVStart", iZoomStart > 0 ? iZoomStart : this.GetFOV());
		SetEntPropFloat(this.entindex, Prop_Data, "m_flFOVTime", GetGameTime());
		SetEntProp(this.entindex, Prop_Data, "m_iFOV", iFOV);
		SetEntPropFloat(this.entindex, Prop_Data, "m_flFOVRate", flZoomRate);
		return true;
	}
	public QueryCookie QueryConvar(const char[] szCvar, const ConVarQueryFinished fnCallback, const any value = 0)
	{
		return QueryClientConVar(this.entindex, szCvar, fnCallback, value);
	}
	public bool IsDrawingViewModel()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_bDrawViewmodel") != 0;
	}
	public void DrawViewModel(const bool bDrawViewmodel)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bDrawViewmodel", bDrawViewmodel);
	}
	public void StartAdmireGlovesAnimation()
	{
		// TODO:
		// Create HL2DM implementation.
		#if defined SRCCOOP_BLACKMESA
		static const char g_szModelNameProxy[] = "models/weapons/v_357.mdl";
		static const char g_szModelNameGloves[] = "models/weapons/v_hands.mdl";

		if (this.GetActiveWeapon() != NULL_CBASEENTITY)
			return;

		// TODO: Should this straight call into CreateViewModel?
		CPredictedViewModel pViewModel = this.GetViewModel(0);
		if (pViewModel != NULL_CBASEENTITY)
			pViewModel = this.CreateViewModel(0);
		
		if (pViewModel == NULL_CBASEENTITY)
			return;

		// We use a model as a proxy here so that the `SendViewModelMatchingSequence` will
		// send the animation to the gloves. This is done by setting the model then replacing the model index.
		int iModelIndexProxy = PrecacheModel(g_szModelNameProxy);
		int iModelIndex = PrecacheModel(g_szModelNameGloves);
		if (iModelIndexProxy && iModelIndex)
		{
			// TODO:
			// For HL2DM, `ACT_VM_IDLE` might be used instead.
			this.DrawViewModel(true);
			pViewModel.m_fEffects &= ~EF_NODRAW;
			pViewModel.SetModel(g_szModelNameProxy);
			pViewModel.SetModelIndex(iModelIndex);
			pViewModel.SendViewModelMatchingSequence(pViewModel.SelectWeightedSequence(ACT_VM_DRAW));
		}
		#endif
	}
	public float GetLaggedMovement()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flLaggedMovementValue");
	}
	public void SetLaggedMovement(const float flValue)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flLaggedMovementValue", flValue);
	}
	public int GetTickBase()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nTickBase");
	}
	public void SetTickBase(const int iTickBase)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nTickBase", iTickBase);
	}
	public bool ScreenFade(int iDuration, RGBA color, ScreenFadeFlags_t eFadeFlags, int iHoldtime = -1, int iMessageFlags = USERMSG_RELIABLE)
	{
		Handle hMessage = StartMessageOne("Fade", this.entindex, iMessageFlags);
		
		if (!hMessage)
			return false;

		if (GetFeatureStatus(FeatureType_Native, "GetUserMessageType") == FeatureStatus_Available && GetUserMessageType() == UM_Protobuf)
		{
			PbSetInt(hMessage,   "duration",   iDuration);
			PbSetInt(hMessage,   "hold_time",  iHoldtime);
			PbSetInt(hMessage,   "flags",      view_as<int>(eFadeFlags));
			PbSetColor(hMessage, "clr",        color.ToArray());
		}
		else
		{
			BfWriteShort(hMessage,	iDuration);                     // Fade duration
			BfWriteShort(hMessage,	iHoldtime);                     // Fade hold time
			BfWriteShort(hMessage,	view_as<int>(eFadeFlags));      // What to do
			BfWriteByte(hMessage,	color.r);                       // Color R
			BfWriteByte(hMessage,	color.g);                       // Color G
			BfWriteByte(hMessage,	color.b);                       // Color B
			BfWriteByte(hMessage,	color.a);                       // Color Alpha
		}
		EndMessage();

		return true;
	}
}

// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
static float SimpleSpline(const float value)
{
	float valueSquared = value * value;

	// Nice little ease-in, ease-out spline-like curve
	return (3.0 * valueSquared) - (2.0 * valueSquared * value);
}

static float SimpleSplineRemapValClamped(const float val, const float A, const float B, const float C, const float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	if (cVal > 1.0)
	{
		cVal = 1.0;
	}
	else if (cVal < 0.0)
	{
		cVal = 0.0;
	}
	return C + (D - C) * SimpleSpline(cVal);
}

static bool CanOverrideEnvZoomOwner(CBaseEntity pZoomOwner)
{
	char szClassname[MAX_CLASSNAME];
	if (!pZoomOwner.GetClassname(szClassname, sizeof(szClassname)) || strcmp(szClassname, "env_zoom") != 0)
		return false;
	return !!(view_as<CEnvZoom>(pZoomOwner).m_spawnflags & ENV_ZOOM_OVERRIDE);
}