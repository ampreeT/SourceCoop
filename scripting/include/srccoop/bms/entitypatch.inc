#pragma newdecls required
#pragma semicolon 1

// Patches for Black Mesa

static bool g_pPlayerIronsightClassic[MAXPLAYERS + 1];

//------------------------------------------------------
// CNPC_PlayerCompanion - npc_human_scientist*, npc_human_security
// scientists should classify as allies (in MP, base function returns false when player is not passed in)
// this allows npcs to pass filter in "player ally only" triggers
//------------------------------------------------------
public MRESReturn Hook_IsPlayerAlly(int _this, DHookReturn hReturn, DHookParam hParams)
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC - npc_lav
// fixes the client crash seen in bm_c2a5g related to LAV npc
//------------------------------------------------------
public void Hook_LAVSpawnPost(int iEntIndex)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	pEntity.edictFlags |= FL_EDICT_ALWAYS;
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
	if (pOwner != NULL_CBASEENTITY)
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
public MRESReturn Hook_GargAcceptInputPost(int _this, DHookReturn hReturn, DHookParam hParams)
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
// CPropRadiationCharger - prop_radiation_charger
//
// # TODO
//
// - Set "m_bChargingPlayers" on charger to apply screen space effects (currently this will apply to all players)
// - Apply laser particle effect on each individual player within radius
// - Play active sound
// - Call `CBlackMesaPlayer::SetRadiationChargeState` on players to apply gluon weapon effects
//------------------------------------------------------
public MRESReturn Hook_PropRadiationChargerThink(int _this)
{
	CPropChargerBase pCharger = CPropChargerBase(_this);

	//float flArmorCharge = (pCharger.GetChargerRate() / pCharger.GetChargerAmount()) * GetGameFrameTime();
	
	float flChargerRange = pCharger.GetChargerRange();
	float vec3EntityPosition[3];
	pCharger.GetAbsOrigin(vec3EntityPosition);

	for (int i = 1; i <= MaxClients; ++i)
	{
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
		if (pPlayer != NULL_CBASEENTITY && pPlayer.IsAlive())
		{
			float vec3PlayerPosition[3];
			pPlayer.GetAbsOrigin(vec3PlayerPosition);

			if (GetVectorDistance(vec3EntityPosition, vec3PlayerPosition) <= flChargerRange)
			{
				// Naive reimplementation of `CPropRadiationCharger::ShouldApplyEffect` and `CPropRadiationCharger::ApplyEffect`.
				// The original implementation uses frametime while thinking every 0.1 seconds.
				// As a result, the value will always increment by 1 unless
				// `m_flChargerRate` is set to a extremely high value or `m_flChargerAmt` is set to a extremely low value.
				// The original implementation also will overflow the max amount if the increment value is over `1`.
				int iAmmoEnergy = pPlayer.GetAmmoFromIndex(AMMO_ENERGY);
				if (iAmmoEnergy < 100)	// TODO: Programatically get the max reserve ammo.
				{
					pPlayer.SetAmmoFromIndex(AMMO_ENERGY, iAmmoEnergy + 1);
				}
			}
		}
	}
	
	pCharger.SetNextThink(GetGameTime() + 0.1);

	return MRES_Supercede;
}

