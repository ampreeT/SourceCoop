//#define NO_DEBUG   /* Uncomment to disable debugging */

#include <srccoop>

#define PLUGIN_VERSION "1.0.0"

public Plugin myinfo =
{
	name = "SourceCoop",
	author = "ampreeT, Alienmario",
	description = "SourceCoop",
	version = PLUGIN_VERSION,
	url = "https://github.com/ampreeT/SourceCoop"
};

public Address GetServerInterface(const char[] szInterface)
{
	return view_as<Address>(SDKCall(g_pCreateServerInterface, szInterface, 0));
}

public Address GetEngineInterface(const char[] szInterface)
{
	return view_as<Address>(SDKCall(g_pCreateEngineInterface, szInterface, 0));
}

void LoadGameData()
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_GAMEDATA_NAME);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config %s", SRCCOOP_GAMEDATA_NAME);
	
	// Init SDKCalls for classdef
	InitClassdef(pGameConfig);
	
	// ALL Games
	{
		g_serverOS = view_as<OperatingSystem>(pGameConfig.GetOffset("_OS_Detector_"));
	
		char szCreateEngineInterface[] = "CreateEngineInterface";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szCreateEngineInterface))
			SetFailState("Could not obtain game offset %s", szCreateEngineInterface);
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pCreateEngineInterface = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCreateEngineInterface);
		
		char szCreateServerInterface[] = "CreateServerInterface";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szCreateServerInterface))
			SetFailState("Could not obtain game offset %s", szCreateServerInterface);
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		if (!(g_pCreateServerInterface = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCreateServerInterface);
		
		/*
		char szInterfaceEngine[64];
		if (!GameConfGetKeyValue(pGameConfig, "IVEngineServer", szInterfaceEngine, sizeof(szInterfaceEngine)))
			SetFailState("Could not get interface verison for %s", "IVEngineServer");
		if (!(g_VEngineServer = GetEngineInterface(szInterfaceEngine)))
			SetFailState("Could not get interface for %s", "g_ServerGameDLL");
		*/
		
		char szInterfaceGame[64];
		if (!GameConfGetKeyValue(pGameConfig, "IServerGameDLL", szInterfaceGame, sizeof(szInterfaceGame)))
			SetFailState("Could not get interface verison for %s", "IServerGameDLL");
		if (!(g_ServerGameDLL = IServerGameDLL(GetServerInterface(szInterfaceGame))))
			SetFailState("Could not get interface for %s", "g_ServerGameDLL");
	
		LoadDHookVirtual(pGameConfig, hkChangeTeam, "CBasePlayer::ChangeTeam");
		LoadDHookVirtual(pGameConfig, hkShouldCollide, "CBaseEntity::ShouldCollide");
	}
		
	if (g_Engine == Engine_BlackMesa)
	{
		LoadDHookVirtual(pGameConfig, hkFAllowFlashlight, "CMultiplayRules::FAllowFlashlight");
		LoadDHookVirtual(pGameConfig, hkIsMultiplayer, "CMultiplayRules::IsMultiplayer");
		LoadDHookVirtual(pGameConfig, hkIRelationType, "CBaseCombatCharacter::IRelationType");
		LoadDHookVirtual(pGameConfig, hkIsPlayerAlly, "CAI_BaseNPC::IsPlayerAlly");
		LoadDHookVirtual(pGameConfig, hkProtoSniperSelectSchedule, "CProtoSniper::SelectSchedule");
		LoadDHookVirtual(pGameConfig, hkFindNamedEntity, "CSceneEntity::FindNamedEntity");
		LoadDHookVirtual(pGameConfig, hkFindNamedEntityClosest, "CSceneEntity::FindNamedEntityClosest");
		LoadDHookVirtual(pGameConfig, hkSetModel, "CBaseEntity::SetModel");
		LoadDHookVirtual(pGameConfig, hkAcceptInput, "CBaseEntity::AcceptInput");
		LoadDHookVirtual(pGameConfig, hkOnTryPickUp, "CBasePickup::OnTryPickUp");
		LoadDHookVirtual(pGameConfig, hkThink, "CBaseEntity::Think");
		LoadDHookVirtual(pGameConfig, hkIchthyosaurIdleSound, "CNPC_Ichthyosaur::IdleSound");
		LoadDHookVirtual(pGameConfig, hkHandleAnimEvent, "CBaseAnimating::HandleAnimEvent");
		LoadDHookVirtual(pGameConfig, hkRunAI, "CAI_BaseNPC::RunAI");
		LoadDHookDetour(pGameConfig, hkUTIL_GetLocalPlayer, "UTIL_GetLocalPlayer", Hook_UTIL_GetLocalPlayer);
		LoadDHookDetour(pGameConfig, hkSetSuitUpdate, "CBasePlayer::SetSuitUpdate", Hook_SetSuitUpdate, Hook_SetSuitUpdatePost);
		LoadDHookDetour(pGameConfig, hkResolveNames, "CAI_GoalEntity::ResolveNames", Hook_ResolveNames, Hook_ResolveNamesPost);
		LoadDHookDetour(pGameConfig, hkCanSelectSchedule, "CAI_LeadBehavior::CanSelectSchedule", Hook_CanSelectSchedule);
		LoadDHookDetour(pGameConfig, hkPickup_ForcePlayerToDropThisObject, "Pickup_ForcePlayerToDropThisObject", Hook_ForcePlayerToDropThisObject);
		
		// Disabled until we can avoid the annoying physics mayhem bug this causes
		//LoadDHookDetour(pGameConfig, hkSetPlayerAvoidState, "CAI_BaseNPC::SetPlayerAvoidState", Hook_SetPlayerAvoidState);
	}
	
	CloseHandle(pGameConfig);
}

