#if defined _srccoop_entitypatch_bm_included
 #endinput
#endif
#define _srccoop_entitypatch_bm_included

#pragma newdecls required
#pragma semicolon 1

// Patches for Black Mesa

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
// CAI_BaseNPC - Spawn
//------------------------------------------------------
public void Hook_BaseNPCSpawnPost(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	// reduces occurance of random, elusive, linux only crash
	// also assists in fixing the client crash in bm_c2a5g related to lav npc
	pEntity.SetEdictFlags(pEntity.GetEdictFlags() | FL_EDICT_ALWAYS | FL_EDICT_CHANGED);
}

//------------------------------------------------------
// phys_bone_follower
// This hook deletes selected NPC's bone followers to increase multiplayer server perf.
// Using VPhysicsUpdate since Spawn isn't called for this ent.
//------------------------------------------------------
public void Hook_BoneFollowerVPhysicsUpdatePost(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	CBaseEntity pOwner = pEntity.GetOwner();
	if (pOwner.IsValid())
	{
		char szOwnerClass[MAX_CLASSNAME];
		pOwner.GetClassname(szOwnerClass, sizeof(szOwnerClass));
		if (StrEqual(szOwnerClass, "npc_gargantua"))
		{
			RemoveEntity(iEntIndex);
		}
	}
	SDKUnhook(iEntIndex, SDKHook_VPhysicsUpdatePost, Hook_BoneFollowerVPhysicsUpdatePost);
}

//------------------------------------------------------
// CAI_BaseNPC - npc_gargantua
// Use SOLID_BBOX instead of bone followers for collisions
//------------------------------------------------------
public void Hook_GargSpawnPost(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	pEntity.SetSolidType(SOLID_BBOX);
}

//------------------------------------------------------
// CAI_BaseNPC - npc_gargantua
// fix the 'ChasePlayer' input by setting a target ent.
//------------------------------------------------------
public MRESReturn Hook_GargAcceptInputPost(int _this, Handle hReturn, Handle hParams)
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
// CNPC_Houndeye::NPCThink() - npc_houndeye*
// multiplayer crashfix (fixes houndeyes with m_bEnableMemoryUpdateEveryFrame 1 / InputEnableMemUpdatePerFrame input)
//------------------------------------------------------
public MRESReturn Hook_HoundeyeThink(int _this)
{
	SetLocalPlayerCallingEntity(_this);
	return MRES_Ignored;
}
public MRESReturn Hook_HoundeyeThinkPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}

//------------------------------------------------------
// CNPC_Gonarch - npc_gonarch
// MP crash fix
//------------------------------------------------------
public MRESReturn Hook_GonarchRunAI(int _this)
{
	CBaseEntity pThis = CBaseEntity(_this);
	CBasePlayer pPlayer = GetNearestPlayer(pThis, true);
	if(!pPlayer.IsClassPlayer()) {
		return MRES_Supercede;
	}
	SetLocalPlayerCallingEntity(view_as<int>(pPlayer));
	return MRES_Ignored;
}
public MRESReturn Hook_GonarchRunAIPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}

//------------------------------------------------------
// CNPC_Nihilanth - npc_nihilanth
// MP crash fix
//------------------------------------------------------
public MRESReturn Hook_NihilanthRunAI(int _this)
{
	CBaseEntity pThis = CBaseEntity(_this);
	CBasePlayer pPlayer = GetNearestPlayer(pThis, true);
	if(!pPlayer.IsClassPlayer()) {
		return MRES_Supercede;
	}
	SetLocalPlayerCallingEntity(view_as<int>(pPlayer));
	return MRES_Ignored;
}
public MRESReturn Hook_NihilanthRunAIPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}

public MRESReturn Hook_NihilanthHandleAnimEvent(int _this, Handle hParams)
{
	return Hook_NihilanthRunAI(_this);
}
public MRESReturn Hook_NihilanthHandleAnimEventPost(int _this, Handle hParams)
{
	return Hook_NihilanthRunAIPost(_this);
}

//------------------------------------------------------
// CAI_BaseNPC - npc_puffballfungus
// MP TakeDamage crash fix
//------------------------------------------------------
public Action Hook_PuffballFungusDmg(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon, float damageForce[3], float damagePosition[3])
{
	attacker = 0;
	inflictor = 0;
	weapon = -1;
	return Plugin_Changed;
}

