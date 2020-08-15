#if defined _srccoop_entitypatch_included
 #endinput
#endif
#define _srccoop_entitypatch_included

//------------------------------------------------------
// UTIL_GetLocalPlayer
//------------------------------------------------------
public MRESReturn Hook_UTIL_GetLocalPlayer(Handle hReturn)
{
	if (g_pLocalPlayerCallingEntity.IsValid())
	{
		CBlackMesaPlayer pPlayer = GetNearestPlayer(g_pLocalPlayerCallingEntity, false);
		if(pPlayer.IsValid())
		{
			LogDebug("Overriding UTIL_GetLocalPlayer");
			DHookSetReturn(hReturn, pPlayer.GetEntIndex());
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

void SetLocalPlayerCallingEntity(int iEntIndex)
{
	if(g_pLocalPlayerCallingEntity.IsValid())
	{
		LogDebug("WARN: SetLocalPlayerCallingEntity, but one has already been set");
	}
	g_pLocalPlayerCallingEntity = CBaseEntity(iEntIndex);
}

void ClearLocalPlayerCallingEntity()
{
	g_pLocalPlayerCallingEntity = CBaseEntity();
}

//------------------------------------------------------
// CNPC_Human_Scientist - npc_human_scientist*
// crash fix when getting scared
//------------------------------------------------------
public MRESReturn Hook_ScientistIRelationType(int _this, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, D_LI);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC - npc_human_scientist*, npc_human_security
// scientists should classify as allies (in MP, base function returns false when player is not passed in)
// this allows npcs to pass filter in 'player ally only' triggers
//------------------------------------------------------
public MRESReturn Hook_IsPlayerAlly(int _this, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC
// Fix UpdateEnemyMemory input when player string is passed as parameter
//------------------------------------------------------
public MRESReturn Hook_BaseNPCAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "UpdateEnemyMemory", false) == 0)
			{
				char szParam[32];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				if(StrEqual(szParam, "!player", false) || StrEqual(szParam, "!pvsplayer", false))
				{
					CAI_BaseNPC pThis = CAI_BaseNPC(_this);
					CBlackMesaPlayer pPlayer;
					float vecAbsOrigin[3];
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i))
						{
							pPlayer = CBlackMesaPlayer(i);
							pPlayer.GetAbsOrigin(vecAbsOrigin);
							pThis.UpdateEnemyMemory(pPlayer, vecAbsOrigin, pThis);
						}
					}
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CAI_BaseNPC
// Recreation of SetPlayerAvoidState for MP compatibility
// From https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc.cpp#L3822
//------------------------------------------------------
public MRESReturn Hook_SetPlayerAvoidState(int _this)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		LogDebug("Hook_SetPlayerAvoidState");
		CAI_BaseNPC pThis = CAI_BaseNPC(_this);
		
		bool bShouldPlayerAvoid;
		float vNothing[3];
		
		pThis.GetSequenceLinearMotion(pThis.GetSequence(), vNothing);
		bool bIsMoving = (pThis.IsMoving() || !VecEqual(vNothing, vec3_origin));
		
		//If we are coming out of a script, check if we are stuck inside the player.
		if (pThis.GetPerformAvoidance() || (pThis.ShouldPlayerAvoid() && bIsMoving))
		{
			float vMins[3], vMaxs[3], vOrigin[3], vPlrMins[3], vPlrMaxs[3], vPlrOrigin[3];
			pThis.GetMins(vMins);
			pThis.GetMaxs(vMaxs);
			pThis.GetAbsOrigin(vOrigin);
			AddVectors(vMins, vOrigin, vMins);
			AddVectors(vMaxs, vOrigin, vMaxs);
			
			for(int i = 1; i <= MaxClients; i++)
			{
				if(IsClientInGame(i) && IsPlayerAlive(i))
				{
					CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
					pPlayer.GetMins(vPlrMins);
					pPlayer.GetMaxs(vPlrMaxs);
					pPlayer.GetAbsOrigin(vPlrOrigin);
					AddVectors(vPlrMins, vPlrOrigin, vPlrMins);
					AddVectors(vPlrMaxs, vPlrOrigin, vPlrMaxs);
					
					if(IsBoxIntersectingBox(vMins, vMaxs, vPlrMins, vPlrMaxs))
					{
						bShouldPlayerAvoid = true;
						break;
					}
				}
			}
		}
		
		pThis.SetPlayerAvoidState(pThis.ShouldPlayerAvoid());
		pThis.SetPerformAvoidance(bShouldPlayerAvoid);
		
		int iCollGroup = pThis.GetCollisionGroup();
		if(iCollGroup == COLLISION_GROUP_NPC || iCollGroup == COLLISION_GROUP_NPC_ACTOR)
		{
			if(bShouldPlayerAvoid)
			{
				pThis.SetCollisionGroup(COLLISION_GROUP_NPC_ACTOR);
			}
			else
			{
				pThis.SetCollisionGroup(COLLISION_GROUP_NPC);
			}
		}
	}
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC - npc_gargantua
// fix the 'ChasePlayer' input by setting a target ent.
//------------------------------------------------------
public MRESReturn Hook_GargAcceptInputPost(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "chaseplayer", false) == 0)
			{
				SDKHook(_this, SDKHook_ThinkPost, GargantuaChaseThink);
				GargantuaChaseThink(_this);
			}
			else if (strcmp(szInputType, "chaseend", false) == 0)
			{
				SDKUnhook(_this, SDKHook_ThinkPost, GargantuaChaseThink);
			}
		}
	}
	return MRES_Ignored;
}

