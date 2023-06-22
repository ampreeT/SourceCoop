#include <srccoop>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop",
	author = "ampreeT, Alienmario",
	description = "SourceCoop",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
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
	
	g_serverOS = view_as<OperatingSystem>(pGameConfig.GetOffset("_OS_Detector_"));

	char szCreateEngineInterface[] = "CreateEngineInterface";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szCreateEngineInterface))
		SetFailState("Could not obtain game signature %s", szCreateEngineInterface);
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	if (!(g_pCreateEngineInterface = EndPrepSDKCall()))
		SetFailState("Could not prep SDK call %s", szCreateEngineInterface);
	
	char szCreateServerInterface[] = "CreateServerInterface";
	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szCreateServerInterface))
		SetFailState("Could not obtain game signature %s", szCreateServerInterface);
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	if (!(g_pCreateServerInterface = EndPrepSDKCall()))
		SetFailState("Could not prep SDK call %s", szCreateServerInterface);
	
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
	
	#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	char szCreateServerRagdoll[] = "CreateServerRagdoll";
	StartPrepSDKCall(SDKCall_Static);
	if(!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szCreateServerRagdoll))
		SetFailState("Could not obtain gamedata signature %s", szCreateServerRagdoll);
	PrepSDKCall_SetReturnInfo(SDKType_CBaseEntity, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer); // CBaseAnimating *pAnimating
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // int forceBone
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const CTakeDamageInfo &info
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // int collisionGroup
	PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain); // bool bUseLRURetirement
	if (!(g_pCreateServerRagdoll = EndPrepSDKCall()))
		SetFailState("Could not prep SDK call %s", szCreateServerRagdoll);
	#endif

	LoadDHookVirtual(pGameConfig, hkLevelInit, "CServerGameDLL::LevelInit");
	if (hkLevelInit.HookRaw(Hook_Pre, view_as<Address>(g_ServerGameDLL), Hook_OnLevelInit) == INVALID_HOOK_ID)
		SetFailState("Could not hook CServerGameDLL::LevelInit");
	
	LoadDHookVirtual(pGameConfig, hkChangeTeam, "CBasePlayer::ChangeTeam");
	LoadDHookVirtual(pGameConfig, hkShouldCollide, "CBaseEntity::ShouldCollide");
	LoadDHookVirtual(pGameConfig, hkPlayerSpawn, "CBasePlayer::Spawn");
	LoadDHookVirtual(pGameConfig, hkSetModel, "CBaseEntity::SetModel");
	LoadDHookVirtual(pGameConfig, hkAcceptInput, "CBaseEntity::AcceptInput");
	LoadDHookVirtual(pGameConfig, hkThink, "CBaseEntity::Think");
	LoadDHookVirtual(pGameConfig, hkEvent_Killed, "CBaseEntity::Event_Killed");
	LoadDHookVirtual(pGameConfig, hkKeyValue_char, "CBaseEntity::KeyValue_char");

	#if defined ENTPATCH_RELATION_TYPE
	LoadDHookVirtual(pGameConfig, hkIRelationType, "CBaseCombatCharacter::IRelationType");
	#endif

	#if defined ENTPATCH_PLAYER_ALLY
	LoadDHookVirtual(pGameConfig, hkIsPlayerAlly, "CAI_BaseNPC::IsPlayerAlly");
	#endif

	#if defined ENTPATCH_FIND_NAMED_ENTITY
	LoadDHookVirtual(pGameConfig, hkFindNamedEntity, "CSceneEntity::FindNamedEntity");
	LoadDHookVirtual(pGameConfig, hkFindNamedEntityClosest, "CSceneEntity::FindNamedEntityClosest");
	#endif

	#if defined ENTPATCH_SNIPER
	LoadDHookVirtual(pGameConfig, hkProtoSniperSelectSchedule, "CProtoSniper::SelectSchedule");
	#endif

	#if defined GAMEPATCH_ALLOW_FLASHLIGHT
	LoadDHookVirtual(pGameConfig, hkFAllowFlashlight, "CMultiplayRules::FAllowFlashlight");
	#endif

	#if defined GAMEPATCH_IS_MULTIPLAYER
	LoadDHookVirtual(pGameConfig, hkIsMultiplayer, "CMultiplayRules::IsMultiplayer");
	#endif

	#if defined GAMEPATCH_BLOCK_RESTOREWORLD
	LoadDHookVirtual(pGameConfig, hkRestoreWorld, "CBM_MP_GameRules::RestoreWorld");
	#endif

	#if defined GAMEPATCH_BLOCK_RESPAWNPLAYERS
	LoadDHookVirtual(pGameConfig, hkRespawnPlayers, "CBM_MP_GameRules::RespawnPlayers");
	#endif

	#if defined SRCCOOP_BLACKMESA
	LoadDHookVirtual(pGameConfig, hkOnTryPickUp, "CBasePickup::OnTryPickUp");
	#endif

	#if defined ENTPATCH_BM_ICHTHYOSAUR
	LoadDHookVirtual(pGameConfig, hkIchthyosaurIdleSound, "CNPC_Ichthyosaur::IdleSound");
	#endif

	#if defined ENTPATCH_BM_XENTURRET || defined ENTPATCH_BM_NIHILANTH
	LoadDHookVirtual(pGameConfig, hkHandleAnimEvent, "CBaseAnimating::HandleAnimEvent");
	#endif

	#if defined ENTPATCH_BM_XENTURRET || defined ENTPATCH_BM_NIHILANTH || defined ENTPATCH_BM_GONARCH
	LoadDHookVirtual(pGameConfig, hkRunAI, "CAI_BaseNPC::RunAI");
	#endif

	#if defined ENTPATCH_BM_FUNC_TRACKAUTOCHANGE || defined ENTPATCH_BM_FUNC_TRACKTRAIN
	LoadDHookVirtual(pGameConfig, hkBlocked, "CBaseEntity::Blocked");
	#endif

	#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	LoadDHookVirtual(pGameConfig, hkCreateRagdollEntity, "CBasePlayer::CreateRagdollEntity");
	#endif

	// Detours
	
	#if defined PLAYERPATCH_SETSUITUPDATE
	LoadDHookDetour(pGameConfig, hkSetSuitUpdate, "CBasePlayer::SetSuitUpdate", Hook_SetSuitUpdate, Hook_SetSuitUpdatePost);
	#endif
	
	#if defined ENTPATCH_GOALENTITY_RESOLVENAMES
	LoadDHookDetour(pGameConfig, hkResolveNames, "CAI_GoalEntity::ResolveNames", Hook_ResolveNames, Hook_ResolveNamesPost);
	#endif
	
	#if defined ENTPATCH_GOAL_LEAD
	LoadDHookDetour(pGameConfig, hkCanSelectSchedule, "CAI_LeadBehavior::CanSelectSchedule", Hook_CanSelectSchedule);
	#endif

	#if defined ENTPATCH_SETPLAYERAVOIDSTATE
	LoadDHookDetour(pGameConfig, hkSetPlayerAvoidState, "CAI_BaseNPC::SetPlayerAvoidState", Hook_SetPlayerAvoidState);
	#endif

	#if defined GAMEPATCH_UTIL_GETLOCALPLAYER
	LoadDHookDetour(pGameConfig, hkUTIL_GetLocalPlayer, "UTIL_GetLocalPlayer", Hook_UTIL_GetLocalPlayer);
	#endif

	#if defined PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	LoadDHookDetour(pGameConfig, hkPickup_ForcePlayerToDropThisObject, "Pickup_ForcePlayerToDropThisObject", Hook_ForcePlayerToDropThisObject);
	#endif

	#if defined SRCCOOP_BLACKMESA
	if (g_serverOS == OS_Linux)
	{
		LoadDHookDetour(pGameConfig, hkAccumulatePose, "CBoneSetup::AccumulatePose", Hook_AccumulatePose);
	}
	#endif

	// Init SDKCalls for classdef
	InitClassdef(pGameConfig);
	
	CloseHandle(pGameConfig);
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	#if defined CHECK_ENGINE
	char szCompileTarget[] = CHECK_ENGINE
	{
		Format(error, err_max, "This build of SourceCoop was compiled for %s", szCompileTarget);
		return APLRes_Failure;
	}
	#endif

	RegNatives();
	RegPluginLibrary(SRCCOOP_LIBRARY);
	return APLRes_Success;
}

