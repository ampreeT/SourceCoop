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

void LoadGameData()
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_GAMEDATA_NAME);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config: \"%s\"", SRCCOOP_GAMEDATA_NAME);
	
	g_serverOS = view_as<OperatingSystem>(pGameConfig.GetOffset("_OS_Detector_"));

	// Init SDKCalls for classdef
	InitClassdef(pGameConfig);

	LoadDHookVirtual(pGameConfig, hkLevelInit, "CServerGameDLL::LevelInit");
	if (hkLevelInit.HookRaw(Hook_Pre, IServerGameDLL.Get().GetAddress(), Hook_OnLevelInit) == INVALID_HOOK_ID)
		SetFailState("Could not hook CServerGameDLL::LevelInit");
	
	LoadDHookVirtual(pGameConfig, hkChangeTeam, "CBasePlayer::ChangeTeam");
	LoadDHookVirtual(pGameConfig, hkShouldCollide, "CBaseEntity::ShouldCollide");
	LoadDHookVirtual(pGameConfig, hkPlayerSpawn, "CBasePlayer::Spawn");
	LoadDHookVirtual(pGameConfig, hkSetModel, "CBaseEntity::SetModel");
	LoadDHookVirtual(pGameConfig, hkAcceptInput, "CBaseEntity::AcceptInput");
	LoadDHookVirtual(pGameConfig, hkThink, "CBaseEntity::Think");
	LoadDHookVirtual(pGameConfig, hkUpdateOnRemove, "CBaseEntity::UpdateOnRemove");
	LoadDHookVirtual(pGameConfig, hkEvent_Killed, "CBaseEntity::Event_Killed");
	LoadDHookVirtual(pGameConfig, hkKeyValue_char, "CBaseEntity::KeyValue_char");
	
	#if defined SRCCOOP_BLACKMESA
	LoadDHookDetour(pGameConfig, hkGiveDefaultItems, "*Player::GiveDefaultItems", Hook_GiveDefaultItems);
	#endif

	#if defined ENTPATCH_PLAYER_ALLY
	LoadDHookVirtual(pGameConfig, hkIsPlayerAlly, "CAI_BaseNPC::IsPlayerAlly");
	#endif

	#if defined ENTPATCH_NAVIGATION_URGENT
	LoadDHookVirtual(pGameConfig, hkIsNavigationUrgent, "CAI_BaseNPC::IsNavigationUrgent");
	#endif

	#if defined ENTPATCH_NPC_DIALOGUE
	LoadDHookVirtual(pGameConfig, hkFindNamedEntity, "CSceneEntity::FindNamedEntity");
	LoadDHookVirtual(pGameConfig, hkFindNamedEntityClosest, "CSceneEntity::FindNamedEntityClosest");
	LoadDHookDetour(pGameConfig, hkExpresserHostDoModifyOrAppendCriteria, "CAI_ExpresserHost_NPC_DoModifyOrAppendCriteria", _, Hook_ExpresserHost_DoModifyOrAppendCriteriaPost);
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

	#if defined ENTPATCH_FUNC_TRACKAUTOCHANGE || defined ENTPATCH_FUNC_TRACKTRAIN
	LoadDHookVirtual(pGameConfig, hkBlocked, "CBaseEntity::Blocked");
	#endif
	
	#if defined ENTPATCH_NPC_RUNTASK
	LoadDHookVirtual(pGameConfig, hkBaseNpcRunTask, "CAI_BaseNPC::RunTask");
	#endif
	
	#if defined ENTPATCH_BM_PROP_CHARGERS
	LoadDHookVirtual(pGameConfig, hkPropChargerThink, "CPropChargerBase::ChargerThink");
	#endif
	
	#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	LoadDHookVirtual(pGameConfig, hkCreateRagdollEntity, "CBasePlayer::CreateRagdollEntity");
	#endif

	#if defined PLAYERPATCH_RESTORE_MP_FORCERESPAWN
	LoadDHookVirtual(pGameConfig, hkForceRespawn, "CBasePlayer::ForceRespawn");
	#endif

	#if defined PLAYERPATCH_OVERRIDE_DEATH_OBSMODE
	LoadDHookVirtual(pGameConfig, hkStartObserverMode, "CBasePlayer::StartObserverMode");
	#endif
	
	#if defined PLAYERPATCH_HITREG
	LoadDHookVirtual(pGameConfig, hkPlayerWeaponShootPosition, "CBasePlayer::Weapon_ShootPosition");
	#endif

	#if defined ENTPATCH_BM_SP_WEAPONS
	LoadDHookVirtual(pGameConfig, hkBaseCombatWeaponDeploy, "CBaseCombatWeapon::Deploy");
	LoadDHookVirtual(pGameConfig, hkBaseCombatWeaponItemPostFrame, "CBaseCombatWeapon::ItemPostFrame");
	LoadDHookVirtual(pGameConfig, hkBaseCombatWeaponPrimaryAttack, "CBaseCombatWeapon::PrimaryAttack");
	LoadDHookVirtual(pGameConfig, hkBaseCombatWeaponGetPrimaryAttackActivity, "CBaseCombatWeapon::GetPrimaryAttackActivity");
	LoadDHookVirtual(pGameConfig, hkBaseCombatWeaponGetDrawActivity, "CBaseCombatWeapon::GetDrawActivity");
	LoadDHookVirtual(pGameConfig, hkWeaponCrossbowFireBolt, "CWeapon_Crossbow::FireBolt");
	LoadDHookDetour(pGameConfig, hkToggleIronsights, "CBlackMesaBaseWeaponIronSights::ToggleIronSights", Hook_ToggleIronsights);
	LoadDHookDetour(pGameConfig, hkTauFireBeam, "CWeapon_Tau::FireBeam", Hook_TauFireBeam, Hook_TauFireBeamPost);
	LoadDHookDetour(pGameConfig, hkParamsManagerInitInstances, "CParamsManager::InitInstances", Hook_CParamsManager_InitInstances);
	#endif
	
	#if defined SRCCOOP_BLACKMESA
	LoadDHookDetour(pGameConfig, hkStartLagCompensation, "CLagCompensationManager::StartLagCompensation", Hook_StartLagCompensation);
	g_iUserCmdOffset = pGameConfig.GetOffset("CBasePlayer::GetCurrentUserCommand");
	#endif
	
	#if defined PLAYERPATCH_SUIT_SOUNDS
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

	#if defined ENTPATCH_NPC_SLEEP
	LoadDHookDetour(pGameConfig, hkBaseNpcUpdateSleepState, "CAI_BaseNPC::UpdateSleepState", Hook_BaseNpcUpdateSleepState);
	#endif

	#if defined GAMEPATCH_UTIL_GETLOCALPLAYER
	LoadDHookDetour(pGameConfig, hkUTIL_GetLocalPlayer, "UTIL_GetLocalPlayer", Hook_UTIL_GetLocalPlayer);
	#endif

	#if defined PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
	LoadDHookDetour(pGameConfig, hkPickup_ForcePlayerToDropThisObject, "Pickup_ForcePlayerToDropThisObject", Hook_ForcePlayerToDropThisObject);
	#endif

	#if defined ENTPATCH_NPC_THINK_LOCALPLAYER
	LoadDHookDetour(pGameConfig, hkPhysics_RunThinkFunctions, "Physics_RunThinkFunctions", Hook_Physics_RunThinkFunctions);
	#endif

	#if defined ENTPATCH_BM_DISSOLVE
	LoadDHookDetour(pGameConfig, hkDissolve, "CBaseAnimating::Dissolve", Hook_Dissolve);
	#endif
	
	#if defined GAMEPATCH_UTIL_FINDCLIENT
	if (g_serverOS == OS_Windows)
	{
		LoadDHookDetour(pGameConfig, hkUtilFindClientInPVSGuts, "UTIL_FindClientInPVSGuts", Hook_UTIL_FindClient);
	}
	else if (g_serverOS == OS_Linux)
	{
		// `UTIL_FindClientInPVSGuts` is inlined on Linux into these functions.
		LoadDHookDetour(pGameConfig, hkUtilFindClientInPVS, "UTIL_FindClientInPVS", Hook_UTIL_FindClient);
		LoadDHookDetour(pGameConfig, hkUtilFindClientInVisibilityPVS, "UTIL_FindClientInVisibilityPVS", Hook_UTIL_FindClient);
	}
	#endif

	#if defined ENTPATCH_SCRIPTED_SEQUENCE
	LoadDHookDetour(pGameConfig, hkScriptedSequenceStartScript, "CAI_ScriptedSequence::StartScript", Hook_ScriptedSequenceStartScript);
	#endif

	#if defined GAMEPATCH_PREDICTED_EFFECTS
	LoadDHookDetour(pGameConfig, hkIgnorePredictionCull, "CRecipientFilter::IgnorePredictionCull", Hook_IgnorePredictionCull);
	LoadDHookVirtual(pGameConfig, hkDispatchEffect, "CTempEntsSystem::DispatchEffect");
	if (hkDispatchEffect.HookRaw(Hook_Pre, IServerTools.Get().GetTempEntsSystem(), Hook_DispatchEffect) == INVALID_HOOK_ID)
		SetFailState("Could not hook CTempEntsSystem::DispatchEffect");
	#endif

	#if defined SRCCOOP_BLACKMESA
	if (g_serverOS == OS_Linux)
	{
		LoadDHookDetour(pGameConfig, hkAccumulatePose, "CBoneSetup::AccumulatePose", Hook_AccumulatePose);
		LoadDHookDetour(pGameConfig, hkTestGroundMove, "CAI_MoveProbe::TestGroundMove", Hook_TestGroundMove);
	}
	#endif

	// Memory Patches
	g_pCoopModeMemPatchList = new ArrayList();
	
	#if defined ENTPATCH_LAGCOMP_POSE_PARAMS
	LoadMemPatch(pGameConfig, "CLagCompensationManager::RestoreEntityFromRecords::SetPoseParameter", true, false);
	LoadMemPatch(pGameConfig, "CLagCompensationManager::BacktrackEntity::SetPoseParameter", true, false);
	#endif
	
	#if defined GAMEPATCH_BM_GRAVITY
	g_pCoopModeMemPatchList.Push(LoadMemPatch(pGameConfig, "CBM_MP_GameRules::Activate::DoNotHardCodeGravityThnx", false, false));
	#endif
	
	pGameConfig.Close();
}