public void GargantuaChaseThink(int _this)
{
	CAI_BaseNPC pThis = CAI_BaseNPC(_this);
	pThis.SetTargetEnt(GetNearestPlayer(pThis));
}

//------------------------------------------------------
// CNPC_Ichthyosaur - npc_ichthyosaur
// multiplayer crashfix
//------------------------------------------------------
public MRESReturn Hook_IchthyosaurIdleSound(int _this)
{
	// ToDo: do the things as in original code, albeit I can hardly spot a difference
	return MRES_Supercede;
}

//------------------------------------------------------
// CProtoSniper - npc_sniper
// crashed is called by a call to AI_GetSinglePlayer that doesn't check for nullptr
// this function is remade from scratch
//------------------------------------------------------
public MRESReturn Hook_ProtoSniperSelectSchedule(int _this, Handle hReturn)	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/hl2/proto_sniper.cpp#L1385
{
	CProtoSniper pSniper = CProtoSniper(_this);
	
	if (pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		// unsure why this is crashing; unneeded
		/*
		if (PrecacheSound("NPC_Sniper.TargetDestroyed", true))
		{
			EmitGameSoundToAll("NPC_Sniper.TargetDestroyed", pSniper.GetEntIndex());
		}
		*/
	}

	if (!pSniper.IsWeaponLoaded())
	{
		DHookSetReturn(hReturn, SCHED_RELOAD);
		return MRES_Supercede;
	}
	
	// skipped SCHED_PSNIPER_PLAYER_DEAD
	// this schedule is pretty useless as it only turns on the laser to show the dead player where he got sniped from
	
	if (pSniper.HasCondition(view_as<int>(COND_HEAR_DANGER)))
	{
		// Hear Danger; add glint, ect. here
		DHookSetReturn(hReturn, SCHED_PSNIPER_SUPPRESSED);
		return MRES_Supercede;
	}
	
	if (!pSniper.IsEnabled())
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_DISABLEDWAIT);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_SWEEP_TARGET)))
	{
		CBaseEntity pSweepTarget = pSniper.GetSweepTarget();
		if ((pSweepTarget.IsValid() && pSweepTarget.HasSpawnFlag(SF_SNIPERTARGET_NOINTERRUPT)) || pSniper.IsSweepHighestPriority())
		{
			DHookSetReturn(hReturn, SCHED_PSNIPER_SWEEP_TARGET_NOINTERRUPT);
		}
		else
		{
			DHookSetReturn(hReturn, SCHED_PSNIPER_SWEEP_TARGET);
		}
		return MRES_Supercede;
	}
	
	CBaseEntity pEnemy = pSniper.GetEnemy();
	if (!pEnemy.IsValid() || pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		pSniper.SetEnemy(CBaseEntity());
		DHookSetReturn(hReturn, SCHED_PSNIPER_SCAN);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_FRUSTRATED)))
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_FRUSTRATED_ATTACK);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_CANATTACKDECOY)))
	{
		DHookSetReturn(hReturn, SCHED_RANGE_ATTACK2);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_SNIPER_NO_SHOT)))
	{
		DHookSetReturn(hReturn, SCHED_PSNIPER_NO_CLEAR_SHOT);
		return MRES_Supercede;
	}
	
	if (pSniper.HasCondition(view_as<int>(COND_CAN_RANGE_ATTACK1)))
	{
		DHookSetReturn(hReturn, SCHED_RANGE_ATTACK1);
		return MRES_Supercede;
	}

	DHookSetReturn(hReturn, SCHED_PSNIPER_CAMP);
	return MRES_Supercede;
}

