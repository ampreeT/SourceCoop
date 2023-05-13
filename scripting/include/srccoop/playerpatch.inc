#if defined _srccoop_playerpatch_included
 #endinput
#endif
#define _srccoop_playerpatch_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Allow flashlight
//------------------------------------------------------
public MRESReturn Hook_FAllowFlashlight(Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, mp_flashlight.BoolValue);
	return MRES_Supercede;
}

//------------------------------------------------------
// The singleplayer override for PreThink handles our
// prop pickup, sprinting, vm anims, suit queue updates, etc
//
// We also add some additional features here
//------------------------------------------------------
public void Hook_PlayerPreThink(int iClient)
{
	if (!CoopManager.IsCoopModeEnabled())
		return;
	
	if (IsPlayerAlive(iClient))
	{
		g_bIsMultiplayerOverride = false;
	}
}

public void Hook_PlayerPreThinkPost(int iClient)
{
	g_bIsMultiplayerOverride = true;
	if (!CoopManager.IsCoopModeEnabled())
		return;
	
	CBasePlayer pPlayer = CBasePlayer(iClient);
	if (pPlayer.IsAlive())
	{
		#if defined SRCCOOP_BLACKMESA

		CBaseCombatWeapon pWeapon = pPlayer.GetActiveWeapon();
		if (pWeapon.IsValid())
		{
			if (PlayingAdmireGloves(pPlayer))
			{
				// if weapon viewmodel index is different at this point, player canceled admire animation
				if ((pWeapon.GetViewModelIndex() != 1) || (pPlayer.GetViewModel().GetWeaponTimeIdle() <= GetGameTime()))
				{
					PlayAdmireGloves(pPlayer, false);
				}
				else
				{
					// this has to be here because the active weapon cant be -1 due to tpose
					PreventWeaponFiring(pWeapon, 1.0);
				}
			}
			
			if (pPlayer.GetCarriedObject().IsValid())
			{
				// fix clientside prediction errors when dropping objects
				PreventWeaponFiring(pWeapon, 1.0);
			}
			else if (pPlayer.GetWaterLevel() == WL_Eyes)
			{
				// fix clientside firing weapons when dragged by beneathticle
				CBaseEntity pParent = pPlayer.GetParent();
				if (pParent.IsValid())
				{
					if (HasEntProp(pParent.GetEntIndex(), Prop_Data, "m_hPrey"))
					{
						if (GetEntPropEnt(pParent.GetEntIndex(), Prop_Data, "m_hPrey") == pPlayer.GetEntIndex())
						{
							if (!pWeapon.m_bFiresUnderwater() && pWeapon.GetPrimaryAmmoType() != -1)
							{
								PreventWeaponFiring(pWeapon, 0.2);
							}
						}
					}
				}
			}
		}

		#endif // SRCCOOP_BLACKMESA
	}
	else // Dead
	{
		if (pPlayer.GetTeam() != TEAM_SPECTATOR)
		{
			float flRespawnTime = g_pConvarCoopRespawnTime.FloatValue;
			if (flRespawnTime && !SurvivalManager.IsDead(pPlayer) && GetGameTime() - pPlayer.GetDeathTime() > flRespawnTime)
			{
				int iPressed = pPlayer.GetPressedButtons();
				if (iPressed != 0 && iPressed != IN_SCORE)
				{
					DispatchSpawn(iClient);
				}
			}	
		}
	}
}