//------------------------------------------------------
// CPropRadiationCharger - prop_radiation_charger, CPropHevCharger - prop_hev_charger
// ToDo: reimplement on serverside (avoid clientside effects)
//------------------------------------------------------
/**
public MRESReturn Hook_PropChargerThink(int _this)
{
	SetLocalPlayerCallingEntity(_this);
	return MRES_Ignored;
}
public MRESReturn Hook_PropChargerThinkPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}
**/

//------------------------------------------------------
// TE - BlackMesa Shot
// Fix npc_sniper tracers hard crashing some client configurations
//------------------------------------------------------
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
	if(!GetClientCount())
	{
		return MRES_Supercede;
	}
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
	if(!GetClientCount())
	{
		return MRES_Supercede;
	}
	SetLocalPlayerCallingEntity(_this);
	return MRES_Ignored;
}
public MRESReturn Hook_XenTurretRunAIPost(int _this)
{
	ClearLocalPlayerCallingEntity();
	return MRES_Ignored;
}

public void Hook_XenTurretSpawnPost(int iEntIndex)
{
	// Use our fixed model, which doesn't have the clientside particles which affected everyone's screens even when they shouldn't.
	static char szNewModel[] = "models/props_xen/xen_turret_mpfix.mdl";
	if(PrecacheModel(szNewModel))
	{
		SetEntityModel(iEntIndex, szNewModel);
	}
}

//------------------------------------------------------
// CMiscMarionettist - misc_marionettist
// Prevent a crash due to reactivation before finishing or when used by multiple players at once
//------------------------------------------------------
CBaseEntity g_pActiveMarionettist[MAXPLAYERS+1] = {view_as<CBaseEntity>(-1), ...};