public void OnPluginStart()
{
	LoadGameData();
	LoadTranslations("common.phrases");
	InitDebugLog("sourcecoop_debug", "SRCCOOP", ADMFLAG_ROOT);
	CreateConVar("sourcecoop_version", SRCCOOP_VERSION, _, FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);
	g_pConvarCoopTeam = CreateConVar("sourcecoop_team", "scientist", "Sets which team to use in TDM mode. Valid values are [marines] or [scientist] or a team number. Setting anything else will not manage teams.");
	#if defined GAMEPATCH_TEAMSELECT_UI
	g_pConvarDisableTeamSelect = CreateConVar("sourcecoop_disable_teamselect", "1", "Whether to skip the team select screen and spawn in instantly.", _, true, 0.0, true, 1.0);
	#endif
	g_pConvarCoopRespawnTime = CreateConVar("sourcecoop_respawntime", "2.0", "Sets player respawn time in seconds. This can only be used for making respawn times quicker, not longer. Set to 0 to use the game's default.", _, true, 0.0);
	g_pConvarWaitPeriod = CreateConVar("sourcecoop_start_wait_period", "15.0", "The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.", _, true, 0.0);
	g_pConvarEndWaitPeriod = CreateConVar("sourcecoop_end_wait_period", "60.0", "The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.", _, true, 0.0);
	g_pConvarEndWaitFactor = CreateConVar("sourcecoop_end_wait_factor", "1.0", "Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).", _, true, 0.0, true, 1.0);
	g_pConvarHomeMap = CreateConVar("sourcecoop_homemap", "", "The map to return to after finishing a campaign/map.");
	g_pConvarEndWaitDisplayMode = CreateConVar("sourcecoop_end_wait_display_mode", "0", "Sets which method to show countdown. 0 is panel, 1 is hud text.", _, true, 0.0, true, 1.0);

	mp_friendlyfire = FindConVar("mp_friendlyfire");
	mp_flashlight = FindConVar("mp_flashlight");
	#if defined SRCCOOP_BLACKMESA
	sv_always_run = FindConVar("sv_always_run");
	#endif
	
	RegAdminCmd("sourcecoop_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegAdminCmd("sc_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegServerCmd("sourcecoop_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	RegServerCmd("sc_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	
	g_pLevelLump.Initialize();
	g_SpawnSystem.Initialize();
	CoopManager.Initialize();
	g_pInstancingManager.Initialize();
	g_pPostponedSpawns = CreateArray();
	g_pFeatureMap = new FeatureMap();
	EquipmentManager.Initialize();
	InitializeMenus();
	
	g_CoopMapStartFwd = new GlobalForward("OnCoopMapStart", ET_Ignore);
	g_CoopMapConfigLoadedFwd = new GlobalForward("OnCoopMapConfigLoaded", ET_Ignore, Param_Cell, Param_Cell);
	g_OnPlayerRagdollCreatedFwd = new GlobalForward("OnPlayerRagdollCreated", ET_Ignore, Param_Cell, Param_Cell);
	
	HookEvent("player_disconnect", Event_PlayerDisconnect);
	AddNormalSoundHook(PlayerSoundListener);

	#if defined SRCCOOP_BLACKMESA

	HookEvent("broadcast_teamsound", Event_BroadcastTeamsound, EventHookMode_Pre);
	AddTempEntHook("BlackMesa Shot", BlackMesaFireBulletsTEHook);
	UserMsg iIntroCredits = GetUserMessageId("IntroCredits");
	if (iIntroCredits != INVALID_MESSAGE_ID)
	{
		HookUserMessage(iIntroCredits, Hook_IntroCreditsMsg, true);
	}

	#endif // SRCCOOP_BLACKMESA
	
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			OnClientPutInServer(i);
			if (IsClientAuthorized(i))
			{
				OnClientAuthorized(i, "");
			}
		}
	}
}

