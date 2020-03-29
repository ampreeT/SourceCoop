//Uncomment to disable debugging
//#define NO_DEBUG

#include <srccoop>

// move me to classdef as member functions
public void SetPlayerPickup(int iPlayer, int iObject, const bool bLimitMassAndSize)
{
	g_bIsMultiplayerOverride = false; // IsMultiplayer=false uses correct implementation for bLimitMassAndSize
	SDKCall(g_pPickupObject, iPlayer, iObject, bLimitMassAndSize);
	g_bIsMultiplayerOverride = true;
}

public void SetWeaponAnimation(int iWeapon, const int iActivity)
{
	SDKCall(g_pSendWeaponAnim, iWeapon, iActivity);
}

public void ResetGlobalStates()
{
	SDKCall(g_pGameShutdown, g_ServerGameDLL); // GameShutdown() just resets global states
}
// classdef end

public Plugin myinfo =
{
	name = "SourceCoop",
	author = "ampreeT",
	description = "SourceCoop",
	version = "1.0.0",
	url = ""
};

public void load_dhook_detour(const Handle pGameConfig, Handle& pHandle, const char[] szFuncName, bool bPost, DHookCallback pCallback)
{
	pHandle = DHookCreateFromConf(pGameConfig, szFuncName);
	if (pHandle == null)
		SetFailState("Couldn't create hook %s", szFuncName);
	if (!DHookSetFromConf(pHandle, pGameConfig, SDKConf_Signature, szFuncName))
		SetFailState("Couldn't set hook %s", szFuncName);
	if (!DHookEnableDetour(pHandle, bPost, pCallback))
		SetFailState("Couldn't enable detour hook %s", szFuncName);
}

public void load_dhook_virtual(const Handle pGameConfig, Handle& pHandle, const char[] szFuncName)
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
	if (!(g_ServerGameDLL = GetServerInterface(szInterfaceGame)))
		SetFailState("Could not get interface for %s", "g_ServerGameDLL");
	
	char szPickupObject[] = "CBlackMesaPlayer::PickupObject";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szPickupObject))
		SetFailState("Could not obtain gamedata offset %s", szPickupObject);
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
	if (!(g_pPickupObject = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szPickupObject);
	
	char szSendWeaponAnim[] = "CBaseCombatWeapon::SendWeaponAnim";
	StartPrepSDKCall(SDKCall_Entity);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szSendWeaponAnim))
		SetFailState("Could not obtain gamedata offset %s", szSendWeaponAnim);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	if (!(g_pSendWeaponAnim = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendWeaponAnim);
		
	char szGameShutdown[] = "CServerGameDLL::GameShutdown";
	StartPrepSDKCall(SDKCall_Raw);
	if(!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szGameShutdown))
		SetFailState("Could not obtain gamedata offset %s", szGameShutdown);
	if (!(g_pGameShutdown = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGameShutdown);

	load_dhook_virtual(pGameConfig, hkFAllowFlashlight, "CMultiplayRules::FAllowFlashlight");
	load_dhook_virtual(pGameConfig, hkIsMultiplayer, "CMultiplayRules::IsMultiplayer");
	//load_dhook_virtual(pGameConfig, hkIsDeathmatch, "CMultiplayRules::IsDeathmatch");
	load_dhook_virtual(pGameConfig, hkIRelationType, "CBaseCombatCharacter::IRelationType");
	load_dhook_virtual(pGameConfig, hkIsPlayerAlly, "CAI_BaseNPC::IsPlayerAlly");
	load_dhook_virtual(pGameConfig, hkProtoSniperSelectSchedule, "CProtoSniper::SelectSchedule");
	load_dhook_virtual(pGameConfig, hkFindNamedEntity, "CSceneEntity::FindNamedEntity");
	load_dhook_virtual(pGameConfig, hkFindNamedEntityClosest, "CSceneEntity::FindNamedEntityClosest");
	load_dhook_virtual(pGameConfig, hkSetModel, "CBaseEntity::SetModel");
	load_dhook_virtual(pGameConfig, hkAcceptInput, "CBaseEntity::AcceptInput");
	load_dhook_detour(pGameConfig, hkSetSuitUpdate, "CBasePlayer::SetSuitUpdate", false, Hook_SetSuitUpdate);

	CloseHandle(pGameConfig);
}

public Address GetServerInterface(const char[] szInterface)
{
	return view_as<Address>(SDKCall(g_pCreateServerInterface, szInterface, 0));
}

public Address GetEngineInterface(const char[] szInterface)
{
	return view_as<Address>(SDKCall(g_pCreateEngineInterface, szInterface, 0));
}

public void OnPluginStart()
{
	load_gamedata();
	InitDebugLog("sm_coop_debug", "SRCCOOP", ADMFLAG_ROOT);
	g_pConvarCoopEnabled = CreateConVar("sm_coop_enabled", "1", "Sets if coop is enabled on campaign maps");
	g_pConvarWaitPeriod = CreateConVar("sm_coop_wait_period", "15.0", "The max number of seconds to wait since first player spawned in to start the map");
	g_pLevelLump.Initialize();
	g_SpawnSystem.Initialize();
	g_pCoopManager.Initialize();
	g_pInstancingManager.Initialize();
	HookEvent("player_spawn", Hook_PlayerSpawnPost, EventHookMode_Post);
	HookEvent("broadcast_teamsound", Hook_BroadcastTeamsound, EventHookMode_Pre);
}

