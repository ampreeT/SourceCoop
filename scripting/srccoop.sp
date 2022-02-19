//#define NO_DEBUG   /* Uncomment to disable debugging */

#include <srccoop>

#define PLUGIN_VERSION "1.0.4"

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
		// LoadDHookVirtual(pGameConfig, hkIsDeathmatch, "CMultiplayRules::IsDeathmatch");
		LoadDHookVirtual(pGameConfig, hkRestoreWorld, "CBM_MP_GameRules::RestoreWorld");
		LoadDHookVirtual(pGameConfig, hkRespawnPlayers, "CBM_MP_GameRules::RespawnPlayers");
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
	g_pConvarCoopTeam = CreateConVar("sourcecoop_team", "scientist", "Sets which team to use in TDM mode. Valid values are [marines] or [scientist]. Setting anything else will not manage teams.");
	g_pConvarDisableTeamSelect = CreateConVar("sourcecoop_disable_teamselect", "1", "Whether to skip the team select screen and spawn in instantly.", _, true, 0.0, true, 1.0);
	g_pConvarCoopRespawnTime = CreateConVar("sourcecoop_respawntime", "2.0", "Sets player respawn time in seconds. This can only be used for making respawn times quicker, not longer. Set to 0 to use the game's default.", _, true, 0.0);
	g_pConvarWaitPeriod = CreateConVar("sourcecoop_start_wait_period", "15.0", "The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.", _, true, 0.0);
	g_pConvarEndWaitPeriod = CreateConVar("sourcecoop_end_wait_period", "60.0", "The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.", _, true, 0.0);
	g_pConvarEndWaitFactor = CreateConVar("sourcecoop_end_wait_factor", "1.0", "Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).", _, true, 0.0, true, 1.0);
	g_pConvarHomeMap = CreateConVar("sourcecoop_homemap", "", "The map to return to after finishing a campaign/map.");
	g_pConvarEndWaitDisplayMode = CreateConVar("sourcecoop_end_wait_display_mode", "0", "Sets which method to show countdown. 0 is panel, 1 is hud text.", _, true, 0.0, true, 1.0);
	
	mp_friendlyfire = FindConVar("mp_friendlyfire");
	mp_flashlight = FindConVar("mp_flashlight");
	
	RegAdminCmd("sourcecoop_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegAdminCmd("sc_ft", Command_SetFeature, ADMFLAG_ROOT, "Command for toggling plugin features on/off");
	RegAdminCmd("sc_save", Command_Save, ADMFLAG_ROOT, "Exports last saved player equipment state. Equipment state is saved at the end of a map, so this wil record the state as of the start of a map.");
	RegAdminCmd("sc_load", Command_Load, ADMFLAG_ROOT, "Imports saved data from file and attempts to equip each player (if they were present when the data was saved).");
	RegServerCmd("sourcecoop_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	RegServerCmd("sc_dump", Command_DumpMapEntities, "Command for dumping map entities to a file");
	
	g_pLevelLump.Initialize();
	g_SpawnSystem.Initialize();
	g_pCoopManager.Initialize();
	g_pInstancingManager.Initialize();
	g_pPostponedSpawns = CreateArray();
	g_pFeatureMap = new FeatureMap();
	g_pEquipmentManager.Initialize();
	InitializeMenus();
	
	g_CoopMapStartFwd = new GlobalForward("OnCoopMapStart", ET_Ignore);
	g_CoopMapConfigLoadedFwd = new GlobalForward("OnCoopMapConfigLoaded", ET_Ignore, Param_Cell, Param_Cell);
	
	if (g_Engine == Engine_BlackMesa)
	{
		HookEvent("broadcast_teamsound", Event_BroadcastTeamsound, EventHookMode_Pre);
		AddTempEntHook("BlackMesa Shot", BlackMesaFireBulletsTEHook);
		AddNormalSoundHook(PlayerSoundListener);
		UserMsg iIntroCredits = GetUserMessageId("IntroCredits");
		if(iIntroCredits != INVALID_MESSAGE_ID)
		{
			HookUserMessage(iIntroCredits, Hook_IntroCreditsMsg, true);
		}
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
	if (!IsDedicatedServer() && MaxClients == 1)
	{
		SetFailState("Singleplayer detected, unloading SourceCoop (ignore this)");
		return Plugin_Continue;
	}
	
	OnMapEnd(); // this does not always get called, so call it here
	strcopy(g_szPrevMapName, sizeof(g_szPrevMapName), g_szMapName);
	strcopy(g_szMapName, sizeof(g_szMapName), szMapName);
	if (strlen(szMapEntities) < 4)
	{
		SetFailState("Failed to get map entities string! Most likely this version of SourceMod is too new...");
		return Plugin_Continue;
	}
	else g_szEntityString = szMapEntities;
	g_pCoopManager.OnLevelInit(szMapName, szMapEntities);

	return Plugin_Changed;
}

public void OnMapStart()
{
	g_pCoopManager.OnMapStart();
	
	if (g_Engine == Engine_BlackMesa)
	{
		DHookGamerules(hkIsMultiplayer, false, _, Hook_IsMultiplayer);
		//DHookGamerules(hkIsDeathmatch, false, _, Hook_IsDeathmatch);
		DHookGamerules(hkRestoreWorld, false, _, Hook_RestoreWorld);
		DHookGamerules(hkRespawnPlayers, false, _, Hook_RespawnPlayers);
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
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx80.vtx");
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.dx90.vtx");
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.mdl");
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.phy");
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.sw.vtx");
		AddFileToDownloadsTable("models/props_xen/xen_turret_mpfix.vvd");
	}
}

public void OnClientPutInServer(int client)
{
	CBasePlayer pPlayer = CBasePlayer(client);
	
	// fixes visual bug for players which had different view ent at mapchange
	pPlayer.SetViewEntity(pPlayer);
	
	if(g_Engine == Engine_BlackMesa)
	{
		// fixes bugged trigger_teleport prediction (camera jerking around as if being teleported)
		ClientCommand(client, "cl_predicttriggers 0");
	}
	
	g_pCoopManager.OnClientPutInServer(pPlayer);
	g_pInstancingManager.OnClientPutInServer(client);
	
	SDKHook(client, SDKHook_PreThinkPost, Hook_PlayerPreThinkPost);
	SDKHook(client, SDKHook_PreThink, Hook_PlayerPreThink);
	SDKHook(client, SDKHook_SpawnPost, Hook_PlayerSpawnPost);
	SDKHook(client, SDKHook_TraceAttack, Hook_PlayerTraceAttack);
	SDKHook(client, SDKHook_OnTakeDamage, Hook_PlayerTakeDamage);
	SDKHook(client, SDKHook_WeaponEquipPost, Hook_PlayerWeaponEquipPost);
	DHookEntity(hkChangeTeam, false, client, _, Hook_PlayerChangeTeam);
	DHookEntity(hkShouldCollide, false, client, _, Hook_PlayerShouldCollide);
	GreetPlayer(client);
	SetSteamId(client);
}

public void OnClientDisconnect(int client)
{
	g_pInstancingManager.OnClientDisconnect(client);
	ClearSteamId(client);
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
		SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EntitySpawnPost);
		
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
					SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_XenTurretSpawnPost);
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
					SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_GargSpawnPost);
				}
				else if (strncmp(szClassname, "npc_houndeye", 12) == 0)
				{
					DHookEntity(hkThink, false, iEntIndex, _, Hook_HoundeyeThink);
					DHookEntity(hkThink, true, iEntIndex, _, Hook_HoundeyeThinkPost);
				}
				else if (strcmp(szClassname, "npc_gonarch") == 0)
				{
					DHookEntity(hkRunAI, false, iEntIndex, _, Hook_GonarchRunAI);
					DHookEntity(hkRunAI, true, iEntIndex, _, Hook_GonarchRunAIPost);
				}
				else if (strcmp(szClassname, "npc_nihilanth") == 0)
				{
					DHookEntity(hkRunAI, false, iEntIndex, _, Hook_NihilanthRunAI);
					DHookEntity(hkRunAI, true, iEntIndex, _, Hook_NihilanthRunAIPost);
					DHookEntity(hkHandleAnimEvent, false, iEntIndex, _, Hook_NihilanthHandleAnimEvent);
					DHookEntity(hkHandleAnimEvent, true, iEntIndex, _, Hook_NihilanthHandleAnimEventPost);
				}
			}
			else if ((strcmp(szClassname, "instanced_scripted_scene", false) == 0) ||
					(strcmp(szClassname, "logic_choreographed_scene", false) == 0) ||
					(strcmp(szClassname, "scripted_scene", false) == 0))
			{
				DHookEntity(hkFindNamedEntity, true, iEntIndex, _, Hook_FindNamedEntity);
				DHookEntity(hkFindNamedEntityClosest, true, iEntIndex, _, Hook_FindNamedEntity);
			}
			else if (strncmp(szClassname, "item_", 5) == 0 && pEntity.IsPickupItem())
			{
				if (g_pCoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
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
			else if (strcmp(szClassname, "trigger_autosave") == 0)
			{
				SDKHook(iEntIndex, SDKHook_Spawn, Hook_AutosaveSpawn);
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
			else if (strcmp(szClassname, "env_zoom") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvZoomAcceptInput);
			}
			else if (strcmp(szClassname, "env_credits") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_EnvCreditsAcceptInput);
			}
			else if (strcmp(szClassname, "env_sprite") == 0)
			{
				SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_EnvSpriteSpawn);
			}
			else if (strcmp(szClassname, "ai_script_conditions") == 0)
			{
				DHookEntity(hkThink, false, iEntIndex, _, Hook_AIConditionsThink);
			}
			else if (strcmp(szClassname, "func_rotating") == 0)
			{
				CreateTimer(30.0, Timer_FixRotatingAngles, pEntity, TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
			}
			else if (strcmp(szClassname, "prop_hev_charger") == 0 || strcmp(szClassname, "prop_radiation_charger") == 0)
			{
				//DHookEntity(hkThink, false, iEntIndex, _, Hook_PropChargerThink);
				//DHookEntity(hkThink, true, iEntIndex, _, Hook_PropChargerThinkPost);
			}
			else if (strcmp(szClassname, "misc_marionettist") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_MarionettistAcceptInput);
			}
			else if (strcmp(szClassname, "player_loadsaved") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_LoadSavedAcceptInput);
			}
			else if (strcmp(szClassname, "game_end") == 0)
			{
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_GameEndAcceptInput);
			}
			else if (strcmp(szClassname, "phys_bone_follower") == 0)
			{
				SDKHook(iEntIndex, SDKHook_VPhysicsUpdatePost, Hook_BoneFollowerVPhysicsUpdatePost);
			}
			else if (strcmp(szClassname, "music_track") == 0)
			{
				DHookEntity(hkThink, false, iEntIndex, _, Hook_MusicTrackThink);
				DHookEntity(hkAcceptInput, false, iEntIndex, _, Hook_MusicTrackAceptInput);
			}
			
			// if some explosions turn out to be damaging all players except one, this is the fix
			//else if (strcmp(szClassname, "env_explosion") == 0)
			//{
			//	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ExplosionSpawn);
			//}
		}
	}
}