//------------------------------------------------------
// OnPlayerRunCmd
//------------------------------------------------------
public Action OnPlayerRunCmd(int iClient, int &iButtons, int &iImpulse, float fVel[3], float fAngles[3], int &iWeapon, int& subtype, int& cmdnum, int& tickcount, int& seed, int mouse[2])
{
	if (IsFakeClient(iClient) || !CoopManager.IsCoopModeEnabled()) {
		return Plugin_Continue;
	}

	if (mouse[0] || mouse[1])
	{
		g_bPostTeamSelect[iClient] = true;
	}

	// Spectator fixes
	// Credit: harper

	if (IsClientObserver(iClient))
	{
		// hide bugged ctrl menu
		if (g_bPostTeamSelect[iClient] && tickcount % 64 == 0)
			ShowVGUIPanel(iClient, "specmenu", _, false);

		Obs_Mode iMode = view_as<Obs_Mode> (GetEntProp(iClient, Prop_Send, "m_iObserverMode"));
		CBasePlayer pTarget = CBasePlayer(GetEntPropEnt(iClient, Prop_Send, "m_hObserverTarget"));
		CBasePlayer pClient = CBasePlayer(iClient);

		// Make sure target is not an info_observer_menu
		// force free-look where appropriate - this removes the extra (pointless) third person spec mode
		if (iMode == OBS_MODE_POI && pTarget != pClient && (!pTarget.IsValid() || pTarget.IsClassPlayer())) {
			SetEntProp(iClient, Prop_Data, "m_iObserverMode", OBS_MODE_ROAMING);
		}
	}

	return Plugin_Continue;
}

//------------------------------------------------------
// Player sounds
//------------------------------------------------------
char szLastSuitSound[MAXPLAYERS+1][PLATFORM_MAX_PATH];

public Action PlayerSoundListener(int clients[MAXPLAYERS], int &numClients, char sample[PLATFORM_MAX_PATH], int &entity, int &channel, float &volume, int &level, int &pitch, int &flags, char soundEntry[PLATFORM_MAX_PATH], int &seed)
{
	if (strncmp(sample, "!HEV_", 5) == 0)
	{
		// Here we save last played HEV sound so we can stop it on respawn
		for (int i = 0; i < numClients; i++)
		{
			szLastSuitSound[clients[i]] = sample;
		}
	}

	#if defined SRCCOOP_BLACKMESA
	if (strncmp(sample, "physics\\footsteps\\", 18) == 0 || strncmp(sample, "player/suit_sprint.wav", 22) == 0)
	{
		// Footsteps are emitted on the server but not sent to client, because the client usually plays them on its own - but not when sprinting in MP mode (actually just pressing +speed!)
		// We can resolve this by sending the footsteps, which are already generated on the server, to the client.
		CBasePlayer pPlayer = CBasePlayer(entity);
		if (pPlayer.IsValid() && pPlayer.GetButtons() & IN_SPEED)
		{
			clients[0] = entity;
			numClients = 1;
			return Plugin_Changed;
		}
	}
	#endif // SRCCOOP_BLACKMESA

	return Plugin_Continue;
}

public MRESReturn Hook_SetSuitUpdate(int _this, Handle hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_HEV_SOUNDS))
	{
		g_bIsMultiplayerOverride = false;
	}
	return MRES_Ignored;
}

public MRESReturn Hook_SetSuitUpdatePost(int _this, Handle hParams)
{
	g_bIsMultiplayerOverride = true;
	return MRES_Ignored;
}