public void OnMapStart()
{
	g_pCoopManager.OnMapStart();
	DHookGamerules(hkIsMultiplayer, false, _, Hook_IsMultiplayer);
}

public void OnConfigsExecuted()
{
	if (g_pCoopManager.IsFeaturePatchingEnabled())
	{
		DHookGamerules(hkFAllowFlashlight, false, _, Hook_FAllowFlashlight);
		//DHookGamerules(hkIsDeathmatch, false, _, Hook_IsDeathmatch);
	}
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		CBaseEntity pGameEquip = CreateByClassname("game_player_equip");	// will spawn players with nothing if it exists
		if (pGameEquip.IsValidIndex())
		{
			pGameEquip.SetSpawnFlags(SF_PLAYER_EQUIP_STRIP_SUIT);
			pGameEquip.Spawn();
		}
		CBaseEntity pGameGamerules = CreateByClassname("game_mp_gamerules");
		if (pGameGamerules.IsValidIndex())
		{
			pGameGamerules.Spawn();
			pGameGamerules.AcceptInputStr("DisableCanisterDrops");
			pGameGamerules.Kill();
		}
	}
	
	PrecacheScriptSound("HL2Player.SprintStart");
}

public void OnClientPutInServer(int client)
{
	g_pCoopManager.OnClientPutInServer(client);
	g_pInstancingManager.OnClientPutInServer(client);
}

public void OnClientDisconnect(int client)
{
	g_pInstancingManager.OnClientDisconnect(client);
}

public void OnMapEnd()
{
	g_pLevelLump.RevertConvars();
}