public void OnPluginStart()
{
	g_Engine = GetEngineVersion();
	LoadGameData();
	
	InitDebugLog("sourcecoop_debug", "SRCCOOP", ADMFLAG_ROOT);
	CreateConVar("sourcecoop_version", PLUGIN_VERSION, _, FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);
	g_pConvarCoopEnabled = CreateConVar("sourcecoop_enabled", "1", "Sets if coop is enabled on coop maps", _, true, 0.0, true, 1.0);
	g_pConvarCoopTeam = CreateConVar("sourcecoop_team", "scientist", "Sets which team to use in TDM mode. Valid values are [marines] or [scientist]. Setting anything else will not manage teams.");
	g_pConvarCoopRespawnTime = CreateConVar("sourcecoop_respawntime", "2.0", "Sets player respawn time in seconds. (This can only be used for making respawn times quicker, not longer). Set to 0 to disable.", _, true, 0.0);
	g_pConvarWaitPeriod = CreateConVar("sourcecoop_start_wait_period", "15.0", "The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.", _, true, 0.0);
	g_pConvarEndWaitPeriod = CreateConVar("sourcecoop_end_wait_period", "60.0", "The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.", _, true, 0.0);
	g_pConvarEndWaitFactor = CreateConVar("sourcecoop_end_wait_factor", "1.0", "Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).", _, true, 0.0, true, 1.0);
	RegAdminCmd("sourcecoop_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegAdminCmd("sc_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegServerCmd("sourcecoop_dump", Command_DumpMapEntities, "Command for toggling plugin features on/off");
	RegServerCmd("sc_dump", Command_DumpMapEntities, "Command for toggling plugin features on/off");
	
	g_pLevelLump.Initialize();
	g_SpawnSystem.Initialize();
	g_pCoopManager.Initialize();
	g_pInstancingManager.Initialize();
	g_pPostponedSpawns = CreateArray();
	g_pFeatureMap = new FeatureMap();
	InitializeMenus();
	
	g_CoopMapStartFwd = new GlobalForward("OnCoopMapStart", ET_Ignore);
	
	if (g_Engine == Engine_BlackMesa)
	{
		HookEvent("broadcast_teamsound", Hook_BroadcastTeamsound, EventHookMode_Pre);
		UserMsg iIntroCredits = GetUserMessageId("IntroCredits");
		if(iIntroCredits != INVALID_MESSAGE_ID)
			HookUserMessage(iIntroCredits, Hook_IntroCreditsMsg, true);
		AddTempEntHook("BlackMesa Shot", BlackMesaFireBulletsTEHook);
		AddNormalSoundHook(PlayerSoundListener);
	}
	
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			OnClientPutInServer(i);
		}
	}
}