//------------------------------------------------------
// Player AcceptInput hook
// Prevent input of kill to player as this will put the player in a hanging state
// This can occur in some maps where a trigger is set to kill the !activator and the player so happens to be there
//------------------------------------------------------
public MRESReturn Hook_PlayerAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

		if (strcmp(szInputType, "kill", false) == 0)
		{
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// ClientPutInServer
//------------------------------------------------------
public void PlayerPatch_OnClientPutInServer(CBasePlayer pPlayer)
{
	#if defined SRCCOOP_BLACKMESA
	// fixes bugged trigger_teleport prediction (camera jerking around as if being teleported)
	ClientCommand(pPlayer.GetEntIndex(), "cl_predicttriggers 0");
	#endif // SRCCOOP_BLACKMESA
}

//------------------------------------------------------
// Player spawn
//------------------------------------------------------
public MRESReturn Hook_PlayerSpawn(int _this, Handle hReturn, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBasePlayer pPlayer = CBasePlayer(_this);
		
		// Clear ragdoll parent
		pPlayer.ClearParent();

		// ignore initial in-game spawn
		if (pPlayer.GetTeam() != TEAM_UNASSIGNED)
		{
			if (!SurvivalManager.HandlePlayerSpawn(pPlayer))
			{
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Player spawn post
//------------------------------------------------------
public void Hook_PlayerSpawnPost(int iClient)
{
	if (!CoopManager.IsCoopModeEnabled())
		return;

	CBasePlayer pPlayer = CBasePlayer(iClient);
	if (pPlayer.IsValid())
	{
		if (pPlayer.IsAlive())
		{
			// Ensure death camera and ragdoll get removed
			CBaseEntity pRagdoll = pPlayer.GetRagdoll();
			if (pRagdoll.IsValid())
			{
				pRagdoll.Kill();
			}

			// Ensure view entity is player and observer mode is off
			// Set before spawnsystem SpawnPlayer in case view entitiy is changed there
			pPlayer.SetViewEntity(pPlayer);
			pPlayer.SetObserverMode(OBS_MODE_NONE);

			// Fix for if player died on a ladder
			SetEntPropEnt(pPlayer.GetEntIndex(), Prop_Data, "m_hLadder", -1);

			#if defined SRCCOOP_BLACKMESA

			// Fix for client viewmodel animation when always run is off
			if (!IsFakeClient(iClient))
			{
				sv_always_run.ReplicateToClient(iClient, "1");
				
				QueryClientConVar(iClient, "r_bloomtintexponent_nextgen", QueryConVar_Bloom, 2.2);
				QueryClientConVar(iClient, "r_bloomtintr_nextgen", QueryConVar_Bloom, 0.25);
				QueryClientConVar(iClient, "r_bloomtintb_nextgen", QueryConVar_Bloom, 0.25);
				QueryClientConVar(iClient, "r_bloomtintb_nextgen", QueryConVar_Bloom, 0.25);
				QueryClientConVar(iClient, "gb_flashlight_intensity", QueryConVar_Bloom, 0.8);
			}
			
			// Fix for lag no collide with mp_teamplay 0
			if (!CBM_MP_GameRules.IsTeamplay()) pPlayer.SetCollisionGroup(COLLISION_GROUP_INTERACTIVE_DEBRIS);
			
			// Correct the 'damages' from the prethink hook hack
			pPlayer.SetSuit(false);
			pPlayer.SetMaxSpeed(190.0);
			pPlayer.SetIsSprinting(false);
			pPlayer.SetSolidFlags(pPlayer.GetSolidFlags() & ~FSOLID_NOT_SOLID);
			
			#endif // SRCCOOP_BLACKMESA

			// Stop HEV sounds
			StopSound(iClient, SNDCHAN_STATIC, szLastSuitSound[iClient]);
		}
		else // Not alive => entered the game
		{
			#if defined GAMEPATCH_TEAMSELECT_UI

			if (GetClientTeam(iClient) == TEAM_SPECTATOR)
			{
				if (g_pConvarDisableTeamSelect.BoolValue)
				{
					// hide team select
					ClientCommand(iClient, "hidepanel team");
					ClientCommand(iClient, "hidepanel deathmatch");
					ChangeClientTeam(iClient, TEAM_SCIENTIST);
					g_bPostTeamSelect[iClient] = true;
				}
				else
				{
					// always show DM variant of team select
					ClientCommand(iClient, "hidepanel team");
					ClientCommand(iClient, "showpanel deathmatch");
				}
			}

			#endif // GAMEPATCH_TEAMSELECT_UI
		}

		CoopManager.OnPlayerSpawned(pPlayer);
		g_pInstancingManager.OnPlayerSpawned(pPlayer);
	}
}

//------------------------------------------------------
// Player death hooks
// - Force serverside ragdolls
//------------------------------------------------------
#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
CBaseEntity g_pNewRagdoll = NULL_CBASEENTITY;
#endif

public MRESReturn Hook_PlayerKilled(int _this, DHookParam hParams)
{
	#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS

	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	
	CBasePlayer pPlayer = CBasePlayer(_this);
	if (pPlayer.IsValid())
	{
		Address pTakeDmgInfo = hParams.GetAddress(1);
		int iForceBone = GetEntProp(_this, Prop_Send, "m_nForceBone");
		g_pNewRagdoll = CBaseEntity(SDKCall(g_pCreateServerRagdoll, _this, iForceBone, pTakeDmgInfo, COLLISION_GROUP_DEBRIS_TRIGGER, false));
		SDKHook(g_pNewRagdoll.GetEntIndex(), SDKHook_OnTakeDamage, Hook_NoDmg);

		#if defined SRCCOOP_HL2DM
		PlayerPatch_ApplyServerRagdoll(pPlayer);
		#endif
	}

	#endif // PLAYERPATCH_SERVERSIDE_RAGDOLLS

	return MRES_Ignored;
}

public MRESReturn Hook_PlayerKilledPost(int _this, DHookParam hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBasePlayer pPlayer = CBasePlayer(_this);
	
		#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS

		#if defined SRCCOOP_BLACKMESA
		PlayerPatch_ApplyServerRagdoll(pPlayer);
		#endif
		
		#if defined SRCCOOP_HL2DM
		// Tells the death think not to move the player anymore. Player used to move despite being parented to its ragdoll, causing rendering issues.
		pPlayer.SetFlags(FL_ONGROUND);
		pPlayer.Teleport(vec3_origin, NULL_VECTOR, vec3_origin);
		#endif
		
		#endif // PLAYERPATCH_SERVERSIDE_RAGDOLLS

		SurvivalManager.HandlePlayerDeath(pPlayer);
	}
	return MRES_Ignored;
}

void PlayerPatch_ApplyServerRagdoll(CBasePlayer pPlayer)
{
	if (!g_pNewRagdoll.IsValid())
		return;

	CBaseEntity pRagdoll = pPlayer.GetRagdoll();
	if (pRagdoll.IsValid())
	{
		pRagdoll.Kill();
	}
	pPlayer.SetRagdoll(g_pNewRagdoll);
	
	#if defined SRCCOOP_HL2DM
	// Bone merge player to the new ragdoll. This ensures thirdperson death camera actually follows it (firstperson is unaffected).
	// The downside is we have to keep check on its removal in order not to kill the player entity.
	pPlayer.SetParent(g_pNewRagdoll);
	pPlayer.SetParentAttachment("forward");
	pPlayer.SetEffects(pPlayer.GetEffects() | EF_BONEMERGE);
	#endif

	#if defined SRCCOOP_BLACKMESA
	pPlayer.SetObserverMode(OBS_MODE_CHASE);
	pPlayer.SetObserverTarget(g_pNewRagdoll);
	#endif

	Call_StartForward(g_OnPlayerRagdollCreatedFwd);
	Call_PushCell(pPlayer);
	Call_PushCell(g_pNewRagdoll);
	Call_Finish();
	
	g_pNewRagdoll = NULL_CBASEENTITY;
}

public MRESReturn Hook_CreateRagdollEntity(int _this)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	// avoid invalid cast on m_hRagdoll in hl2mp with the serverside ragdolls
	return MRES_Supercede;
}

public void OnEntityDestroyed_Ragdoll(CBaseEntity pRagdoll)
{
	CBasePlayer pPlayerChild = view_as<CBasePlayer>(pRagdoll.GetMoveChild());
	if (pPlayerChild.IsValid())
	{
		// prevent parented player entity removal
		pPlayerChild.ClearParent();
	}
}

//------------------------------------------------------
// OnClientDisconnect
//------------------------------------------------------
public void PlayerPatch_OnClientDisconnect(CBasePlayer pPlayer)
{
	#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS

	if (pPlayer.IsValid())
	{
		// prevent hl2dm crash with serverside ragdolls
		CBaseEntity pRagdoll = pPlayer.GetRagdoll();
		if (pRagdoll.IsValid())
		{
			pRagdoll.Kill();
		}
	}

	#endif // SRCCOOP_HL2DM && PLAYERPATCH_SERVERSIDE_RAGDOLLS
}

//------------------------------------------------------
// Player TraceAttack hook
//------------------------------------------------------
public Action Hook_PlayerTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
	if (CoopManager.IsCoopModeEnabled() && !mp_friendlyfire.BoolValue)
	{
		// Disable FF
		if (attacker != victim && CBasePlayer(attacker).IsClassPlayer())
		{
			return Plugin_Handled;
		}
	}
	return Plugin_Continue;
}

//------------------------------------------------------
// Player TakeDamage hook
//------------------------------------------------------
public Action Hook_PlayerTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype)
{
	if (CoopManager.IsFeatureEnabled(FT_NO_TELEFRAGGING))
	{
		CBaseEntity pInflictor = CBaseEntity(inflictor);
		
		// Disable telefragging (CTriggerTeleport::TelefragEntities)
		if (pInflictor.IsValid() && pInflictor.IsClassname("trigger_teleport"))
		{
			return Plugin_Handled;
		}
	}
	if (CoopManager.IsCoopModeEnabled() && !mp_friendlyfire.BoolValue)
	{
		// Disable FF from other sources such as the tau cannon
		if (attacker != victim && CBasePlayer(attacker).IsClassPlayer())
		{
			return Plugin_Handled;
		}
	}
	return Plugin_Continue;
}