//------------------------------------------------------
// CPropHevCharger - prop_hev_charger
//
// # TODO
//
// - Set "m_bChargingPlayers" on charger to apply screen space effects (currently this will apply to all players)
// - Apply laser particle effect on each individual player within radius
// - Play active sound
//------------------------------------------------------
public MRESReturn Hook_PropHevChargerThink(int _this)
{
	CPropChargerBase pCharger = CPropChargerBase(_this);

	//float flArmorCharge = (pCharger.GetChargerRate() / pCharger.GetChargerAmount()) * GetGameFrameTime();
	
	float flChargerRange = pCharger.GetChargerRange();
	float vec3EntityPosition[3];
	pCharger.GetAbsOrigin(vec3EntityPosition);

	for (int i = 1; i <= MaxClients; ++i)
	{
		CBlackMesaPlayer pPlayer = CBlackMesaPlayer(i);
		if (pPlayer != NULL_CBASEENTITY && pPlayer.IsAlive())
		{
			float vec3PlayerPosition[3];
			pPlayer.GetAbsOrigin(vec3PlayerPosition);

			if (GetVectorDistance(vec3EntityPosition, vec3PlayerPosition) <= flChargerRange)
			{
				// Naive reimplementation of `CPropHevCharger::ShouldApplyEffect` and `CPropHevCharger::ApplyEffect`.
				// The original implementation uses frametime while thinking every 0.1 seconds.
				// As a result, the value will always increment by 1 unless
				// `m_flChargerRate` is set to a extremely high value or `m_flChargerAmt` is set to a extremely low value.
				// The original implementation also will overflow the max amount if the increment value is over `1`.
				int iArmor = pPlayer.GetArmor();
				if (pPlayer.HasSuit() && iArmor < 100)
				{
					pPlayer.SetArmor(iArmor + 1);
				}
			}
		}
	}
	
	pCharger.SetNextThink(GetGameTime() + 0.1);

	return MRES_Supercede;
}

//------------------------------------------------------
// TE - BlackMesa Shot
// Fix npc_sniper tracers hard crashing some client configurations and
// hides the tracers of certain npcs due to incorrect firing position
//------------------------------------------------------
public Action BlackMesaFireBulletsTEHook(const char[] szTEName, const int[] iClients, int iNumClients, float flDelay)
{
	// Prevent crashes from `npc_sniper` on certain client configurations. 
	AmmoType_t eAmmoType = view_as<AmmoType_t>(TE_ReadNum("m_iAmmoID"));
	if (eAmmoType == AMMO_SNIPERROUND)
		return Plugin_Changed;
	
	// TE will crash all clients without a valid entity.
	CBaseEntity pEntity = CBaseEntity(TE_ReadNum("m_iEntityIndex"));
	if (pEntity == NULL_CBASEENTITY)
		return Plugin_Changed;

	char szClassname[MAX_CLASSNAME];
	if (!pEntity.GetClassname(szClassname, sizeof(szClassname)))
		return Plugin_Changed;

	// Hide tracers of broken npcs.
	// TODO:
	// Instead of hiding the tracers, they should be displayed correctly.
	if (strcmp(szClassname, "npc_sentry_ground") == 0 ||
		strcmp(szClassname, "npc_sentry_ceiling") == 0 ||
		strcmp(szClassname, "npc_abrams") == 0 ||
		strcmp(szClassname, "npc_lav") == 0)
		return Plugin_Changed;

	return Plugin_Continue;
}

//------------------------------------------------------
// CNPC_XenTurret - npc_xenturret
//------------------------------------------------------
public void Hook_XenTurretSpawnPost(int iEntIndex)
{
	// Use our fixed model, which doesn't have the clientside particles which affected everyone's screens even when they shouldn't.
	static char szNewModel[] = "models/props_xen/xen_turret_mpfix.mdl";
	if (PrecacheModel(szNewModel))
	{
		SetEntityModel(iEntIndex, szNewModel);
	}
}

//------------------------------------------------------
// CMiscMarionettist - misc_marionettist
// Prevent a crash due to reactivation before finishing or when used by multiple players at once
//------------------------------------------------------
CBaseEntity g_pActiveMarionettist[MAXPLAYERS+1] = {view_as<CBaseEntity>(-1), ...};