#pragma dynamic ENTITYSTRING_LENGTH

public MRESReturn Hook_OnLevelInit(DHookReturn hReturn, DHookParam hParams)
{
	if (!IsDedicatedServer() && MaxClients == 1)
	{
		SetFailState("Singleplayer detected, unloading SourceCoop (ignore this)");
	}
	
	OnMapEnd(); // this does not always get called, so call it here

	char szMapName[MAX_MAPNAME];
	hParams.GetString(1, szMapName, sizeof(szMapName));
	g_szPrevMapName = g_szMapName;
	g_szMapName = szMapName;

	static char szMapEntities[ENTITYSTRING_LENGTH];
	hParams.GetString(2, szMapEntities, sizeof(szMapEntities));

	// save original string for dumps
	g_szEntityString = szMapEntities;

	if (CoopManager.OnLevelInit(szMapEntities))
	{
		hParams.SetString(2, szMapEntities);
		return MRES_ChangedHandled;
	}

	return MRES_Ignored;
}

public void OnMapStart()
{
	CoopManager.OnMapStart();
	
	#if defined GAMEPATCH_IS_MULTIPLAYER
	DHookGamerules(hkIsMultiplayer, false, _, Hook_IsMultiplayer);
	#endif
	
	#if defined GAMEPATCH_BLOCK_RESTOREWORLD
	DHookGamerules(hkRestoreWorld, false, _, Hook_RestoreWorld);
	#endif

	#if defined GAMEPATCH_BLOCK_RESPAWNPLAYERS
	DHookGamerules(hkRespawnPlayers, false, _, Hook_RespawnPlayers);
	#endif

	#if defined GAMEPATCH_ALLOW_FLASHLIGHT
	DHookGamerules(hkFAllowFlashlight, false, _, Hook_FAllowFlashlight);
	#endif
	
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
	#if defined SRCCOOP_BLACKMESA

	if (CoopManager.IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT))
	{
		CBaseEntity pGameEquip = CBaseEntity.Create("game_player_equip");	// will spawn players with nothing if it exists
		if (pGameEquip.IsValid())
		{
			if (!CoopManager.IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT))
			{
				pGameEquip.SetSpawnFlags(SF_PLAYER_EQUIP_STRIP_SUIT);
			}
			pGameEquip.Spawn();
		}
	}
	if (CoopManager.IsFeatureEnabled(FT_DISABLE_CANISTER_DROPS))
	{
		CBaseEntity pGameGamerules = CBaseEntity.Create("game_mp_gamerules");
		if (pGameGamerules.IsValid())
		{
			pGameGamerules.Spawn();
			pGameGamerules.AcceptInputStr("DisableCanisterDrops");
			pGameGamerules.Kill();
		}
	}
	
	PrecacheScriptSound("HL2Player.SprintStart");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx80.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx90.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.mdl");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.phy");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.sw.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.vvd");

	#endif // SRCCOOP_BLACKMESA
}