//------------------------------------------------------
// Player team change hooks
// - Force everyone to single team in TDM
//------------------------------------------------------
public MRESReturn Hook_PlayerChangeTeam(int _this, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		if (!CMultiplayRules.IsTeamplay())
			return MRES_Ignored;
		
		int iTeamNum = DHookGetParam(hParams, 1);
		if (iTeamNum == TEAM_SCIENTIST || iTeamNum == TEAM_MARINES || iTeamNum == TEAM_UNASSIGNED)
		{
			char szTeam[16]; g_pConvarCoopTeam.GetString(szTeam, sizeof(szTeam));
			if (StrEqual(szTeam, "scientist", false) || StrEqual(szTeam, "3"))
			{
				DHookSetParam(hParams, 1, TEAM_SCIENTIST);
				return MRES_ChangedHandled;
			}
			else if (StrEqual(szTeam, "marines", false) || StrEqual(szTeam, "2"))
			{
				DHookSetParam(hParams, 1, TEAM_MARINES);
				return MRES_ChangedHandled;
			}
			if (iTeamNum == TEAM_UNASSIGNED)
			{
				DHookSetParam(hParams, 1, TEAM_SCIENTIST);
				return MRES_ChangedHandled;
			}
		}
	}
	return MRES_Ignored;
}