public void OnPluginEnd()
{
	
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (!g_bTempDontHookEnts && pEntity.IsValid())
	{
		if (pEntity.IsClassPlayer())
		{
			SDKHook(iEntIndex, SDKHook_PreThinkPost, Hook_PlayerPreThinkPost);
		}
		else if (pEntity.IsClassScientist())
		{
			DHookEntity(hkIRelationType, true, iEntIndex, _, Hook_ScientistIRelationType);
			DHookEntity(hkIsPlayerAlly, true, iEntIndex, _, Hook_IsPlayerAlly);
		}
		//else if (strcmp(szClassname, "npc_sniper", false) == 0)
		//{
		//	DHookEntity(hkProtoSniperSelectSchedule, false, pEntity.GetEntIndex(), _, Hook_ProtoSniperSelectSchedule);
		//	DHookEntity(hkProtoSniperSelectSchedule, true, pEntity.GetEntIndex(), _, Hook_ProtoSniperSelectSchedule);
		//}
		else if ((strcmp(szClassname, "instanced_scripted_scene", false) == 0) ||
				(strcmp(szClassname, "logic_choreographed_scene", false) == 0) ||
				(strcmp(szClassname, "scripted_scene", false) == 0))
		{
			DHookEntity(hkFindNamedEntity, true, iEntIndex, _, Hook_FindNamedEntity);
			DHookEntity(hkFindNamedEntityClosest, true, iEntIndex, _, Hook_FindNamedEntity);
		}
		else if (strncmp(szClassname, "item_", 5) == 0)
		{
			if(g_pLevelLump.m_bInstanceItems)
			{
				SDKHook(iEntIndex, SDKHook_Spawn, Hook_ItemSpawnDelay);
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
		// if some explosions turn out to be damaging all players except one, this is the fix
		//else if (strcmp(szClassname, "env_explosion") == 0)
		//{
		//	SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_ExplosionSpawn);
		//}
		SDKHook(iEntIndex, SDKHook_SpawnPost, Hook_SpawnPost);
	}
}

public void Hook_SpawnPost(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if(pEntity.GetMoveType() == MOVETYPE_VPHYSICS)
	{
		char szClass[13]; pEntity.GetClassname(szClass, sizeof(szClass));
		if(strcmp(szClass, "prop_physics") == 0) // prefix
		{
			pEntity.SetSpawnFlags(pEntity.GetSpawnFlags() | SF_PHYSPROP_ENABLE_PICKUP_OUTPUT);
		}
		SDKHook(iEntIndex, SDKHook_UsePost, Hook_Use);
	}
	
	if (g_pCoopManager.IsCoopModeEnabled())
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

// Delay items' Spawn() until Gamerules IsMultiplayer() gets hooked in OnMapStart()
public Action Hook_ItemSpawnDelay(int iEntIndex)
{
	SDKUnhook(iEntIndex, SDKHook_Spawn, Hook_ItemSpawnDelay);
	RequestFrame(Request_ItemSpawnDelay, CBaseEntity(iEntIndex));
	return Plugin_Stop;
}

public void Request_ItemSpawnDelay(CBaseEntity pEntity)
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
	if (g_pCoopManager.IsFeaturePatchingEnabled())
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
	}
	return Plugin_Stop;
}

public void RequestStopThink(CBaseEntity pEntity)
{
	if(pEntity.IsValid())
	{
		pEntity.SetNextThinkTick(0);
	}
}

public Action Hook_PlayerSpawnPost(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	int iClientID = GetEventInt(hEvent, "userid");
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(GetClientOfUserId(iClientID));
	if (pPlayer.IsValid())
	{
		g_pCoopManager.OnPlayerSpawned(pPlayer);
		g_pInstancingManager.OnPlayerSpawned(pPlayer);
	}
}

public Action Hook_BroadcastTeamsound(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	if(g_pCoopManager.IsFeaturePatchingEnabled())
	{
		// block multiplayer announcer
		hEvent.BroadcastDisabled = true;
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

public void Hook_PlayerPreThinkPost(int iClient)
{
	if(!g_pCoopManager.IsFeaturePatchingEnabled())
		return;
	
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iClient);
	if (pPlayer.IsValid())
	{
		int iButtons = pPlayer.GetButtons();
		int iOldButtons = pPlayer.GetOldButtons();
		
		if(pPlayer.IsAlive())	// sprinting stuff
		{
			bool bIsHoldingSpeed = view_as<bool>(iButtons & IN_SPEED);
			bool bWasHoldingSpeed = view_as<bool>(iOldButtons & IN_SPEED);
			bool bIsMoving = view_as<bool>((iButtons & IN_FORWARD) || (iButtons & IN_BACK) || (iButtons & IN_MOVELEFT) || (iButtons & IN_MOVERIGHT));
			bool bWasMoving = view_as<bool>((iOldButtons & IN_FORWARD) || (iOldButtons & IN_BACK) || (iOldButtons & IN_MOVELEFT) || (iOldButtons & IN_MOVERIGHT));
			
			// should deactivate this if crouch is held
			if(pPlayer.HasSuit() && bIsHoldingSpeed) 
			{
				pPlayer.SetMaxSpeed(320.0);
				if(!bWasHoldingSpeed)
				{
					pPlayer.PlayGameSound("HL2Player.SprintStart");
				}
			}
			else
			{
				pPlayer.SetMaxSpeed(190.0);
			}
			
			CBaseCombatWeapon pWeapon = view_as<CBaseCombatWeapon>(pPlayer.GetActiveWeapon());
			if (pWeapon.IsValid())
			{
				if (bIsHoldingSpeed && bWasHoldingSpeed && bIsMoving && bWasMoving)
					SetWeaponAnimation(pWeapon.GetEntIndex(), ACT_VM_SPRINT_IDLE);
				else if ((!bIsHoldingSpeed && bWasHoldingSpeed && bIsMoving) || (bIsHoldingSpeed && !bIsMoving && bWasMoving))
					SetWeaponAnimation(pWeapon.GetEntIndex(), ACT_VM_SPRINT_LEAVE);	
			}
		}
		else
		{
			if(pPlayer.GetTeam() != TEAM_SPECTATOR && GetGameTime() - pPlayer.GetDeathTime() > 1.0)
			{
				int iPressed = pPlayer.GetPressedButtons();
				if(iPressed != 0 && iPressed != IN_SCORE)
				{
					DispatchSpawn(iClient);
				}
			}
		}
	}
}

public MRESReturn Hook_FAllowFlashlight(Handle hReturn, Handle hParams)		// enable sp flashlight
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

public MRESReturn Hook_IsMultiplayer(Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, g_bIsMultiplayerOverride);
	return MRES_Supercede;
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
	if (g_pCoopManager.IsFeaturePatchingEnabled())
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

public void Hook_Use(int entity, int activator, int caller, UseType type, float value)
{
	if (g_pCoopManager.IsFeaturePatchingEnabled())
	{
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(caller);
		if (pPlayer.IsValid() && pPlayer.IsAlive())
		{
			SetPlayerPickup(pPlayer.GetEntIndex(), entity, true);
		}
	}
}

public MRESReturn Hook_SetSuitUpdate(int _this, Handle hParams)		// suit sounds; should be implemented like the game's code
{
	if (g_pCoopManager.IsFeaturePatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szName[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szName, sizeof(szName));
			ClientCommand(_this, "speak %s", szName);	// probably too loud but i can't tell
			PrintToServer("Hook_SetSuitUpdate(...) called %s", szName);
		}
	}
	
	return MRES_Ignored;
}

public void Hook_ExplosionSpawn(int iEntIndex)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		char buffer[MAX_VALUE];
		GetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", buffer, sizeof(buffer)); // this is entity handle m_hEntityIgnore in other games
		if(StrEqual(buffer, "!player"))
		{
			SetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", "");
			SetEntProp(iEntIndex, Prop_Data, "m_iClassIgnore", CLASS_PLAYER);
		}
	}
}

// todo read this later
// http://cdn.akamai.steamstatic.com/steam/apps/362890/manuals/bms_workshop_guide.pdf?t=1431372141
// https://forums.alliedmods.net/showthread.php?t=314271