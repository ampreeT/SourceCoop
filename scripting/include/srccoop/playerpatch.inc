#if defined _srccoop_playerpatch_included
 #endinput
#endif
#define _srccoop_playerpatch_included

//------------------------------------------------------
// Allow flashlight
//------------------------------------------------------
public MRESReturn Hook_FAllowFlashlight(Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, mp_flashlight.BoolValue);
	return MRES_Supercede;
}

//------------------------------------------------------
// Singleplayer (firstperson) death camera
//------------------------------------------------------
public void Hook_CameraDeathSpawn(int iEntIndex)
{
	if (CoopManager.IsFeatureEnabled(FT_FIRSTPERSON_DEATHCAM))
	{
		CBaseEntity pEntity = CBaseEntity(iEntIndex);
		CBlackMesaPlayer pOwner = view_as<CBlackMesaPlayer>(pEntity.GetOwner());
		if (pOwner.IsValid())
		{
			CBaseEntity pViewEntity = pOwner.GetViewEntity();
			if (!pViewEntity.IsValid() || pViewEntity == pOwner)
			{
				pOwner.SetViewEntity(pEntity);
			}
		}
	}
	else
	{
		AcceptEntityInput(iEntIndex, "Kill");
	}
}

void OnEntityDestroyed_CameraDeath(CBaseEntity pEntity)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i))
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer.GetViewEntity() == pEntity)
			{
				pPlayer.SetViewEntity(pPlayer);
			}
		}
	}
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
	
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iClient);
	if (pPlayer.IsAlive())
	{
		if (g_Engine == Engine_BlackMesa)
		{
			CBaseCombatWeapon pWeapon = pPlayer.GetActiveWeapon();
			if (pWeapon.IsValid())
			{
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
		}
	}
	else // Dead
	{
		float flRespawnTime = g_pConvarCoopRespawnTime.FloatValue;
		if (flRespawnTime && pPlayer.GetTeam() > TEAM_SPECTATOR && !SurvivalManager.IsDead(pPlayer) && GetGameTime() - pPlayer.GetDeathTime() > flRespawnTime)
		{
			int iPressed = pPlayer.GetPressedButtons();
			if (iPressed != 0 && iPressed != IN_SCORE)
			{
				DispatchSpawn(iClient);
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

		// fix bug where spectator can't move while free-looking
		if (iMode == OBS_MODE_ROAMING) {
			SetEntityMoveType(iClient, MOVETYPE_NOCLIP);
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
	else if (strncmp(sample, "physics\\footsteps\\", 18) == 0 || strncmp(sample, "player/suit_sprint.wav", 22) == 0)
	{
		// Footsteps are emitted on the server but not sent to client, because the client usually plays them on its own - but not when sprinting in MP mode (actually just pressing +speed!)
		// We can resolve this by sending the footsteps, which are already generated on the server, to the client.
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(entity);
		if (pPlayer.IsValid() && pPlayer.GetButtons() & IN_SPEED)
		{
			clients[0] = entity;
			numClients = 1;
			return Plugin_Changed;
		}
	}
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
// Player spawn
//------------------------------------------------------
public MRESReturn Hook_PlayerSpawn(int _this, Handle hReturn, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBasePlayer pPlayer = CBasePlayer(_this);
		
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

	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iClient);
	if (pPlayer.IsValid())
	{
		CoopManager.OnPlayerSpawned(pPlayer);
		g_pInstancingManager.OnPlayerSpawned(pPlayer);
		
		if (g_Engine == Engine_BlackMesa)
		{
			if (pPlayer.IsAlive())
			{
				// Ensure view entity is player and observer mode is off
				// Set before spawnsystem SpawnPlayer in case view entitiy is changed there
				pPlayer.SetViewEntity(pPlayer);
				pPlayer.SetObserverMode(OBS_MODE_NONE);

				// Fix for if player died on a ladder
				SetEntPropEnt(pPlayer.GetEntIndex(), Prop_Data, "m_hLadder", -1);

				// Fix for client viewmodel animation when always run is off
				sv_always_run.ReplicateToClient(iClient, "1");
				
				// Correct the 'damages' from the prethink hook hack
				pPlayer.SetSuit(false);
				pPlayer.SetMaxSpeed(190.0);
				pPlayer.SetIsSprinting(false);
				pPlayer.SetSolidFlags(pPlayer.GetSolidFlags() & ~FSOLID_NOT_SOLID);
				
				// Stop HEV sounds
				StopSound(iClient, SNDCHAN_STATIC, szLastSuitSound[iClient]);
			}
			else // Not alive => entered the game
			{
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
			}
		}
	}
}

//------------------------------------------------------
// Player death hooks
// - Force serverside ragdolls
//------------------------------------------------------
CBaseEntity g_pNewRagdoll = view_as<CBaseEntity>(-1);

public MRESReturn Hook_PlayerKilled(int _this, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	
	CBasePlayer pPlayer = CBasePlayer(_this);
	if (pPlayer.IsValid())
	{		
		Address pTakeDmgInfo = hParams.GetAddress(1);
		int iForceBone = GetEntProp(_this, Prop_Send, "m_nForceBone");
		g_pNewRagdoll = CBaseEntity(SDKCall(g_pCreateServerRagdoll, _this, iForceBone, pTakeDmgInfo, COLLISION_GROUP_DEBRIS_TRIGGER, false));
		SDKHook(g_pNewRagdoll.GetEntIndex(), SDKHook_OnTakeDamage, Hook_NoDmg);
	}
	return MRES_Ignored;
}

public MRESReturn Hook_PlayerKilledPost(int _this, DHookParam hParams)
{
	CBasePlayer pPlayer = CBasePlayer(_this);
	if (g_pNewRagdoll.IsValid())
	{
		CBaseEntity pRagdoll = pPlayer.GetRagdoll();
		CBaseEntity pViewEnt = pPlayer.GetViewEntity();
		
		if (pRagdoll.IsValid())
		{
			pRagdoll.Kill();
		}
		pPlayer.SetRagdoll(g_pNewRagdoll);
		
		if (pViewEnt.IsValid() && !pViewEnt.IsClassPlayer())
		{
			// Parent death camera to server-side ragdoll
			pViewEnt.SetParent(g_pNewRagdoll);
			pViewEnt.SetParentAttachment("eyes");
		}

		g_pNewRagdoll = CBaseEntity(-1);
		
	}
	if (CoopManager.IsCoopModeEnabled())
	{
		SurvivalManager.HandlePlayerDeath(pPlayer);
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Player TraceAttack hook
//------------------------------------------------------
public Action Hook_PlayerTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
	if (CoopManager.IsCoopModeEnabled() && !mp_friendlyfire.BoolValue)
	{
		// Disable FF
		if (attacker != victim && CBlackMesaPlayer(attacker).IsClassPlayer())
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
		if (!CBM_MP_GameRules.IsTeamplay())
			return MRES_Ignored;
		
		int iTeamNum = DHookGetParam(hParams, 1);
		if (iTeamNum == TEAM_SCIENTIST || iTeamNum == TEAM_MARINES || iTeamNum == TEAM_UNASSIGNED)
		{
			char szTeam[16]; g_pConvarCoopTeam.GetString(szTeam, sizeof(szTeam));
			if (StrEqual(szTeam, "scientist", false))
			{
				DHookSetParam(hParams, 1, TEAM_SCIENTIST);
				return MRES_ChangedHandled;
			}
			else if (StrEqual(szTeam, "marines", false))
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
			view_as<CBlackMesaPlayer>(pParam).ForceDropOfCarriedPhysObjects();
		}
		else
		{
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i))
				{
					// is this player carrying the required item?
					CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
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