public MRESReturn Hook_PlayerChangeTeamPost(int _this, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		SurvivalManager.GameOverCheck();
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// NoBlock
//------------------------------------------------------
public MRESReturn Hook_PlayerShouldCollide(int _this, Handle hReturn, Handle hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_NOBLOCK))
	{
		int iCollisionGroup = DHookGetParam(hParams, 1);
		if (iCollisionGroup == COLLISION_GROUP_PLAYER || iCollisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT)
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Pickup_ForcePlayerToDropThisObject
// reconstructed to avoid crash on UTIL_GetLocalPlayer nullptr
//------------------------------------------------------
public MRESReturn Hook_ForcePlayerToDropThisObject(Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		// singleplayer doesn't care what's passed in so we have to check for both player or item
		CBaseEntity pParam = CBaseEntity(DHookGetParam(hParams, 1));
		if (pParam.IsClassPlayer())
		{
			view_as<CBasePlayer>(pParam).ForceDropOfCarriedPhysObjects();
		}
		else
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i))
				{
					// is this player carrying the required item?
					CBasePlayer pPlayer = CBasePlayer(i);
					if (pPlayer.GetCarriedObject() == pParam)
					{
						pPlayer.ForceDropOfCarriedPhysObjects();
						break;
					}
				}
			}
		}
	}
	return MRES_Supercede;
}

