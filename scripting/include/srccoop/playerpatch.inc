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
// Default spawn items
//------------------------------------------------------
public MRESReturn Hook_GiveDefaultItems(int _this)
{
	if (CoopManager.IsCoopModeEnabled() && !g_pSpawnSystemData.m_bDefaultGameEquipment)
	{
		return MRES_Supercede;
	}
	return MRES_Ignored;
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
	
	#if defined SRCCOOP_BLACKMESA
	IdleAnims_Hook_PlayerPreThinkPost(iClient);
	#endif
	
	CBasePlayer pPlayer = CBasePlayer(iClient);
	if (pPlayer.IsAlive())
	{
		#if defined SRCCOOP_BLACKMESA
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
							if (!pWeapon.CanFireUnderwater() && pWeapon.GetPrimaryAmmoType() != -1)
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
			if (GetGameTime() - pPlayer.GetDeathTime() > g_pConvarCoopRespawnTime.FloatValue)
			{
				int iPressed = pPlayer.GetPressedButtons();
				if (iPressed != 0 && iPressed != IN_SCORE)
				{
					pPlayer.SetGroundEntity(NULL_CBASEENTITY);
					pPlayer.SetButtons(0);
					DispatchSpawn(iClient);
				}
			}
		}
	}
}

//------------------------------------------------------
// CBasePlayer::ForceRespawn
// [Black Mesa] Restores mp_forcerespawn functionality,
// allowing the player to stay dead (e.g. for revives) 
//------------------------------------------------------
public MRESReturn Hook_PlayerForceRespawn(int iClient)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	
	CBasePlayer pPlayer = CBasePlayer(iClient);
	float flDeathTime = pPlayer.GetDeathTime();
	if (g_bUnblockRespawn[iClient]
		|| flDeathTime == 0.0
		|| (mp_forcerespawn.BoolValue && (GetGameTime() - flDeathTime) > g_pConvarCoopRespawnTime.FloatValue)
	) {
		g_bUnblockRespawn[iClient] = false;
		return MRES_Ignored;
	}
	return MRES_Supercede;
}