#pragma dynamic ENTITYSTRING_LENGTH
public Action OnLevelInit(const char[] szMapName, char szMapEntities[ENTITYSTRING_LENGTH])
{
	OnMapEnd(); // this does not always get called, so call it here
	strcopy(g_szPrevMapName, sizeof(g_szPrevMapName), g_szMapName);
	strcopy(g_szMapName, sizeof(g_szMapName), szMapName);
	g_szEntityString = szMapEntities;
	g_pCoopManager.OnLevelInit(szMapName, szMapEntities);

	return Plugin_Changed;
}

public void OnMapStart()
{
	g_pCoopManager.OnMapStart();
	
	if (g_Engine == Engine_BlackMesa)
	{
		DHookGamerules(hkIsMultiplayer, false, _, Hook_IsMultiplayer);
		DHookGamerules(hkFAllowFlashlight, false, _, Hook_FAllowFlashlight);
	}
	
	for (int i = 0; i < g_pPostponedSpawns.Length; i++)
	{
		CBaseEntity pEnt = g_pPostponedSpawns.Get(i);
		RequestFrame(SpawnPostponedItem, pEnt);
	}
	
	g_pPostponedSpawns.Clear();
	g_bMapStarted = true;
}

public void OnConfigsExecuted()
{
	RequestFrame(OnConfigsExecutedPost); // prevents a bug where this is fired too early if map changes in OnMapStart
}

public void OnConfigsExecutedPost()
{
	if (g_Engine == Engine_BlackMesa)
	{
		if (g_pCoopManager.IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT))
		{
			CBaseEntity pGameEquip = CreateByClassname("game_player_equip");	// will spawn players with nothing if it exists
			if (pGameEquip.IsValid())
			{
				if(!g_pCoopManager.IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT))
				{
					pGameEquip.SetSpawnFlags(SF_PLAYER_EQUIP_STRIP_SUIT);	
				}
				pGameEquip.Spawn();
			}
		}
		if (g_pCoopManager.IsFeatureEnabled(FT_DISABLE_CANISTER_DROPS))
		{
			CBaseEntity pGameGamerules = CreateByClassname("game_mp_gamerules");
			if (pGameGamerules.IsValid())
			{
				pGameGamerules.Spawn();
				pGameGamerules.AcceptInputStr("DisableCanisterDrops");
				pGameGamerules.Kill();
			}
		}
		PrecacheScriptSound("HL2Player.SprintStart");
	}
}

public void OnClientPutInServer(int client)
{
	if(IsFakeClient(client))
	{
		return;
	}
	
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(client);
	
	// fixes visual bug for players which had different view ent at mapchange
	pPlayer.SetViewEntity(pPlayer);
	
	g_pCoopManager.OnClientPutInServer(pPlayer);
	g_pInstancingManager.OnClientPutInServer(client);
	
	SDKHook(client, SDKHook_PreThinkPost, Hook_PlayerPreThinkPost);
	SDKHook(client, SDKHook_PreThink, Hook_PlayerPreThink);
	SDKHook(client, SDKHook_SpawnPost, Hook_PlayerSpawnPost);
	SDKHook(client, SDKHook_TraceAttack, Hook_PlayerTraceAttack);
	SDKHook(client, SDKHook_OnTakeDamage, Hook_PlayerTakeDamage);
	DHookEntity(hkChangeTeam, false, client, _, Hook_PlayerChangeTeam);
	DHookEntity(hkShouldCollide, false, client, _, Hook_PlayerShouldCollide);

	GreetPlayer(client);
}

public void OnClientDisconnect(int client)
{
	g_pInstancingManager.OnClientDisconnect(client);
}