//------------------------------------------------------
// Admire gloves animation
//------------------------------------------------------
bool g_bPlayingAdmireGloves[MAXPLAYERS+1];

public void PlayAdmireGloves(CBasePlayer pPlayer, const bool bStartAnim)
{
	CPredictedViewModel pViewModel = pPlayer.GetViewModel();
	if (pViewModel.IsValid())
	{
		if (bStartAnim)
		{
			g_bPlayingAdmireGloves[pPlayer.GetEntIndex()] = true;
			
			if (!IsModelPrecached("models/weapons/v_hands.mdl")) PrecacheModel("models/weapons/v_hands.mdl");
			pViewModel.SetModel("models/weapons/v_hands.mdl");
			
			if (pPlayer.GetActiveWeapon().IsValid())
			{
				// This will force weapon to not reset the sequence on viewmodel once client-side flCycle reaches 1.0
				pPlayer.GetActiveWeapon().SetViewModelIndex(1);
				pPlayer.GetActiveWeapon().AcceptInput("HideWeapon");
			}
			
			// Reset cycle and playbackrate
			pViewModel.SetCycle(0.0);
			pViewModel.SetPlaybackRate(1.0);
			// This is an estimated time of sequence due to m_flCycle not incrementing
			// CBaseCombatWeapon and CPredictedViewModel have .Get and .SetWeaponIdleTime()
			pViewModel.SetWeaponTimeIdle(GetGameTime() + 6.0);
		}
		else
		{
			g_bPlayingAdmireGloves[pPlayer.GetEntIndex()] = false;
			
			CBaseCombatWeapon pVMActiveWeapon = pViewModel.GetOwningWeapon();
			pPlayer.EquipPlayerWeapon(pVMActiveWeapon);
			
			CBaseCombatWeapon pLastWeapon = pPlayer.GetLastWeapon();
			if (pLastWeapon.IsValid())
			{
				pLastWeapon.SetViewModelIndex(0);
			}
			
			if (pVMActiveWeapon.IsValid())
			{
				pVMActiveWeapon.SetViewModelIndex(0);
				
				// Reset viewmodel if exists
				char szModelName[64];
				pVMActiveWeapon.GetModelName(szModelName, sizeof(szModelName));
				ReplaceString(szModelName, sizeof(szModelName), "/w_", "/v_", false);
				if (FileExists(szModelName, true, NULL_STRING)) pViewModel.SetModel(szModelName);
			}
		}
	}
}

public bool PlayingAdmireGloves(CBasePlayer pPlayer)
{
	return g_bPlayingAdmireGloves[pPlayer.GetEntIndex()];
}

//------------------------------------------------------
// Check for default broken values and fix them
// This occurs if the client has not loaded any background map which can happen from either running -dev or by direct steam://connect links.
//------------------------------------------------------
public void QueryConVar_Bloom(QueryCookie cookie, int iClient, ConVarQueryResult result, const char[] cvarName, const char[] cvarValue, float flDefaultValue)
{
	if (!strcmp(cvarName, "r_bloomtintexponent_nextgen"))
	{
		if (!strcmp(cvarValue, "0.750"))
		{
			ClientCommand(iClient, "%s %f", cvarName, flDefaultValue);
		}
	}
	else if (!strcmp(cvarName, "gb_flashlight_intensity"))
	{
		if (!strcmp(cvarValue, "2.5"))
		{
			ClientCommand(iClient, "%s %f", cvarName, flDefaultValue);
		}
	}
	else if (!strcmp(cvarValue, "1.0"))
	{
		ClientCommand(iClient, "%s %f", cvarName, flDefaultValue);
	}
}