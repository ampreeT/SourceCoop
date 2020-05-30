#if defined _srccoop_playerpatch_included
 #endinput
#endif
#define _srccoop_playerpatch_included

//------------------------------------------------------
// The singleplayer override for PreThink handles our
// prop pickup, sprinting, vm anims, suit queue updates, etc
//
// We also add some additional features here
//------------------------------------------------------
public void Hook_PlayerPreThink(int iClient)
{
	if(!g_pCoopManager.IsCoopModeEnabled())
		return;
		
	if(IsPlayerAlive(iClient))
		g_bIsMultiplayerOverride = false;
}

public void Hook_PlayerPreThinkPost(int iClient)
{
	g_bIsMultiplayerOverride = true;
	if(!g_pCoopManager.IsCoopModeEnabled())
		return;
	
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iClient);
	if(pPlayer.IsAlive())
	{	
	}
	else
	{
		if(pPlayer.GetTeam() != TEAM_SPECTATOR && GetGameTime() - pPlayer.GetDeathTime() > g_pConvarCoopRespawnTime.FloatValue)
		{
			int iPressed = pPlayer.GetPressedButtons();
			if(iPressed != 0 && iPressed != IN_SCORE)
			{
				DispatchSpawn(iClient);
			}
		}
	}
}

//------------------------------------------------------
// Player sounds
//------------------------------------------------------
char szLastSuitSound[MAXPLAYERS+1][PLATFORM_MAX_PATH];

public Action PlayerSoundListener(int clients[MAXPLAYERS], int &numClients, char sample[PLATFORM_MAX_PATH], int &entity, int &channel, float &volume, int &level, int &pitch, int &flags, char soundEntry[PLATFORM_MAX_PATH], int &seed)
{
	if(strncmp(sample, "!HEV_", 5) == 0)
	{
		// Here we save last played HEV sound so we can stop it on respawn
		for(int i = 0; i < numClients; i++)
		{
			szLastSuitSound[clients[i]] = sample;
		}
	}
	if(strncmp(sample, "physics\\footsteps\\", 18) == 0 || strncmp(sample, "player/suit_sprint.wav", 22) == 0)
	{
		// Footsteps are emitted on the server but not sent to client, because the client usually plays them on its own - but not when sprinting in MP mode (actually just pressing +speed!)
		// We can resolve this by sending the footsteps, which are already generated on the server, to the client.
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(entity);
		if(pPlayer.IsValid() && pPlayer.GetButtons() & IN_SPEED)
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
	if (g_pCoopManager.IsCoopModeEnabled())
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
// Player spawn
//------------------------------------------------------
public void Hook_PlayerSpawnPost(int iClient)
{
	if(!g_pCoopManager.IsCoopModeEnabled())
		return;
	
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iClient);
	// Reset sprinting
	pPlayer.SetSuit(false);
	pPlayer.SetMaxSpeed(190.0);
	pPlayer.SetIsSprinting(false);
	// Stop HEV sounds
	StopSound(iClient, SNDCHAN_STATIC, szLastSuitSound[iClient]);
}

//------------------------------------------------------
// Disable FF
//------------------------------------------------------
public Action Hook_PlayerTraceAttack(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &ammotype, int hitbox, int hitgroup)
{
	if(g_pCoopManager.IsCoopModeEnabled())
	{
		if(attacker != victim && CBlackMesaPlayer(attacker).IsClassPlayer())
		{
			return Plugin_Handled;
		}
	}
	return Plugin_Continue;
}

//------------------------------------------------------
// Force everyone to one team in TDM
//------------------------------------------------------
public MRESReturn Hook_PlayerChangeTeam(int _this, Handle hParams)
{
	if(g_pCoopManager.IsCoopModeEnabled())
	{
		int iTeamNum = DHookGetParam(hParams, 1);
		if(iTeamNum == TEAM_SCIENTIST || iTeamNum == TEAM_MARINES)
		{
			char szTeam[16]; g_pConvarCoopTeam.GetString(szTeam, sizeof(szTeam));
			if(StrEqual(szTeam, "scientist", false))
			{
				if(iTeamNum == TEAM_MARINES)
				{
					DHookSetParam(hParams, 1, TEAM_SCIENTIST);
					return MRES_ChangedHandled;
				}
			}
			else if(StrEqual(szTeam, "marines", false))
			{
				if(iTeamNum == TEAM_SCIENTIST)
				{
					DHookSetParam(hParams, 1, TEAM_MARINES);
					return MRES_ChangedHandled;
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// NoBlock
//------------------------------------------------------
public MRESReturn Hook_PlayerShouldCollide(int _this, Handle hReturn, Handle hParams)
{
	if(g_pCoopManager.IsCoopModeEnabled())
	{
		int iCollisionGroup = DHookGetParam(hParams, 1);
		if(iCollisionGroup == COLLISION_GROUP_PLAYER || iCollisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT)
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}