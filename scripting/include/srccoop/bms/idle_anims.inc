#pragma newdecls required
#pragma semicolon 1

enum IdleAnimType
{
	IdleAnimType_None = 0,
	IdleAnimType_IdleNoWeapon,
	IdleAnimType_SwimIdleNoWeapon,
	IdleAnimType_SwimNoWeapon,
	IdleAnimType_RunNoWeapon,
	IdleAnimType_LadderNoWeapon,
	IdleAnimType_JumpStartNoWeapon,
	IdleAnimType_CIdleNoWeapon,
	IdleAnimType_CWalkNoWeapon,
}

enum struct IdleAnimEntry
{
	CDynamicProp m_pModel;
	IdleAnimType m_eIdleType;
}

public IdleAnimEntry g_pIdle[MAXPLAYERS + 1];

public void IdleAnims_Initialize()
{
	for (int i = 0; i < sizeof(g_pIdle); ++i)
	{
		g_pIdle[i].m_pModel = NULL_CBASEENTITY;
	}
}

public MRESReturn IdleAnims_Hook_PlayerSetModel_Post(const int iPlayerIndex, DHookParam hParams)
{
	if (!DHookIsNullParam(hParams, 1) && g_pIdle[iPlayerIndex].m_pModel.IsValid())
	{
		char szModelName[MAX_MODELNAME];
		DHookGetParamString(hParams, 1, szModelName, sizeof(szModelName));
		if (PrecacheModel(szModelName))
		{
			g_pIdle[iPlayerIndex].m_pModel.SetModel(szModelName);
			if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_IdleNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_SwimIdleNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swimidle_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_SwimNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swim_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_RunNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("run_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_LadderNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("ladder_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_JumpStartNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("jump_start_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_CIdleNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cidle_noweapon");
			}
			else if (g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_CWalkNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cwalk_noweapon");
			}
		}
	}
	return MRES_Ignored;
}

public Action IdleAnims_Hook_SetTransmit(const int iEntity, const int iClient)
{
	CDynamicProp pDynamicProp = CDynamicProp(iEntity);
	CBasePlayer pPlayer = CBasePlayer(iClient);
	CBasePlayer pParentPlayer = view_as<CBasePlayer>(pDynamicProp.GetParent());
	if (pParentPlayer == NULL_CBASEENTITY)
		return Plugin_Handled;
	
	Obs_Mode eObserverMode = pPlayer.GetObserverMode();

	if (eObserverMode != OBS_MODE_NONE)
	{
		if (eObserverMode == OBS_MODE_IN_EYE && pPlayer.GetObserverTarget() == pParentPlayer)
		{
			return Plugin_Handled;
		}
	}
	else if (pPlayer == pParentPlayer)
	{
		// TODO:
		// Add thirdperson support
		return Plugin_Handled;
	}

	return Plugin_Continue;
}