// For some reason, sniper tracers hard crash my client, thankfully it's easy to turn them off.
public Action BlackMesaFireBulletsTEHook(const char[] szTEName, const int[] iClients, int iNumClients, float flDelay)
{
	int iAmmoId = TE_ReadNum("m_iAmmoID");
	if(iAmmoId == AMMO_SNIPERROUND)
	{
		TE_WriteNum("m_bDoTracers", 0);
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

//------------------------------------------------------
// CNPC_XenTurret - npc_xenturret
// Crash fix - selecting schedules in empty server causes all sorts of issues due to calls to UTIL_GetLocalPlayer()
//------------------------------------------------------
public MRESReturn Hook_XenTurretSelectSchedule(int _this, Handle hReturn)
{
	if(!GetClientCount())
	{
		DHookSetReturn(hReturn, 88);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

public MRESReturn Hook_XenTurretHandleAnimEvent(int _this, Handle hParams)
{
	SetLocalPlayerCallingEntity(_this);
	return MRES_Ignored;
}
public MRESReturn Hook_XenTurretHandleAnimEventPost(int _this, Handle hParams)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}

public MRESReturn Hook_XenTurretRunAI(int _this)
{
	SetLocalPlayerCallingEntity(_this);
	return MRES_Ignored;
}
public MRESReturn Hook_XenTurretRunAIPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}


//------------------------------------------------------
// Scene entities
// fix findnamedentity returning sp player ( nullptr )
//------------------------------------------------------
public MRESReturn Hook_FindNamedEntity(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1) && !DHookIsNullParam(hParams, 2))
	{
		char szName[MAX_CLASSNAME];
		DHookGetParamString(hParams, 1, szName, sizeof(szName));
		if ((strcmp(szName, "Player", false) == 0) || (strcmp(szName, "!player", false) == 0))
		{
			CBaseEntity pActor = CBaseEntity(DHookGetParam(hParams, 2));
			if (pActor.IsValid())
			{
				CBlackMesaPlayer pBestPlayer = GetNearestPlayer(pActor);
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

//------------------------------------------------------
// CChangelevel - trigger_changelevel
// Hook to handle mapchanges
//------------------------------------------------------
void ChangeLevelTriggered(CChangelevel pChangelevel, CBlackMesaPlayer pActivator)
{
	char szMapName[MAX_MAPNAME];
	if (pChangelevel.GetMapName(szMapName, sizeof(szMapName)) && g_pLevelLump.m_pNextMapList.IsInMapList(szMapName))
	{
		if(pActivator.IsClassPlayer())
		{
			g_pCoopManager.OnTouchChangeLevel(pChangelevel, pActivator);
		}
		else
		{
			SDKUnhook(pChangelevel.GetEntIndex(), SDKHook_Touch, Hook_ChangelevelOnTouch);
			pChangelevel.ChangeLevelNow();
		}
	}
}

public void Hook_ChangelevelSpawn(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	DHookEntity(hkAcceptInput, true, pEntity.GetEntIndex(), _, Hook_ChangelevelAcceptInput);

	if(!pEntity.HasSpawnFlag(SF_CHANGELEVEL_NOTOUCH))
	{
		SDKHook(pEntity.GetEntIndex(), SDKHook_Touch, Hook_ChangelevelOnTouch);
	}
}

public MRESReturn Hook_ChangelevelAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ChangeLevel", false) == 0)
			{
				CChangelevel pChangelevel = CChangelevel(_this);
				ChangeLevelTriggered(pChangelevel, CBlackMesaPlayer(-1));
			}
		}
	}

	return MRES_Ignored;
}

public void Hook_ChangelevelOnTouch(int _this, int iOther)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		CChangelevel pChangelevel = CChangelevel(_this);
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(iOther);

		if (pPlayer.IsValid())
		{
			ChangeLevelTriggered(pChangelevel, pPlayer);
		}
	}
}

