#pragma newdecls required
#pragma semicolon 1

// Patches common to multiple source games

//------------------------------------------------------
// Local Player override
//------------------------------------------------------

/**
 * Pushes a player or an entity, for which a player will be searched on demand, to the stack.
 * Must be followed by PopLocalPlayerOverride - use AddLocalPlayerPlaceholder for pairing without adding an override.
 */
void AddLocalPlayerOverride(CBaseEntity pEntity)
{
	if (pEntity == NULL_CBASEENTITY)
	{
		g_iLocalPlayerStackSkips[g_iLocalPlayerStackPointer]++;
		return;
	}
	if (++g_iLocalPlayerStackPointer >= sizeof(g_pLocalPlayerEntity))
	{
		g_iLocalPlayerStackPointer--;
		ThrowError("LocalPlayer override stack overflow (max: %d)", sizeof(g_pLocalPlayerEntity) - 1);
	}
	g_pLocalPlayerEntity[g_iLocalPlayerStackPointer] = pEntity;
}

void AddLocalPlayerOverrideEx(int iEntIndex)
{
	AddLocalPlayerOverride(CBaseEntity(iEntIndex));
}

void AddLocalPlayerPlaceholder()
{
	AddLocalPlayerOverride(NULL_CBASEENTITY);
}

/* 
 * Undo's current local player override entity by popping the stack.
 * Must be preceded by a call to AddLocalPlayerOverride or AddLocalPlayerPlaceholder.
 */
void PopLocalPlayerOverride()
{
	if (g_iLocalPlayerStackSkips[g_iLocalPlayerStackPointer] > 0)
	{
		g_iLocalPlayerStackSkips[g_iLocalPlayerStackPointer]--;
		return;
	}
	if (g_iLocalPlayerStackPointer <= 0)
	{
		ThrowError("Tried to pop LocalPlayer override, but none was set.");
	}
	g_iLocalPlayerStackPointer--;
}

//------------------------------------------------------
// MaxClients override
//------------------------------------------------------

/**
 * Overrides MaxClients to 1 and increments internal override counter. 
 * Must be followed by PopMaxClientsOverride - use AddMaxClientsPlaceholder for pairing without adding an override.
 */
void AddMaxClientsOverride()
{
	#define MAX_MAXPLAYERS_OVERRIDES 100
	if (g_iMaxClientsOverrides >= MAX_MAXPLAYERS_OVERRIDES)
	{
		ThrowError("MaxClients override overflow (max: %d)", MAX_MAXPLAYERS_OVERRIDES);
	}
	CGlobalVars.Get().SetMaxPlayers(1);
	g_iMaxClientsOverrides++;
}

void AddMaxClientsPlaceholder()
{
	g_iMaxClientsOverrides++;
}

/* 
 * Decrements internal MaxClients override counter and undo's the override when reaching 0.
 * Must be preceded by a call to AddMaxClientsOverride or AddMaxClientsPlaceholder.
 */
void PopMaxClientsOverride()
{
	if (g_iMaxClientsOverrides <= 0)
	{
		ThrowError("Tried to pop MaxClients override, but none was set.");
	}
	if (!--g_iMaxClientsOverrides)
	{
		CGlobalVars.Get().SetMaxPlayers(MaxClients);
	}
}

//------------------------------------------------------
// Convenience methods
//------------------------------------------------------

/**
 * Convenience method that adds both LocalPlayer and MaxClients overrides.
 */
void AddSinglePlayerOverride(CBaseEntity pLocalPlayerEntity)
{
	AddLocalPlayerOverride(pLocalPlayerEntity);
	AddMaxClientsOverride();
}

/**
 * Convenience method that adds placeholder entries to both LocalPlayer and MaxClients overrides.
 */
void AddSinglePlayerPlaceholder()
{
	AddLocalPlayerPlaceholder();
	AddMaxClientsPlaceholder();
}

/**
 * Convenience method that pops both LocalPlayer and MaxClients overrides.
 */
void PopSinglePlayerOverride()
{
	PopLocalPlayerOverride();
	PopMaxClientsOverride();
}

//------------------------------------------------------
// UTIL_GetLocalPlayer
//------------------------------------------------------
public MRESReturn Hook_UTIL_GetLocalPlayer(DHookReturn hReturn)
{
	CBaseEntity pOverrideEntity = g_pLocalPlayerEntity[g_iLocalPlayerStackPointer];
	if (pOverrideEntity.IsValid())
	{
		CBasePlayer pPlayer;
		if (pOverrideEntity.IsPlayer())
		{
			pPlayer = view_as<CBasePlayer>(pOverrideEntity);
		}
		else
		{
			pPlayer = GetNearestPlayerPreferAlive(pOverrideEntity);
			if (pPlayer == NULL_CBASEENTITY)
			{
				// BUGBUG Crash avoidance for Linux SM 1.12 - 7164+
				// return MRES_Ignored;
				DHookSetReturn(hReturn, -1);
				return MRES_Supercede;
			}
		}
		DHookSetReturn(hReturn, pPlayer.entindex);
		return MRES_Supercede;
	}
	// BUGBUG Crash avoidance for Linux SM 1.12 - 7164+
	// return MRES_Ignored;
	DHookSetReturn(hReturn, -1);
	return MRES_Supercede;
	
}