//------------------------------------------------------
// CBasePlayer::StartObserverMode
// [Black Mesa] Force "chase" death camera. This skips the problematic death/freezecam.
//------------------------------------------------------
public MRESReturn Hook_PlayerStartObserverMode(int iClient, DHookReturn hReturn, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	
	Obs_Mode mode = hParams.Get(1);
	if (mode == OBS_MODE_FREEZECAM || mode == OBS_MODE_DEATHCAM)
	{
		hParams.Set(1, OBS_MODE_CHASE);
		return MRES_Handled;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// OnPlayerRunCmd
//------------------------------------------------------
public Action OnPlayerRunCmd(int iClient, int &iButtons, int &iImpulse, float fVel[3], float fAngles[3], int &iWeapon, int& subtype, int& cmdnum, int& tickcount, int& seed, int mouse[2])
{
	if (IsFakeClient(iClient) || !CoopManager.IsCoopModeEnabled())
		return Plugin_Continue;

	if (mouse[0] || mouse[1])
		g_bPostTeamSelect[iClient] = true;

	// Spectator fixes
	// Credit: harper

	Obs_Mode iObsMode = view_as<Obs_Mode> (GetEntProp(iClient, Prop_Send, "m_iObserverMode"));
	if (iObsMode > OBS_MODE_DEATHCAM)
	{
		// hide bugged ctrl menu
		if (g_bPostTeamSelect[iClient] && tickcount % 10 == 0)
			ShowVGUIPanel(iClient, "specmenu", _, false);

		CBasePlayer pTarget = CBasePlayer(GetEntPropEnt(iClient, Prop_Send, "m_hObserverTarget"));
		CBasePlayer pClient = CBasePlayer(iClient);

		// Make sure target is not an info_observer_menu
		// force free-look where appropriate - this removes the extra (pointless) third person spec mode
		if (iObsMode == OBS_MODE_POI && pTarget != pClient && (!pTarget.IsValid() || pTarget.IsClassPlayer()))
		{
			SetEntProp(iClient, Prop_Data, "m_iObserverMode", OBS_MODE_ROAMING);
		}
	}

	if (g_iAddButtons[iClient])
	{
		iButtons |= g_iAddButtons[iClient];
		g_iAddButtons[iClient] = 0;
		return Plugin_Changed;
	}
	
	#if defined PLAYERPATCH_FINDLADDER
	// By using m_afButtonPressed, this is actually ignored by the game code if players mount ladders the normal way as it calls ::SwallowUseKey()
	if ((GetEntProp(iClient, Prop_Data, "m_afButtonPressed") & IN_USE) && (GetEntProp(iClient, Prop_Data, "m_afButtonLast") & IN_USE) == 0)
	{
		Movement_FindLadder(CBasePlayer(iClient));
	}
	#endif
	
	return Plugin_Continue;
}

//------------------------------------------------------
// Player commands
//------------------------------------------------------
public Action PlayerCommandListener(int client, const char[] command, int argc)
{
	int spec2Buttons;
	if (StrEqual(command, "spec_next"))
	{
		spec2Buttons = IN_ATTACK;
	}
	else if (StrEqual(command, "spec_prev"))
	{
		spec2Buttons = IN_ATTACK2;
	}
	else if (StrEqual(command, "spec_mode"))
	{
		spec2Buttons = IN_JUMP;
	}

	if (spec2Buttons)
	{
		CBasePlayer pPlayer = CBasePlayer(client);
		if (pPlayer.GetObserverMode() != OBS_MODE_NONE /* We are spectating something */
			&& pPlayer.GetTeam() != TEAM_SPECTATOR /* But we're still able to respawn */
			&& (!SurvivalManager.GetMode() || !g_pSurvivalManagerData.m_bSpecInit[client]) /* In survival, allow spectate controls */
		) {
			// Translate commands back to buttons so spawning works as normal.
			// (Not necessary for BM)
			g_iAddButtons[client] |= spec2Buttons;
			return Plugin_Handled;
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
	#if defined PLAYERPATCH_SUIT_SOUNDS
	if (strncmp(sample, "!HEV_", 5) == 0)
	{
		// Here we save last played HEV sound so we can stop it on respawn
		for (int i = 0; i < numClients; i++)
		{
			szLastSuitSound[clients[i]] = sample;
		}
	}
	#endif

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
public void PlayerPatch_OnClientPutInServer(int client)
{
	SendClientConvars(client, false);
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
		if (pPlayer.GetSimulationTick() != -1)
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
public MRESReturn Hook_PlayerSpawnPost(int iClient, Handle hReturn, Handle hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;

	// BUG: sometimes SourceTV can slip through even though we never hook it?
	if (IsFakeClient(iClient))
		return MRES_Ignored;
	
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

			// Apply on-spawn convars
			SendClientConvars(iClient, true);

			#if defined SRCCOOP_BLACKMESA

			QueryClientConVar(iClient, "r_bloomtintexponent_nextgen", QueryConVar_Bloom, 2.2);
			QueryClientConVar(iClient, "r_bloomtintr_nextgen", QueryConVar_Bloom, 0.25);
			QueryClientConVar(iClient, "r_bloomtintg_nextgen", QueryConVar_Bloom, 0.25);
			QueryClientConVar(iClient, "r_bloomtintb_nextgen", QueryConVar_Bloom, 0.25);
			QueryClientConVar(iClient, "gb_flashlight_intensity", QueryConVar_Bloom, 0.8);

			// Fix for lag no collide with mp_teamplay 0
			if (!CBM_MP_GameRules.IsTeamplay()) pPlayer.SetCollisionGroup(COLLISION_GROUP_INTERACTIVE_DEBRIS);
			
			// Correct the 'damages' from the prethink hook hack
			pPlayer.SetSuit(false);
			pPlayer.SetMaxSpeed(190.0);
			pPlayer.SetIsSprinting(false);
			pPlayer.SetSolidFlags(pPlayer.GetSolidFlags() & ~FSOLID_NOT_SOLID);
			
			#endif // SRCCOOP_BLACKMESA

			#if defined PLAYERPATCH_SUIT_SOUNDS

			// Stop HEV sounds
			pPlayer.ClearSuitQueue();
			StopSound(iClient, SNDCHAN_STATIC, szLastSuitSound[iClient]);
			
			#endif // PLAYERPATCH_SUIT_SOUNDS

			CoopManager.OnPlayerSpawned(pPlayer);
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
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Player death hooks
// - Force serverside ragdolls
//------------------------------------------------------
#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
bool g_bServerRagdoll[MAXPLAYERS + 1];
#endif

public MRESReturn Hook_PlayerKilled(int _this, DHookParam hParams)
{
	#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	g_bServerRagdoll[_this] = false;
	#endif

	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;
	
	CBasePlayer pPlayer = CBasePlayer(_this);
	PackPlayerItems(pPlayer);

	return MRES_Ignored;
}

public MRESReturn Hook_PlayerKilledPost(int _this, DHookParam hParams)
{
	if (!CoopManager.IsCoopModeEnabled())
		return MRES_Ignored;

	CBasePlayer pPlayer = CBasePlayer(_this);

	#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
	Address pTakeDmgInfo = hParams.GetAddress(1);
	PlayerPatch_ApplyServerRagdoll(pPlayer, pTakeDmgInfo);
	#endif
	
	SurvivalManager.OnPlayerDeath(pPlayer);

	return MRES_Ignored;
}

#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS

void PlayerPatch_ApplyServerRagdoll(CBasePlayer pPlayer, Address pTakeDmgInfo)
{
	int iDmgType = LoadFromAddress(pTakeDmgInfo + view_as<Address>(60), NumberType_Int32);
	if (iDmgType & DMG_DISSOLVE)
	{
		// dont bother
		return;
	}

	CBaseAnimating pRagdoll = pPlayer.GetRagdoll();
	if (pRagdoll != NULL_CBASEENTITY)
	{
		pRagdoll.Kill();
	}

	if (pPlayer.GetTeam() == TEAM_SPECTATOR)
	{
		// teamchange induced suicide - no ragdolls
		return;
	}
	
	#if defined SRCCOOP_BLACKMESA
	int iRagdollInheritIndex;
	if (g_pIdle[pPlayer.GetEntIndex()].m_pModel.IsValid())
	{
		// Player is currently using model for idle animations.
		// The nodraw is required for hiding the player the tick he dies.
		// The model that was parented is cleared since `CreateServerRagdoll` will not work otherwise.
		pPlayer.AddEffects(EF_NODRAW);
		g_pIdle[pPlayer.GetEntIndex()].m_pModel.ClearParent();
		iRagdollInheritIndex = g_pIdle[pPlayer.GetEntIndex()].m_pModel.GetEntIndex();
	}
	else
	{
		pPlayer.RemoveEffects(EF_NODRAW);
		iRagdollInheritIndex = pPlayer.GetEntIndex();
	}
	#else
	int iRagdollInheritIndex = pPlayer.GetEntIndex();
	#endif
	
	pRagdoll = CBaseAnimating(SDKCall(g_pCreateServerRagdoll, iRagdollInheritIndex, pPlayer.GetForceBone(), pTakeDmgInfo, COLLISION_GROUP_DEBRIS_TRIGGER, false));
	if (pRagdoll.IsValid())
	{
		SDKHook(pRagdoll.entindex, SDKHook_OnTakeDamage, Hook_NoDmg);

		pPlayer.SetRagdoll(pRagdoll);
		pPlayer.SetObserverMode(OBS_MODE_CHASE);
		pPlayer.SetObserverTarget(pRagdoll);
		g_bServerRagdoll[pPlayer.entindex] = true;

		Call_StartForward(g_OnPlayerRagdollCreatedFwd);
		Call_PushCell(pPlayer);
		Call_PushCell(pRagdoll);
		Call_Finish();
	}
}

public MRESReturn Hook_CreateRagdollEntity(int _this)
{
	// avoid invalid cast on m_hRagdoll in hl2mp with the serverside ragdolls
	if (g_bServerRagdoll[_this])
		return MRES_Supercede;
	
	return MRES_Ignored;

}

#endif // PLAYERPATCH_SERVERSIDE_RAGDOLLS

//------------------------------------------------------
// OnClientDisconnect
//------------------------------------------------------
public void PlayerPatch_OnClientDisconnect(CBasePlayer pPlayer)
{
	if (!CoopManager.IsCoopModeEnabled())
		return;

	if (pPlayer.IsValid())
	{
		ClearPackedItems(pPlayer);

		#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
		// prevent hl2dm crash with serverside ragdolls
		CBaseEntity pRagdoll = pPlayer.GetRagdoll();
		if (pRagdoll.IsValid())
		{
			pRagdoll.Kill();
		}
		#endif // SRCCOOP_HL2DM && PLAYERPATCH_SERVERSIDE_RAGDOLLS
	}
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
	if (!CoopManager.IsCoopModeEnabled())
		return Plugin_Continue;

	if (!mp_friendlyfire.BoolValue)
	{
		// Disable FF from other sources such as the tau cannon
		if (attacker != victim && CBasePlayer(attacker).IsClassPlayer())
		{
			return Plugin_Handled;
		}
	}

	CBaseEntity pInflictor = CBaseEntity(inflictor);
	if (pInflictor != NULL_CBASEENTITY)
	{
		// Disable telefragging (CTriggerTeleport::TelefragEntities)
		if (pInflictor.IsClassname("trigger_teleport"))
		{
			if (CoopManager.IsFeatureEnabled(FT_NOBLOCK))
			{
				return Plugin_Handled;
			}
		}
	}
	
	return Plugin_Continue;
}

//------------------------------------------------------
// Player team change hooks
// - Force everyone to single team in TDM
//------------------------------------------------------
public MRESReturn Hook_PlayerChangeTeam(int iClient, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled() && iClient != -1)
	{
		// Allow next ForceRespawn() call after a team change
		g_bUnblockRespawn[iClient] = true;

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

public MRESReturn Hook_PlayerChangeTeamPost(int iClient, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		CBasePlayer pPlayer = CBasePlayer(iClient);
		int iTeamNum = DHookGetParam(hParams, 1);
		if (iTeamNum == TEAM_SPECTATOR)
		{
			CBaseEntity pRagdoll = pPlayer.GetRagdoll();
			if (pRagdoll.IsValid())
			{
				pRagdoll.Kill();
			}
		}

		SurvivalManager.OnPlayerChangeTeam(pPlayer, iTeamNum);
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
// CHL2GameMovement::Findladder
// fix issue of FindLadder trace not passing through additional players making it difficult to use ladders with many players
//------------------------------------------------------
public void Movement_FindLadder(CBasePlayer pPlayer)
{
	// If we don't have a valid player, don't run
	if (!pPlayer.IsValid() || pPlayer.GetLadder().IsValid())
		return;
	
	static float flMaxDist = 64.0;
	float bestDist = 16384.0; // Or MAX_COORD_INTEGER
	float bestOrigin[3];
	CFuncLadder bestLadder = NULL_CBASEENTITY;
	
	//float maxdistSqr = flMaxDist * flMaxDist;
	float closest[3];
	
	// Normally this would be by static member: CFuncLadder::GetLadderCount()
	ArrayList pLadders = new ArrayList();
	int iEntity = -1;
	while ((iEntity = FindEntityByClassname(iEntity, "func_useableladder")) != INVALID_ENT_REFERENCE)
	{
		pLadders.Push(CFuncLadder(iEntity));
	}
	
	float vecPlayerOrigin[3];
	float vecTmp[3];
	float distSqr;
	pPlayer.GetAbsOrigin(vecPlayerOrigin);
	pPlayer.GetMaxs(vecTmp);
	float sizeZ = vecTmp[2];
	pPlayer.GetMins(vecTmp);
	sizeZ = (sizeZ - vecTmp[2]) * 0.5;
	
	int len = pLadders.Length;
	for (int i = 0; i < len; i++)
	{
		CFuncLadder pLadder = pLadders.Get(i);
		
		if (!pLadder.IsEnabled())
			continue;
		
		float topPosition[3], bottomPosition[3];
		pLadder.GetTopPosition(topPosition);
		pLadder.GetBottomPosition(bottomPosition);
		
		if ((vecPlayerOrigin[2] < bottomPosition[2]) || (vecPlayerOrigin[2] > topPosition[2]))
			continue;
		
		// Have to do both for slanted ladders, also now just checking for bottom and top instead of anywhere along ladder line,
		// this could potentially be changed by setting bottomPosition[2] = vecPlayerOrigin[2] and test distance though.
		float distTop = GetVectorDistance(vecPlayerOrigin, topPosition, false);
		float distBot = GetVectorDistance(vecPlayerOrigin, bottomPosition, false);
		if (distTop > distBot)
		{
			distSqr = distBot;
			closest = bottomPosition;
		}
		else
		{
			distSqr = distTop;
			closest = topPosition;
		}
		
		if (distSqr > flMaxDist)
			continue;
		
		// Finally the traces that need to be overridden to ignore players in the way...
		CTraceRay pTraceRay = new CTraceRay(vecPlayerOrigin, closest, MASK_PLAYERSOLID, RayType_EndPoint, TraceEntityFilter_IgnorePlayers, -1);
		if (pTraceRay.GetFraction() != 1.0 && pTraceRay.GetEntity() != pLadder)
		{
			pTraceRay.Close();
			
			AddVectors(vecTmp, vecPlayerOrigin, vecTmp);
			vecTmp[2] += sizeZ;
			pTraceRay = new CTraceRay(vecTmp, closest, MASK_PLAYERSOLID, RayType_EndPoint, TraceEntityFilter_IgnorePlayers, -1);
			if (pTraceRay.GetFraction() != 1.0 && pTraceRay.GetEntity() != pLadder)
			{
				pTraceRay.Close();
				continue;
			}
		}
		pTraceRay.Close();
		
		// See if this is the best one so far
		if (distSqr < bestDist)
		{
			bestDist = distSqr;
			bestLadder = pLadder;
			bestOrigin = closest;
		}
	}
	pLadders.Close();
	
	if (bestLadder != NULL_CBASEENTITY)
	{
		// Because there is no smoothing/adjustments, jump up to where player feet are
		if (vecPlayerOrigin[2] > bestOrigin[2])
			bestOrigin[2] = vecPlayerOrigin[2];
		pPlayer.Teleport(bestOrigin);
		pPlayer.SetLadder(bestLadder);
	}
}

//------------------------------------------------------
// Item packing
// Detaches and stores player's equipped items.
// Makes it possible to re-equip the player on potential revives.
//------------------------------------------------------
CCoopEquipment g_pPackedEquipment[MAXPLAYERS + 1];

bool HasPackedItems(CBasePlayer pPlayer)
{
	return g_pPackedEquipment[pPlayer.GetEntIndex()].IsInitialized();
}

void PackPlayerItems(CBasePlayer pPlayer)
{
	ClearPackedItems(pPlayer);
	
	int client = pPlayer.GetEntIndex();
	g_pPackedEquipment[client].Initialize(true);
	g_pPackedEquipment[client].Fill(pPlayer);
	
	int size = g_pPackedEquipment[client].m_pWeaponEntityList.Length;
	for (int i = 0; i < size; i++)
	{
		CBaseCombatWeapon pWeapon = g_pPackedEquipment[client].m_pWeaponEntityList.Get(i);
		pPlayer.RemoveWeapon(pWeapon);
		pWeapon.SetModelIndex(pWeapon.GetWeaponModelIndex(.bViewModel = false));
		pWeapon.SetSpawnFlags(pWeapon.GetSpawnFlags() | SF_ITEM_NORESPAWN);
		pWeapon.AddEffects(EF_NODRAW | EF_NOSHADOW);
	}
	pPlayer.RemoveAllItems();
}

void RestorePackedItems(CBasePlayer pPlayer)
{
	int client = pPlayer.GetEntIndex();
	if (!g_pPackedEquipment[client].IsInitialized())
		return;

	#if defined SRCCOOP_BLACKMESA
	// Prevent a small chance of viewmodel becoming invisible on client AFTER switching weapons
	for (int i = 0; i < MAX_VIEWMODELS; i++)
	{
		CPredictedViewModel vm = pPlayer.GetViewModel(i);
		if (vm != NULL_CBASEENTITY)
		{
			vm.Kill();
			pPlayer.SetViewModel(NULL_CBASEENTITY, i);
			pPlayer.CreateViewModel(i);
		}
	}
	#endif

	g_pPackedEquipment[client].Equip(pPlayer);
	g_pPackedEquipment[client].Close();
}

void ClearPackedItems(CBasePlayer pPlayer)
{
	int client = pPlayer.GetEntIndex();
	if (!g_pPackedEquipment[client].IsInitialized())
		return;

	int size = g_pPackedEquipment[client].m_pWeaponEntityList.Length;
	for (int i = 0; i < size; i++)
	{
		CBaseEntity pItem = g_pPackedEquipment[client].m_pWeaponEntityList.Get(i);
		if (pItem.IsValid())
		{
			pItem.Kill();
		}
	}
	g_pPackedEquipment[client].Close();
}

//------------------------------------------------------
// Weapon prediction fix implemented
// using the pack & restore mechanism
//------------------------------------------------------

bool FixWeaponPrediction(CBasePlayer pPlayer)
{
	if (HasPackedItems(pPlayer))
		return false;
	
	PackPlayerItems(pPlayer);
	CreateTimer(0.1, Timer_FixWeaponPrediction, pPlayer, TIMER_FLAG_NO_MAPCHANGE);
	return true;
}

Action Timer_FixWeaponPrediction(Handle timer, CBasePlayer pPlayer)
{
	if (pPlayer.IsValid() && pPlayer.IsAlive())
		RestorePackedItems(pPlayer);

	return Plugin_Handled;
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

void SendClientConvars(int client, bool bSpawn)
{
	CClientConsoleCommand cmd;
	int count = g_pLevelLump.m_pClientCvarList.Length;
	for (int i = 0; i < count; i++)
	{
		g_pLevelLump.m_pClientCvarList.GetArray(i, cmd);
		if (bSpawn != cmd.m_bApplyOnSpawn)
		{
			continue;
		}
		if (cmd.m_pConvar)
		{
			cmd.m_pConvar.ReplicateToClient(client, cmd.m_szValue);
		}
		else
		{
			// Verify it's actual clientside convar and not some malicious command
			QueryClientConVar(client, cmd.m_szConvar, QueryConVarSafeSet, i);
		}
	}
}

void QueryConVarSafeSet(QueryCookie cookie, int client, ConVarQueryResult result, const char[] cvarName, const char[] cvarValue, any cvarListIndex)
{
	if (result == ConVarQuery_Okay || result == ConVarQuery_Protected)
	{
		CClientConsoleCommand cmd;
		g_pLevelLump.m_pClientCvarList.GetArray(cvarListIndex, cmd);
		char szCmd[256];
		FormatEx(szCmd, sizeof(szCmd), "%s %s", cmd.m_szConvar, cmd.m_szValue);
		ClientCommand(client, szCmd);
	}
	else if (result == ConVarQuery_NotValid)
	{
		LogMessage("Issuing client commands is not allowed. Please remove \"%s\" from client_console in the map's edt file.", cvarName);
	}
}