public MRESReturn Hook_MarionettistAcceptInput(int _this, Handle hReturn, Handle hParams)
{
	if (CoopManager.IsCoopModeEnabled())
	{
		if (!DHookIsNullParam(hParams, 1))
		{
			char szInputType[MAX_FORMAT];
			DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
			if (strcmp(szInputType, "Manipulate", false) == 0)
			{
				char szParam[MAX_FORMAT];
				DHookGetParamObjectPtrString(hParams, 4, 0, ObjectValueType_String, szParam, sizeof(szParam));
				if(strcmp(szParam, "!activator", false) == 0)
				{
					if (!DHookIsNullParam(hParams, 2))
					{
						int iActivator = DHookGetParam(hParams, 2);
						CBaseEntity pActivator = CBaseEntity(iActivator);
						if (pActivator.IsClassPlayer())
						{
							MarionettistManipulate(_this, iActivator, view_as<CBasePlayer>(pActivator));
							DHookSetReturn(hReturn, true);
							return MRES_Supercede;
						}
					}
				}
				else if (strcmp(szParam, "!player", false) == 0 || strcmp(szParam, "player", false) == 0)
				{
					for (int i = 1; i <= MaxClients; i++)
					{
						if (IsClientInGame(i) && IsPlayerAlive(i))
						{
							MarionettistManipulate(_this, i, CBasePlayer(i));
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

void MarionettistManipulate(int iMarionettist, int iClient, CBasePlayer pPlayer)
{
	// cancel previous
	if (g_pActiveMarionettist[iClient].IsValid())
	{
		g_pActiveMarionettist[iClient].Kill();
	}
	
	// create a puppet from the master, without hooking it
	g_bTempDontHookEnts = true;
	CBaseEntity pPuppet = CloneMarionettist(iMarionettist);
	g_bTempDontHookEnts = false;
	
	// start it
	pPuppet.AcceptInputStr("Manipulate", "!activator", pPlayer);
	DHookEntity(hkThink, true, pPuppet.GetEntIndex(), _, Hook_MarionettistPuppetThinkPost);
	g_pActiveMarionettist[iClient] = pPuppet;
}

CBaseEntity CloneMarionettist(int iEnt)
{
	int iClone = CreateEntityByName("misc_marionettist");
	char buff[MAX_VALUE];
	GetEntPropString(iEnt, Prop_Data, "m_iszSoundScriptStart", buff, sizeof(buff));
	SetEntPropString(iClone, Prop_Data, "m_iszSoundScriptStart", buff);
	GetEntPropString(iEnt, Prop_Data, "m_iszSoundScriptLoop", buff, sizeof(buff));
	SetEntPropString(iClone, Prop_Data, "m_iszSoundScriptLoop", buff);
	GetEntPropString(iEnt, Prop_Data, "m_iszSoundScriptEnd", buff, sizeof(buff));
	SetEntPropString(iClone, Prop_Data, "m_iszSoundScriptEnd", buff);
	char szDestNamesProp[29];
	for(int i = 0; i < 16; i++)
	{
		FormatEx(szDestNamesProp, sizeof(szDestNamesProp), "m_iszDestinationsNames[ %d ]", i);
		GetEntPropString(iEnt, Prop_Data, szDestNamesProp, buff, sizeof(buff));
		SetEntPropString(iClone, Prop_Data, szDestNamesProp, buff);
	}
	SetEntPropFloat(iClone, Prop_Data, "m_flInnerDestinationRadiusPrevious", GetEntPropFloat(iEnt, Prop_Data, "m_flInnerDestinationRadiusPrevious"));
	SetEntPropFloat(iClone, Prop_Data, "m_flInnerPullSpeedPrevious", GetEntPropFloat(iEnt, Prop_Data, "m_flInnerPullSpeedPrevious"));
	SetEntPropFloat(iClone, Prop_Data, "m_flOuterDestinationRadiusPrevious", GetEntPropFloat(iEnt, Prop_Data, "m_flOuterDestinationRadiusPrevious"));
	SetEntPropFloat(iClone, Prop_Data, "m_flOuterPullSpeedPrevious", GetEntPropFloat(iEnt, Prop_Data, "m_flOuterPullSpeedPrevious"));
	SetEntProp(iClone, Prop_Data, "m_bIgnoreCollisions", GetEntProp(iEnt, Prop_Data, "m_bIgnoreCollisions"));
	ActivateEntity(iClone);
	DispatchSpawn(iClone);
	
	CBaseEntity pClone = CBaseEntity(iClone);
	pClone.SetParent(CBaseEntity(iEnt));
	return pClone;
}

public MRESReturn Hook_MarionettistPuppetThinkPost(int _this)
{
	CBaseEntity pThis = CBaseEntity(_this);
	if (pThis.GetNextThinkTick() == -1)
	{
		// last think, our puppet has finished
		pThis.Kill();
		return MRES_Ignored;
	}
	for (int i = 1; i <= MaxClients; i++)
	{
		if (g_pActiveMarionettist[i] == pThis)
		{
			if (!IsClientInGame(i) || !IsPlayerAlive(i))
			{
				// player left / died while manipulating
				pThis.Kill();
				break;
			}
		}
	}
	return MRES_Ignored;
}

// Catches all sources of removal, potentially resets player noclip.
void OnEntityDestroyed_Marionettist(CBaseEntity pEntity)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (g_pActiveMarionettist[i] == pEntity)
		{
			if (GetEntProp(g_pActiveMarionettist[i].GetEntIndex(), Prop_Data, "m_bIgnoreCollisions"))
			{
				if (IsClientInGame(i))
				{
					CBasePlayer pPlayer = CBasePlayer(i);
					pPlayer.SetMoveType(MOVETYPE_WALK);
				}
			}
			g_pActiveMarionettist[i] = CBaseEntity(-1);
			break;
		}
	}
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
	return Plugin_Handled;
}

//------------------------------------------------------
// env_explosion
// fix !player ignore entity
//------------------------------------------------------
public void Hook_ExplosionSpawn(int iEntIndex)
{
	char buffer[MAX_VALUE];
	GetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", buffer, sizeof(buffer)); // this is entity handle m_hEntityIgnore in other games
	if(StrEqual(buffer, "!player", false))
	{
		SetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", "");
		SetEntProp(iEntIndex, Prop_Data, "m_iClassIgnore", CLASS_PLAYER);
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

//------------------------------------------------------
// item_weapon_snark
// Once the snark nest has exploded, don't do it again. This used to cause chain reaction because we set zero item respawn time.
//------------------------------------------------------
public void Hook_OnItemSnarkDamagePost(int victim, int attacker, int inflictor, float damage, int damagetype)
{
	if (CoopManager.IsCoopModeEnabled())
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

//------------------------------------------------------
// CMusicTrack - music_track
// crash fix
//------------------------------------------------------
public MRESReturn Hook_MusicTrackThink(int _this)
{
	return MRES_Supercede;
}

public MRESReturn Hook_MusicTrackAceptInput(int _this, Handle hReturn, Handle hParams)
{
	char szInputType[MAX_FORMAT];
	char szBuffer[MAX_VALUE];
	DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));

	if (strcmp(szInputType, "Play", false) == 0)
	{
		GetEntPropString(_this, Prop_Data, "m_TrackScriptSound", szBuffer, sizeof(szBuffer));
		
		Event event = CreateEvent("music_track_register", true);
		event.SetString("soundscriptname", szBuffer);
		event.Fire();
		event = CreateEvent("music_track_play", true);
		event.SetString("soundscriptname", szBuffer);
		event.Fire();
		
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	else if (strcmp(szInputType, "Fade", false) == 0)
	{
		GetEntPropString(_this, Prop_Data, "m_TrackScriptSound", szBuffer, sizeof(szBuffer));
		
		Event event = CreateEvent("music_track_fade", true);
		event.SetString("soundscriptname", szBuffer);
		event.Fire();
		
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	else if (strcmp(szInputType, "Next", false) == 0)
	{
		GetEntPropString(_this, Prop_Data, "m_strNextTrackEntity", szBuffer, sizeof(szBuffer));
		int iNext = FindEntityByTargetname(-1, szBuffer, "music_track");
		if (iNext != -1)
		{
			AcceptEntityInput(iNext, "Play");
		}
		AcceptEntityInput(_this, "Fade");
		
		DHookSetReturn(hReturn, true);
		return MRES_Supercede;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// func_trackautochange
// Prevent track from being blocked by players and forced to go back down
//------------------------------------------------------
public MRESReturn Hook_TrackChangeBlocked(int _this, Handle hReturn, Handle hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		CBasePlayer pPlayer = DHookGetParam(hParams, 1);
		if (pPlayer.IsValid())
		{
			pPlayer.Suicide();
			
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	
	return MRES_Ignored;
}

//------------------------------------------------------
// func_tracktrain
// Prevent trains from being blocked by players
// Moves players up if not blocked, otherwise kills blocking player
//------------------------------------------------------
public MRESReturn Hook_TrackTrainBlocked(int _this, Handle hReturn, Handle hParams)
{
	static float vecOriginAdjusted[3];
	
	if (!DHookIsNullParam(hParams, 1))
	{
		CBasePlayer pPlayer = DHookGetParam(hParams, 1);
		if (pPlayer.IsValid())
		{
			pPlayer.GetAbsOrigin(vecOriginAdjusted);
			
			CanTeleportResult CanTP = CanTeleportPlayer(pPlayer, vecOriginAdjusted);
			if (!CanTP)
			{
				pPlayer.Suicide();
				DHookSetReturn(hReturn, false);
				return MRES_Supercede;
			}
			pPlayer.Teleport(vecOriginAdjusted);
			
			if ((CanTP == CAN_TP_MUSTDUCK) && (!pPlayer.IsDucked()))
			{
				pPlayer.StartDucking();
			}
		}
	}
	
	return MRES_Ignored;
}

//------------------------------------------------------
// item_suit
// play admire gloves animation
//------------------------------------------------------
public Action Hook_SuitTouchPickup(const char[] output, int caller, int activator, float delay)
{
	if (CBaseEntity(caller).HasSpawnFlag(SF_BOOTSEQUENCE))
	{
		CBasePlayer pPlayer = CBasePlayer(activator);
		if (pPlayer.IsValid())
		{
			PlayAdmireGloves(pPlayer, true);
		}
	}
	
	return Plugin_Continue;
}

//------------------------------------------------------
// CBoneSetup::AccumulatePose
// fix few random cases where the cycle is NaN
//------------------------------------------------------
public MRESReturn Hook_AccumulatePose(Address _this, DHookParam hParams)
{
	float cycle = hParams.Get(4);
	// NaN Check
	if (cycle != cycle)
	{
		hParams.Set(4, 0.0);
		return MRES_ChangedHandled;
	}
	return MRES_Ignored;
}