void ToggleGlobalPatches(bool bCoopMode)
{
	for (int i = 0; i < g_pCoopModeMemPatchList.Length; i++)
	{
		MemoryPatch pPatch = g_pCoopModeMemPatchList.Get(i);
		if (bCoopMode)
		{
			pPatch.Enable();
		}
		else
		{
			pPatch.Disable();
		}
	}
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
	Conf.Initialize(LoadSourceCoopConfig());
	LoadGameData();
	LoadTranslations("common.phrases"); /* reuse some translations (identified by use of capital letters) */
	LoadTranslations("srccoop.phrases");
	InitDebugLog("sourcecoop_debug", "SRCCOOP", ADMFLAG_ROOT);
	CreateConVar("sourcecoop_version", SRCCOOP_VERSION, _, FCVAR_SPONLY|FCVAR_REPLICATED|FCVAR_NOTIFY);
	g_pConvarCoopTeam = CreateConVar("sourcecoop_team", "3", "Sets which team to use in TDM mode. Accepts either a (partial) team name or a team number. Invalid values will disable team enforcement.");
	#if defined GAMEPATCH_TEAMSELECT_UI
	g_pConvarDisableTeamSelect = CreateConVar("sourcecoop_disable_teamselect", "1", "Whether to skip the team select screen and spawn in instantly.", _, true, 0.0, true, 1.0);
	#endif
	#if defined SRCCOOP_BLACKMESA
	g_pConvarCleanHud = CreateConVar("sourcecoop_clean_hud", "1", "Whether to hide non-essential hud elements. (Black Mesa: hides status at top of the screen)", _, true, 0.0, true, 1.0);
	#endif
	g_pConvarCoopRespawnTime = CreateConVar("sourcecoop_respawntime", "2.0", "Sets player respawn time in seconds.", _, true, 0.1);
	g_pConvarStartWaitPeriod = CreateConVar("sourcecoop_start_wait_period", "15.0", "The max number of seconds to wait since first player spawned in to start the map. ", _, true, 0.0);
	g_pConvarStartWaitMode = CreateConVar("sourcecoop_start_wait_mode", "2", "\n0 = The timer is not skipped (exceptions are maps without an intro_type or delayed outputs set).\n1 = The timer is skipped when all players enter the game.\n2 = The timer is skipped when player count matches the previous map's player count.", _, true, 0.0, true, 2.0);
	g_pConvarEndWaitPeriod = CreateConVar("sourcecoop_end_wait_period", "60.0", "The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.", _, true, 0.0);
	g_pConvarEndWaitFactor = CreateConVar("sourcecoop_end_wait_factor", "1.0", "Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).", _, true, 0.0, true, 1.0);
	g_pConvarHomeMap = CreateConVar("sourcecoop_homemap", "", "The map to return to after finishing a campaign/map.");
	g_pConvarEndWaitDisplayMode = CreateConVar("sourcecoop_end_wait_display_mode", "1", "Sets which method to show countdown. 0 is panel, 1 is hud text.", _, true, 0.0, true, 1.0);
	g_pConvarValidateSteamIds = CreateConVar("sourcecoop_validate_steamids", "0", "Validate players steam id's? Increases security at the cost of some functionality breakage when Steam goes down.\n At the time of writing this includes survival mode and equipment persistence.", _, true, 0.0, true, 1.0);
	g_pConvarDefaultMapConfig = CreateConVar("sourcecoop_default_config", "", "Default edt file, relative to game folder. This file is copied when starting a map with missing config as \"<sourcecoop_default_config_dest>/<mapname>.edt\". Useful for rapid development, or to enforce sourcecoop on all maps on the server.");
	g_pConvarDefaultMapConfigDest = CreateConVar("sourcecoop_default_config_dest", "maps", "Destination folder for \"sourcecoop_default_config\", relative to game folder. Must be a one of edt scan paths!");

	mp_friendlyfire = FindConVar("mp_friendlyfire");
	mp_flashlight = FindConVar("mp_flashlight");
	mp_forcerespawn = FindConVar("mp_forcerespawn");
	sv_ladder_useonly = FindConVar("sv_ladder_useonly");

	// Black Mesa ConVars.
	#if defined SRCCOOP_BLACKMESA
	sv_always_run = FindConVar("sv_always_run");
	sv_speed_sprint = FindConVar("sv_speed_sprint");
	sv_speed_walk = FindConVar("sv_speed_walk");
	sv_jump_long_enabled = FindConVar("sv_jump_long_enabled");
	sv_long_jump_manacost = FindConVar("sv_long_jump_manacost");
	#endif

	#if defined PLAYERPATCH_BM_CLIENT_PREDICTION
	sv_always_run.Flags &= ~FCVAR_REPLICATED;
	HookConVarChange(sv_always_run, Hook_ConVar_AlwaysRun);
	#endif
	
	RegAdminCmd("sourcecoop_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegAdminCmd("sc_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegServerCmd("sourcecoop_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	RegServerCmd("sc_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	RegServerCmd("sc_mkconfigs", Command_MakeConfigs, "Creates default SourceCoop configs for maps found in the maps directory, which are missing one.\n - Note: Please disable \"SlowScriptTimeout\" in SourceMod core.cfg beforehand as this could run for long time! (Restart required after editing)\n - Format: sc_mkconfigs <MAPFILTER> [CONFIRM]\n  MAPFILTER:\n    - filters the map names to include; use * for all; supports wildcards with * such as coop_*\n  CONFIRM:\n    - [0 = dry run, 1 = live run]");
	RegServerCmd("sc_importconfigs", Command_ImportConfigs, "Imports other formats of map configs into SourceCoop configs.\n - Note: Please disable \"SlowScriptTimeout\" in SourceMod core.cfg beforehand as this could run for long time! (Restart required after editing)\n - Format: sc_importconfigs <TYPE> <MAPFILTER> <UPDATE> <CREATE> [CONFIRM]\n  TYPE:\n    - The type of configs to import\n    - [stripper = Stripper:source]\n  MAPFILTER:\n    - filters the map names to import; use * for all; supports wildcards with * such as coop_*\n  UPDATE:\n    - [1 = allow updating SourceCoop configs that already exist, 0 = skips existing configs]\n  CREATE:\n    - [1 = attempts to create default SourceCoop config for a map if it's missing, 0 = prints a warning and skips if missing]\n  CONFIRM:\n    - [0 = dry run, 1 = live run]");
	RegServerCmd("sc_debug_missing_weapons", Command_Debug_MissingWeapons, "Internal debug command for testing missing weapons on spawn.");
	
	g_pLevelLump.Initialize();
	CCoopSpawnSystem.Initialize();
	CoopManager.Initialize();
	ItemInstancingManager.Initialize();
	g_pPostponedSpawns = CreateArray();
	g_pFeatureMap = new FeatureMap();
	g_pSpawnOptions.Reset();
	EquipmentManager.Initialize();
	DnManager.Initialize();
	InitializeMenus();
	PerformEntityClassAliasing();
	
	g_CoopMapStartFwd = new GlobalForward("SC_OnCoopMapStart", ET_Ignore);
	g_CoopMapConfigLoadedFwd = new GlobalForward("SC_OnCoopMapConfigLoaded", ET_Ignore, Param_Cell, Param_Cell);
	g_OnPlayerRagdollCreatedFwd = new GlobalForward("SC_OnPlayerRagdollCreated", ET_Ignore, Param_Cell, Param_Cell);
	g_OnCoopMapEndFwd = new GlobalForward("SC_OnCoopMapEnd", ET_Event, Param_String, Param_String, Param_Cell);
	
	HookUserMessage(GetUserMessageId("TextMsg"), UserMessage_TextMsg, true);
	HookEvent("player_death", Event_PlayerDeath, EventHookMode_Pre);
	HookEvent("entity_killed", Event_EntityKilled, EventHookMode_Pre);
	HookEvent("player_disconnect", Event_PlayerDisconnect);
	AddNormalSoundHook(PlayerSoundListener);
	AddCommandListener(PlayerCommandListener);

	#if defined SRCCOOP_BLACKMESA

	#if defined GAMEPATCH_BLOCK_ANNOUNCER
	HookEvent("broadcast_teamsound", Event_BroadcastTeamsound, EventHookMode_Pre);
	HookEvent("broadcast_killstreak", Event_BroadcastKillstreak, EventHookMode_Pre);
	#endif

	#if defined ENTPATCH_SNIPER
	AddTempEntHook("BlackMesa Shot", BlackMesaFireBulletsTEHook);
	#endif

	#if defined ENTPATCH_ENV_INTROCREDITS
	UserMsg iIntroCredits = GetUserMessageId("IntroCredits");
	if (iIntroCredits != INVALID_MESSAGE_ID)
	{
		HookUserMessage(iIntroCredits, Hook_IntroCreditsMsg, true);
	}
	#endif

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

	#if defined ENTPATCH_BARNACLE_PREDICTION
	HookEntityOutput("npc_barnacle", "OnGrab", Hook_Barnacle_OnGrab);
	HookEntityOutput("npc_barnacle", "OnRelease", Hook_Barnacle_OnRelease);
	#endif
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

	bool bCoopMode = CoopManager.OnLevelInit(szMapEntities);
	ToggleGlobalPatches(bCoopMode);

	// This ends up calling our hook for `CParamsManager::InitInstances`.
	#if defined ENTPATCH_BM_SP_WEAPONS
	ServerCommand("params_reload_server");
	#endif

	#if defined PLAYERPATCH_BM_CLIENT_PREDICTION
	SetMovementPredictionConVars();
	#endif

	if (bCoopMode)
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
		CItem pItem = g_pPostponedSpawns.Get(i);
		RequestFrame(SpawnPostponedItem, pItem);
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
	g_pLevelLump.ApplyLateConvars();

	#if defined SRCCOOP_BLACKMESA

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

	#if defined ENTPATCH_BM_XENTURRET
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx80.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx90.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.mdl");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.phy");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.sw.vtx");
	AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.vvd");
	#endif

	#endif // SRCCOOP_BLACKMESA
}

public void OnClientPutInServer(int client)
{
	GreetPlayer(client);

	if (IsFakeClient(client))
		return;
	
	if (!g_iPlayerCount++)
	{
		#if defined ENTPATCH_NPC_THINK_LOCALPLAYER
		// resume entity thinking
		hkPhysics_RunThinkFunctions.Disable(Hook_Pre, Hook_Physics_RunThinkFunctions);
		#endif
	}

	CCoopSpawnSystem.OnClientPutInServer(client);
	ItemInstancingManager.OnClientPutInServer(client);
	PlayerPatch_OnClientPutInServer(client);
	
	SDKHook(client, SDKHook_PreThinkPost, Hook_PlayerPreThinkPost);
	SDKHook(client, SDKHook_PreThink, Hook_PlayerPreThink);
	SDKHook(client, SDKHook_TraceAttack, Hook_PlayerTraceAttack);
	SDKHook(client, SDKHook_OnTakeDamage, Hook_PlayerTakeDamage);
	SDKHook(client, SDKHook_WeaponEquipPost, Hook_PlayerWeaponEquipPost);
	#if defined PLAYERPATCH_HITREG
	DHookEntity(hkPlayerWeaponShootPosition, true, client, _, Hook_PlayerWeaponShootPosition_Post);
	#endif
	DHookEntity(hkChangeTeam, false, client, _, Hook_PlayerChangeTeam);
	DHookEntity(hkChangeTeam, true, client, _, Hook_PlayerChangeTeamPost);
	DHookEntity(hkShouldCollide, false, client, _, Hook_PlayerShouldCollide);
	DHookEntity(hkPlayerSpawn, false, client, _, Hook_PlayerSpawn);
	DHookEntity(hkPlayerSpawn, true, client, _, Hook_PlayerSpawnPost);
	DHookEntity(hkAcceptInput, false, client, _, Hook_PlayerAcceptInput);
	DHookEntity(hkEvent_Killed, false, client, _, Hook_PlayerKilled);
	DHookEntity(hkEvent_Killed, true, client, _, Hook_PlayerKilledPost);

	#if defined PLAYERPATCH_RESTORE_MP_FORCERESPAWN
	DHookEntity(hkForceRespawn, false, client, _, Hook_PlayerForceRespawn);
	#endif

	#if defined PLAYERPATCH_OVERRIDE_DEATH_OBSMODE
	DHookEntity(hkStartObserverMode, false, client, _, Hook_PlayerStartObserverMode);
	#endif
	
	#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	DHookEntity(hkCreateRagdollEntity, false, client, _, Hook_CreateRagdollEntity);
	#endif

	// `item_ammo_canister` has a client side dlight that will
	// always appear even if the ammo canister is not being transmitted.
	// If this ConVar is set too late, then the dlight will have already been
	// created on the client in a frozen spot.
	#if defined SRCCOOP_BLACKMESA
	CBasePlayer pPlayer = CBasePlayer(client);
	pPlayer.SendCommand("cl_ammo_box_dlights 0");
	#endif
}

public void OnClientAuthorized(int client, const char[] auth)
{
	int sid = GetSteamAccountID(client, g_pConvarValidateSteamIds.BoolValue);
	if (sid)
	{
		IntToString(sid, g_szSteamIds[client], sizeof(g_szSteamIds[]));
	}
}

public void OnClientDisconnect(int client)
{
	if (IsFakeClient(client))
		return;
	
	CBasePlayer pPlayer = CBasePlayer(client);
	PlayerPatch_OnClientDisconnect(pPlayer);
}

public void OnClientDisconnect_Post(int client)
{
	if (g_iPlayerCount)
	{
		g_iPlayerCount = GetRealClientCount(true);
		
		#if defined ENTPATCH_NPC_THINK_LOCALPLAYER
		if (!g_iPlayerCount)
		{
			// pause entity thinking
			hkPhysics_RunThinkFunctions.Enable(Hook_Pre, Hook_Physics_RunThinkFunctions);
		}
		#endif
	}
	SurvivalManager.OnClientDisconnectPost(client);
	CoopManager.OnClientDisconnectPost(client);
	ItemInstancingManager.OnClientDisconnectPost(client);
	g_szSteamIds[client] = "";
	g_bPostTeamSelect[client] = false;
	g_iAddButtons[client] = 0;
}

public Action UserMessage_TextMsg(UserMsg pMsg, BfRead bf, const int[] pPlayers, int iPlayerCount, bool bReliable, bool bInitialize)
{
	return DnManager.TextMsg(bf);
}

public Action Event_PlayerDeath(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	return DnManager.PlayerDeath(hEvent);
}

public Action Event_EntityKilled(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	DnManager.EntityKilled(hEvent);
	return Plugin_Continue;
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
	DnManager.Clear();
	g_pLevelLump.RevertConvars();
	g_bMapStarted = false;
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	if (g_bTempDontHookEnts)
		return;

	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity == NULL_CBASEENTITY)
		return;

	SDKHook(iEntIndex, SDKHook_Spawn, Hook_FixupBrushModels);
	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EntitySpawnPost);
	
	bool bIsNPC = pEntity.IsNPC();

	if (bIsNPC)
	{
		#if defined ENTPATCH_NPC_THINK_LOCALPLAYER
		DHookEntity(hkThink, false, iEntIndex, _, Hook_BaseNPCThink);
		DHookEntity(hkThink, true, iEntIndex, _, Hook_BaseNPCThinkPost);
		#endif
		
		#if defined ENTPATCH_NPC_RUNTASK
		DHookEntity(hkBaseNpcRunTask, false, iEntIndex, _, Hook_BaseNPCRunTask);
		DHookEntity(hkBaseNpcRunTask, true, iEntIndex, _, Hook_BaseNPCRunTaskPost);
		#endif
		
		#if defined ENTPATCH_CUSTOM_NPC_MODELS
		DHookEntity(hkKeyValue_char, true, iEntIndex, _, Hook_BaseNPCKeyValuePost);
		#endif
		
		#if defined ENTPATCH_UPDATE_ENEMY_MEMORY
		DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_BaseNPCAcceptInput);
		#endif
		
		#if defined ENTPATCH_SNIPER
		if (strcmp(szClassname, "npc_sniper", false) == 0 || strcmp(szClassname, "proto_sniper", false) == 0)
		{
			DHookEntity(hkProtoSniperSelectSchedule, false, iEntIndex, _, Hook_ProtoSniperSelectSchedule);
			return;
		}
		#endif

		#if defined SRCCOOP_BLACKMESA

		if (strncmp(szClassname, "npc_human_scientist", 19) == 0)
		{
			#if defined ENTPATCH_PLAYER_ALLY
			DHookEntity(hkIsPlayerAlly, false, iEntIndex, _, Hook_IsPlayerAlly);
			#endif
			
			#if defined ENTPATCH_NAVIGATION_URGENT
			DHookEntity(hkIsNavigationUrgent, false, iEntIndex, _, Hook_IsNavigationUrgent);
			#endif

			return;
		}

		#if defined ENTPATCH_PLAYER_ALLY
		if (strcmp(szClassname, "npc_human_security") == 0)
		{
			#if defined ENTPATCH_PLAYER_ALLY
			DHookEntity(hkIsPlayerAlly, false, iEntIndex, _, Hook_IsPlayerAlly);
			#endif

			#if defined ENTPATCH_NAVIGATION_URGENT
			DHookEntity(hkIsNavigationUrgent, false, iEntIndex, _, Hook_IsNavigationUrgent);
			#endif

			return;
		}
		#endif
		
		#endif // SRCCOOP_BLACKMESA

		#if defined ENTPATCH_BM_XENTURRET
		if (strcmp(szClassname, "npc_xenturret") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_XenTurretSpawnPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_ICHTHYOSAUR
		if (strcmp(szClassname, "npc_ichthyosaur") == 0)
		{
			DHookEntity(hkIchthyosaurIdleSound, false, iEntIndex, _, Hook_IchthyosaurIdleSound);
			DHookEntity(hkIchthyosaurIdleSound, true, iEntIndex, _, Hook_IchthyosaurIdleSoundPost);
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

		#if defined ENTPATCH_BM_PUFFBALLFUNGUS
		if (strcmp(szClassname, "npc_puffballfungus") == 0)
		{
			SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_PuffballFungusDmg);
			return;
		}
		#endif
		
		#if defined ENTPATCH_BM_LAV
		if (strcmp(szClassname, "npc_lav") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_LAVSpawnPost);
			return;
		}
		#endif
		
		#if defined ENTPATCH_BM_MEDIC
		if (strcmp(szClassname, "npc_human_medic") == 0)
		{
			DHookEntity(hkEvent_Killed, false, iEntIndex, _, Hook_HumanMedicKilled);
			return;
		}
		#endif
	}
	else // !isNPC
	{
		#if defined ENTPATCH_BM_XENPORTAL_PUSH_PLAYERS
		if (strcmp(szClassname, "env_xen_portal_effect") == 0)
		{
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_XenPortalEffect_AcceptInput);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_SP_WEAPONS
		if (strcmp(szClassname, "grenade_frag") == 0)
		{
			SDKHook(iEntIndex, SDKHook_Spawn, Hook_GrenadeFragSpawn);
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_GrenadeFragSpawnPost);
			return;
		}

		#endif

		if (pEntity.IsWeapon())
		{
			#if defined ENTPATCH_BM_SP_WEAPONS
			if (strcmp(szClassname, "weapon_357") == 0)
			{
				DHookEntity(hkBaseCombatWeaponDeploy, false, iEntIndex, _, Hook_IronsightDeployPost_SaveSettings);
				DHookEntity(hkBaseCombatWeaponGetPrimaryAttackActivity, false, iEntIndex, _, Hook_IronsightGetPrimaryAttackActivity);
			}

			if (strcmp(szClassname, "weapon_crossbow") == 0)
			{
				DHookEntity(hkBaseCombatWeaponItemPostFrame, true, iEntIndex, _, Hook_CrossbowItemPostFramePost);
				DHookEntity(hkBaseCombatWeaponDeploy, false, iEntIndex, _, Hook_IronsightDeployPost_SaveSettings);
				DHookEntity(hkBaseCombatWeaponDeploy, true, iEntIndex, _, Hook_CrossbowDeployPost);
				DHookEntity(hkBaseCombatWeaponPrimaryAttack, true, iEntIndex, _, Hook_CrossbowPrimaryAttackPost);
				DHookEntity(hkBaseCombatWeaponGetDrawActivity, false, iEntIndex, _, Hook_CrossbowGetDrawActivity);
				DHookEntity(hkBaseCombatWeaponGetPrimaryAttackActivity, false, iEntIndex, _, Hook_CrossbowGetPrimaryAttackActivity);
				DHookEntity(hkWeaponCrossbowFireBolt, false, iEntIndex, _, Hook_CrossbowFireBolt);
				DHookEntity(hkWeaponCrossbowFireBolt, true, iEntIndex, _, Hook_CrossbowFireBoltPost);
			}
			#endif

			#if defined ENTPATCH_WEAPON_MODELS
			DHookEntity(hkSetModel, false, iEntIndex, _, Hook_WeaponSetModel);
			#endif

			return;
		}

		if (strcmp(szClassname, "env_message", false) == 0)
		{
			CoopManager.CheckAndAssignChapterTitle();
			return;
		}

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
		
		if (strcmp(szClassname, "env_sprite") == 0)
		{
			#if defined ENTPATCH_ENV_SPRITE
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EnvSpriteSpawnPost);
			#endif

			#if defined SRCCOOP_BLACKMESA
			if (CoopManager.IsCoopModeEnabled())
			{
				RequestFrame(Hook_AmmoCanister_Sprite_OnCreated, view_as<CSprite>(pEntity));
			}
			#endif

			return;
		}
		
		#if defined ENTPATCH_BM_WALL_CHARGERS
		if (strcmp(szClassname, "item_suitcharger") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_WallChargerRateMultiplierReset);
			return;
		}

		if (strcmp(szClassname, "item_healthcharger") == 0)
		{
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_WallChargerRateMultiplierReset);
			return;
		}
		#endif
		
		#if defined ENTPATCH_AI_SCRIPT_CONDITIONS
		if (strcmp(szClassname, "ai_script_conditions") == 0)
		{
			DHookEntity(hkThink, false, iEntIndex, _, Hook_AIConditionsThink);
			DHookEntity(hkThink, true, iEntIndex, _, Hook_AIConditionsThinkPost);
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
		
		#if defined ENTPATCH_NPC_DIALOGUE
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
		
		// ToDo: support all games
		#if defined SRCCOOP_BLACKMESA
		if (strncmp(szClassname, "item_", 5) == 0 || strcmp(szClassname, "prop_soda", false) == 0)
		{
			if (pEntity.IsPickupItem())
			{
				if (CoopManager.IsCoopModeEnabled())
				{
					SDKHook(iEntIndex, SDKHook_Spawn, Hook_Item_OnSpawn);
					
					#if defined ENTPATCH_BM_BATTERY_DLIGHT
					if (strcmp(szClassname, "item_battery") == 0)
					{
						RequestFrame(Hook_Battery_OnCreated, view_as<CItem>(pEntity));
						return;
					}
					#endif
				}
				
				#if defined ENTPATCH_BM_SNARK_NEST
				if (strcmp(szClassname, "item_weapon_snark") == 0)
				{
					SDKHook(iEntIndex, SDKHook_OnTakeDamagePost, Hook_OnItemSnarkDamagePost);
					return;
				}
				#endif

				if (strcmp(szClassname, "item_suit") == 0)
				{
					DHookEntity(hkOnTryPickUp, true, iEntIndex, _, Hook_OnEquipmentTryPickUpPost);
					pEntity.HookOutput("OnPlayerPickup", Hook_SuitTouchPickup);
					return;
				}

				if (strcmp(szClassname, "item_longjump") == 0)
				{
					DHookEntity(hkOnTryPickUp, true, iEntIndex, _, Hook_OnEquipmentTryPickUpPost);
					return;
				}
			}
			return;
		}
		#endif

		#if defined ENTPATCH_BM_TRIPMINE_COLORS
		if (strcmp(szClassname, "spriteteam") == 0)
		{
			// This entity spawns after `beamteam`.
			SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_SpriteTeamSpawnPost);
			return;
		}
		#endif

		#if defined ENTPATCH_BM_PROP_CHARGERS
		if (strcmp(szClassname, "prop_radiation_charger") == 0)
		{
			DHookEntity(hkPropChargerThink, false, iEntIndex, _, Hook_PropRadiationChargerThink);
			return;
		}

		if (strcmp(szClassname, "prop_hev_charger") == 0)
		{
			DHookEntity(hkPropChargerThink, false, iEntIndex, _, Hook_PropHevChargerThink);
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
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_MusicTrackAcceptInput);
			return;
		}
		#endif
		
		#if defined ENTPATCH_ENV_SCREENOVERLAY
		if (strcmp(szClassname, "env_screenoverlay") == 0)
		{
			pEntity.SetUserData("m_bIsEnabled", false); //needed to fix not working switching overlays
			DHookEntity(hkUpdateOnRemove, false, iEntIndex, _, Hook_EnvScreenoverlayUpdateOnRemove);
			DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvScreenoverlayAcceptInput);
			return;
		}
		#endif

		#if defined ENTPATCH_FUNC_TRACKAUTOCHANGE
		if (strcmp(szClassname, "func_trackautochange") == 0)
		{
			DHookEntity(hkBlocked, false, iEntIndex, _, Hook_TrackChangeBlocked);
			return;
		}
		#endif

		#if defined ENTPATCH_FUNC_TRACKTRAIN
		if (strcmp(szClassname, "func_tracktrain") == 0)
		{
			DHookEntity(hkBlocked, false, iEntIndex, _, Hook_TrackTrainBlocked);
			return;
		}
		#endif
		
		#if defined ENTPATCH_TRIGGER_COOP
		if (strcmp(szClassname, "trigger_coop") == 0)
		{
			view_as<CTriggerCoop>(pEntity).OnCreated();
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

public void Hook_EntitySpawnPost(int iEntIndex)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);

		#if defined SRCCOOP_BLACKMESA
		if (g_serverOS == OS_Linux)
		{
			#if defined ENTPATCH_NPC_ALWAYS_TRANSMIT
			// fix NPC sliding (likely caused by desync in CBaseAnimatingOverlay animations started outside of PVS)
			// this is verifiable on linux server on bm_c1a2c by issuing cl_fullupdate when the guard starts sliding
			if (pEntity.IsNPC())
			{
				pEntity.edictFlags |= FL_EDICT_ALWAYS;
			}
			#endif
		}
		#endif // SRCCOOP_BLACKMESA

		CoopManager.EntitySpawnPost(pEntity);
	}
}