public void OnMapEnd()
{
	g_pLevelLump.RevertConvars();
	g_bMapStarted = false;
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	if(g_bTempDontHookEnts) {
		return;
	}
	
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity.IsValid())
	{
		SDKHook(iEntIndex, SDKHook_Spawn, Hook_FixupBrushModels);
		SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_SpawnPost);
		
		if (g_Engine == Engine_BlackMesa)
		{
			if(pEntity.IsClassNPC())
			{
				SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_BaseNPCSpawnPost);
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_BaseNPCAcceptInput);
				if (strncmp(szClassname, "npc_human_scientist", 19) == 0)
				{
					DHookEntity(hkIRelationType, true, iEntIndex, _, Hook_ScientistIRelationType);
					DHookEntity(hkIsPlayerAlly, true, iEntIndex, _, Hook_IsPlayerAlly);
				}
				else if (strcmp(szClassname, "npc_human_security") == 0)
				{
					DHookEntity(hkIsPlayerAlly, true, iEntIndex, _, Hook_IsPlayerAlly);
				}
				else if (strcmp(szClassname, "npc_sniper", false) == 0)
				{
					DHookEntity(hkProtoSniperSelectSchedule, false, iEntIndex, _, Hook_ProtoSniperSelectSchedule);
				}
				else if (strcmp(szClassname, "npc_xenturret", false) == 0)
				{
					DHookEntity(hkProtoSniperSelectSchedule, false, iEntIndex, _, Hook_XenTurretSelectSchedule);
					DHookEntity(hkHandleAnimEvent, false, iEntIndex, _, Hook_XenTurretHandleAnimEvent);
					DHookEntity(hkHandleAnimEvent, true, iEntIndex, _, Hook_XenTurretHandleAnimEventPost);
					DHookEntity(hkRunAI, false, iEntIndex, _, Hook_XenTurretRunAI);
					DHookEntity(hkRunAI, true, iEntIndex, _, Hook_XenTurretRunAIPost);
				}
				else if (strcmp(szClassname, "npc_ichthyosaur") == 0)
				{
					DHookEntity(hkIchthyosaurIdleSound, false, iEntIndex, _, Hook_IchthyosaurIdleSound);
				}
				else if (strcmp(szClassname, "npc_gargantua") == 0)
				{
					DHookEntity(hkAcceptInput, true, iEntIndex, _, Hook_GargAcceptInputPost);
				}
			}
			else if ((strcmp(szClassname, "instanced_scripted_scene", false) == 0) ||
					(strcmp(szClassname, "logic_choreographed_scene", false) == 0) ||
					(strcmp(szClassname, "scripted_scene", false) == 0))
			{
				DHookEntity(hkFindNamedEntity, true, iEntIndex, _, Hook_FindNamedEntity);
				DHookEntity(hkFindNamedEntityClosest, true, iEntIndex, _, Hook_FindNamedEntity);
			}
			else if (strncmp(szClassname, "item_", 5) == 0)
			{
				if (g_pCoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
				{
					if(pEntity.IsPickupItem())
					{
						SDKHook(iEntIndex, SDKHook_Spawn, Hook_ItemSpawnDelay);
					}
				}
				if (strcmp(szClassname, "item_weapon_snark") == 0)
				{
					SDKHook(iEntIndex, SDKHook_OnTakeDamagePost, Hook_OnItemSnarkDamagePost);
				}
			}
			else if (pEntity.IsClassWeapon())
			{
				DHookEntity(hkSetModel, false, iEntIndex, _, Hook_WeaponSetModel);
			}
			else if (strcmp(szClassname, "trigger_changelevel") == 0)
			{
				SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ChangelevelSpawn);
			}
			else if (strcmp(szClassname, "camera_death") == 0)
			{
				SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_CameraDeathSpawn);
			}
			else if (strcmp(szClassname, "point_teleport") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_PointTeleportAcceptInput);
			}
			else if (strcmp(szClassname, "point_viewcontrol") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_PointViewcontrolAcceptInput);
			}
			else if (strcmp(szClassname, "player_speedmod") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_SpeedmodAcceptInput);
			}
			else if (strcmp(szClassname, "point_clientcommand") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_ClientCommandAcceptInput);
			}
			else if (strcmp(szClassname, "point_servercommand") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_ServerCommandAcceptInput);
			}
			else if (strcmp(szClassname, "env_credits") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvCreditsAcceptInput);
			}
			else if (strcmp(szClassname, "ai_script_conditions") == 0)
			{
				DHookEntity(hkThink, false, iEntIndex, _, Hook_AIConditionsThink);
			}
			else if (strcmp(szClassname, "func_rotating") == 0)
			{
				CreateTimer(30.0, Timer_FixRotatingAngles, pEntity, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
			}
			// if some explosions turn out to be damaging all players except one, this is the fix
			//else if (strcmp(szClassname, "env_explosion") == 0)
			//{
			//	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ExplosionSpawn);
			//}
		}
	}
}

