
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
	static bool bHookSkip = false;
	LogDebug("Hook_PointViewcontrolAcceptInput ent=%d", _this);
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Enable") == 0)
			{
				// if activator is not a player, refire this with a player
				// spin up temporary repeater cameras for every other player
				// this is the only way to make this work 100% without recreating the entity entirely
				
				int iActivator = -1;
				bool bRefiredThisOutput;
				if(!DHookIsNullParam(hParams, 2))
				{
					iActivator = DHookGetParam(hParams, 2);
					LogDebug("Hook_PointViewcontrolAcceptInput iActivator=%d", iActivator);
					if(!CBaseEntity(iActivator).IsClassPlayer())
					{
						iActivator = -1;
					}
				}
				
				bHookSkip = true;
				g_bTempDontHookEnts = true;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i) && IsPlayerAlive(i))
					{
						if(iActivator == -1)
						{
							iActivator = i;
							LogDebug("Hook_PointViewcontrolAcceptInput new iActivator=%d, refiring", iActivator);
							AcceptEntityInput(_this, "Enable", iActivator);
							bRefiredThisOutput = true;
							continue;
						}
						if(i == iActivator)
						{
							continue;
						}
						
						int iFollower = CreateEntityByName("point_viewcontrol");
						CBaseEntity pFollower = CBaseEntity(iFollower);
						if(pFollower.IsValidIndex())
						{
							LogDebug("Hook_PointViewcontrolAcceptInput new follower %d", iFollower);
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
							DHookEntity(hkAcceptInput, false, iFollower, _, Hook_PointViewcontrolBlockEnable);
						}
					}
				}
				bHookSkip = false;
				g_bTempDontHookEnts = false;
				if(bRefiredThisOutput)
				{
					DHookSetReturn(hReturn, true);
					LogDebug("Hook_PointViewcontrolAcceptInput MRES_Supercede");
					return MRES_Supercede;
				}
			}
		}
	}
	LogDebug("Hook_PointViewcontrolAcceptInput MRES_Ignored");
	return MRES_Ignored;
}

public MRESReturn Hook_PointViewcontrolBlockEnable(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "Enable") == 0)
		{
			LogDebug("Hook_PointViewcontrolBlockEnable ent=%d", _this);
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}