//------------------------------------------------------
// CMultiplayRules::IsMultiplayer
//------------------------------------------------------
public MRESReturn Hook_IsMultiplayer(DHookReturn hReturn)
{
	DHookSetReturn(hReturn, g_bIsMultiplayerOverride);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC - KeyValue(char)
// Lets mappers use custom npc models
// (Using keyvalues proves most reliable against BM overriding models with its character manifest file before Spawn())
//------------------------------------------------------
public MRESReturn Hook_BaseNPCKeyValuePost(int _this, DHookReturn hReturn, DHookParam hParams)
{
	CBaseEntity pEntity = CBaseEntity(_this);
	
	static char szKey[MAX_FORMAT];
	static char szVal[MAX_VALUE];
	DHookGetParamString(hParams, 1, szKey, sizeof(szKey));
	if (StrEqual(szKey, "custommodel"))
	{
		DHookGetParamString(hParams, 2, szVal, sizeof(szVal));
		int iModelIndex = PrecacheModel(szVal);
		if (iModelIndex)
		{
			pEntity.SetModel(szVal);
			pEntity.SetModelIndex(iModelIndex);
			DHookEntity(hkSetModel, false, _this, _, BaseNPCSetModelBlock);
		}
	}
	return MRES_Ignored;
}

public MRESReturn BaseNPCSetModelBlock(int _this, DHookParam hParams)
{
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC::Think
// Universal GetLocalPlayer pre-setter, ran before NPC thinks
//------------------------------------------------------
public MRESReturn Hook_BaseNPCThink(int _this)
{
	// Set this entity into context
	AddLocalPlayerOverrideEx(_this);
	return MRES_Ignored;
}

public MRESReturn Hook_BaseNPCThinkPost(int _this)
{
	PopLocalPlayerOverride();
	return MRES_Ignored;
}

//------------------------------------------------------
// Physics_RunThinkFunctions - The main physics simulation loop against all entities
// This hook is only active when there are no real players on the server!
// Its main purpose is to ensure NPCs can always find a valid player pointer when thinking. 
//------------------------------------------------------
public MRESReturn Hook_Physics_RunThinkFunctions(DHookParam hParams)
{
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC
// Fix UpdateEnemyMemory input when player string is passed as parameter
//------------------------------------------------------
public MRESReturn Hook_BaseNPCAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "UpdateEnemyMemory", false) == 0)
		{
			char szParam[32];
			DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
			if (StrEqual(szParam, "!player", false) || StrEqual(szParam, "!pvsplayer", false))
			{
				CAI_BaseNPC pThis = CAI_BaseNPC(_this);
				CBasePlayer pPlayer;
				float vecAbsOrigin[3];
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
					{
						pPlayer = CBasePlayer(i);
						pPlayer.GetAbsOrigin(vecAbsOrigin);
						pThis.UpdateEnemyMemory(pPlayer, vecAbsOrigin, pThis);
					}
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Scene entities
// fix findnamedentity returning sp player ( nullptr )
//------------------------------------------------------
public MRESReturn Hook_FindNamedEntity(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1) && !DHookIsNullParam(hParams, 2))
	{
		char szName[MAX_CLASSNAME];
		DHookGetParamString(hParams, 1, szName, sizeof(szName));
		if ((strcmp(szName, "Player", false) == 0)
			|| (strcmp(szName, "!player", false) == 0)
			|| (strcmp(szName, "!nearestfriend", false) == 0)
			|| (strcmp(szName, "!friend", false) == 0))
		{
			CBaseEntity pActor = CBaseEntity(DHookGetParam(hParams, 2));
			if (pActor != NULL_CBASEENTITY)
			{
				CBasePlayer pBestPlayer = GetNearestPlayer(pActor);
				if (pBestPlayer != NULL_CBASEENTITY)
				{
					DHookSetReturn(hReturn, pBestPlayer.entindex);
					return MRES_Supercede;
				}
			}
		}
	}

	return MRES_Ignored;
}

//------------------------------------------------------
// NPCs that use the response system https://developer.valvesoftware.com/wiki/Response_System
// The original function calls `AI_GetSinglePlayer()` which will end up returning null.
//------------------------------------------------------
public MRESReturn Hook_ExpresserHost_DoModifyOrAppendCriteriaPost(DHookParam hParams)
{
	CBaseEntity pSpeaker = CBaseEntity(DHookGetParam(hParams, 1));
	CBasePlayer pPlayer = GetNearestPlayerPreferAlive(pSpeaker, true);
	if (pPlayer != NULL_CBASEENTITY)
	{
		AI_CriteriaSet pCriteria = AI_CriteriaSet(DHookGetParam(hParams, 2));

		float vec3SpeakerPosition[3];
		pSpeaker.GetAbsOrigin(vec3SpeakerPosition);

		float vec3PlayerPosition[3];
		pPlayer.GetAbsOrigin(vec3PlayerPosition);

		int iLength = RoundFloat(GetVectorDistance(vec3SpeakerPosition, vec3PlayerPosition));
		
		// This is rounded from a float to string to save stack space for string formatting.
		char szLength[MAX_CHARS_INT(INT_MIN)];
		IntToString(iLength, szLength, sizeof(szLength));
		pCriteria.AppendCriteria("distancetoplayer", szLength);

		// Original SDK code checks if the player is looking at the speaker.
		// This is not used as the closest player that is alive may not be looking at the speaker while other players are.
		if (/*pPlayer.FInViewConeEntity(pSpeaker) &&*/ pPlayer.FVisibleEntity(pSpeaker))
		{
			pCriteria.AppendCriteria("seenbyplayer", "1");
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CChangelevel - trigger_changelevel
// Hook to handle mapchanges
//------------------------------------------------------
public void Hook_ChangelevelSpawn(int iEntIndex)
{
	CChangelevel pEntity = CChangelevel(iEntIndex);
	DHookEntity(hkAcceptInput, true, pEntity.entindex, _, Hook_ChangelevelAcceptInput);

	if (!(pEntity.m_spawnflags & SF_CHANGELEVEL_NOTOUCH))
	{
		SDKHook(pEntity.entindex, SDKHook_Touch, Hook_ChangelevelOnTouch);
		CreateTimer(Conf.CHANGELEVEL_EFFECT_PERIOD, ChangelevelEffects, pEntity, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT);
	}
}

public MRESReturn Hook_ChangelevelAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ChangeLevel", false) == 0)
			{
				CChangelevel pChangelevel = CChangelevel(_this);
				CoopManager.OnChangeLevelInput(pChangelevel);
			}
		}
	}

	return MRES_Ignored;
}

public void Hook_ChangelevelOnTouch(int _this, int iOther)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CChangelevel pChangelevel = CChangelevel(_this);
		CBasePlayer pPlayer = CBasePlayer(iOther);

		if (pPlayer.IsValid())
		{
			CoopManager.OnChangeLevelTouch(pChangelevel, pPlayer);
		}
	}
}