public void OnClientPutInServer(int client)
{
	CBasePlayer pPlayer = CBasePlayer(client);
	
	PlayerPatch_OnClientPutInServer(pPlayer);
	CoopManager.OnClientPutInServer(pPlayer);
	g_pInstancingManager.OnClientPutInServer(client);
	
	SDKHook(client, SDKHook_PreThinkPost, Hook_PlayerPreThinkPost);
	SDKHook(client, SDKHook_PreThink, Hook_PlayerPreThink);
	SDKHook(client, SDKHook_SpawnPost, Hook_PlayerSpawnPost);
	SDKHook(client, SDKHook_TraceAttack, Hook_PlayerTraceAttack);
	SDKHook(client, SDKHook_OnTakeDamage, Hook_PlayerTakeDamage);
	SDKHook(client, SDKHook_WeaponEquipPost, Hook_PlayerWeaponEquipPost);
	DHookEntity(hkChangeTeam, false, client, _, Hook_PlayerChangeTeam);
	DHookEntity(hkChangeTeam, true, client, _, Hook_PlayerChangeTeamPost);
	DHookEntity(hkShouldCollide, false, client, _, Hook_PlayerShouldCollide);
	DHookEntity(hkPlayerSpawn, false, client, _, Hook_PlayerSpawn);
	DHookEntity(hkAcceptInput, false, client, _, Hook_PlayerAcceptInput);
	DHookEntity(hkEvent_Killed, false, client, _, Hook_PlayerKilled);
	DHookEntity(hkEvent_Killed, true, client, _, Hook_PlayerKilledPost);
	
	#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	DHookEntity(hkCreateRagdollEntity, false, client, _, Hook_CreateRagdollEntity);
	#endif
	
	GreetPlayer(client);
}