//------------------------------------------------------
// CPointTeleport - point_teleport
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_PointTeleportAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Teleport", false) == 0)
			{
				char szTarget[MAX_FORMAT];
				CBaseEntity pTeleport = CBaseEntity(_this);
				pTeleport.GetTarget(szTarget, sizeof(szTarget));
				if(strcmp(szTarget, "!player", false) == 0)
				{
					float origin[3], angles[3];
					GetEntPropVector(_this, Prop_Data, "m_vSaveOrigin", origin);
					GetEntPropVector(_this, Prop_Data, "m_vSaveAngles", angles);
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i) && IsPlayerAlive(i))
						{
							TeleportEntity(i, origin, angles, NULL_VECTOR);
						}
					}
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CTriggerCamera - point_viewcontrol
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_PointViewcontrolAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	LogDebug("Hook_PointViewcontrolAcceptInput ent=%d", _this);
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			CBaseEntity pThis = CBaseEntity(_this);
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_PointViewcontrolAcceptInput szInputType=%s", szInputType);

			if (strcmp(szInputType, "Disable", false) == 0)
			{
				HookSingleEntityOutput(_this, "OnEndFollow", OnEndFollowBlocker);
				bHookSkip = true;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
					{
						CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
						if(pPlayer.GetViewEntity() == pThis)
						{
							SetEntPropEnt(_this, Prop_Data, "m_hPlayer", i);
							pThis.AcceptInputStr("Disable");
							SetClientViewEntity(i, i); // game wont reset player if dead
						}
					}
				}
				SetEntPropEnt(_this, Prop_Data, "m_hPlayer", -1);
				UnhookSingleEntityOutput(_this, "OnEndFollow", OnEndFollowBlocker);
				pThis.FireOutput("OnEndFollow", _this);
				bHookSkip = false;
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
			else if (strcmp(szInputType, "Enable", false) == 0)
			{
				CBlackMesaPlayer pActivator;
				if(!DHookIsNullParam(hParams, 2))
				{
					pActivator = CBlackMesaPlayer(DHookGetParam(hParams, 2));
				}
				if(pActivator.IsClassPlayer())
				{
					// prevent bugging out if already using cam
					CBaseEntity pViewEntity = pActivator.GetViewEntity();
					if(pViewEntity.IsValid() && pViewEntity.IsClassname("point_viewcontrol") && pViewEntity != pThis)
					{
						SetEntPropEnt(pViewEntity.GetEntIndex(), Prop_Data, "m_hPlayer", pActivator.GetEntIndex());
						bHookSkip = true;
						pViewEntity.AcceptInputStr("Disable");
						bHookSkip = false;
						pActivator.SetViewEntity(pActivator); // game wont reset player if dead
					}
				}
				else
				{
					LogDebug("Hook_PointViewcontrolAcceptInput activator not a player > EnableAll > MRES_Supercede");
					AcceptEntityInput(_this, "EnableAll");
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
			else if (strcmp(szInputType, "EnableAll", false) == 0) // new input
			{
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i) && IsPlayerAlive(i))
					{
						AcceptEntityInput(_this, "Enable", i);
					}
				}
				DHookSetReturn(hReturn, true);
				LogDebug("Hook_PointViewcontrolAcceptInput MRES_Supercede");
				return MRES_Supercede;
			}
		}
	}
	LogDebug("Hook_PointViewcontrolAcceptInput MRES_Ignored");
	return MRES_Ignored;
}

public Action OnEndFollowBlocker(const char[] output, int caller, int activator, float delay)
{
	return Plugin_Stop;
}