public Action ChangelevelEffects(Handle timer, CChangelevel pChangelevel)
{
	if (pChangelevel.IsValid())
	{
		if (CoopManager.IsFeatureEnabled(FT_CHANGELEVEL_FX))
		{
			char szMapName[MAX_MAPNAME];
			pChangelevel.GetMapName(szMapName, sizeof(szMapName));

			if (pChangelevel.IsEnabled() && CoopManager.IsMapAllowedNext(szMapName))
			{
				float vecMins[3], vecMaxs[3], vecOrigin[3];
				pChangelevel.GetMins(vecMins);
				pChangelevel.GetMaxs(vecMaxs);
				pChangelevel.GetAbsOrigin(vecOrigin);
				AddVectors(vecOrigin, vecMins, vecMins);
				AddVectors(vecOrigin, vecMaxs, vecMaxs);

				vecMins[2] -= 100.0;
				
				int model = PrecacheModel(Conf.CHANGELEVEL_EFFECT_MODEL);
				TE_Start("Bubbles");
				TE_WriteVector("m_vecMins", vecMins);
				TE_WriteVector("m_vecMaxs", vecMaxs);
				TE_WriteNum("m_nModelIndex", model);
				TE_WriteNum("m_nCount", Conf.CHANGELEVEL_EFFECT_AMOUNT);
				TE_WriteFloat("m_fHeight", vecMaxs[2] - vecMins[2]);
				TE_WriteFloat("m_fSpeed", Conf.CHANGELEVEL_EFFECT_SPEED);
				TE_SendToAll();
			}
		}
		return Plugin_Continue;
	}
	return Plugin_Stop;
}