public void OnEntityDestroyed(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity.IsValid())
	{
		static char szClassname[MAX_CLASSNAME];
		pEntity.GetClassname(szClassname, sizeof(szClassname));
		
		if (strcmp(szClassname, "camera_death") == 0)
		{
			OnEntityDestroyed_CameraDeath(pEntity);
		}
		else if (strcmp(szClassname, "misc_marionettist") == 0)
		{
			OnEntityDestroyed_Marionettist(pEntity);
		}
	}
}

public void Hook_EntitySpawnPost(int iEntIndex)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		
		// fix linux physics crashes
		if(g_Engine == Engine_BlackMesa && g_serverOS == OS_Linux)
		{
			static char szModel[PLATFORM_MAX_PATH];
			if(pEntity.GetModel(szModel, sizeof(szModel)) && strncmp(szModel, "models/gibs/humans/", 19) == 0)
			{
				SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_NoGibDmg);
			}
		}
		
		// find and hook output hooks for entity
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

public Action Event_BroadcastTeamsound(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		// block multiplayer announcer
		hEvent.BroadcastDisabled = true;
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

public MRESReturn Hook_OnEquipmentTryPickUpPost(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsFeatureEnabled(FT_KEEP_EQUIPMENT))
	{
		bool bPickedUp = DHookGetReturn(hReturn);
		if(bPickedUp)
		{
			CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
			if(pPlayer.IsClassPlayer())
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
	if (g_pCoopManager.IsFeatureEnabled(FT_KEEP_EQUIPMENT))
	{
		CBaseEntity pItem = CBaseEntity(weapon);
		char szClass[MAX_CLASSNAME];
		pItem.GetClassname(szClass, sizeof(szClass));
		g_SpawnSystem.AddSpawnItem(szClass);
	}
}

public MRESReturn Hook_RestoreWorld(Handle hReturn)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		// disable gamerules resetting the world on 'round start', this caused crashes
		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

public MRESReturn Hook_RespawnPlayers(Handle hReturn)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		// disable gamerules respawning players on 'round start'
		DHookSetReturn(hReturn, 0);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

void GreetPlayer(int client)
{
	if (g_pCoopManager.IsFeatureEnabled(FT_SHOW_WELCOME_MESSAGE))
	{
		Msg(client, "This server runs SourceCoop version %s.\nPress %s=%s or type %s/coopmenu%s for extra settings.", PLUGIN_VERSION, CHAT_COLOR_SEC, CHAT_COLOR_PRI, CHAT_COLOR_SEC, CHAT_COLOR_PRI);
	}
}

void SetSteamId(int client){
	char steamId[32];
	if(GetClientAuthId(client, AuthId_SteamID64, steamId, 32, true)){
		g_szSteamIds[client] = steamId;
	}else{
		Msg(client, "Could not retrieve Steam ID - Progress will not be saved.");
	}
}

void ClearSteamId(int client){
	g_szSteamIds[client] = "";
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

void GetSaveDataPath(char buffer[PLATFORM_MAX_PATH], char szFileName[64]){
	char szFolder[10] = "SaveData/";

	char relativePath[PLATFORM_MAX_PATH + 64];
	strcopy(relativePath, sizeof(relativePath), szFolder);
	StrCat(relativePath, sizeof(relativePath), szFileName);

	BuildPath(Path_SM, buffer, sizeof(buffer), relativePath);
}

public Action Command_Save(int iClient, int iArgs)
{
	if(iArgs != 1)
	{
		MsgReply(iClient, "Format: sc_save <FILENAME>");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	GetSaveDataPath(szFullPath, szFileName);

	KeyValues kv = new KeyValues("SaveData");
	bool shouldSave = false;
	for(int i = 0; i < MAXPLAYERS + 1; i++)
	{
		if(strlen(g_szSteamIds[i]) == 0)
			continue;
		
		CCoopEquipment equipment; 
		equipment.Initialize();

		if(!g_pEquipmentManager.GetEquipment(g_szSteamIds[i], equipment))
			continue;


		kv.JumpToKey(g_szSteamIds[i], true);
		kv.SetNum("hp", equipment.m_iHealth);
		kv.SetNum("armor", equipment.m_iArmor);
		kv.JumpToKey("weapons", true);

		for(int j = 0; j < equipment.m_pWeaponList.Length; j++)
		{
			CCoopWeaponEntry pWeaponEntry;
			if (equipment.m_pWeaponList.GetArray(j, pWeaponEntry, sizeof(pWeaponEntry)))
			{
				kv.SetNum(pWeaponEntry.m_szClassname, 1);
			}

		}

		kv.GoBack();
		kv.JumpToKey("ammo", true);
		for(int k = 0; k < MAX_AMMO_TYPES; k++)
		{
			char key[3];
			IntToString(k, key, sizeof(key));
			kv.SetNum(key, equipment.m_iAmmoCount[k]);
		}

		shouldSave = true;
	}

	if(!shouldSave)
	{
		MsgReply(iClient, "No valid players found. Could not save data");
		return Plugin_Handled;
	}

	kv.Rewind();
	kv.ExportToFile(szFullPath);
	delete kv;
	return Plugin_Handled;
}

public Action Command_Load(int iClient, int iArgs){
	if(iArgs != 1)
	{
		MsgReply(iClient, "Format: sc_load <FILENAME>");
		return Plugin_Handled;
	}

	char szFileName[64];
	GetCmdArg(1, szFileName, sizeof(szFileName));

	char szFullPath[PLATFORM_MAX_PATH];
	GetSaveDataPath(szFullPath, szFileName);

	if(!FileExists(szFullPath))
	{
		MsgReply(iClient, "Could not find specified save file %s", szFullPath);
		return Plugin_Handled;
	}

	
	KeyValues kv = new KeyValues("SaveData");
	if(!kv.ImportFromFile(szFullPath))
	{
		MsgReply(iClient, "Failed to parse KeyValues file %s", szFullPath);
		return Plugin_Handled;	
	}

	for(int i = 0; i < MAXPLAYERS + 1; i++)
	{
		if(strlen(g_szSteamIds[i]) == 0)
			continue;

		if(!kv.JumpToKey(g_szSteamIds[i]))
			continue;

		CBasePlayer pPlayer = CBasePlayer(i);
		if(!pPlayer.IsValid() || !pPlayer.IsInGame())
			continue;

		CCoopEquipment equipment;
		equipment.Initialize();

		equipment.m_iHealth = kv.GetNum("hp");
		equipment.m_iArmor = kv.GetNum("armor");

		if(!kv.JumpToKey("weapons"))
		{
			MsgReply(iClient, "Unable to find weapons section in KeyValues file");
			return Plugin_Handled;	
		}

		if(kv.GotoFirstSubKey(false))
		{
			do
			{
				char weaponName[MAX_CLASSNAME];
				if(kv.GetSectionName(weaponName, sizeof(weaponName)))
				{
					CCoopWeaponEntry pWeaponEntry;
					strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), weaponName);
					pWeaponEntry.m_iPrimaryAmmo = -1;
					pWeaponEntry.m_iSecondaryAmmo = -1;
					equipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
				}
			}
			while(kv.GotoNextKey(false));

			kv.GoBack(); //return to weapons node
		}

		kv.GoBack(); //return to player node
		
		if(!kv.JumpToKey("ammo"))
		{
			MsgReply(iClient, "Unable to find ammo section in KeyValues file");
			return Plugin_Handled;	
		}

		if(kv.GotoFirstSubKey(false))
		{
			do 
			{
				char ammoKey[3];
				if(kv.GetSectionName(ammoKey, sizeof(ammoKey)))
				{
					int index = StringToInt(ammoKey);
					int count = kv.GetNum(ammoKey);
					equipment.m_iAmmoCount[index] = count;
				}
			}
			while(kv.GotoNextKey(false));
		}

		g_pEquipmentManager.StoreEquipment(g_szSteamIds[i], equipment);
		g_SpawnSystem.StripPlayer(pPlayer);
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);

		PrintToServer("Loaded Equipment for player %s", g_szSteamIds[i]);
	}

	return Plugin_Handled;
}