public void Hook_SpawnPost(int iEntIndex)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		
		if(!g_pCoopManager.m_bStarted)
		{
			Array_t pOutputHookList = g_pLevelLump.GetOutputHooksForEntity(pEntity);
			if(pOutputHookList.Length > 0)
			{
				if(pEntity.IsClassname("logic_auto"))
				{
					// do not let it get killed, so the output can fire later
					int iSpawnFlags = pEntity.GetSpawnFlags();
					if(iSpawnFlags & SF_AUTO_FIREONCE)
						pEntity.SetSpawnFlags(iSpawnFlags &~ SF_AUTO_FIREONCE);
				}
				for (int i = 0; i < pOutputHookList.Length; i++)
				{
					CEntityOutputHook pOutputHook; pOutputHookList.GetArray(i, pOutputHook);
					HookSingleEntityOutput(iEntIndex, pOutputHook.m_szOutputName, OutputCallbackForDelay);
				}
			}
			pOutputHookList.Close();
		}
	}
}

// Postpone items' Spawn() until Gamerules IsMultiplayer() gets hooked in OnMapStart()
public Action Hook_ItemSpawnDelay(int iEntIndex)
{
	SDKUnhook(iEntIndex, SDKHook_Spawn, Hook_ItemSpawnDelay);
	
	CBaseEntity pEnt = CBaseEntity(iEntIndex);
	if(g_bMapStarted)
	{
		RequestFrame(SpawnPostponedItem, pEnt);
	}
	else
	{
		g_pPostponedSpawns.Push(pEnt);
	}
	return Plugin_Stop;
}

public void SpawnPostponedItem(CBaseEntity pEntity)
{
	if(pEntity.IsValid())
	{
		SDKHook(pEntity.GetEntIndex(), SDKHook_SpawnPost, Hook_Instancing_ItemSpawn);
		g_bIsMultiplayerOverride = false; // IsMultiplayer=false will spawn items with physics
		pEntity.Spawn();
		g_bIsMultiplayerOverride = true;
	}
}