public void IdleAnims_Hook_PlayerPreThinkPost(const int iClient)
{
	CBasePlayer pPlayer = CBasePlayer(iClient);
	int iPlayerIndex = pPlayer.entindex;
	bool bHasModel = g_pIdle[iPlayerIndex].m_pModel.IsValid();
	if (pPlayer.IsAlive() && pPlayer.GetActiveWeapon() == NULL_CBASEENTITY)
	{
		if (!bHasModel)
		{
			char szModelName[MAX_MODELNAME];
			if (pPlayer.GetModelName(szModelName, sizeof(szModelName)))
			{
				g_pIdle[iPlayerIndex].m_pModel = CDynamicProp.CreateAndSpawn(szModelName);
				if (g_pIdle[iPlayerIndex].m_pModel != NULL_CBASEENTITY)
				{
					float vec3Pos[3];
					pPlayer.GetAbsOrigin(vec3Pos);

					float vec3Angles[3];
					pPlayer.GetAngles(vec3Angles);
					vec3Angles[0] = 0.0;
					vec3Angles[2] = 0.0;

					g_pIdle[iPlayerIndex].m_pModel.SetOrigin(vec3Pos);
					g_pIdle[iPlayerIndex].m_pModel.SetAngles(vec3Angles);
					g_pIdle[iPlayerIndex].m_pModel.SetParent(pPlayer);
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_IdleNoWeapon;

					SDKHook(g_pIdle[iPlayerIndex].m_pModel.entindex, SDKHook_SetTransmit, IdleAnims_Hook_SetTransmit);
					hkUpdateOnRemove.HookEntity(Hook_Pre, g_pIdle[iPlayerIndex].m_pModel.entindex, IdleAnims_Hook_IdleKilled);
					pPlayer.SetFadeMaxDist(0.000001);
					
					bHasModel = true;
				}
			}
		}
	}
	else
	{
		if (bHasModel)
		{
			g_pIdle[iPlayerIndex].m_pModel.Kill();
			bHasModel = false;
		}
	}

	if (!bHasModel)
		return;

	bool bHasGroundEntity = pPlayer.GetGroundEntity() != NULL_CBASEENTITY;
	float vec3Velocity[3];
	pPlayer.GetAbsVelocity(vec3Velocity);

	if (pPlayer.GetLadder().IsValid())
	{
		if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_LadderNoWeapon)
		{
			g_pIdle[iPlayerIndex].m_pModel.SetAnimation("ladder_noweapon");
			g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_LadderNoWeapon;
		}
	}
	else if (pPlayer.GetWaterLevel() == WL_Eyes)
	{
		if (vec3Velocity[0] == 0.0 && vec3Velocity[1] == 0.0)
		{
			if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_SwimIdleNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swimidle_noweapon");
				g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_SwimIdleNoWeapon;
			}
		}
		else
		{
			if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_SwimNoWeapon)
			{
				// No idea on why the idle animation has to be played first, but
				// the animation would not play properly without it.
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swimidle_noweapon");
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swim_noweapon");
				g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_SwimNoWeapon;
			}
		}
	}
	else if ((!bHasGroundEntity && g_pIdle[iPlayerIndex].m_eIdleType == IdleAnimType_JumpStartNoWeapon) ||
				(bHasGroundEntity && !pPlayer.WasPressingButton(IN_JUMP) && pPlayer.IsPressingButton(IN_JUMP)))
	{
		if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_JumpStartNoWeapon)
		{
			g_pIdle[iPlayerIndex].m_pModel.SetAnimation("jump_start_noweapon");
			g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_JumpStartNoWeapon;
		}
		// TODO:
		// Implement the following anims:
		// - jump_float_noweapon
		// - jump_land_noweapon
	}
	else
	{
		if (vec3Velocity[0] == 0.0 && vec3Velocity[1] == 0.0)
		{
			if (pPlayer.IsDucked())
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_CIdleNoWeapon)
				{
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cidle_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_CIdleNoWeapon;
				}
			}
			else
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_IdleNoWeapon)
				{
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_IdleNoWeapon;
				}
			}
		}
		else
		{
			if (pPlayer.IsDucked())
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_CWalkNoWeapon)
				{
					// No idea on why the idle animation has to be played first, but
					// the animation would not play properly without it.
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cidle_noweapon");
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cwalk_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_CWalkNoWeapon;
				}
			}
			else
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != IdleAnimType_RunNoWeapon)
				{
					// No idea on why the idle animation has to be played first, but
					// the animation would not play properly without it.
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("run_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleAnimType_RunNoWeapon;
				}
			}
		}
	}

	g_pIdle[iPlayerIndex].m_pModel.SetRenderMode(pPlayer.GetRenderMode());
	g_pIdle[iPlayerIndex].m_pModel.SetRenderFx(pPlayer.GetRenderFx());
	g_pIdle[iPlayerIndex].m_pModel.SetSkin(pPlayer.GetSkin());
	g_pIdle[iPlayerIndex].m_pModel.SetBody(pPlayer.GetBody());
	g_pIdle[iPlayerIndex].m_pModel.SetRenderColor(pPlayer.GetRenderColor());
	
	// TODO:
	// Set other black mesa specific variables
	// "cheek_depth", "cheek_fat_max", "cheek_fat_min", ect.
	// Should m_AnimOverlay be modifed too?
	
	for (int i = 0; i < 24; ++i)
	{
		g_pIdle[iPlayerIndex].m_pModel.SetPoseParameter(i, pPlayer.GetPoseParameter(i));
	}
}

static MRESReturn IdleAnims_Hook_IdleKilled(int iIdle)
{
	CBaseEntity pIdle = CBaseEntity(iIdle);
	CBasePlayer pPlayer = view_as<CBasePlayer>(pIdle.GetParent());
	if (pPlayer.IsValid())
	{
		pPlayer.SetFadeMaxDist(0.0);
		g_pIdle[pPlayer.entindex].m_pModel = NULL_CBASEENTITY;
	}
	return MRES_Ignored;
}