//------------------------------------------------------
// game_end
// Does not work in BM
// Other games benefit from our custom map series logic
//------------------------------------------------------
public MRESReturn Hook_GameEndAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
	{
		return MRES_Ignored;
	}

	char szInputType[MAX_FORMAT];
	DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

	if (strcmp(szInputType, "EndGame", false) == 0)
	{
		CoopManager.OnMapSeriesFinished();
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CTriggerSave - trigger_autosave
// Allow spawning in MP so we can use them for checkpoints
//------------------------------------------------------
public Action Hook_AutosaveSpawn(int iEntIndex)
{
	CBaseTrigger pTrigger = CBaseTrigger(iEntIndex);
	pTrigger.InitTrigger();
	return Plugin_Stop;
}

//------------------------------------------------------
// CPointTeleport - point_teleport
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_PointTeleportAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "Teleport", false) == 0)
		{
			char szTarget[MAX_FORMAT];
			CBaseEntity pTeleport = CBaseEntity(_this);
			pTeleport.GetTarget(szTarget, sizeof(szTarget));
			if (strcmp(szTarget, "!player", false) == 0)
			{
				float origin[3], angles[3];
				GetEntPropVector(_this, Prop_Data, "m_vSaveOrigin", origin);
				GetEntPropVector(_this, Prop_Data, "m_vSaveAngles", angles);
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
					{
						TeleportEntity(i, origin, angles, NULL_VECTOR);
					}
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CTriggerCamera - point_viewcontrol
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_PointViewcontrolAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			CBaseEntity pThis = CBaseEntity(_this);
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

			if (strcmp(szInputType, "Disable", false) == 0)
			{
				HookSingleEntityOutput(_this, "OnEndFollow", OnEndFollowBlocker);
				bHookSkip = true;
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i))
					{
						CBasePlayer pPlayer = CBasePlayer(i);
						if (pPlayer.GetViewEntity() == pThis)
						{
							SetEntPropEnt(_this, Prop_Data, "m_hPlayer", i);
							pThis.AcceptInputStr("Disable");
							SetClientViewEntity(i, i); // game wont reset player if dead
						}
					}
				}
				SetEntPropEnt(_this, Prop_Data, "m_hPlayer", -1);
				UnhookSingleEntityOutput(_this, "OnEndFollow", OnEndFollowBlocker);
				pThis.FireOutput("OnEndFollow", _this);
				bHookSkip = false;
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
			else if (strcmp(szInputType, "Enable", false) == 0)
			{
				CBasePlayer pActivator;
				if (!DHookIsNullParam(hParams, 2))
				{
					pActivator = CBasePlayer(DHookGetParam(hParams, 2));
				}
				if (pActivator.IsPlayer())
				{
					// prevent bugging out if already using cam
					CBaseEntity pViewEntity = pActivator.GetViewEntity();
					if (pViewEntity.IsValid() && pViewEntity.IsClassname("point_viewcontrol") && pViewEntity != pThis)
					{
						SetEntPropEnt(pViewEntity.entindex, Prop_Data, "m_hPlayer", pActivator.entindex);
						bHookSkip = true;
						pViewEntity.AcceptInputStr("Disable");
						bHookSkip = false;
						pActivator.SetViewEntity(pActivator); // game wont reset player if dead
					}
				}
				else
				{
					AcceptEntityInput(_this, "EnableAll");
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
			else if (strcmp(szInputType, "EnableAll", false) == 0) // new input
			{
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i) && IsPlayerAlive(i))
					{
						AcceptEntityInput(_this, "Enable", i);
					}
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

public Action OnEndFollowBlocker(const char[] output, int caller, int activator, float delay)
{
	return Plugin_Stop;
}

//------------------------------------------------------
// CMovementSpeedMod - player_speedmod
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_SpeedmodAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ModifySpeedAll", false) == 0) // new input
			{
				char szParam[32];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				bHookSkip = true;
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i))
					{
						SetVariantString(szParam);
						AcceptEntityInput(_this, "ModifySpeed", i);
					}
				}
				bHookSkip = false;
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CEnvZoom - env_zoom
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_EnvZoomAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInput[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInput, sizeof(szInput));
		
			CBaseEntity pActivator;
			if (!DHookIsNullParam(hParams, 2))
			{
				pActivator = CBaseEntity(DHookGetParam(hParams, 2));
			}
			
			char szNewInput[8];
			int iNewClient;
			
			if (strcmp(szInput, "Zoom", false) == 0)
			{
				szNewInput = "Zoom";
				if (pActivator.IsPlayer())
				{
					iNewClient = pActivator.entindex;
				}
			}
			else if (strcmp(szInput, "ZoomAll", false) == 0)
			{
				szNewInput = "Zoom";
			}
			else if (strcmp(szInput, "UnZoom", false) == 0)
			{
				szNewInput = "UnZoom";
			}
			else
			{
				return MRES_Ignored;
			}
			
			bHookSkip = true;
			if (iNewClient > 0)
			{
				AddLocalPlayerOverrideEx(iNewClient);
				AcceptEntityInput(_this, szNewInput);
				PopLocalPlayerOverride();
			}
			else
			{
				for (int i = 1; i <= MaxClients; i++)
				{
					if (IsClientInGame(i))
					{
						AddLocalPlayerOverrideEx(i);
						AcceptEntityInput(_this, szNewInput);
						PopLocalPlayerOverride();
					}
				}
			}
			bHookSkip = false;
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// point_clientcommand
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_ClientCommandAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInput[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInput, sizeof(szInput));
			
			if (!StrEqual(szInput, "CommandAll", false) && !StrEqual(szInput, "Command", false))
			{
				return MRES_Ignored;
			}
			
			char szParam[MAX_FORMAT];
			DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
			TrimString(szParam);
			
			if (CoopManager.IsFeatureEnabled(FT_AUTODETECT_MAP_END))
			{
				if (StrContains(szParam, "disconnect", false) != -1 || StrContains(szParam, "startupmenu", false) != -1)
				{
					CoopManager.OnMapSeriesFinished();
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
			
			if (strcmp(szInput, "CommandAll", false) == 0) // new input
			{
				// do special handling for the give command: give it to all and add to equipment for later spawns
				int iStart = 5;
				if (strncmp(szParam, "give ", iStart, false) == 0)
				{
					if (strncmp(szParam[iStart], "item_weapon_", 12, false) == 0)
					{
						iStart += 5; // start after item_
					}
					for (int i = 1; i <= MaxClients; i++)
					{
						if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
						{
							CBasePlayer pPlayer = CBasePlayer(i);
							if (!pPlayer.HasWeapon(szParam[iStart]))
							{
								pPlayer.GiveItem(szParam[iStart]);
							}
						}
					}
					CCoopSpawnSystem.AddSpawnItem(szParam[iStart]);
				}
				else
				{
					bHookSkip = true;
					for (int i = 1; i <= MaxClients; i++)
					{
						if (IsClientInGame(i))
						{
							SetVariantString(szParam);
							AcceptEntityInput(_this, "Command", i);
						}
					}
					bHookSkip = false;
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// point_servercommand
// treat changelevel command as natural mapchange
//------------------------------------------------------
public MRESReturn Hook_ServerCommandAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Command", false) == 0)
			{
				char szParam[MAX_FORMAT];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				TrimString(szParam);

				if (strncmp(szParam, "changelevel ", 12, false) == 0)
				{
					CoopManager.EndMap(szParam[12], SC_NORMAL_MAPCHANGE);
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
				if (CoopManager.IsFeatureEnabled(FT_AUTODETECT_MAP_END))
				{
					if (StrEqual(szParam, "disconnect", false))
					{
						CoopManager.OnMapSeriesFinished();
						DHookSetReturn(hReturn, true);
						return MRES_Supercede;
					}
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// env_credits
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_EnvCreditsAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "ShowLogo", false) == 0)
		{
			float flLogoLength = GetEntPropFloat(_this, Prop_Data, "m_flLogoLength");
			if (flLogoLength)
			{
				Handle pMsg = StartMessageAll("LogoTimeMsg", USERMSG_RELIABLE);
				if (pMsg != null)
				{
					BfWriteFloat(pMsg, flLogoLength);
					EndMessage();
				}
			}
			else
			{
				Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
				if (pMsg != null)
				{
					BfWriteByte(pMsg, 1);
					EndMessage();
				}
			}
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
		if (strcmp(szInputType, "RollCredits", false) == 0)
		{
			Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
			if (pMsg != null)
			{
				BfWriteByte(pMsg, 2);
				EndMessage();
			}
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
		if (strcmp(szInputType, "RollEndCredits", false) == 0 || strcmp(szInputType, "RollOutroCredits", false) == 0)
		{
			Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
			if (pMsg != null)
			{
				BfWriteByte(pMsg, 3);
				EndMessage();
			}
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// env_sprite
// fix m_flGlowProxySize out of bounds console spam
//------------------------------------------------------
public void Hook_EnvSpriteSpawnPost(int iEntIndex)
{
	CreateTimer(0.1, Timer_EnvSpritePostSpawn, CBaseEntity(iEntIndex), TIMER_FLAG_NO_MAPCHANGE);
}

public void Timer_EnvSpritePostSpawn(Handle timer, CBaseEntity pEntity)
{
	if (pEntity.IsValid())
	{
		if (HasEntProp(pEntity.entindex, Prop_Data, "m_flGlowProxySize"))
		{
			if (GetEntPropFloat(pEntity.entindex, Prop_Data, "m_flGlowProxySize") > 64.0)
			{
				SetEntPropFloat(pEntity.entindex, Prop_Data, "m_flGlowProxySize", 64.0);
			}
		}
	}
}

//------------------------------------------------------
// CAI_GoalEntity - ai_goal_*
// set goal entity to nearest player
//------------------------------------------------------
public MRESReturn Hook_ResolveNamesPost(int _this)
{
	char szGoal[MAX_TARGET_LENGTH];
	GetEntPropString(_this, Prop_Data, "m_iszGoal", szGoal, sizeof(szGoal));

	if (szGoal[0] == '\0' || strcmp(szGoal, "!player", false) == 0)
	{
		CBaseEntity pEnt = CBaseEntity(_this);
		CBasePlayer pPlayer = GetNearestPlayer(pEnt);
		SetEntPropEnt(_this, Prop_Data, "m_hGoalEntity", pPlayer.entindex);
	}
	return MRES_Ignored;
}

// this pre hook is required, otherwise the post hook will crash!
public MRESReturn Hook_ResolveNames(int _this)
{
	return MRES_Ignored;
}

//------------------------------------------------------
// CAI_LeadBehavior
// multiplayer support for ai_goal_lead
//------------------------------------------------------
public MRESReturn Hook_CanSelectSchedule(DHookReturn hReturn)
{
	// ToDo: acutally check conditions as in original code
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_ScriptConditions - ai_script_conditions
// Sets the nearest player to this entity's actor(s)/target into context for UTIL_GetLocalPlayer override to use.
//------------------------------------------------------
public MRESReturn Hook_AIConditionsThink(int _this)
{
	CAI_ScriptConditions pThis = CAI_ScriptConditions(_this);
	CBasePlayer pLocalPlayerOverride = NULL_CBASEENTITY;

	if (pThis.IsEnabled())
	{
		int count;
		float vecMidpoint[3], vecOrigin[3];

		CUtlVector pElementsList = CUtlVector(pThis.address + FindDataMapInfo(_this, "m_ElementList"));
		int len = pElementsList.Count();

		for (int i = 0; i < len; i++)
		{
			// 0 - EHANDLE			m_hActor
			// 4 - CSimTimer		m_Timer
			// 12 - CSimTimer		m_Timeout
			// == 20
			Address conditionsElement = pElementsList.Get(i, 20);
			if (!conditionsElement)
				continue;
			
			CBaseEntity pActor = CBaseEntity(LoadEntityFromHandleAddress(conditionsElement));
			if (!pActor.IsValid())
				continue;
			
			count++;
			pActor.GetAbsOrigin(vecOrigin);
			AddVectors(vecMidpoint, vecOrigin, vecMidpoint);
		}

		if (count)
		{
			ScaleVector(vecMidpoint, 1.0 / count);
		}
		else
		{
			// secondary valid usage is to specify target only
			CBaseEntity pTarget = pThis.GetTargetEnt();
			if (pTarget.IsValid())
			{
				pTarget.GetAbsOrigin(vecMidpoint);
			}
		}

		pLocalPlayerOverride = GetNearestPlayerPreferAliveEx(vecMidpoint);
		if (pLocalPlayerOverride == NULL_CBASEENTITY)
		{
			AddSinglePlayerPlaceholder();
			return MRES_Supercede;
		}
	}
	AddSinglePlayerOverride(pLocalPlayerOverride);
	return MRES_Ignored;
}

public MRESReturn Hook_AIConditionsThinkPost(int _this)
{
	PopSinglePlayerOverride();
	return MRES_Ignored;
}

//------------------------------------------------------
// CAI_BaseNPC - Recreation of SetPlayerAvoidState for MP compatibility
// From https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc.cpp#L3822
//------------------------------------------------------
public MRESReturn Hook_SetPlayerAvoidState(int _this)
{
	CAI_BaseNPC pThis = CAI_BaseNPC(_this);
	
	bool bShouldPlayerAvoid;
	float vNothing[3];
	
	pThis.GetSequenceLinearMotion(pThis.GetSequence(), vNothing);
	bool bIsMoving = (pThis.IsMoving() || !VecEqual(vNothing, vec3_origin));
	
	// If we are coming out of a script, check if we are stuck inside the player.
	if (pThis.GetPerformAvoidance() || (pThis.ShouldPlayerAvoid() && bIsMoving))
	{
		float vMins[3], vMaxs[3], vOrigin[3], vPlrMins[3], vPlrMaxs[3], vPlrOrigin[3];
		pThis.GetMins(vMins);
		pThis.GetMaxs(vMaxs);
		pThis.GetAbsOrigin(vOrigin);
		AddVectors(vMins, vOrigin, vMins);
		AddVectors(vMaxs, vOrigin, vMaxs);
		
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				pPlayer.GetMins(vPlrMins);
				pPlayer.GetMaxs(vPlrMaxs);
				pPlayer.GetAbsOrigin(vPlrOrigin);
				AddVectors(vPlrMins, vPlrOrigin, vPlrMins);
				AddVectors(vPlrMaxs, vPlrOrigin, vPlrMaxs);
				
				if (IsBoxIntersectingBox(vMins, vMaxs, vPlrMins, vPlrMaxs))
				{
					bShouldPlayerAvoid = true;
					break;
				}
			}
		}
	}
	
	pThis.SetPlayerAvoidState(pThis.ShouldPlayerAvoid());
	pThis.SetPerformAvoidance(bShouldPlayerAvoid);
	
	Collision_Group_t iCollGroup = pThis.GetCollisionGroup();
	if (iCollGroup == COLLISION_GROUP_NPC || iCollGroup == COLLISION_GROUP_NPC_ACTOR)
	{
		if (bShouldPlayerAvoid)
		{
			pThis.SetCollisionGroup(COLLISION_GROUP_NPC_ACTOR);
		}
		else
		{
			pThis.SetCollisionGroup(COLLISION_GROUP_NPC);
		}
	}
	return MRES_Supercede;
}

// `UpdateSleepState` ends early due to a null pointer check for the local player.
// The entire function is reconstructed to remove the check and to support multiple players.
// 
// Class: `CAI_BaseNPC`
//
// ## Citations
//
// - [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc.cpp#L3353)
//
public MRESReturn Hook_BaseNpcUpdateSleepState(int _this, DHookParam hParams)
{
	// TODO: does bInPVS work for any and all players?
	CAI_BaseNPC pBaseNPC = CAI_BaseNPC(_this);
	bool bInPVS = DHookGetParam(hParams, 1);
	
	AI_SleepState_t eSleepState = pBaseNPC.GetSleepState();
	if (eSleepState > AISS_AWAKE)
	{
		// wake up if any player has walked into radius
		if (HasAnyPlayerWalkedIntoWakeRadius(pBaseNPC))
		{
			pBaseNPC.Wake();
		}
		else if (eSleepState == AISS_WAITING_FOR_PVS)
		{
			if (bInPVS)
			{
				pBaseNPC.Wake();
			}
		}
		else if (eSleepState == AISS_WAITING_FOR_THREAT)
		{
			if (pBaseNPC.HasCondition(view_as<int>(COND_LIGHT_DAMAGE)) || pBaseNPC.HasCondition(view_as<int>(COND_HEAVY_DAMAGE)))
			{
				pBaseNPC.Wake();
			}
			else
			{
				if (bInPVS)
				{
					for (int i = 1; i <= MaxClients; ++i)
					{
						CBasePlayer pPlayer = CBasePlayer(i);
						if (pPlayer.IsValid() && pPlayer.IsAlive() && !(pPlayer.m_fFlags & FL_NOTARGET) && pPlayer.FVisibleEntity(pBaseNPC))
						{
							pBaseNPC.Wake();
							break;
						}
					}
				}
				
				if ((pBaseNPC.GetSoundInterests() & SOUND_DANGER) && !(pBaseNPC.m_spawnflags & SF_NPC_WAIT_TILL_SEEN))
				{
					CSoundEnt pSoundEnt = CSoundEnt.Get();
					if (pSoundEnt.IsValid())
					{
						CSound pSound;
						pSoundEnt.ActiveList(pSound);
						for (; pSound.IsValid(); pSound.Next())
						{
							if ((pSound.GetType() & SOUND_DANGER) && pBaseNPC.CanHearSound(pSound) && pBaseNPC.IsSoundVisible(pSound))
							{
								pBaseNPC.Wake();
								break;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// NPC is awake
		// Don't let an NPC sleep if they're running a script!
		if (!pBaseNPC.IsInAScript() && pBaseNPC.GetState() != NPC_STATE_SCRIPT)
		{
			if (pBaseNPC.m_SleepFlags & AI_SLEEP_FLAG_AUTO_PVS)
			{
				if (!pBaseNPC.HasCondition(view_as<int>(COND_IN_PVS)))
				{
					pBaseNPC.SetSleepState(AISS_WAITING_FOR_PVS);
					pBaseNPC.Sleep();
				}
			}

			if (pBaseNPC.m_SleepFlags & AI_SLEEP_FLAG_AUTO_PVS_AFTER_PVS)
			{
				if (pBaseNPC.HasCondition(view_as<int>(COND_IN_PVS)))
				{
					// OK, we're in the player's PVS. Now switch to regular old AUTO_PVS sleep rules.
					pBaseNPC.m_SleepFlags |= AI_SLEEP_FLAG_AUTO_PVS;
					pBaseNPC.m_SleepFlags &= ~AI_SLEEP_FLAG_AUTO_PVS_AFTER_PVS;
				}
			}
		}
	}

	return MRES_Supercede;
}

public bool HasAnyPlayerWalkedIntoWakeRadius(CAI_BaseNPC pBaseNPC)
{
	float flWakeRadius = pBaseNPC.GetWakeRadius();
	bool bHasBigEnoughRadius = flWakeRadius > 0.1;
	if (!bHasBigEnoughRadius)
	{
		return false;
	}

	float vec3EntityPosition[3];
	pBaseNPC.GetAbsOrigin(vec3EntityPosition);

	for (int i = 1; i <= MaxClients; ++i)
	{
		CBasePlayer pPlayer = CBasePlayer(i);
		if (pPlayer.IsValid() && pPlayer.IsAlive() && !(pPlayer.m_fFlags & FL_NOTARGET))
		{
			float vec3PlayerPosition[3];
			pPlayer.GetAbsOrigin(vec3PlayerPosition);

			if (GetVectorDistance(vec3EntityPosition, vec3PlayerPosition) <= flWakeRadius)
			{
				return true;
			}
		}
	}

	return false;
}

// Fixes local player task issues.
// `CAI_BaseNPC::RunTask` calls into `UTIL_GetLocalPlayer` and `AI_GetSinglePlayer` which is null in multiplayer.
//
// ## Citations
//
// - [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc_schedule.cpp#L3191)
//
public MRESReturn Hook_BaseNPCRunTask(int _this, DHookParam hParams)
{
	CAI_BaseNPC pEntity = CAI_BaseNPC(_this);
	Task_t pTask = DHookGetParam(hParams, 1);
	sharedtasks_e eTask = pTask.GetTask();
	int iTaskInterrupt = pEntity.GetTaskInterrupt();
	
	if ((eTask == TASK_MOVE_AWAY_PATH && iTaskInterrupt == 0) ||
		(eTask == TASK_FACE_PLAYER) ||
		(eTask == view_as<sharedtasks_e>(100000)))	// TASK_FEAR_GET_PATH_TO_SAFETY_HINT
	{
		AddSinglePlayerOverride(pEntity);
	}
	else
	{
		AddSinglePlayerPlaceholder();
	}
	return MRES_Ignored;
}

public MRESReturn Hook_BaseNPCRunTaskPost(int _this, DHookParam hParams)
{
	PopSinglePlayerOverride();
	return MRES_Ignored;
}

// `SelectSchedule` crashes because a null local player pointer was accessed.
// The entire function is reconstructed to remove the check.
// 
// Class: `CProtoSniper` - npc_sniper
//
// ## Citations
//
// - [Source SDK 2013](https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/hl2/proto_sniper.cpp#L1385)
//
public MRESReturn Hook_ProtoSniperSelectSchedule(int _this, DHookReturn hReturn)
{
	CProtoSniper pSniper = CProtoSniper(_this);
	
	if (pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		// unsure why this is crashing; unneeded
		/*
		if (PrecacheSound("NPC_Sniper.TargetDestroyed", true))
		{
			EmitGameSoundToAll("NPC_Sniper.TargetDestroyed", pSniper.entindex);
		}
		*/
	}

	if (!pSniper.IsWeaponLoaded())
	{
		DHookSetReturn(hReturn, SCHED_RELOAD);
		return MRES_Supercede;
	}
	
	// skipped SCHED_PSNIPER_PLAYER_DEAD
	// this schedule is pretty useless as it only turns on the laser to show the dead player where he got sniped from
	
	if (pSniper.HasCondition(view_as<int>(COND_HEAR_DANGER)))
	{
		// Hear Danger; add glint, ect. here
		DHookSetReturn(hReturn, SCHED_PSNIPER_SUPPRESSED);
		return MRES_Supercede;
	}
	
	if (!pSniper.IsEnabled())
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_DISABLEDWAIT);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_SWEEP_TARGET)))
	{
		CSniperTarget pSweepTarget = pSniper.GetSweepTarget();
		if ((pSweepTarget.IsValid() && (pSweepTarget.m_spawnflags & SF_SNIPERTARGET_NOINTERRUPT)) || pSniper.IsSweepHighestPriority())
		{
			DHookSetReturn(hReturn, SCHED_PSNIPER_SWEEP_TARGET_NOINTERRUPT);
		}
		else
		{
			DHookSetReturn(hReturn, SCHED_PSNIPER_SWEEP_TARGET);
		}
		return MRES_Supercede;
	}
	
	CBaseEntity pEnemy = pSniper.GetEnemy();
	if (!pEnemy.IsValid() || pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		pSniper.SetEnemy(CBaseEntity());
		DHookSetReturn(hReturn, SCHED_PSNIPER_SCAN);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_FRUSTRATED)))
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_FRUSTRATED_ATTACK);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_CANATTACKDECOY)))
	{
		DHookSetReturn(hReturn, SCHED_RANGE_ATTACK2);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_NO_SHOT)))
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_NO_CLEAR_SHOT);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_CAN_RANGE_ATTACK1)))
	{
		DHookSetReturn(hReturn, SCHED_RANGE_ATTACK1);
		return MRES_Supercede;
	}

	DHookSetReturn(hReturn, SCHED_PSNIPER_CAMP);
	return MRES_Supercede;
}

