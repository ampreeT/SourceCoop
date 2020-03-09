#include <srccoop>

ConVar g_pConvarCoopEnabled;

Handle g_pPickupObject;

Handle hkFAllowFlashlight;
Handle hkIRelationType;
Handle hkProtoSniperSelectSchedule;
Handle hkFindNamedEntity;
Handle hkFindNamedEntityClosest;
Handle hkSetModel;
Handle hkAcceptInput;

CGlobalLevelLump g_pLevelLump;
CCoopSpawn g_SpawnSystem;

char g_szMapName[MAX_MAPNAME];
bool g_bIsCoopMap = false;

public Plugin myinfo =
{
	name = "SourceCoop",
	author = "ampreeT",
	description = "SourceCoop",
	version = "1.0.0",
	url = ""
};

public void load_dhook(const Handle pGameConfig, Handle& pHandle, const char[] szFuncName)
{
	pHandle = DHookCreateFromConf(pGameConfig, szFuncName);
	if (pHandle == null)
		SetFailState("Couldn't create hook %s", szFuncName);
	if (!DHookSetFromConf(pHandle, pGameConfig, SDKConf_Virtual, szFuncName))
		SetFailState("Couldn't set hook %s", szFuncName);
}

public void load_gamedata()
{
	char szConfigName[] = "srccoop.games";
	Handle pGameConfig = LoadGameConfigFile(szConfigName);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config %s", szConfigName);
	
	StartPrepSDKCall(SDKCall_Player);
	PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, "CBlackMesaPlayer::PickupObject");
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
	g_pPickupObject = EndPrepSDKCall();
	if (g_pPickupObject == null)
		SetFailState("Could not prep SDK call %s", "CBlackMesaPlayer::PickupObject");

	load_dhook(pGameConfig, hkFAllowFlashlight, "CMultiplayRules::FAllowFlashlight");
	load_dhook(pGameConfig, hkIRelationType, "CBaseCombatCharacter::IRelationType");
	load_dhook(pGameConfig, hkProtoSniperSelectSchedule, "CProtoSniper::SelectSchedule");
	load_dhook(pGameConfig, hkFindNamedEntity, "CSceneEntity::FindNamedEntity");
	load_dhook(pGameConfig, hkFindNamedEntityClosest, "CSceneEntity::FindNamedEntityClosest");
	load_dhook(pGameConfig, hkSetModel, "CBaseEntity::SetModel");
	load_dhook(pGameConfig, hkAcceptInput, "CBaseEntity::AcceptInput");

	CloseHandle(pGameConfig);
}

public void SetPlayerPickup(int iPlayer, int iObject, const bool bLimitMassAndSize)
{
	SDKCall(g_pPickupObject, iPlayer, iObject, bLimitMassAndSize);
}

#pragma dynamic 2097152 
public Action OnLevelInit(const char[] szMapName, char szMapEntities[2097152])		// you probably need to incease SlowScriptTimeout in core.cfg
{
	strcopy(g_szMapName, sizeof(g_szMapName), szMapName);
	g_bIsCoopMap = g_pLevelLump.IsCoopMap(szMapName);
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		g_pLevelLump.ParseMapEntities(szMapEntities);
		g_pLevelLump.ParseConfigFile(szMapName);
		g_pLevelLump.ToString(szMapEntities);
	}
	
	return Plugin_Changed;
}

public void OnMapStart()
{
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		AddWarmupTime(60 * 60 * 24 * 7);
		g_SpawnSystem.ParseConfigFile(g_szMapName);
		DHookGamerules(hkFAllowFlashlight, false, _, Hook_FAllowFlashlight);
		
		CBaseEntity pGameEquip = CreateByClassname("game_player_equip");	// will spawn players with nothing if it exists
		if (pGameEquip.IsValid())
			pGameEquip.Spawn();
	}
}

public void OnPluginStart()
{
	load_gamedata();
	g_pConvarCoopEnabled = CreateConVar("sm_coop_enabled", "1", "Sets if coop is enabled on campaign maps");
	g_pLevelLump.Initialize();
	g_SpawnSystem.Initialize(Callback_Checkpoint);
	HookEvent("player_spawn", Hook_PlayerSpawnPost, EventHookMode_Post);
	
	HookEntityOutput("prop_physics", "OnPlayerUse", Callback_OnUseObject);
	HookEntityOutput("prop_flare", "OnPlayerUse", Callback_OnUseObject);
	HookEntityOutput("func_physbox", "OnPlayerUse", Callback_OnUseObject);
	HookEntityOutput("grenade_frag", "OnPlayerUse", Callback_OnUseObject);	// not sure if this works
	HookEntityOutput("item_crate", "OnPlayerUse", Callback_OnUseObject);
}

