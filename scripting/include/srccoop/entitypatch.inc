
//------------------------------------------------------
// CNPC_Human_Scientist - npc_human_scientist
// crash fix on getting scared
//------------------------------------------------------

public MRESReturn Hook_ScientistIRelationType(int _this, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, D_LI);
	return MRES_Supercede;
}

//------------------------------------------------------
// CProtoSniper - npc_sniper
// crash fix
//------------------------------------------------------

public MRESReturn Hook_ProtoSniperSelectSchedule(int _this, Handle hReturn, Handle hParams)	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/hl2/proto_sniper.cpp#L1385
{
	CProtoSniper pSniper = CProtoSniper(_this);

	if (pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		if (PrecacheSound("NPC_Sniper.TargetDestroyed", true))
		{
			EmitGameSoundToAll("NPC_Sniper.TargetDestroyed", pSniper.GetEntIndex());
		}
	}

	if (!pSniper.IsWeaponLoaded())
	{
		DHookSetReturn(hReturn, SCHED_RELOAD);
		return MRES_Supercede;
	}

	// Hear Danger

	if (!pSniper.IsEnabled())
	{
		DHookSetReturn(hReturn, 54);
		return MRES_Supercede;
	}

	CBaseEntity pEnemy = pSniper.GetEnemy();
	if (!pEnemy.IsValid() || pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		pSniper.SetEnemy(CBaseEntity());
		DHookSetReturn(hReturn, 89);
		return MRES_Supercede;
	}

	if (pSniper.HasCondition(view_as<int>(COND_LOST_ENEMY)))
	{
		DHookSetReturn(hReturn, 89);
		return MRES_Supercede;
	}

	if (pSniper.HasCondition(view_as<int>(COND_CAN_RANGE_ATTACK1)))
	{
		DHookSetReturn(hReturn, 43);
		return MRES_Supercede;
	}

	DHookSetReturn(hReturn, 89);
	return MRES_Supercede;
}

//------------------------------------------------------
// Scene entities
// fix findnamedentity returning sp player ( nullptr )
//------------------------------------------------------

public MRESReturn Hook_FindNamedEntity(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1) && !DHookIsNullParam(hParams, 2))
	{
		char szName[MAX_CLASSNAME];
		DHookGetParamString(hParams, 1, szName, sizeof(szName));
		if ((strcmp(szName, "Player", false) == 0) || (strcmp(szName, "!player", false) == 0))
		{
			CBaseEntity pActor = CBaseEntity(DHookGetParam(hParams, 2));
			if (pActor.IsValid())
			{
				float vecActorPosition[3];
				pActor.GetAbsOrigin(vecActorPosition);

				CBlackMesaPlayer pBestPlayer = CBlackMesaPlayer();
				float flBestDistance = FLT_MAX;
				for (int i = 1; i < (MaxClients + 1); i++)
				{
					CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
					if (pPlayer.IsValid() && pPlayer.IsAlive())
					{
						float vecPlayerPosition[3];
						pPlayer.GetAbsOrigin(vecPlayerPosition);

						float flDistance = GetVectorDistance(vecActorPosition, vecPlayerPosition, false);
						if (flDistance < flBestDistance)
						{
							pBestPlayer = pPlayer;
							flBestDistance = flDistance;
							continue;
						}
					}
				}

				if (pBestPlayer.IsValid())
				{
					DHookSetReturn(hReturn, pBestPlayer.GetEntIndex());
					return MRES_Supercede;
				}
			}
		}
	}

	return MRES_Ignored;
}

//------------------------------------------------------
// CChangelevel
//------------------------------------------------------

public bool ChangeLevelToNextMap(CChangelevel pChangelevel)
{
	char szMapName[MAX_MAPNAME];
	if (pChangelevel.GetMapName(szMapName, sizeof(szMapName)) && g_pLevelLump.m_pNextMapList.IsInMapList(szMapName))
	{
		ServerCommand("changelevel %s", szMapName);
	}
}