public MRESReturn Hook_MarionettistAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
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
				if (strcmp(szParam, "!activator", false) == 0)
				{
					if (!DHookIsNullParam(hParams, 2))
					{
						int iActivator = DHookGetParam(hParams, 2);
						CBaseEntity pActivator = CBaseEntity(iActivator);
						if (pActivator != NULL_CBASEENTITY && pActivator.IsPlayer())
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
						if (IsClientInGame(i) && !IsFakeClient(i) && IsPlayerAlive(i))
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
	hkThink.HookEntity(Hook_Post, pPuppet.entindex, Hook_MarionettistPuppetThinkPost);
	hkUpdateOnRemove.HookEntity(Hook_Post, pPuppet.entindex, Hook_MarionettistPuppetRemoved);
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
	for (int i = 0; i < 16; i++)
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
MRESReturn Hook_MarionettistPuppetRemoved(int iEntIndex)
{
	CBaseEntity pPuppet = CBaseEntity(iEntIndex);
	for (int i = 1; i <= MaxClients; i++)
	{
		if (g_pActiveMarionettist[i] == pPuppet)
		{
			if (GetEntProp(g_pActiveMarionettist[i].entindex, Prop_Data, "m_bIgnoreCollisions"))
			{
				if (IsClientInGame(i))
				{
					CBasePlayer pPlayer = CBasePlayer(i);
					pPlayer.SetMoveType(MOVETYPE_WALK);
				}
			}
			g_pActiveMarionettist[i] = NULL_CBASEENTITY;
			break;
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
	if (type)
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

public void IntroCreditsMsgRepeater(Handle timer, DataPack dp)
{
	Handle pNewMsg = StartMessageAll("IntroCredits", USERMSG_RELIABLE|USERMSG_BLOCKHOOKS);
	if (pNewMsg != null)
	{
		int type = dp.ReadCell();
		BfWriteByte(pNewMsg, type);
		if (type)
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
	char buffer[MAX_VALUE];
	GetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", buffer, sizeof(buffer)); // this is entity handle m_hEntityIgnore in other games
	if (StrEqual(buffer, "!player", false))
	{
		SetEntPropString(iEntIndex, Prop_Data, "m_strEntityNameToIgnore", "");
		SetEntProp(iEntIndex, Prop_Data, "m_iClassIgnore", CLASS_PLAYER);
	}
}

//------------------------------------------------------
// weapons
// use sp weapon models
//------------------------------------------------------
public MRESReturn Hook_WeaponSetModel(int _this, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1))
	{
		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		if (pWeapon != NULL_CBASEENTITY)
		{
			CBaseCombatCharacter pOwner = pWeapon.GetOwner();
			if (pOwner != NULL_CBASEENTITY && !pOwner.IsPlayer())
			{
				static const char szWeaponModel[][][] =
				{
					{ "models/weapons/w_glock_mp.mdl", "models/weapons/w_glock.mdl" },
					{ "models/weapons/w_357_mp.mdl", "models/weapons/w_357.mdl" },
					{ "models/weapons/w_mp5_mp.mdl", "models/weapons/w_mp5.mdl" },
					{ "models/weapons/w_shotgun_mp.mdl", "models/weapons/w_shotgun.mdl" },
					{ "models/weapons/w_rpg_mp.mdl", "models/weapons/w_rpg.mdl" },
				};

				char szModelName[MAX_MODELNAME];
				DHookGetParamString(hParams, 1, szModelName, sizeof(szModelName));

				for (int i = 0; i < sizeof(szWeaponModel); i++)
				{
					if (strcmp(szModelName, szWeaponModel[i][0], false) == 0)
					{
						if (PrecacheModel(szWeaponModel[i][1], false))
						{
							hParams.SetString(1, szWeaponModel[i][1]);
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

public MRESReturn Hook_MusicTrackAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
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
// item_suit
// play admire gloves animation
//------------------------------------------------------
public Action Hook_SuitTouchPickup(const char[] szOutput, int iCaller, int iActivator, float flDelay)
{
	CItemSuit pCaller = CItemSuit(iCaller);
	if (pCaller.m_spawnflags & SF_BOOTSEQUENCE)
	{
		CBaseEntity pActivator = CBaseEntity(iActivator);
		if (pActivator != NULL_CBASEENTITY && pActivator.IsPlayer())
		{
			view_as<CBasePlayer>(pActivator).StartAdmireGlovesAnimation();
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

//------------------------------------------------------
// CAI_MoveProbe::TestGroundMove
// Linux crash fix (Xen grunts?)
//------------------------------------------------------
public MRESReturn Hook_TestGroundMove(Address _this, DHookReturn hReturn, DHookParam hParams)
{
	if (!hParams.IsNull(6))
	{
		float flTotalDist = hParams.GetObjectVar(6, 32, ObjectValueType_Float);
		if (flTotalDist == 1.0 / 0.0) // infinity check
		{
			LogDebug("Infinity in TestGroundMove prevented");
			hReturn.Value = false;
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// `env_xen_portal_effect`
// Fixes xen portals not pushing players.
//------------------------------------------------------
public MRESReturn Hook_XenPortalEffect_AcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	char szInputType[MAX_FORMAT];
	DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
	if (strcmp(szInputType, "PushAway", false) == 0 || strcmp(szInputType, "Burst", false) == 0)
	{
		CEnv_XenPortalEffect pPortalEffect = CEnv_XenPortalEffect(_this);

		float flSize = pPortalEffect.GetSize();
		float vec3PortalEffectPosition[3];
		pPortalEffect.GetAbsOrigin(vec3PortalEffectPosition);

		for (int i = 1; i <= MaxClients; ++i)
		{
			CBasePlayer pPlayer = CBasePlayer(i);
			if (pPlayer != NULL_CBASEENTITY && pPlayer.IsAlive())
			{
				float vec3PlayerPosition[3];
				pPlayer.GetAbsOrigin(vec3PlayerPosition);
				
				if (GetVectorDistance(vec3PortalEffectPosition, vec3PlayerPosition) < (flSize * 2))
				{
					float vec3Distance[3];
					SubtractVectors(vec3PlayerPosition, vec3PortalEffectPosition, vec3Distance);
					NormalizeVector(vec3Distance, vec3Distance);

					float vec3Angles[3];
					GetVectorAngles(vec3Distance, vec3Angles);

					float vec3Forward[3];
					GetAngleVectors(vec3Angles, vec3Forward, NULL_VECTOR, NULL_VECTOR);
					ScaleVector(vec3Forward, 1024.0);	// TODO: Add better implementation matching original game.

					pPlayer.SetAbsVelocity(vec3Forward);
				}
			}
		}
	}
	return MRES_Ignored;
}

// Fixes map-placed tripmines not having the proper color with `mp_teamplay 1`.
//
public void Hook_SpriteTeamSpawnPost(int iEntIndex)
{
	// If teamplay is off, then the game will set the color properly.
	if (!CoopManager.IsCoopModeEnabled() || !CBM_MP_GameRules.IsTeamplay())
		return;

	// Create a timer in the future to give initialization time for `grenade_tripmine`.
	CreateTimer(0.0, Timer_SetTripmineSpriteColor, CSpriteTeam(iEntIndex));
}

// Fixes map-placed tripmines not having the proper color with `mp_teamplay 1`.
//
public void Timer_SetTripmineSpriteColor(Handle timer, CSpriteTeam pSpriteTeam)
{
	if (!pSpriteTeam.IsValid())
		return;

	CBlackMesaBaseDetonator pTripmine = pSpriteTeam.GetTripmine();
	if (pTripmine == NULL_CBASEENTITY)
		return;
	
	CEnvBeamTeam pBeamTeam = pTripmine.GetBeam();
	if (pBeamTeam == NULL_CBASEENTITY)
		return;
	
	// Check if a player created this tripmine.
	// TODO:
	// This should assign a team color if a player disconnected before the tripmine is active.
	CBlackMesaPlayer pOwner = pSpriteTeam.GetOwner();
	if (pTripmine.GetTeam() != TEAM_UNASSIGNED || pOwner != NULL_CBASEENTITY)
		return;
	
	CSprite pSprite = CSprite.Create();
	if (pSprite == NULL_CBASEENTITY)
		return;
	
	CEnvBeam pBeam = CEnvBeam.Create();
	if (pBeam == NULL_CBASEENTITY)
	{
		pSprite.Kill();
		return;
	}

	char szSpriteTexture[MAX_MODELNAME];
	pSpriteTeam.GetModelName(szSpriteTexture, sizeof(szSpriteTexture));
	
	// Take the sprite color and alpha color.
	// At this point, sprite alpha is set to `0`.
	RGBA col4Sprite = pSpriteTeam.GetRenderColor();
	pSprite.SetRenderColor(RGBA(col4Sprite.r, col4Sprite.g, col4Sprite.b, 224));
	pSprite.SetRenderMode(pSpriteTeam.GetRenderMode());
	pSprite.SetRenderFx(pSpriteTeam.GetRenderFx());

	// Sets the sprite texture.
	pSprite.SetModelName(szSpriteTexture);
	pSprite.m_spawnflags |= SF_SPRITE_STARTON;
	pSprite.SetScale(pSpriteTeam.GetScale());

	// Attach to the tripmine and spawn.
	pSprite.SetParent(pTripmine);
	pSprite.SetParentAttachment("Beam");
	pSprite.Spawn();

	float vec3TripminePos[3];
	float vec3SpriteOffset[3];
	float vec3BeamStartPos[3];
	float vec3BeamEndPos[3];
	pTripmine.GetAbsOrigin(vec3TripminePos);
	pSprite.GetAbsOrigin(vec3SpriteOffset);
	pBeamTeam.GetAbsOrigin(vec3BeamStartPos);
	AddVectors(vec3TripminePos, vec3SpriteOffset, vec3BeamEndPos);

	// TODO:
	// Use alpha directly when real tripmine laser texture is used.
	// Right now, the alpha that has to be set has to be higher.
	RGBA col4Beam = pBeamTeam.GetRenderColor();
	pBeam.SetRenderColor(RGBA(col4Beam.r, col4Beam.g, col4Beam.b, 128));
	pBeam.SetRenderMode(pBeamTeam.GetRenderMode());
	pBeam.SetRenderFx(pBeamTeam.GetRenderFx());

	// TODO:
	// Use the real tripmine laser texture.
	pBeam.SetModel("sprites/purplelaser1.vmt");
	pBeam.SetTextureName("sprites/purplelaser1.spr");
	pBeam.m_spawnflags |= SF_BEAM_STARTON;
	pBeam.SetDuration(0.0);
	pBeam.SetWidth(8.0);
	pBeam.SetOrigin(vec3BeamStartPos);

	// Attach the beam so that the game manages the deletion.
	pBeam.SetParent(pTripmine);

	// Attach the laser from the beam to the sprite and spawn.
	pBeam.SetBeamType(BEAM_ENTS);
	pBeam.SetAttachment(0, pBeam);
	pBeam.SetAttachment(1, pSprite);
	pBeam.SetAttachmentCount(2);
	pBeam.Spawn();
	
	// Hide the original tripmine's entities.
	pBeamTeam.SetRenderAlpha(0);
	pSpriteTeam.SetRenderAlpha(0);
}

//------------------------------------------------------
// grenade_frag
// Fixes grenades physics being different in multiplayer and not being able to be picked up.
//------------------------------------------------------
public void Hook_GrenadeFragSpawn(const int iEntIndex)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;
	}
}

//------------------------------------------------------
// grenade_frag
// See `Hook_GrenadeFragSpawn` for details.
//------------------------------------------------------
public void Hook_GrenadeFragSpawnPost(const int iEntIndex)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
}

//------------------------------------------------------
// weapon_357/weapon_crossbow
// New constructed logic of toggling ironsights.
//------------------------------------------------------
public MRESReturn Hook_ToggleIronsights(int _this, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		CBlackMesaBaseCombatWeaponIronsights pWeapon = CBlackMesaBaseCombatWeaponIronsights(_this);
		if (pWeapon.IsReloading())
			return MRES_Supercede;
		
		CBlackMesaPlayer pPlayer = view_as<CBlackMesaPlayer>(pWeapon.GetOwner());
		if (!pPlayer.IsValid())
			return MRES_Supercede;
		
		char szClassname[MAX_CLASSNAME];
		bool bIsCrossbow = pWeapon.GetClassname(szClassname, sizeof(szClassname)) && strcmp(szClassname, "weapon_crossbow") == 0;
		bool bWillBeUsingIronsights = !pWeapon.IsInIronsights();

		int iFOV;
		if (bIsCrossbow)
		{
			CWeapon_Crossbow pCrossbow = view_as<CWeapon_Crossbow>(pWeapon);
			bool bIsRearming = pCrossbow.IsRearming();
			if (bWillBeUsingIronsights && bIsRearming)
				return MRES_Supercede;
			
			// Values grabbed out of `weapon_crossbow.dmx`.
			iFOV = g_pPlayerIronsightClassic[pPlayer.entindex] ? 20 : 10;

			if (!g_pPlayerIronsightClassic[pPlayer.entindex])
			{
				CPredictedViewModel pViewModel = pPlayer.GetViewModel();
				if (pViewModel.IsValid())
				{
					if (bWillBeUsingIronsights || !bIsRearming)
					{
						pWeapon.SendWeaponAnim(bWillBeUsingIronsights ? ACT_VM_IDLE_TO_IS : ACT_VM_IS_TO_IDLE);
					}
				}
			}
		}
		else
		{
			// Values grabbed out of `weapon_357.dmx`.
			iFOV = g_pPlayerIronsightClassic[pPlayer.entindex] ? 40 : 55;

			if (!g_pPlayerIronsightClassic[pPlayer.entindex])
			{
				pWeapon.SendWeaponAnim(bWillBeUsingIronsights ? ACT_VM_IDLE_TO_IS : ACT_VM_IS_TO_IDLE);
				pPlayer.ShowCrosshair(!bWillBeUsingIronsights);
			}
		}
		pPlayer.SetFOV(pWeapon, bWillBeUsingIronsights ? iFOV : 0, 0.5);
		pWeapon.SetIronsights(bWillBeUsingIronsights);
		pWeapon.DelayNextAttack(0.5);

		return MRES_Supercede;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_357
// Fixes the shooting animation played while using ironsights.
//------------------------------------------------------
public MRESReturn Hook_IronsightGetPrimaryAttackActivity(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		CBlackMesaBaseCombatWeaponIronsights pWeapon = CBlackMesaBaseCombatWeaponIronsights(_this);
		if (pWeapon.IsInIronsights())
		{
			CBaseCombatCharacter pOwner = pWeapon.GetOwner();
			if (pOwner != NULL_CBASEENTITY && pOwner.IsPlayer() && !g_pPlayerIronsightClassic[pOwner.entindex])
			{
				DHookSetReturn(hReturn, ACT_VM_PRIMARYATTACK_IS);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// Fixes the shooting animation played while using ironsights.
//------------------------------------------------------
public MRESReturn Hook_CrossbowGetPrimaryAttackActivity(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		CBlackMesaBaseCombatWeaponIronsights pWeapon = CBlackMesaBaseCombatWeaponIronsights(_this);
		if (pWeapon.IsInIronsights())
		{
			DHookSetReturn(hReturn, ACT_VM_PRIMARYATTACK);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// Caches the player ConVar `cl_ironsight_classic`.
//------------------------------------------------------
void ConvarQuery_IronsightClassic(QueryCookie hCookie, int iClient, ConVarQueryResult hResult, const char[] szCvar, const char[] szValue, any value)
{
	if (strcmp(szValue, "false") == 0 || strcmp(szValue, "0") == 0)
	{
		g_pPlayerIronsightClassic[iClient] = false;
	}
	else
	{
		g_pPlayerIronsightClassic[iClient] = true;
	}
}

//------------------------------------------------------
// Caches the client settings used for ironsights.
//------------------------------------------------------
public MRESReturn Hook_IronsightDeployPost_SaveSettings(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		CBaseCombatCharacter pOwner = pWeapon.GetOwner();
		if (pOwner != NULL_CBASEENTITY && pOwner.IsPlayer())
		{
			view_as<CBasePlayer>(pOwner).QueryConvar("cl_ironsight_classic", ConvarQuery_IronsightClassic);
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// grenade_bolt
// Disables explosive bolts by blocking the call.
//------------------------------------------------------
public MRESReturn Hook_GrenadeBoltAcceptInput(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		char szInputType[MAX_FORMAT];
		DHookGetParamString(hParams, 1, szInputType, sizeof(szInputType));
		if (strcmp(szInputType, "SetToExplode", false) == 0)
		{
			DHookSetReturn(hReturn, true);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// Sets the skin of the crossbow.
//------------------------------------------------------
public MRESReturn Hook_CrossbowDeployPost(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		CBaseCombatCharacter pOwner = pWeapon.GetOwner();
		if (pOwner != NULL_CBASEENTITY && pOwner.IsPlayer())
		{
			CBasePlayer pPlayer = view_as<CBasePlayer>(pOwner);
			CPredictedViewModel pViewModel = pPlayer.GetViewModel();
			if (pViewModel != NULL_CBASEENTITY)
			{
				pViewModel.SetSkin(0);
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// Sets the rearming animation (not the state!) after firing.
// Also fixes the scope in animation covering the screen by setting it to idle.
//
// TODO: Hooking ItemPostFrame with IsMultiplayer = false causes prediction issues.
// Fix this by not calling IsMultiplayer = false.
//------------------------------------------------------
public MRESReturn Hook_CrossbowItemPostFrame(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;

		CBaseCombatWeapon pWeapon = CBaseCombatWeapon(_this);
		CBaseCombatCharacter pOwner = pWeapon.GetOwner();
		if (pOwner != NULL_CBASEENTITY && pOwner.IsPlayer())
		{
			CBasePlayer pPlayer = view_as<CBasePlayer>(pOwner);
			CPredictedViewModel pViewModel = pPlayer.GetViewModel();
			if (pViewModel != NULL_CBASEENTITY)
			{
				// Switch to idle animation so the scope does not block the view.
				if (pViewModel.GetSequence() == pViewModel.SelectWeightedSequence(ACT_VM_IDLE_TO_IS) && pViewModel.GetCycle() >= 0.55)
				{
					pWeapon.SendWeaponAnim(ACT_VM_IDLE);
				}
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// See `Hook_CrossbowItemPostFrame` for details.
//------------------------------------------------------
public MRESReturn Hook_CrossbowItemPostFramePost(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// This does the following:
//
// - Causes `CWeapon_Crossbow::FireBolt` to get called instead of `CWeapon_Crossbow::FireHitscanBolt` on secondary attacks
// - Sets the rearm bool
// - Sets animations
//------------------------------------------------------
public MRESReturn Hook_CrossbowPrimaryAttack(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// See `Hook_CrossbowPrimaryAttack` for details.
//------------------------------------------------------
public MRESReturn Hook_CrossbowPrimaryAttackPost(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// Fixes the rearm animation when pulling out the crossbow.
//------------------------------------------------------
public MRESReturn Hook_CrossbowGetDrawActivity(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// See `Hook_CrossbowGetDrawActivity` for details.
//------------------------------------------------------
public MRESReturn Hook_CrossbowGetDrawActivityPost(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// `CWeapon_Crossbow::FireBolt` is only called by `CWeapon_Crossbow::PrimaryAttack` which is hooked.
// If this gets called with singleplayer being set, this will crash the server.
//------------------------------------------------------
public MRESReturn Hook_CrossbowFireBolt(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_crossbow
// See `Hook_CrossbowFireBolt` for details.
//------------------------------------------------------
public MRESReturn Hook_CrossbowFireBoltPost(int _this, DHookReturn hReturn, DHookParam hParams)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_tau
// Fixes the tau velocity.
//------------------------------------------------------
public MRESReturn Hook_TauFireBeam(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = false;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// weapon_tau
// See `Hook_TauFireBeamPost` for details.
//------------------------------------------------------
public MRESReturn Hook_TauFireBeamPost(int _this, DHookReturn hReturn)
{
	if (CoopManager.IsFeatureEnabled(FT_SP_WEAPONS))
	{
		g_bIsMultiplayerOverride = true;
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CBaseAnimating::Dissolve
// Prevents removal of players by dissolve types
// 3, 4 -- 'Fade', 'Remove now'
//------------------------------------------------------
public MRESReturn Hook_Dissolve(int _this, DHookReturn hReturn, DHookParam hParams)
{
	EntityDissolveType_t eDissolveType = hParams.Get(4);
	if (eDissolveType == ENTITY_DISSOLVE_FADE || eDissolveType == ENTITY_DISSOLVE_REMOVE_NOW)
	{
		if (CBaseEntity(_this).IsPlayer())
		{
			hParams.Set(4, ENTITY_DISSOLVE_NORMAL);
			return MRES_ChangedHandled;
		}
	}
	return MRES_Ignored;
}

// Switches between using singleplayer and multiplayer weapon values.
// This will get triggered by the command `params_reload_server`.
public MRESReturn Hook_CParamsManager_InitInstances(Address _this, DHookReturn hReturn, DHookParam hParams)
{
	// TODO:
	// Currently, this is getting only called during level init, but
	// we also need to call this if the feature is turned on/off mid-round.
	CParamsManager pParamsManager = CParamsManager.FromAddress(_this);
	pParamsManager.SetMultiplayer(!CoopManager.IsFeatureEnabled(FT_SP_WEAPONS));
	return MRES_Ignored;
}

// Fixes the incorrect sprite position on the ammo canister.
//
public void Hook_AmmoCanister_Sprite_OnCreated(const CSprite pSprite)
{
	if (!pSprite.IsValid())
		return;

	CBaseEntity pParent = pSprite.GetParent();
	if (pParent == NULL_CBASEENTITY)
		return;
	
	char szClassname[MAX_CLASSNAME];
	pParent.GetClassname(szClassname, sizeof(szClassname));
	if (strcmp(szClassname, "item_ammo_canister") != 0)
		return;
	
	char szModelName[MAX_MODELNAME];
	pParent.GetModelName(szModelName, sizeof(szModelName));

	// Fix sprite position as it is centered on the model.
	pSprite.ClearParent();
	pParent.SetModel(szModelName);
	pSprite.SetParent(pParent);
	pSprite.SetParentAttachment("w_weaponbox_dlight");

	if (CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
	{
		SDKHook(pSprite.entindex, SDKHook_SetTransmit, Hook_Instancing_ItemParent_Transmit);
	}

	/*
	// TODO: Find out another way of drawing the light.
	// This is commented out as the light will still appear with the `SetTransmit` hook returning `Plugin_Stop`.
	CBaseEntity pDynamicLight = CBaseEntity.Create("light_dynamic");
	if (pDynamicLight != NULL_CBASEENTITY)
	{
		// TODO: Create methodmap `CDynamicLight` and replace these calls.
		pDynamicLight.SetRenderColor(RGBA(255, 160, 0, 255));
		pDynamicLight.KeyValue("brightness", "2");
		pDynamicLight.KeyValue("distance", "16");
		pDynamicLight.KeyValue("style", "1");
		pDynamicLight.Spawn();
		pDynamicLight.AcceptInput("TurnOn");
		pDynamicLight.SetParent(pParent);
		pDynamicLight.SetParentAttachment("w_weaponbox_dlight");
		SDKHook(pDynamicLight.entindex, SDKHook_SetTransmit, Hook_Instancing_ItemParent_Transmit);
	}
	*/
}

// Fixes the missing dynamic light on the battery.
//
public void Hook_Battery_OnCreated(const CItem pItem)
{
	if (!pItem.IsValid())
		return;
	
	// TODO: Create a methodmap.
	CBaseEntity pBatteryLight = CBaseEntity.Create("newLight_Point");
	if (pBatteryLight == NULL_CBASEENTITY)
		return;

	pBatteryLight.KeyValue("lightcolor", "20 181 214 255");
	pBatteryLight.KeyValue("style", "1");	// Flicker A
	pBatteryLight.KeyValue("Intensity", "256");
	pBatteryLight.KeyValue("texName", "lights/lightcookie_battery.vtf");
	pBatteryLight.KeyValue("bTexLight", "1");
	pBatteryLight.KeyValue("SpecMultiplier", "0");
	pBatteryLight.KeyValue("lighttype", "2");
	pBatteryLight.KeyValue("range", "30");
	pBatteryLight.Spawn();

	pBatteryLight.SetParent(pItem);
	pBatteryLight.Teleport({0.0, 0.0, 6.0}, vec3_origin);

	pBatteryLight.AcceptInput("TurnOn");

	// Without modifying these flags, the light's parent will be forced to always transmit.
	pBatteryLight.edictFlags &= ~FL_EDICT_ALWAYS;
	pBatteryLight.edictFlags |= FL_EDICT_PVSCHECK;

	if (CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
	{
		SDKHook(pBatteryLight.entindex, SDKHook_SetTransmit, Hook_Instancing_ItemParent_Transmit);
	}
}