// Postpone items' Spawn() until Gamerules IsMultiplayer() gets hooked in OnMapStart()
#if defined SRCCOOP_BLACKMESA
static Action Hook_Item_OnSpawn(int iEntIndex)
{
	SDKUnhook(iEntIndex, SDKHook_Spawn, Hook_Item_OnSpawn);
	
	CItem pItem = CItem(iEntIndex);
	if (g_bMapStarted)
	{
		RequestFrame(SpawnPostponedItem, pItem);
	}
	else
	{
		g_pPostponedSpawns.Push(pItem);
	}

	return Plugin_Stop;
}
#endif

static void SpawnPostponedItem(const CItem pItem)
{
	if (pItem.IsValid())
	{
		// TODO: Hooks on Spawn/SpawnPost will get called twice since this is hooked with SDKHooks.
		g_bIsMultiplayerOverride = false; // IsMultiplayer=false will spawn items with physics
		pItem.Spawn();
		g_bIsMultiplayerOverride = true;

		ItemInstancingManager.OnItemSpawnPost(pItem);

		pItem.SetCollisionGroup(COLLISION_GROUP_WEAPON);
	}
}

void PerformEntityClassAliasing()
{
	// Static mappings:
#if defined ENTPATCH_TRIGGER_COOP
	CEntityFactoryDictionary.Get().CreateAlias("trigger_coop", "trigger_multiple");
#endif
	
	// Dynamic mappings:
	char szPath[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, szPath, sizeof(szPath), "configs/srccoop/entitymappings.cfg");
	if (!FileExists(szPath))
		return;
	
	KeyValues hKV = CreateKeyValues("Entities");
	if (!hKV.ImportFromFile(szPath))
	{
		LogError("Failed reading entity list from \"%s\"", szPath);
		hKV.Close();
		return;
	}

	bool bFirst = hKV.GotoFirstSubKey(true);
	char szEntityClass[128];
	char szBaseClass[128];
	while (bFirst || hKV.GotoNextKey(true))
	{
		bFirst = false;
		hKV.GetSectionName(szEntityClass, sizeof(szEntityClass));
		hKV.GetString("baseclass", szBaseClass, sizeof(szBaseClass), "");
		
		if (szBaseClass[0] == EOS)
			continue;
		
		if (CEntityFactoryDictionary.Get().CreateAlias(szEntityClass, szBaseClass))
			LogDebug("New class registration \"%s\" with baseclass \"%s\" succeeded", szEntityClass, szBaseClass);
	}
	hKV.Close();
}