public void OnClientAuthorized(int client, const char[] auth)
{
	int sid = GetSteamAccountID(client);
	if (sid)
	{
		IntToString(sid, g_szSteamIds[client], sizeof(g_szSteamIds[]));
	}
}

public void OnClientDisconnect(int client)
{
	CBasePlayer pPlayer = CBasePlayer(client);
	PlayerPatch_OnClientDisconnect(pPlayer);
}

public void OnClientDisconnect_Post(int client)
{
	g_pInstancingManager.OnClientDisconnect(client);
	SurvivalManager.GameOverCheck();
	g_szSteamIds[client] = "";
	g_bPostTeamSelect[client] = false;
}

public void Event_PlayerDisconnect(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	int iClient = GetClientOfUserId(GetEventInt(hEvent, "userid"));
	if (strlen(g_szSteamIds[iClient]))
	{
		EquipmentManager.Clear(g_szSteamIds[iClient]);
	}
}

public void OnMapEnd()
{
	g_pLevelLump.RevertConvars();
	g_bMapStarted = false;
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	if (g_bTempDontHookEnts)
	{
		return;
	}

	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (!pEntity.IsValid())
	{
		return;
	}

	SDKHook(iEntIndex, SDKHook_Spawn, Hook_FixupBrushModels);
	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EntitySpawnPost);
	
	bool isNPC = pEntity.IsClassNPC();

	if (isNPC)
	{
		#if defined ENTPATCH_CUSTOM_NPC_MODELS
		DHookEntity(hkKeyValue_char, true, iEntIndex, _, Hook_BaseNPCKeyValuePost);
		#endif
		
		#if defined ENTPATCH_UPDATE_ENEMY_MEMORY
		DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_BaseNPCAcceptInput);
		#endif
		
		#if defined ENTPATCH_SNIPER
		if (strcmp(szClassname, "npc_sniper", false) == 0)
		{
			DHookEntity(hkProtoSniperSelectSchedule, false, iEntIndex, _, Hook_ProtoSniperSelectSchedule);
			return;
		}
		#endif

		#if defined SRCCOOP_BLACKMESA
		SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_BaseNPCSpawnPost);
		
		if (strncmp(szClassname, "npc_human_scientist", 19) == 0)
		{
			#if defined ENTPATCH_RELATION_TYPE
			DHookEntity(hkIRelationType, true, iEntIndex, _, Hook_ScientistIRelationType);
			#endif
			
			#if defined ENTPATCH_PLAYER_ALLY
			DHookEntity(hkIsPlayerAlly, true, iEntIndex, _, Hook_IsPlayerAlly);
			#endif
			return;
		}

		#if defined ENTPATCH_PLAYER_ALLY
		if (strcmp(szClassname, "npc_human_security") == 0)
		{
			DHookEntity(hkIsPlayerAlly, true, iEntIndex, _, Hook_IsPlayerAlly);
			return;
		}
		#endif
		#endif // SRCCOOP_BLACKMESA

		#if defined ENTPATCH_BM_XENTURRET
		if (strcmp(szClassname, "npc_xenturret", false) == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_XenTurretSpawnPost);
			DHookEntity(hkProtoSniperSelectSchedule, false, iEntIndex, _, Hook_XenTurretSelectSchedule);
			DHookEntity(hkHandleAnimEvent, false, iEntIndex, _, Hook_XenTurretHandleAnimEvent);
			DHookEntity(hkHandleAnimEvent, true, iEntIndex, _, Hook_XenTurretHandleAnimEventPost);
			DHookEntity(hkRunAI, false, iEntIndex, _, Hook_XenTurretRunAI);
			DHookEntity(hkRunAI, true, iEntIndex, _, Hook_XenTurretRunAIPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_ICHTHYOSAUR
		if (strcmp(szClassname, "npc_ichthyosaur") == 0)
		{
			DHookEntity(hkIchthyosaurIdleSound, false, iEntIndex, _, Hook_IchthyosaurIdleSound);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_GARGANTUA
		if (strcmp(szClassname, "npc_gargantua") == 0)
		{
			DHookEntity(hkAcceptInput, true, iEntIndex, _, Hook_GargAcceptInputPost);
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_GargSpawnPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_HOUNDEYE
		if (strncmp(szClassname, "npc_houndeye", 12) == 0)
		{
			DHookEntity(hkThink, false, iEntIndex, _, Hook_HoundeyeThink);
			DHookEntity(hkThink, true, iEntIndex, _, Hook_HoundeyeThinkPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_GONARCH
		if (strcmp(szClassname, "npc_gonarch") == 0)
		{
			DHookEntity(hkRunAI, false, iEntIndex, _, Hook_GonarchRunAI);
			DHookEntity(hkRunAI, true, iEntIndex, _, Hook_GonarchRunAIPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_NIHILANTH
		if (strcmp(szClassname, "npc_nihilanth") == 0)
		{
			DHookEntity(hkRunAI, false, iEntIndex, _, Hook_NihilanthRunAI);
			DHookEntity(hkRunAI, true, iEntIndex, _, Hook_NihilanthRunAIPost);
			DHookEntity(hkHandleAnimEvent, false, iEntIndex, _, Hook_NihilanthHandleAnimEvent);
			DHookEntity(hkHandleAnimEvent, true, iEntIndex, _, Hook_NihilanthHandleAnimEventPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_PUFFBALLFUNGUS
		if (strcmp(szClassname, "npc_puffballfungus") == 0)
		{	
			SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_PuffballFungusDmg);
			return;
		}
		#endif
	}
	else // !isNPC
	{
		#if defined ENTPATCH_TRIGGER_CHANGELEVEL
		if (strcmp(szClassname, "trigger_changelevel") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ChangelevelSpawn);
			return;
		}
		#endif

		#if defined ENTPATCH_TRIGGER_AUTOSAVE
		if (strcmp(szClassname, "trigger_autosave") == 0)
		{
			SDKHook(iEntIndex, SDKHook_Spawn, Hook_AutosaveSpawn);
			return;
		}
		#endif

		#if defined ENTPATCH_POINT_TELEPORT
		if (strcmp(szClassname, "point_teleport") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_PointTeleportAcceptInput);
			return;
		}
		#endif

		#if defined ENTPATCH_POINT_VIEWCONTROL
		if (strcmp(szClassname, "point_viewcontrol") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_PointViewcontrolAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_PLAYER_SPEEDMOD
		if (strcmp(szClassname, "player_speedmod") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_SpeedmodAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_POINT_CLIENTCOMMAND
		if (strcmp(szClassname, "point_clientcommand") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_ClientCommandAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_POINT_SERVERCOMMAND_CHANGELEVEL
		if (strcmp(szClassname, "point_servercommand") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_ServerCommandAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_ENV_ZOOM
		if (strcmp(szClassname, "env_zoom") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvZoomAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_ENV_CREDITS
		if (strcmp(szClassname, "env_credits") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvCreditsAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_ENV_SPRITE
		if (strcmp(szClassname, "env_sprite") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EnvSpriteSpawnPost);
			return;
		}
		#endif
		
		#if defined ENTPATCH_AI_SCRIPT_CONDITIONS
		if (strcmp(szClassname, "ai_script_conditions") == 0)
		{
			DHookEntity(hkThink, false, iEntIndex, _, Hook_AIConditionsThink);
			return;
		}
		#endif
		
		#if defined ENTPATCH_FUNC_ROTATING
		if (strcmp(szClassname, "func_rotating") == 0)
		{
			CreateTimer(30.0, Timer_FixRotatingAngles, pEntity, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
			return;
		}
		#endif
		
		#if defined ENTPATCH_PLAYER_LOADSAVED
		if (strcmp(szClassname, "player_loadsaved") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_LoadSavedAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_LOGIC_AUTOSAVE_SURVIVAL_RESPAWN
		if (strcmp(szClassname, "logic_autosave") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_LogicAutosaveAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_GAME_END
		if (strcmp(szClassname, "game_end") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_GameEndAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_FIND_NAMED_ENTITY
		if ((strcmp(szClassname, "instanced_scripted_scene", false) == 0) ||
				(strcmp(szClassname, "logic_choreographed_scene", false) == 0) ||
				(strcmp(szClassname, "scripted_scene", false) == 0))
		{
			DHookEntity(hkFindNamedEntity, true, iEntIndex, _, Hook_FindNamedEntity);
			DHookEntity(hkFindNamedEntityClosest, true, iEntIndex, _, Hook_FindNamedEntity);
			return;
		}
		#endif

		#if defined ENTPATCH_REMOVE_BONE_FOLLOWERS
		if (strcmp(szClassname, "phys_bone_follower") == 0)
		{
			SDKHook(iEntIndex, SDKHook_VPhysicsUpdatePost, Hook_BoneFollowerVPhysicsUpdatePost);
			return;
		}
		#endif

		#if defined ENTPATCH_WEAPON_MODELS
		if (pEntity.IsClassWeapon())
		{
			DHookEntity(hkSetModel, false, iEntIndex, _, Hook_WeaponSetModel);
			return;
		}
		#endif
		
		// ToDo: support all games
		#if defined SRCCOOP_BLACKMESA
		if (strncmp(szClassname, "item_", 5) == 0)
		{
			if (pEntity.IsPickupItem())
			{
				if (CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
				{
					SDKHook(iEntIndex, SDKHook_Spawn, Hook_ItemSpawnDelay);
				}
				if (strcmp(szClassname, "item_weapon_snark") == 0)
				{
					SDKHook(iEntIndex, SDKHook_OnTakeDamagePost, Hook_OnItemSnarkDamagePost);
				}
				if (strcmp(szClassname, "item_suit") == 0)
				{
					DHookEntity(hkOnTryPickUp, true, iEntIndex, _, Hook_OnEquipmentTryPickUpPost);
					HookSingleEntityOutput(iEntIndex, "OnPlayerPickup", Hook_SuitTouchPickup, true);
				}
			}
			return;
		}
		#endif

		#if defined ENTPATCH_BM_PROP_CHARGERS
		if (strcmp(szClassname, "prop_hev_charger") == 0 || strcmp(szClassname, "prop_radiation_charger") == 0)
		{
			//DHookEntity(hkThink, false, iEntIndex, _, Hook_PropChargerThink);
			//DHookEntity(hkThink, true, iEntIndex, _, Hook_PropChargerThinkPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_MISC_MARIONETTIST
		if (strcmp(szClassname, "misc_marionettist") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_MarionettistAcceptInput);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_MUSIC_TRACK
		if (strcmp(szClassname, "music_track") == 0)
		{
			DHookEntity(hkThink, false, iEntIndex, _, Hook_MusicTrackThink);
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_MusicTrackAceptInput);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_FUNC_TRACKAUTOCHANGE
		if (strcmp(szClassname, "func_trackautochange") == 0)
		{
			DHookEntity(hkBlocked, false, iEntIndex, _, Hook_TrackChangeBlocked);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_FUNC_TRACKTRAIN
		if (strcmp(szClassname, "func_tracktrain") == 0)
		{
			DHookEntity(hkBlocked, false, iEntIndex, _, Hook_TrackTrainBlocked);
			return;
		}
		#endif
		
		// if some explosions turn out to be damaging all players except one, this is the fix
		// if (strcmp(szClassname, "env_explosion") == 0)
		// {
		// 	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ExplosionSpawn);
		// 	return;
		// }
	}
}

public void OnEntityDestroyed(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity.IsValid())
	{
		static char szClassname[MAX_CLASSNAME];
		pEntity.GetClassname(szClassname, sizeof(szClassname));
		
		#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
		if (strcmp(szClassname, "prop_ragdoll") == 0)
		{
			OnEntityDestroyed_Ragdoll(pEntity);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_MISC_MARIONETTIST
		if (strcmp(szClassname, "misc_marionettist") == 0)
		{
			OnEntityDestroyed_Marionettist(pEntity);
			return;
		}
		#endif
	}
}

public void Hook_EntitySpawnPost(int iEntIndex)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);

		#if defined SRCCOOP_BLACKMESA
		// fix linux physics crashes
		if (g_serverOS == OS_Linux)
		{
			static char szModel[PLATFORM_MAX_PATH];
			if (pEntity.GetModelName(szModel, sizeof(szModel)) && strncmp(szModel, "models/gibs/humans/", 19) == 0)
			{
				SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_NoDmg);
			}
		}
		#endif
		
		// find and hook output hooks for entity
		if (!g_pCoopManagerData.m_bStarted)
		{
			ArrayList pOutputHookList = g_pLevelLump.GetOutputHooksForEntity(pEntity);
			if (pOutputHookList.Length > 0)
			{
				if (pEntity.IsClassname("logic_auto"))
				{
					// do not let it get killed, so the output can fire later
					int iSpawnFlags = pEntity.GetSpawnFlags();
					if (iSpawnFlags & SF_AUTO_FIREONCE)
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
	if (g_bMapStarted)
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
	if (pEntity.IsValid())
	{
		SDKHook(pEntity.GetEntIndex(), SDKHook_SpawnPost, Hook_Instancing_ItemSpawn);
		g_bIsMultiplayerOverride = false; // IsMultiplayer=false will spawn items with physics
		pEntity.Spawn();
		g_bIsMultiplayerOverride = true;
	}
}

public Action OutputCallbackForDelay(const char[] output, int caller, int activator, float delay)
{
	if (g_pCoopManagerData.m_bStarted)
	{
		return Plugin_Continue;
	}
	FireOutputData pFireOutputData;
	strcopy(pFireOutputData.m_szName, sizeof(pFireOutputData.m_szName), output);
	pFireOutputData.m_pCaller = CBaseEntity(caller);
	pFireOutputData.m_pActivator = CBaseEntity(activator);
	pFireOutputData.m_flDelay = delay;
	CoopManager.AddDelayedOutput(pFireOutputData);
	
	if (pFireOutputData.m_pCaller.IsValid())
	{
		// stop from deleting itself
		if (pFireOutputData.m_pCaller.IsClassname("trigger_once"))
		{
			RequestFrame(RequestStopThink, pFireOutputData.m_pCaller);
		}
		else if (pFireOutputData.m_pCaller.IsClassname("logic_relay"))
		{
			int sf = pFireOutputData.m_pCaller.GetSpawnFlags();
			if (sf & SF_REMOVE_ON_FIRE)
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
	if (g_pCoopManagerData.m_bStarted)
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
	if (pEntity.IsValid())
	{
		pEntity.SetNextThinkTick(0);
	}
}

public Action Event_BroadcastTeamsound(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// block multiplayer announcer
		hEvent.BroadcastDisabled = true;
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

public MRESReturn Hook_OnEquipmentTryPickUpPost(int _this, Handle hReturn, Handle hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_KEEP_EQUIPMENT))
	{
		bool bPickedUp = DHookGetReturn(hReturn);
		if (bPickedUp)
		{
			CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
			if (pPlayer.IsClassPlayer())
			{
				CBaseEntity pItem = CBaseEntity(_this);
				char szClass[MAX_CLASSNAME];
				pItem.GetClassname(szClass, sizeof(szClass));
				g_SpawnSystem.AddSpawnItem(szClass);
			}
		}
	}
	return MRES_Ignored;
}

public void Hook_PlayerWeaponEquipPost(int client, int weapon)
{
	if (CoopManager.IsFeatureEnabled(FT_KEEP_EQUIPMENT))
	{
		CBaseEntity pItem = CBaseEntity(weapon);
		char szClass[MAX_CLASSNAME];
		pItem.GetClassname(szClass, sizeof(szClass));
		g_SpawnSystem.AddSpawnItem(szClass);
	}
}

public MRESReturn Hook_RestoreWorld(Handle hReturn)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// disable gamerules resetting the world on 'round start', this caused crashes
		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

public MRESReturn Hook_RespawnPlayers(Handle hReturn)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// disable gamerules respawning players on 'round start'
		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

void GreetPlayer(int client)
{
	if (CoopManager.IsFeatureEnabled(FT_SHOW_WELCOME_MESSAGE))
	{
		Msg(client, "This server runs SourceCoop version %s.\nPress %s=%s or type %s/coopmenu%s for extra settings.", SRCCOOP_VERSION, SRCCOOP_CHAT_COLOR_SEC, SRCCOOP_CHAT_COLOR_PRI, SRCCOOP_CHAT_COLOR_SEC, SRCCOOP_CHAT_COLOR_PRI);
	}
}