public void OnEntityDestroyed(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity.IsValid())
	{
		if (pEntity.IsClassname("camera_death"))
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

public Action OutputCallbackForDelay(const char[] output, int caller, int activator, float delay)
{
	if(g_pCoopManager.m_bStarted)
	{
		return Plugin_Continue;
	}
	FireOutputData pFireOutputData;
	strcopy(pFireOutputData.m_szName, sizeof(pFireOutputData.m_szName), output);
	pFireOutputData.m_pCaller = CBaseEntity(caller);
	pFireOutputData.m_pActivator = CBaseEntity(activator);
	pFireOutputData.m_flDelay = delay;
	g_pCoopManager.AddDelayedOutput(pFireOutputData);
	
	if(pFireOutputData.m_pCaller.IsValid())
	{
		// stop from deleting itself
		if(pFireOutputData.m_pCaller.IsClassname("trigger_once"))
		{
			RequestFrame(RequestStopThink, pFireOutputData.m_pCaller);
		}
		if(pFireOutputData.m_pCaller.IsClassname("logic_relay"))
		{
			int sf = pFireOutputData.m_pCaller.GetSpawnFlags();
			if(sf & SF_REMOVE_ON_FIRE)
			{
				pFireOutputData.m_pCaller.SetSpawnFlags(sf &~ SF_REMOVE_ON_FIRE);
				UnhookSingleEntityOutput(caller, output, OutputCallbackForDelay);
				HookSingleEntityOutput(caller, output, DeleteEntOnDelayedOutputFire);
			}
		}
	}

	return Plugin_Stop;
}

public Action DeleteEntOnDelayedOutputFire(const char[] output, int caller, int activator, float delay)
{
	if(g_pCoopManager.m_bStarted)
	{
		RemoveEntity(caller);
		return Plugin_Continue;
	}
	else
	{
		return Plugin_Stop;
	}
}

public void RequestStopThink(CBaseEntity pEntity)
{
	if(pEntity.IsValid())
	{
		pEntity.SetNextThinkTick(0);
	}
}

public Action Hook_BroadcastTeamsound(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		// block multiplayer announcer
		hEvent.BroadcastDisabled = true;
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

public MRESReturn Hook_IsMultiplayer(Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, g_bIsMultiplayerOverride);
	return MRES_Supercede;
}

void GreetPlayer(int client)
{
	if (g_pCoopManager.IsFeatureEnabled(FT_SHOW_WELCOME_MESSAGE))
	{
		Msg(client, "This server runs SourceCoop version %s.\nYou can press %s=%s or type %s/coopmenu%s for extra settings.", PLUGIN_VERSION, CHAT_COLOR_SEC, CHAT_COLOR_PRI, CHAT_COLOR_SEC, CHAT_COLOR_PRI);
	}
}

public Action Command_SetFeature(int iClient, int iArgs)
{
	if(iArgs != 2)
	{
		MsgReply(iClient, "Format: sourcecoop_ft <FEATURE> <1/0>");
		return Plugin_Handled;
	}
	
	char szFeature[MAX_FORMAT];
	GetCmdArg(1, szFeature, sizeof(szFeature));
	
	SourceCoopFeature feature;
	if (g_pFeatureMap.GetFeature(szFeature, feature))
	{
		char szVal[MAX_FORMAT];
		GetCmdArg(2, szVal, sizeof(szVal));
		
		if (StrEqual(szVal, "1") || IsEnableSynonym(szVal))
		{
			g_pCoopManager.EnableFeature(feature);
			MsgReply(iClient, "Enabled feature %s", szFeature);
		}
		else if (StrEqual(szVal, "0") || IsDisableSynonym(szVal))
		{
			g_pCoopManager.DisableFeature(feature);
			MsgReply(iClient, "Disabled feature %s", szFeature);
		}
	}
	else
	{
		MsgReply(iClient, "Unknown feature: %s", szFeature);
	}
	return Plugin_Handled;
}

public Action Command_DumpMapEntities(int iArgs)
{
	if(g_szEntityString[0] == '\0')
	{
		PrintToServer("No entity data recorded for current map.");
		return Plugin_Handled;
	}
	
	char szDumpPath[PLATFORM_MAX_PATH];
	char szTime[128];
	FormatTime(szTime, sizeof(szTime), "%Y-%m-%d-%H%M%S");
	BuildPath(Path_SM, szDumpPath, sizeof(szDumpPath), "data/srccoop/dumps");
	CreateDirectory(szDumpPath, FPERM_U_READ|FPERM_U_WRITE|FPERM_U_EXEC|FPERM_G_READ|FPERM_G_WRITE|FPERM_G_EXEC|FPERM_O_READ|FPERM_O_EXEC);
	Format(szDumpPath, sizeof(szDumpPath), "%s/%s-%s.txt", szDumpPath, g_szMapName, szTime);
	
	File pDumpFile = OpenFile(szDumpPath, "w");
	if(pDumpFile != null)
	{
		pDumpFile.WriteString(g_szEntityString, false);
		CloseHandle(pDumpFile);
		PrintToServer("Dumped map entities in %s", szDumpPath);
	}
	else
	{
		PrintToServer("Failed opening file for writing: %s", szDumpPath);
	}
	return Plugin_Handled;
}