//------------------------------------------------------
// func_rotating
// Fix the "Bad SetLocalAngles" error where the rotating entity stops after its angles have reached a limit
//------------------------------------------------------
public Action Timer_FixRotatingAngles(Handle timer, CBaseEntity pEntity)
{
	if (pEntity.IsValid())
	{
		float vecAngles[3];
		pEntity.GetAngles(vecAngles);
		
		for (int i = 0; i < 3; i++)
		{
			vecAngles[i] = FloatFraction(vecAngles[i]) + RoundToFloor(vecAngles[i]) % 360;
		}
		pEntity.SetAngles(vecAngles);
		
		return Plugin_Continue;
	}
	return Plugin_Stop;
}

//------------------------------------------------------
// General hook
// Disable damage - can be used to prevent physics crashes on linux with specific models in BM
//------------------------------------------------------
public Action Hook_NoDmg(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon, float damageForce[3], float damagePosition[3])
{
	return Plugin_Handled;
}

//------------------------------------------------------
// player_loadsaved
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_LoadSavedAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
	{
		return MRES_Ignored;
	}

	char szInputType[MAX_FORMAT];
	DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

	if (strcmp(szInputType, "Reload", false) == 0)
	{
		if (!DHookIsNullParam(hParams, 2))
		{
			int iActivator = DHookGetParam(hParams, 2);
			CBaseEntity pActivator = CBaseEntity(iActivator);
			if (pActivator != NULL_CBASEENTITY && pActivator.IsPlayer())
			{
				CBasePlayer pPlayer = view_as<CBasePlayer>(pActivator);
				CRevertSaved pThis = CRevertSaved(_this);
				pPlayer.ScreenFade(RoundFloat(pThis.GetDuration() * 1000), pThis.GetRenderColor(), FFADE_OUT | FFADE_STAYOUT);
				
				// This breaks the spectator overlay if the player is already dead
				if (pPlayer.IsAlive())
				{
					pPlayer.Suicide();
				}
			}
		}
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	
	return MRES_Ignored;
}

