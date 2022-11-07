#if defined _srccoop_entitypatch_common_included
 #endinput
#endif
#define _srccoop_entitypatch_common_included

// Patches common to multiple source games

void SetLocalPlayerCallingEntity(int iEntIndex)
{
	if(g_pLocalPlayerEntity.IsValid())
	{
		LogDebug("WARN: SetLocalPlayerCallingEntity, but one has already been set (%d).", g_pLocalPlayerEntity.GetEntIndex());
	}
	g_pLocalPlayerEntity = CBaseEntity(iEntIndex);
}

void ClearLocalPlayerCallingEntity()
{
	g_pLocalPlayerEntity = CBaseEntity();
}

//------------------------------------------------------
// UTIL_GetLocalPlayer
//------------------------------------------------------
public MRESReturn Hook_UTIL_GetLocalPlayer(Handle hReturn)
{
	if (g_pLocalPlayerEntity.IsValid())
	{
		CBasePlayer pPlayer;
		if(g_pLocalPlayerEntity.IsClassPlayer())
		{
			pPlayer = view_as<CBasePlayer>(g_pLocalPlayerEntity);
		}
		else
		{
			pPlayer = GetNearestPlayer(g_pLocalPlayerEntity, false);
			if(!pPlayer.IsValid())
			{
				return MRES_Ignored;
			}
		}
		LogDebug("Overriding UTIL_GetLocalPlayer");
		DHookSetReturn(hReturn, pPlayer.GetEntIndex());
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CAI_BaseNPC - KeyValue(char)
// Lets mappers use custom npc models
// (Using keyvalues proves most reliable against BM overriding models with its character manifest file before Spawn())
//------------------------------------------------------
public MRESReturn Hook_BaseNPCKeyValuePost(int _this, Handle hReturn, Handle hParams)
{
	CBaseEntity pEntity = CBaseEntity(_this);
	
	static char szKey[MAX_FORMAT];
	static char szVal[MAX_VALUE];
	DHookGetParamString(hParams, 1, szKey, sizeof(szKey));
	if (StrEqual(szKey, "model"))
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

public MRESReturn BaseNPCSetModelBlock(int _this, Handle hParams)
{
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC
// Fix UpdateEnemyMemory input when player string is passed as parameter
//------------------------------------------------------
public MRESReturn Hook_BaseNPCAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "UpdateEnemyMemory", false) == 0)
		{
			char szParam[32];
			DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
			if(StrEqual(szParam, "!player", false) || StrEqual(szParam, "!pvsplayer", false))
			{
				CAI_BaseNPC pThis = CAI_BaseNPC(_this);
				CBasePlayer pPlayer;
				float vecAbsOrigin[3];
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
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
// CChangelevel - trigger_changelevel
// Hook to handle mapchanges
//------------------------------------------------------
public void Hook_ChangelevelSpawn(int iEntIndex)
{
	CChangelevel pEntity = CChangelevel(iEntIndex);
	DHookEntity(hkAcceptInput, true, pEntity.GetEntIndex(), _, Hook_ChangelevelAcceptInput);

	if(!pEntity.HasSpawnFlag(SF_CHANGELEVEL_NOTOUCH))
	{
		SDKHook(pEntity.GetEntIndex(), SDKHook_Touch, Hook_ChangelevelOnTouch);
		CreateTimer(2.0, ChangelevelEffects, pEntity, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
	}
}

public MRESReturn Hook_ChangelevelAcceptInput(int _this, Handle hReturn, Handle hParams)
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
			pChangelevel.GetMapName(szMapName, sizeof(szMapName))

			if (!pChangelevel.IsDisabled() && CoopManager.IsMapAllowedNext(szMapName))
			{
				float vecMins[3], vecMaxs[3];
				pChangelevel.GetMins(vecMins);
				pChangelevel.GetMaxs(vecMaxs);
				vecMins[2] -= 100.0;
				
				int model = PrecacheModel("particle/xen_portal_tracer.vmt");
				TE_Start("Bubbles");
				TE_WriteVector("m_vecMins", vecMins);
				TE_WriteVector("m_vecMaxs", vecMaxs);
				TE_WriteNum("m_nModelIndex", model);
				TE_WriteNum("m_nCount", 100);
				TE_WriteFloat("m_fHeight", vecMaxs[2] - vecMins[2]);
				TE_WriteFloat("m_fSpeed", 10.0);
				TE_SendToAll();
			}
		}
		return Plugin_Continue;
	}
	return Plugin_Stop;
}

//------------------------------------------------------
// CTriggerSave - trigger_autosave
// Allow spawning in MP so we can use them for checkpoints
//------------------------------------------------------
public Action Hook_AutosaveSpawn(int iEntIndex)
{
	CBaseEntity pTrigger = CBaseEntity(iEntIndex);
	InitTrigger(pTrigger);
	return Plugin_Stop;
}

// void CBaseTrigger::InitTrigger() from SDK
void InitTrigger(CBaseEntity pTrigger)
{
	pTrigger.SetSolidType(pTrigger.GetParent().IsValid()? SOLID_VPHYSICS : SOLID_BSP);
	int iSolidFlags = pTrigger.GetSolidFlags();
	if(GetEntProp(pTrigger.GetEntIndex(), Prop_Data, "m_bDisabled"))
	{
		iSolidFlags &= ~FSOLID_TRIGGER;
	}
	else
	{
		iSolidFlags |= FSOLID_TRIGGER;
	}
	if(pTrigger.HasSpawnFlag(SF_TRIG_TOUCH_DEBRIS))
	{
		iSolidFlags |= FSOLID_TRIGGER_TOUCH_DEBRIS;
	}
	pTrigger.SetSolidFlags(iSolidFlags | FSOLID_NOT_SOLID);
	pTrigger.SetMoveType(MOVETYPE_NONE);
	static char szBuffer[PLATFORM_MAX_PATH];
	pTrigger.GetModelName(szBuffer, sizeof(szBuffer));
	pTrigger.SetModel(szBuffer); // initialize the model from name
	pTrigger.SetEffects(pTrigger.GetEffects() | EF_NODRAW);
}

//------------------------------------------------------
// CPointTeleport - point_teleport
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_PointTeleportAcceptInput(int _this, Handle hReturn, Handle hParams)
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
			if(strcmp(szTarget, "!player", false) == 0)
			{
				float origin[3], angles[3];
				GetEntPropVector(_this, Prop_Data, "m_vSaveOrigin", origin);
				GetEntPropVector(_this, Prop_Data, "m_vSaveAngles", angles);
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i) && IsPlayerAlive(i))
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
public MRESReturn Hook_PointViewcontrolAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	LogDebug("Hook_PointViewcontrolAcceptInput ent=%d", _this);
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			CBaseEntity pThis = CBaseEntity(_this);
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_PointViewcontrolAcceptInput szInputType=%s", szInputType);

			if (strcmp(szInputType, "Disable", false) == 0)
			{
				HookSingleEntityOutput(_this, "OnEndFollow", OnEndFollowBlocker);
				bHookSkip = true;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
					{
						CBasePlayer pPlayer = CBasePlayer(i);
						if(pPlayer.GetViewEntity() == pThis)
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
				if(!DHookIsNullParam(hParams, 2))
				{
					pActivator = CBasePlayer(DHookGetParam(hParams, 2));
				}
				if(pActivator.IsClassPlayer())
				{
					// prevent bugging out if already using cam
					CBaseEntity pViewEntity = pActivator.GetViewEntity();
					if(pViewEntity.IsValid() && pViewEntity.IsClassname("point_viewcontrol") && pViewEntity != pThis)
					{
						SetEntPropEnt(pViewEntity.GetEntIndex(), Prop_Data, "m_hPlayer", pActivator.GetEntIndex());
						bHookSkip = true;
						pViewEntity.AcceptInputStr("Disable");
						bHookSkip = false;
						pActivator.SetViewEntity(pActivator); // game wont reset player if dead
					}
				}
				else
				{
					LogDebug("Hook_PointViewcontrolAcceptInput activator not a player > EnableAll > MRES_Supercede");
					AcceptEntityInput(_this, "EnableAll");
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
			else if (strcmp(szInputType, "EnableAll", false) == 0) // new input
			{
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i) && IsPlayerAlive(i))
					{
						AcceptEntityInput(_this, "Enable", i);
					}
				}
				DHookSetReturn(hReturn, true);
				LogDebug("Hook_PointViewcontrolAcceptInput MRES_Supercede");
				return MRES_Supercede;
			}
		}
	}
	LogDebug("Hook_PointViewcontrolAcceptInput MRES_Ignored");
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
public MRESReturn Hook_SpeedmodAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_SpeedmodAcceptInput szInputType=%s", szInputType);
			if (strcmp(szInputType, "ModifySpeedAll", false) == 0) // new input
			{
				fieldtype_t fieldType = DHookGetParamObjectPtrVar(hParams, 4, 16, ObjectValueType_Int);
				char szParam[32];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				LogDebug("Hook_SpeedmodAcceptInput ft=%d, %s", fieldType, szParam);
				bHookSkip = true;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
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
public MRESReturn Hook_EnvZoomAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInput[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInput, sizeof(szInput));
		
			CBaseEntity pActivator;
			if(!DHookIsNullParam(hParams, 2))
			{
				pActivator = CBaseEntity(DHookGetParam(hParams, 2));
			}
			
			char szNewInput[8];
			int iNewClient;
			
			if (strcmp(szInput, "Zoom", false) == 0)
			{
				szNewInput = "Zoom";
				if(pActivator.IsClassPlayer())
				{
					iNewClient = pActivator.GetEntIndex();
				}
			}
			else if(strcmp(szInput, "ZoomAll", false) == 0)
			{
				szNewInput = "Zoom";
			}
			else if(strcmp(szInput, "UnZoom", false) == 0)
			{
				szNewInput = "UnZoom";
			}
			else
			{
				return MRES_Ignored;
			}
			
			bHookSkip = true;
			if(iNewClient > 0)
			{
				SetLocalPlayerCallingEntity(iNewClient);
				AcceptEntityInput(_this, szNewInput);
				ClearLocalPlayerCallingEntity();
			}
			else
			{
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
					{
						SetLocalPlayerCallingEntity(i);
						AcceptEntityInput(_this, szNewInput);
						ClearLocalPlayerCallingEntity();
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
public MRESReturn Hook_ClientCommandAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip)
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInput[MAX_FORMAT]; char szParam[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInput, sizeof(szInput));
			DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
			LogDebug("Hook_ClientCommandAcceptInput input=%s, param=%s", szInput, szParam);
			
			if (!StrEqual(szInput, "CommandAll", false) && !StrEqual(szInput, "Command", false))
			{
				return MRES_Ignored;
			}
			
			if(CoopManager.IsFeatureEnabled(FT_AUTODETECT_MAP_END))
			{
				if(StrContains(szParam, "disconnect", false) != -1 || StrContains(szParam, "startupmenu", false) != -1)
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
				if(strncmp(szParam, "give ", iStart, false) == 0)
				{
					if(strncmp(szParam[iStart], "item_weapon_", 12, false) == 0)
					{
						iStart += 5; // start after item_
					}
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i) && IsPlayerAlive(i))
						{
							CBasePlayer pPlayer = CBasePlayer(i);
							if(!pPlayer.HasWeapon(szParam[iStart]))
							{
								pPlayer.GiveItem(szParam[iStart]);
							}
						}
					}
					g_SpawnSystem.AddSpawnItem(szParam[iStart]);
				}
				else
				{
					bHookSkip = true;
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i))
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
public MRESReturn Hook_ServerCommandAcceptInput(int _this, Handle hReturn, Handle hParams)
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
				if(strncmp(szParam, "changelevel ", 12, false) == 0)
				{
					CoopManager.EndMap(szParam[12], SC_NORMAL_MAPCHANGE);
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
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
public MRESReturn Hook_EnvCreditsAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "ShowLogo", false) == 0)
		{
			float flLogoLength = GetEntPropFloat(_this, Prop_Data, "m_flLogoLength");
			if(flLogoLength)
			{
				Handle pMsg = StartMessageAll("LogoTimeMsg", USERMSG_RELIABLE);
				if(pMsg != null) {
					BfWriteFloat(pMsg, flLogoLength);
					EndMessage();
				}
			}
			else
			{
				Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
				if(pMsg != null) {
					BfWriteByte(pMsg, 1);
					EndMessage();
				}
			}
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
		if(strcmp(szInputType, "RollCredits", false) == 0)
		{
			Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
			if(pMsg != null) {
				BfWriteByte(pMsg, 2);
				EndMessage();
			}
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
		if(strcmp(szInputType, "RollEndCredits", false) == 0 || strcmp(szInputType, "RollOutroCredits", false) == 0)
		{
			Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
			if(pMsg != null) {
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

public Action Timer_EnvSpritePostSpawn(Handle timer, CBaseEntity pEntity)
{
	if (pEntity.IsValid())
	{
		if (HasEntProp(pEntity.GetEntIndex(), Prop_Data, "m_flGlowProxySize"))
		{
			if (GetEntPropFloat(pEntity.GetEntIndex(), Prop_Data, "m_flGlowProxySize") > 64.0)
			{
				SetEntPropFloat(pEntity.GetEntIndex(), Prop_Data, "m_flGlowProxySize", 64.0);
			}
		}
	}
	return Plugin_Handled;
}

//------------------------------------------------------
// ai_script_conditions
// completely broken in mp
//------------------------------------------------------
public MRESReturn Hook_AIConditionsThink(int _this)
{
	LogDebug("Hook_AIConditionsThink");
	CAI_ScriptConditions pThis = CAI_ScriptConditions(_this);
	if(!pThis.IsDisabled())
	{
		// Just fire the output and disable. To even begin fixing this, one would need to read the CUtlVector< CAI_ScriptConditionsElement > m_ElementList.
		pThis.FireOutput("OnConditionsSatisfied");
		pThis.SetDisabled(true);
		pThis.SetNextThink(-1.0);
	}
	return MRES_Supercede;
}

//------------------------------------------------------
// func_rotating
// Fix the "Bad SetLocalAngles" error where the rotating entity stops after its angles have reached a limit
//------------------------------------------------------
public Action Timer_FixRotatingAngles(Handle timer, CBaseEntity pEntity)
{
	if(pEntity.IsValid())
	{
		float vecAngles[3];
		pEntity.GetAngles(vecAngles);
		
		for(int i = 0; i < 3; i++)
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
public MRESReturn Hook_LoadSavedAcceptInput(int _this, Handle hReturn, Handle hParams)
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
			CBasePlayer pActivator = CBasePlayer(iActivator);
			if (pActivator.IsClassPlayer())
			{
				int r, g, b, a;
				GetEntityRenderColor(_this, r, g, b, a);
				float duration = GetEntPropFloat(_this, Prop_Data, "m_Duration");
				Client_ScreenFade(iActivator, RoundFloat(duration * 1000), FFADE_OUT|FFADE_STAYOUT, _, r, g, b, a);
				
				// This breaks the spectator overlay if the player is already dead
				if (pActivator.IsAlive())
					pActivator.Suicide();
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
// May need to hook logic_active_autosave at some point if it is used, most maps do not use it though
//------------------------------------------------------
public MRESReturn Hook_LogicAutosaveAcceptInput(int _this, Handle hReturn, Handle hParams)
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

