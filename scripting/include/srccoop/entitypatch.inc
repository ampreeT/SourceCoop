#if defined _srccoop_entitypatch_included
 #endinput
#endif
#define _srccoop_entitypatch_included

//------------------------------------------------------
// UTIL_GetLocalPlayer
//------------------------------------------------------
public MRESReturn Hook_UTIL_GetLocalPlayer(Handle hReturn)
{
	if (g_pCoopManager.IsBugPatchingEnabled())
	{
		LogDebug("WARN: Game called UTIL_GetLocalPlayer");
		for(int i = 1; i <= MaxClients; i++)
		{
			if(IsClientInGame(i))
			{
				DHookSetReturn(hReturn, i);
				return MRES_Supercede;
			}
		}
	}
	return MRES_Ignored;
}

//------------------------------------------------------
// CNPC_Human_Scientist - npc_human_scientist
// crash fix when getting scared
//------------------------------------------------------
public MRESReturn Hook_ScientistIRelationType(int _this, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, D_LI);
	return MRES_Supercede;
}

//------------------------------------------------------
// CAI_BaseNPC - npc_human_scientist, npc_human_security
// scientists should classify as allies (in MP, base function returns false when player is not passed in)
// this allows npcs to pass filter in 'player ally only' triggers
//------------------------------------------------------
public MRESReturn Hook_IsPlayerAlly(int _this, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

//------------------------------------------------------
// CProtoSniper - npc_sniper
// crash fix
//------------------------------------------------------
public MRESReturn Hook_ProtoSniperSelectSchedule(int _this, Handle hReturn, Handle hParams)	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/hl2/proto_sniper.cpp#L1385
{
	CProtoSniper pSniper = CProtoSniper(_this);

	if (pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		if (PrecacheSound("NPC_Sniper.TargetDestroyed", true))
		{
			EmitGameSoundToAll("NPC_Sniper.TargetDestroyed", pSniper.GetEntIndex());
		}
	}

	if (!pSniper.IsWeaponLoaded())
	{
		DHookSetReturn(hReturn, SCHED_RELOAD);
		return MRES_Supercede;
	}

	// Hear Danger

	if (!pSniper.IsEnabled())
	{
		DHookSetReturn(hReturn, 54);
		return MRES_Supercede;
	}

	CBaseEntity pEnemy = pSniper.GetEnemy();
	if (!pEnemy.IsValid() || pSniper.HasCondition(view_as<int>(COND_ENEMY_DEAD)))
	{
		pSniper.SetEnemy(CBaseEntity());
		DHookSetReturn(hReturn, 89);
		return MRES_Supercede;
	}

	if (pSniper.HasCondition(view_as<int>(COND_LOST_ENEMY)))
	{
		DHookSetReturn(hReturn, 89);
		return MRES_Supercede;
	}

	if (pSniper.HasCondition(view_as<int>(COND_CAN_RANGE_ATTACK1)))
	{
		DHookSetReturn(hReturn, 43);
		return MRES_Supercede;
	}

	DHookSetReturn(hReturn, 89);
	return MRES_Supercede;
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
// CChangelevel
//------------------------------------------------------
bool ChangeLevelTriggered(CChangelevel pChangelevel, CBlackMesaPlayer pActivator)
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
					if(pViewEntity.IsValidIndex() && pViewEntity.IsClassname("point_viewcontrol") && pViewEntity != pThis)
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