public void Hook_ChangelevelSpawn(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	DHookEntity(hkAcceptInput, true, pEntity.GetEntIndex(), _, Hook_ChangelevelAcceptInput);

	if(!pEntity.HasSpawnFlag(SF_CHANGELEVEL_NOTOUCH))
	{
		SDKHook(pEntity.GetEntIndex(), SDKHook_Touch, Hook_ChangelevelOnTouch);
	}
}

public MRESReturn Hook_ChangelevelAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ChangeLevel") == 0)
			{
				CChangelevel pChangelevel = CChangelevel(_this);
				ChangeLevelToNextMap(pChangelevel);
			}
		}
	}

	return MRES_Ignored;
}

public void Hook_ChangelevelOnTouch(int _this, int iOther)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		CChangelevel pChangelevel = CChangelevel(_this);
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iOther);

		if (pPlayer.IsValid() && pPlayer.IsClassPlayer())
		{
			ChangeLevelToNextMap(pChangelevel);
		}
	}
}

//------------------------------------------------------
// CPointTeleport - point_teleport
// multiplayer support
//------------------------------------------------------

public MRESReturn Hook_PointTeleportAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Teleport") == 0)
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
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CTriggerCamera - point_viewcontrol
// multiplayer support
//------------------------------------------------------

public MRESReturn Hook_PointViewcontrolAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkipEnable = false;
	LogDebug("Hook_PointViewcontrolAcceptInput ent=%d", _this);
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			CBaseEntity pThis = CBaseEntity(_this);
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_PointViewcontrolAcceptInput szInputType=%s", szInputType);

			if (strcmp(szInputType, "Disable") == 0)
			{
				int iFollowerEnt = -1;
				while ((iFollowerEnt = FindEntityByOwner(iFollowerEnt, pThis, "point_viewcontrol")) != -1)
				{
					LogDebug("Hook_PointViewcontrolAcceptInput disabling follower %d", iFollowerEnt);
					AcceptEntityInput(iFollowerEnt, "Disable");
				}
				// continue as normal //
			}
			else if (strcmp(szInputType, "Enable") == 0 && !bHookSkipEnable)
			{
				CBaseEntity pActivator;
				if(!DHookIsNullParam(hParams, 2))
				{
					pActivator = CBaseEntity(DHookGetParam(hParams, 2));
				}
				if(pActivator.IsClassPlayer())
				{
					if(GetEntProp(_this, Prop_Data, "m_state") == USE_ON)
					{
						AcceptEntityInput(_this, "Disable");
					}
					// continue as normal //
				}
				else
				{
					LogDebug("Hook_PointViewcontrolAcceptInput MRES_Supercede -  activator not a player > EnableAll");
					AcceptEntityInput(_this, "EnableAll");
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
			else if (strcmp(szInputType, "EnableAll") == 0) // new input
			{
				// use this (primary) viewcontrol with first client
				// spin up temporary repeater cameras for every other client
				// this is the only way to make this work 100% without recreating the entity entirely
				
				bHookSkipEnable = true;
				g_bTempDontHookEnts = true;
				bool bUsedThis;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i) && IsPlayerAlive(i))
					{
						if(!bUsedThis)
						{
							LogDebug("Hook_PointViewcontrolAcceptInput enabling primary camera");
							if(GetEntProp(_this, Prop_Data, "m_state") == USE_ON)
							{
								AcceptEntityInput(_this, "Disable");
							}
							AcceptEntityInput(_this, "Enable", i);
							bUsedThis = true;
							continue;
						}
						
						int iFollower = CreateEntityByName("point_viewcontrol");
						CBaseEntity pFollower = CBaseEntity(iFollower);
						if(pFollower.IsValidIndex())
						{
							LogDebug("Hook_PointViewcontrolAcceptInput new follower camera %d", iFollower);
							// copy all relevant properties
							float vec[3];
							char buffer[MAX_VALUE];
							SetEntPropFloat(iFollower, Prop_Data, "m_fov", GetEntPropFloat(_this, Prop_Data, "m_fov"));
							SetEntPropFloat(iFollower, Prop_Data, "m_fovSpeed", GetEntPropFloat(_this, Prop_Data, "m_fovSpeed"));
							SetEntPropFloat(iFollower, Prop_Data, "m_trackSpeed", GetEntPropFloat(_this, Prop_Data, "m_trackSpeed"));
							SetEntPropFloat(iFollower, Prop_Data, "m_acceleration", GetEntPropFloat(_this, Prop_Data, "m_acceleration"));
							SetEntPropFloat(iFollower, Prop_Data, "m_deceleration", GetEntPropFloat(_this, Prop_Data, "m_deceleration"));
							SetEntPropFloat(iFollower, Prop_Data, "m_flSpeed", GetEntPropFloat(_this, Prop_Data, "m_flSpeed"));
							SetEntPropFloat(iFollower, Prop_Data, "m_flWait", GetEntPropFloat(_this, Prop_Data, "m_flWait"));
							SetEntProp(iFollower, Prop_Data, "m_spawnflags", GetEntProp(_this, Prop_Data, "m_spawnflags"));
							GetEntPropVector(_this, Prop_Data, "m_angAbsRotation", vec); SetEntPropVector(iFollower, Prop_Data, "m_angAbsRotation", vec);
							GetEntPropVector(_this, Prop_Data, "m_vecAbsOrigin", vec); SetEntPropVector(iFollower, Prop_Data, "m_vecAbsOrigin", vec);
							GetEntPropString(_this, Prop_Data, "m_iName", buffer, sizeof(buffer)); SetEntPropString(iFollower, Prop_Data, "m_iName", buffer);
							GetEntPropString(_this, Prop_Data, "m_sPath", buffer, sizeof(buffer)); SetEntPropString(iFollower, Prop_Data, "m_sPath", buffer);
							GetEntPropString(_this, Prop_Data, "m_iParent", buffer, sizeof(buffer)); SetEntPropString(iFollower, Prop_Data, "m_iParent", buffer);
							GetEntPropString(_this, Prop_Data, "m_target", buffer, sizeof(buffer)); SetEntPropString(iFollower, Prop_Data, "m_target", buffer);
							GetEntPropString(_this, Prop_Data, "m_iszTargetAttachment", buffer, sizeof(buffer)); SetEntPropString(iFollower, Prop_Data, "m_iszTargetAttachment", buffer);
							pFollower.Spawn();
							pFollower.AcceptInputStr("AddOutput", "OnEndFollow !self:kill::0:1");
							pFollower.AcceptInputStr("Enable", _, CBlackMesaPlayer(i));
							pFollower.SetOwner(pThis);
							DHookEntity(hkAcceptInput, false, iFollower, _, Hook_PointViewcontrolNeverEnable);
						}
					}
				}
				bHookSkipEnable = false;
				g_bTempDontHookEnts = false;
				DHookSetReturn(hReturn, true);
				LogDebug("Hook_PointViewcontrolAcceptInput MRES_Supercede");
				return MRES_Supercede;
			}
		}
	}
	LogDebug("Hook_PointViewcontrolAcceptInput MRES_Ignored");
	return MRES_Ignored;
}

public MRESReturn Hook_PointViewcontrolNeverEnable(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "Enable") == 0)
		{
			LogDebug("Hook_PointViewcontrolNeverEnable ent=%d", _this);
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CMovementSpeedMod - player_speedmod
// multiplayer support
//------------------------------------------------------

public MRESReturn Hook_SpeedmodAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ModifySpeedAll") == 0) // new input
			{
				char szParam[32];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
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
// point_clientcommand
// multiplayer support
//------------------------------------------------------

public MRESReturn Hook_ClientCommandAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_ClientCommandAcceptInput input=%s", szInputType);
			if (strcmp(szInputType, "CommandAll") == 0) // new input
			{
				char szParam[MAX_FORMAT];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
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
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}