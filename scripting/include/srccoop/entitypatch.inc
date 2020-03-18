/*
* ******************************************************
* CAI_BaseNPC - npc_human_scientist
* crash fix on getting scared
* ******************************************************
*/
public MRESReturn Hook_ScientistIRelationType(int _this, Handle hReturn, Handle hParams)	
{
	DHookSetReturn(hReturn, D_LI);
	return MRES_Supercede; 
}

/*
* ******************************************************
* CAI_BaseNPC - npc_sniper
* crash fix
* ******************************************************
*/
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

/*
* ******************************************************
* Scene entities
* fix findnamedentity returning sp player ( nullptr )
* ******************************************************
*/
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

/**
* ******************************************************
* CChangelevel
* ******************************************************
*/

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
	CChangelevel pChangelevel = CChangelevel(_this);
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ChangeLevel") == 0)
			{
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

/**
* ******************************************************
* CPointTeleport - point_teleport
* enable multiplayer support
* ******************************************************
*/

/**
* ******************************************************
* CTriggerCamera - point_viewcontrol
* enable multiplayer support
* ******************************************************
*/