//------------------------------------------------------
// logic_autosave
// Survival mode hook
// TODO: May need to hook logic_active_autosave at some point if it is used, most maps do not use it though
//------------------------------------------------------
public MRESReturn Hook_LogicAutosaveAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
	{
		return MRES_Ignored;
	}

	char szInputType[MAX_FORMAT];
	DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

	if (strcmp(szInputType, "Save", false) == 0)
	{
		SurvivalManager.OnLogicAutosave(_this, false);
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	if (strcmp(szInputType, "SaveDangerous", false) == 0)
	{
		SurvivalManager.OnLogicAutosave(_this, true);
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}

	return MRES_Ignored;
}

//------------------------------------------------------
// func_trackautochange
// Prevent track from being blocked by players and forced to go back down
//------------------------------------------------------
public MRESReturn Hook_TrackChangeBlocked(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		CBasePlayer pPlayer = DHookGetParam(hParams, 1);
		if (pPlayer.IsValid())
		{
			pPlayer.Suicide();
			
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	
	return MRES_Ignored;
}

//------------------------------------------------------
// func_tracktrain
// Prevent trains from being blocked by players
// Moves players up if not blocked, otherwise kills blocking player
//------------------------------------------------------
public MRESReturn Hook_TrackTrainBlocked(int _this, DHookReturn hReturn, DHookParam hParams)
{
	static float vecOriginAdjusted[3];
	
	if (!DHookIsNullParam(hParams, 1))
	{
		CBasePlayer pPlayer = DHookGetParam(hParams, 1);
		if (pPlayer.IsValid())
		{
			pPlayer.GetAbsOrigin(vecOriginAdjusted);
			
			CanTeleportResult CanTP = CanTeleportPlayer(pPlayer, vecOriginAdjusted, false, false, true);
			if (!CanTP)
			{
				SDKHooks_TakeDamage(pPlayer.entindex, _this, _this, 1000.0, DMG_CRUSH, _, _, _, false);
				DHookSetReturn(hReturn, false);
				return MRES_Supercede;
			}
			pPlayer.Teleport(vecOriginAdjusted);
		}
	}
	
	return MRES_Ignored;
}

//------------------------------------------------------
// Unblocks sending missing TE's (impact effects)
// by disabling serverside prediction recipient culling
//------------------------------------------------------
bool g_bIgnorePredictionCull = true;

public MRESReturn Hook_IgnorePredictionCull(DHookReturn hReturn)
{
	hReturn.Value = g_bIgnorePredictionCull;
	g_bIgnorePredictionCull = true; // reset for next call
	return MRES_Supercede;
}

public MRESReturn Hook_DispatchEffect(DHookParam hParams)
{
	// World impacts are correctly predicted by client.
	// Filter them out out of the server dispatched ones.
	char szName[16];
	hParams.GetString(4, szName, sizeof(szName));
	if (StrEqual(szName, "Impact"))
	{
		g_bIgnorePredictionCull = false;
	}
	#if defined SRCCOOP_HL2DM
	if (StrEqual(szName, "StunstickImpact"))
	{
		g_bIgnorePredictionCull = false;
	}
	#endif
	return MRES_Ignored;
}

//------------------------------------------------------
// Fixes `nullptr` being returned due to the function only checking one new client every function call.
// - NPC dialogue should always work on `+use` even if other players are out of PVS of the NPC.
// - NPC pose parameters should not ever reset anymore due out of PVS player.
//------------------------------------------------------
public MRESReturn Hook_UTIL_FindClient(DHookReturn hReturn, DHookParam hParams)
{
	// Naive solution with fast execution.
	// The original implementation below this implementation causes issues with NPCs doing nothing.
	CBaseEntity pEntity = CBaseEntity(hParams.Get(1));
	DHookSetReturn(hReturn, (pEntity != NULL_CBASEENTITY) ? GetNearestPlayerPreferAlive(pEntity).entindex : -1);
	return MRES_Supercede;

	/*
	CBaseEntity pNPC = CBaseEntity(hParams.Get(1));

	float vec3Origin[3];
	pNPC.GetAbsOrigin(vec3Origin);

	for (int i = 1; i <= MaxClients; ++i)
	{
		CBasePlayer pPlayer = CBasePlayer(i);
		if (pPlayer != NULL_CBASEENTITY && !pPlayer.HasFlags(FL_NOTARGET))
		{
			float vec3PlayerPosition[3];
			pPlayer.GetAbsOrigin(vec3PlayerPosition);

			int iCluster = CVEngineServer.GetClusterForOrigin(vec3PlayerPosition);
			if (iCluster != -1)
			{
				MemoryBlock pPVS = CVEngineServer.GetPVSForCluster(iCluster);
				bool bFound = CVEngineServer.CheckOriginInPVS(vec3Origin, pPVS);
				delete pPVS;

				if (bFound)
				{
					DHookSetReturn(hReturn, i);
					return MRES_Supercede;
				}
			}
		}
	}
	DHookSetReturn(hReturn, -1);
	return MRES_Supercede;
	*/
}

//------------------------------------------------------
// CAI_ScriptedSequence::StartScript
// Attempts to fix null reference on m_hTargetEnt inside CAI_ScriptedSequence::ScriptThink
// by preventing CAI_ScriptedSequence::StartScript from nullifying its own target entity.
//------------------------------------------------------
public MRESReturn Hook_ScriptedSequenceStartScript(int _this)
{
	CAI_ScriptedSequence pThis = CAI_ScriptedSequence(_this);
	CBaseEntity pTarget = pThis.GetTargetEnt();
	if (pTarget != NULL_CBASEENTITY && pTarget.IsNPC())
	{
		CAI_BaseNPC pTargetNpc = view_as<CAI_BaseNPC>(pTarget);
		if (pThis == pTargetNpc.GetCine())
		{
			pTargetNpc.SetCine(NULL_CBASEENTITY);
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// npc_barnacle
// Fixes prediction and view jitter while being grabbed by a barnacle.
//------------------------------------------------------
public void Hook_Barnacle_OnGrab(const char[] szOutput, const int iCaller, const int iActivator, const float flDelay)
{
	CNPC_Barnacle pBarnacle = CNPC_Barnacle(iCaller);

	// The enemy of the barnacle is not set until after `CNPC_Barnacle::AttachTongueToTarget` is finished executing.
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/server/hl2/npc_barnacle.cpp#L1359
	CreateTimer(0.0, Timer_Barnacle_OnGrab, pBarnacle, TIMER_FLAG_NO_MAPCHANGE);
}

//------------------------------------------------------
// npc_barnacle
// Fixes prediction and view jitter while being grabbed by a barnacle.
//------------------------------------------------------
public void Timer_Barnacle_OnGrab(Handle hTimer, const CNPC_Barnacle pBarnacle)
{
	if (pBarnacle.IsValid() && pBarnacle.IsAlive())
	{
		CBaseEntity pEnemy = pBarnacle.GetEnemy();
		if (pEnemy != NULL_CBASEENTITY && pEnemy.IsPlayer())
		{
			CBasePlayer pPlayer = view_as<CBasePlayer>(pEnemy);

			// Prevents client-side prediction errors for movement while being pulled up.
			pPlayer.m_fFlags |= FL_ATCONTROLS;
			// Removes the initial view jitter when being pulled up.
			pPlayer.SetMoveType(MOVETYPE_FLY);
			// Remove the view jitter while being pulled up.
			pPlayer.SetLaggedMovement(0.0);
		}
	}
}

//------------------------------------------------------
// npc_barnacle
// Fixes prediction and view jitter while being grabbed by a barnacle.
//------------------------------------------------------
public void Hook_Barnacle_OnRelease(const char[] szOutput, const int iCaller, const int iActivator, const float flDelay)
{
	CNPC_Barnacle pBarnacle = CNPC_Barnacle(iCaller);
	CBaseEntity pEnemy = pBarnacle.GetEnemy();
	if (pEnemy != NULL_CBASEENTITY && pEnemy.IsPlayer())
	{
		CBasePlayer pPlayer = view_as<CBasePlayer>(pEnemy);
		pPlayer.m_fFlags &= ~FL_ATCONTROLS;
		pPlayer.SetMoveType(MOVETYPE_WALK);
		pPlayer.SetLaggedMovement(1.0);
	}
}