//------------------------------------------------------
// CMovementSpeedMod - player_speedmod
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_SpeedmodAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_SpeedmodAcceptInput szInputType=%s", szInputType);
			if (strcmp(szInputType, "ModifySpeedAll", false) == 0) // new input
			{
				fieldtype_t fieldType = DHookGetParamObjectPtrVar(hParams, 4, 16, ObjectValueType_Int);
				char szParam[32];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				LogDebug("Hook_SpeedmodAcceptInput ft=%d, %s", fieldType, szParam);
				bHookSkip = true;
				for(int i = 1; i <= MaxClients; i++)
				{
					if(IsClientInGame(i))
					{
						SetVariantString(szParam);
						AcceptEntityInput(_this, "ModifySpeed", i);
					}
				}
				bHookSkip = false;
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// point_clientcommand
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_ClientCommandAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	static bool bHookSkip = false;
	if (!bHookSkip && g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			LogDebug("Hook_ClientCommandAcceptInput input=%s", szInputType);
			if (strcmp(szInputType, "CommandAll", false) == 0) // new input
			{
				char szParam[MAX_FORMAT];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				
				// do special handling for the give command: give it to all and add to equipment for later spawns
				
				int iStart = 5;
				if(strncmp(szParam, "give ", iStart, false) == 0)
				{
					if(strncmp(szParam[iStart], "item_weapon_", 12, false) == 0)
					{
						iStart += 5; // start after item_
					}
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i) && IsPlayerAlive(i))
						{
							CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
							if(!pPlayer.HasWeapon(szParam[iStart]))
							{
								pPlayer.GiveItem(szParam[iStart]);
							}
						}
					}
					g_SpawnSystem.AddSpawnItem(szParam[iStart]);
				}
				
				else
				{
					bHookSkip = true;
					for(int i = 1; i <= MaxClients; i++)
					{
						if(IsClientInGame(i))
						{
							SetVariantString(szParam);
							AcceptEntityInput(_this, "Command", i);
						}
					}
					bHookSkip = false;
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// point_servercommand
// treat changelevel command as natural mapchange
//------------------------------------------------------
public MRESReturn Hook_ServerCommandAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Command", false) == 0)
			{
				char szParam[MAX_FORMAT];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				if(strncmp(szParam, "changelevel ", 12, false) == 0)
				{
					ForceChangeLevel(szParam[12], SM_NORMAL_MAPCHANGE);
					DHookSetReturn(hReturn, true);
					return MRES_Supercede;
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// env_credits
// multiplayer support
//------------------------------------------------------
public MRESReturn Hook_EnvCreditsAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "ShowLogo", false) == 0)
			{
				float flLogoLength = GetEntPropFloat(_this, Prop_Data, "m_flLogoLength");
				if(flLogoLength)
				{
					Handle pMsg = StartMessageAll("LogoTimeMsg", USERMSG_RELIABLE);
					if(pMsg != null) {
						BfWriteFloat(pMsg, flLogoLength);
						EndMessage();
					}
				}
				else
				{
					Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
					if(pMsg != null) {
						BfWriteByte(pMsg, 1);
						EndMessage();
					}
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
			if(strcmp(szInputType, "RollCredits", false) == 0)
			{
				Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
				if(pMsg != null) {
					BfWriteByte(pMsg, 2);
					EndMessage();
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
			if(strcmp(szInputType, "RollOutroCredits", false) == 0)
			{
				Handle pMsg = StartMessageAll("CreditsMsg", USERMSG_RELIABLE);
				if(pMsg != null) {
					BfWriteByte(pMsg, 3);
					EndMessage();
				}
				DHookSetReturn(hReturn, true);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// env_introcredits
// multiplayer support - repeat usermessages to all players
//------------------------------------------------------
public Action Hook_IntroCreditsMsg(UserMsg msg_id, BfRead msg, const int[] players, int playersNum, bool reliable, bool init)
{
	DataPack dp; CreateDataTimer(0.0, IntroCreditsMsgRepeater, dp, TIMER_FLAG_NO_MAPCHANGE);
	int type = msg.ReadByte();
	dp.WriteCell(type);
	if(type)
	{
		dp.WriteFloat(msg.ReadFloat());
		dp.WriteFloat(msg.ReadFloat());
		dp.WriteFloat(msg.ReadFloat());
	}
	else
	{
		char buffer[128];
		msg.ReadString(buffer, sizeof(buffer), true);
		dp.WriteString(buffer);
		msg.ReadString(buffer, sizeof(buffer), true);
		dp.WriteString(buffer);
	}
	dp.Reset();
	return Plugin_Handled;
}

public Action IntroCreditsMsgRepeater(Handle timer, DataPack dp)
{
	Handle pNewMsg = StartMessageAll("IntroCredits", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS);
	if(pNewMsg != null) {
		int type = dp.ReadCell();
		BfWriteByte(pNewMsg, type);
		if(type)
		{
			BfWriteFloat(pNewMsg, dp.ReadFloat());
			BfWriteFloat(pNewMsg, dp.ReadFloat());
			BfWriteFloat(pNewMsg, dp.ReadFloat());
		}
		else
		{
			char buffer[128];
			dp.ReadString(buffer, sizeof(buffer));
			BfWriteString(pNewMsg, buffer);
			dp.ReadString(buffer, sizeof(buffer));
			BfWriteString(pNewMsg, buffer);
		}
		EndMessage();
	}
}

//------------------------------------------------------
// env_explosion
// fix !player ignore entity
//------------------------------------------------------
public void Hook_ExplosionSpawn(int iEntIndex)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		char buffer[MAX_VALUE];
		GetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", buffer, sizeof(buffer)); // this is entity handle m_hEntityIgnore in other games
		if(StrEqual(buffer, "!player", false))
		{
			SetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", "");
			SetEntProp(iEntIndex, Prop_Data, "m_iClassIgnore", CLASS_PLAYER);
		}
	}
}

//------------------------------------------------------
// weapons
// use sp weapon models
//------------------------------------------------------
public MRESReturn Hook_WeaponSetModel(int _this, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		if (pWeapon.IsValid())
		{
			//CBaseCombatCharacter pOwner = view_as<CBaseCombatCharacter>(pWeapon.GetOwner());
			//if (pOwner.IsValid() && !pOwner.IsClassPlayer())
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

//------------------------------------------------------
// ai_script_conditions
// completely broken in mp
//------------------------------------------------------
methodmap CAI_ScriptConditions < CBaseEntity
{
	public CAI_ScriptConditions(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptConditions>(InternalGetBaseEntity(iEntIndex));
	}

	public bool IsDisabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled"));
	}
	public void SetDisabled(bool disabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_fDisabled", disabled);
	}
}

public MRESReturn Hook_AIConditionsThink(int _this)
{
	LogDebug("Hook_AIConditionsThink");
	CAI_ScriptConditions pThis = CAI_ScriptConditions(_this);
	if(!pThis.IsDisabled())
	{
		// Just fire the output and disable. To even begin fixing this, one would need to read the CUtlVector< CAI_ScriptConditionsElement > m_ElementList.
		pThis.FireOutput("OnConditionsSatisfied");
		pThis.SetDisabled(true);
		pThis.SetNextThink(-1.0);
	}
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_GoalEntity - ai_goal_*
// set goal entity to nearest player
//------------------------------------------------------
public MRESReturn Hook_ResolveNamesPost(int _this)
{
	char szGoal[MAX_TARGET_LENGTH];
	GetEntPropString(_this, Prop_Data, "m_iszGoal", szGoal, sizeof(szGoal));

	if(szGoal[0] == '\0' || strcmp(szGoal, "!player", false) == 0)
	{
		CBaseEntity pEnt = CBaseEntity(_this);
		CBlackMesaPlayer pPlayer = GetNearestPlayer(pEnt);
		SetEntPropEnt(_this, Prop_Data, "m_hGoalEntity", pPlayer.GetEntIndex());
	}
	return MRES_Ignored;
}

// this pre hook is required, otherwise the post hook will crash!
public MRESReturn Hook_ResolveNames(int _this)
{
	return MRES_Ignored;
}

//------------------------------------------------------
// CAI_LeadBehavior
// multiplayer support for ai_goal_lead
//------------------------------------------------------
public MRESReturn Hook_CanSelectSchedule(Handle hReturn)
{
	LogDebug("Hook_CanSelectSchedule");
	// ToDo: acutally check conditions as in original code
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

//------------------------------------------------------
// item_weapon_snark
// Once the snark nest has exploded, don't do it again. This used to cause chain reaction because we set zero item respawn time.
//------------------------------------------------------
public void Hook_OnItemSnarkDamagePost(int victim, int attacker, int inflictor, float damage, int damagetype)
{
	if (g_pCoopManager.IsCoopModeEnabled())
	{
		SDKHook(victim, SDKHook_OnTakeDamage, Hook_PreventItemSnarkDamage);
		SDKUnhook(victim, SDKHook_OnTakeDamagePost, Hook_OnItemSnarkDamagePost);
	}
}

public Action Hook_PreventItemSnarkDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon,
		float damageForce[3], float damagePosition[3], int damagecustom)
{
	return Plugin_Stop;
}