public void OnPluginEnd()
{
	
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);

	if (pEntity.IsValid())
	{
		if (pEntity.IsClassScientist())
		{
			DHookEntity(hkIRelationType, true, pEntity.GetEntIndex(), _, Hook_ScientistIRelationType);
		}
		//else if (strcmp(szClassname, "npc_sniper", false) == 0)
		//{
		//	DHookEntity(hkProtoSniperSelectSchedule, false, pEntity.GetEntIndex(), _, Hook_ProtoSniperSelectSchedule);
		//	DHookEntity(hkProtoSniperSelectSchedule, true, pEntity.GetEntIndex(), _, Hook_ProtoSniperSelectSchedule);
		//}
		//else if (pEntity.IsClassScene())
		//{
		//	DHookEntity(hkFindNamedEntity, true, pEntity.GetEntIndex(), _, Hook_FindNamedEntity);
		//	DHookEntity(hkFindNamedEntityClosest, true, pEntity.GetEntIndex(), _, Hook_FindNamedEntity);
		//}
		else if (pEntity.IsClassWeapon())
		{
			DHookEntity(hkSetModel, false, pEntity.GetEntIndex(), _, Hook_WeaponSetModel);
		}
		else if (strcmp(szClassname, "trigger_changelevel") == 0)
		{
			DHookEntity(hkAcceptInput, true, pEntity.GetEntIndex(), _, Hook_ChangelevelAcceptInput);
			SDKHook(pEntity.GetEntIndex(), SDKHook_Touch, Hook_ChangelevelOnTouch);
		}
		else if (strcmp(szClassname, "camera_death") == 0)
		{
			SDKHook(pEntity.GetEntIndex(), SDKHook_SpawnPost, Hook_CameraDeathSpawn);
		}
	}
}

public void OnEntityDestroyed(int iEntIndex)
{
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		if (pEntity.IsValid())
		{
			char szClassname[MAX_CLASSNAME];
			if (pEntity.GetClassname(szClassname, sizeof(szClassname)))
			{
				if (strcmp(szClassname, "camera_death") == 0)
				{
					CBlackMesaPlayer pOwner = view_as<CBlackMesaPlayer>(pEntity.GetOwner());
					if (pOwner.IsValid() && pOwner.IsClassPlayer())
					{
						if (pOwner.GetViewEntity() == pEntity)
						{
							pOwner.SetViewEntity(pOwner);
						}
					}
				}
			}
		}
	}
}

public bool AddWarmupTime(const int iSeconds)
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

public Action Hook_PlayerSpawnPost(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		if (hEvent != null)
		{
			int iClientID = GetEventInt(hEvent, "userid");
			CBlackMesaPlayer pPlayer = CBlackMesaPlayer(GetClientOfUserId(iClientID));
			if (pPlayer.IsValid())
			{
				g_SpawnSystem.SpawnPlayer(pPlayer);
			}
		}
	}
}

public MRESReturn Hook_ScientistIRelationType(int _this, Handle hReturn, Handle hParams)	// scientists crash on sight
{
	DHookSetReturn(hReturn, D_LI);
	return MRES_Supercede; 
}

public MRESReturn Hook_FAllowFlashlight(Handle hReturn, Handle hParams)		// enable sp flashlight
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

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