bool TranslateCustomEntityOutput(CBaseEntity pEntity, char[] szOutputName, int iMaxLength, bool bReverse = false)
{
	static char szClassname[MAX_CLASSNAME];
	if (!pEntity.GetClassname(szClassname, sizeof(szClassname)))
		return false;
	
	#if defined ENTPATCH_TRIGGER_COOP
	if (StrEqual(szClassname, "trigger_coop"))
	{
		return CTriggerCoop.TranslateOutput(szOutputName, szOutputName, iMaxLength, bReverse);
	}
	#endif

	return false;
}

public Action Event_BroadcastTeamsound(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// block multiplayer announcer
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

public Action Event_BroadcastKillstreak(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// block multiplayer announcer
		return Plugin_Stop;
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
			if (pPlayer.IsPlayer())
			{
				CBaseEntity pItem = CBaseEntity(_this);
				char szClass[MAX_CLASSNAME];
				pItem.GetClassname(szClass, sizeof(szClass));
				CCoopSpawnSystem.AddSpawnItem(szClass);
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
		CCoopSpawnSystem.AddSpawnItem(szClass);
	}
}

public MRESReturn Hook_RestoreWorld(DHookReturn hReturn)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		// disable gamerules resetting the world on 'round start', this caused crashes
		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

public MRESReturn Hook_RespawnPlayers(DHookReturn hReturn)
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
		Msg(client, "%t", "welcome", SRCCOOP_VERSION);
	}
}

//stock void Test_ConstGetMaxFormatLengthInt()
//{
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(0), 2);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(1), 2);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(16), 3);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(256), 4);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(4096), 5);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(99999), 6);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(2147483647), 11);
//    
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-1), 3);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-16), 4);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-256), 5);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-4096), 6);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-99999), 7);
//    assert_eq(CONST_GET_MAX_FORMAT_LENGTH_INT(-2147483648), 12);
//}