public MRESReturn Hook_FindNamedEntity(int _this, Handle hReturn, Handle hParams)	// fix findnamedentity returning sp player ( nullptr )
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szName[MAX_CLASSNAME];
		DHookGetParamString(hParams, 1, szName, sizeof(szName));
		if ((strcmp(szName, "Player", false) == 0) || (strcmp(szName, "!player", false) == 0))
		{
			CSceneEntity pSceneEntity = CSceneEntity(_this);
			CBaseEntity pActor = pSceneEntity.GetActor();
			if (pActor.IsValid())
			{
				float vecActorPosition[3];
				pActor.GetAbsOrigin(vecActorPosition);
				
				CBlackMesaPlayer pBestPlayer = CBlackMesaPlayer();
				float flBestDistance = FLT_MAX;
				for (int i = 0; i < (MaxClients + 1); i++)
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

public MRESReturn Hook_WeaponSetModel(int _this, Handle hParams)	// use sp weapon models
{
	if (!DHookIsNullParam(hParams, 1))
	{
		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		if (pWeapon.IsValid())
		{
			CBaseCombatCharacter pOwner = view_as<CBaseCombatCharacter>(pWeapon.GetOwner());
			if (pOwner.IsValid() && !pOwner.IsClassPlayer())
			{
				static const char szWeaponModel[][][] =
				{
					{ "models/weapons/w_glock_mp.mdl", "models/weapons/w_glock.mdl", },
					{ "models/weapons/w_357_mp.mdl", "models/weapons/w_357.mdl", },
					{ "models/weapons/w_mp5_mp.mdl", "models/weapons/w_mp5.mdl", },
					{ "models/weapons/w_shotgun_mp.mdl", "models/weapons/w_shotgun.mdl", },
					{ "models/weapons/w_rpg_mp.mdl", "models/weapons/w_rpg.mdl" },
				};
				
				char szModelName[MAX_CLASSNAME];
				DHookGetParamString(hParams, 1, szModelName, sizeof(szModelName));
				
				for (int i = 0; i < sizeof(szWeaponModel); i++)
				{
					if (strcmp(szModelName, szWeaponModel[i][0], false) == 0)
					{
						if (PrecacheModel(szWeaponModel[i][1], false))
						{
							DHookSetParamString(hParams, 1, szWeaponModel[i][1]);
							return MRES_ChangedHandled;
						}

						break;
					}
				}
			}
		}
	}
	
	return MRES_Ignored;
}

public void Hook_CameraDeathSpawn(int iEntIndex)
{
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		if (pEntity.IsValid())
		{
			CBlackMesaPlayer pOwner = view_as<CBlackMesaPlayer>(pEntity.GetOwner());
			if (pOwner.IsValid() && pOwner.IsClassPlayer())
			{
				CBaseEntity pViewEntity = pOwner.GetViewEntity();
				if (!pViewEntity.IsValid() || pViewEntity == pOwner)
				{
					pOwner.SetViewEntity(pEntity);
				}
			}
		}
	}
}

public bool ChangeLevelToNextMap(CChangelevel pChangelevel)
{
	char szMapName[MAX_MAPNAME];
	if (pChangelevel.GetMapName(szMapName, sizeof(szMapName)) && g_pLevelLump.m_pNextMapList.IsInMapList(szMapName))
	{
		ServerCommand("changelevel %s", szMapName);
	}
}

public MRESReturn Hook_ChangelevelAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	CChangelevel pChangelevel = CChangelevel(_this);
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
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
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		CChangelevel pChangelevel = CChangelevel(_this);
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iOther);
		
		if (pPlayer.IsValid() && pPlayer.IsClassPlayer())
		{
			ChangeLevelToNextMap(pChangelevel);
		}
	}
}

public Action Callback_CheckpointTimer(Handle hTimer, CBlackMesaPlayer pPlayerToFilter)
{
	for (int i = 1; i < (MaxClients + 1); i++)
	{
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
		if (pPlayer.IsValid() && pPlayer.GetEntIndex() != pPlayerToFilter.GetEntIndex())
		{
			g_SpawnSystem.SpawnPlayer(pPlayer);
		}
	}
}

public void Callback_Checkpoint(const char[] szName, int iCaller, int iActivator, float flDelay)
{
	CBaseEntity pCaller = CBaseEntity(iCaller);
	
	CCoopSpawnEntry pEntry;
	int iEntriesToKill = -1;
	for (int i = 0; i < g_SpawnSystem.m_pCheckpointList.Size(); i++)
	{
		if (g_SpawnSystem.m_pCheckpointList.GetArray(i, pEntry, sizeof(pEntry)))
		{
			if (pCaller == pEntry.m_pTriggerEnt)
			{
				iEntriesToKill = i;
				break;
			}
		}
	}
	
	if (iEntriesToKill > -1)
	{
		CBaseEntity pActivator = CBlackMesaPlayer(iActivator);
		CBlackMesaPlayer pPlayerToFilter = (pActivator.IsValid() && pActivator.IsClassPlayer()) ? view_as<CBlackMesaPlayer>(pActivator) : CBlackMesaPlayer();
		
		g_SpawnSystem.EraseCheckpoints(iEntriesToKill + 1);
		g_SpawnSystem.SetSpawnLocation(pEntry.m_vecPosition, pEntry.m_vecAngles, pEntry.m_pFollowEnt);
		CreateTimer(pEntry.m_flDelay, Callback_CheckpointTimer, pPlayerToFilter);
	}
}

public void Callback_OnUseObject(const char[] szName, int iCaller, int iActivator, float flDelay)	// this output has been broken atleast since source 2006; activator is the caller
{
	if (g_pConvarCoopEnabled.BoolValue && g_bIsCoopMap)
	{
		CBaseEntity pCaller = CBaseEntity(iCaller);
		if (pCaller.IsValid())
		{
			for (int i = 1; i < MaxClients + 1; i++)
			{
				CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
				if (pPlayer.IsValid() && pPlayer.IsAlive())
				{
					if (!pPlayer.WasPressingButton(IN_USE) && pPlayer.IsPressingButton(IN_USE))
					{
						SetPlayerPickup(pPlayer.GetEntIndex(), pCaller.GetEntIndex(), false);
						break;
					}
				}
			}
		}
	}
}

// todo read this later
// http://cdn.akamai.steamstatic.com/steam/apps/362890/manuals/bms_workshop_guide.pdf?t=1431372141
// https://forums.alliedmods.net/showthread.php?t=314271