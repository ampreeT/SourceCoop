#if defined _srccoop_idle_anims_game_included
 #endinput
#endif
#define _srccoop_idle_anims_game_included

#pragma newdecls required
#pragma semicolon 1

enum IdleAnimType
{
	None = 0,
	IdleNoWeapon,
	SwimIdleNoWeapon,
	SwimNoWeapon,
	RunNoWeapon,
	LadderNoWeapon,
	JumpStartNoWeapon,
	CIdleNoWeapon,
	CWalkNoWeapon,
}

enum struct IdleAnimEntry
{
	CDynamicProp m_pModel;
	IdleAnimType m_eIdleType;
}

public IdleAnimEntry g_pIdle[MAXPLAYERS + 1];

public Action IdleAnims_SetTransmit(const int iEntity, const int iClient)
{
	CDynamicProp pDynamicProp = CDynamicProp(iEntity);
	CBasePlayer pPlayer = CBasePlayer(iClient);
	CBasePlayer pParentPlayer = view_as<CBasePlayer>(pDynamicProp.GetParent());
	if (pParentPlayer == pPlayer)
	{
		// TODO:
		// Thirdperson support
		return Plugin_Handled;
	}
	if (pParentPlayer.IsValid() && pPlayer.GetObserverTarget() == pParentPlayer && pPlayer.GetObserverMode() == OBS_MODE_IN_EYE)
	{
		return Plugin_Handled;
	}

	return Plugin_Continue;
}

public void IdleAnims_Player_PreThink(const int iClient)
{
	CBasePlayer pPlayer = CBasePlayer(iClient);
	int iPlayerIndex = pPlayer.GetEntIndex();
	if (pPlayer.IsAlive() && !pPlayer.GetActiveWeapon().IsValid())
	{
		if (!g_pIdle[iPlayerIndex].m_pModel.IsValid())
		{
			char szModelName[MAX_MODELNAME];
			if (pPlayer.GetModelName(szModelName, sizeof(szModelName)))
			{
				g_pIdle[iPlayerIndex].m_pModel = CDynamicProp.Create(szModelName);
				if (g_pIdle[iPlayerIndex].m_pModel.IsValid())
				{
					float vec3Pos[3];
					pPlayer.GetAbsOrigin(vec3Pos);

					float vec3Angles[3];
					pPlayer.GetAbsAngles(vec3Angles);

					g_pIdle[iPlayerIndex].m_pModel.SetOrigin(vec3Pos);
					g_pIdle[iPlayerIndex].m_pModel.SetAngles(vec3Angles);
					g_pIdle[iPlayerIndex].m_pModel.SetParent(pPlayer);
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = IdleNoWeapon;

					SDKHook(g_pIdle[iPlayerIndex].m_pModel.GetEntIndex(), SDKHook_SetTransmit, IdleAnims_SetTransmit);
				}
			}
		}
	}
	else
	{
		if (g_pIdle[iPlayerIndex].m_pModel.IsValid())
		{
			g_pIdle[iPlayerIndex].m_pModel.Kill();
			g_pIdle[iPlayerIndex].m_pModel = NULL_CBASEENTITY;
		}
	}

	if (g_pIdle[iPlayerIndex].m_pModel.IsValid())
	{
		float vec3Velocity[3];
		pPlayer.GetAbsVelocity(vec3Velocity);

		if (pPlayer.GetLadder().IsValid())
		{
			if (g_pIdle[iPlayerIndex].m_eIdleType != LadderNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("ladder_noweapon");
				g_pIdle[iPlayerIndex].m_eIdleType = LadderNoWeapon;
			}
		}
		else if (pPlayer.GetWaterLevel() == WL_Eyes)
		{
			if (vec3Velocity[0] == 0.0 && vec3Velocity[1] == 0.0)
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != SwimIdleNoWeapon)
				{
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swimidle_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = SwimIdleNoWeapon;
				}
			}
			else
			{
				if (g_pIdle[iPlayerIndex].m_eIdleType != SwimNoWeapon)
				{
					// No idea on why the idle animation has to be played first, but
					// the animation would not play properly without it.
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swimidle_noweapon");
					g_pIdle[iPlayerIndex].m_pModel.SetAnimation("swim_noweapon");
					g_pIdle[iPlayerIndex].m_eIdleType = SwimNoWeapon;
				}
			}
		}
		else if ((!pPlayer.GetGroundEntity().IsValid() && g_pIdle[iPlayerIndex].m_eIdleType == JumpStartNoWeapon) ||
				 (pPlayer.GetGroundEntity().IsValid() && !pPlayer.WasPressingButton(IN_JUMP) && pPlayer.IsPressingButton(IN_JUMP)))
		{
			if (g_pIdle[iPlayerIndex].m_eIdleType != JumpStartNoWeapon)
			{
				g_pIdle[iPlayerIndex].m_pModel.SetAnimation("jump_start_noweapon");
				g_pIdle[iPlayerIndex].m_eIdleType = JumpStartNoWeapon;
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
					if (g_pIdle[iPlayerIndex].m_eIdleType != CIdleNoWeapon)
					{
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cidle_noweapon");
						g_pIdle[iPlayerIndex].m_eIdleType = CIdleNoWeapon;
					}
				}
				else
				{
					if (g_pIdle[iPlayerIndex].m_eIdleType != IdleNoWeapon)
					{
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
						g_pIdle[iPlayerIndex].m_eIdleType = IdleNoWeapon;
					}
				}
			}
			else
			{
				if (pPlayer.IsDucked())
				{
					if (g_pIdle[iPlayerIndex].m_eIdleType != CWalkNoWeapon)
					{
						// No idea on why the idle animation has to be played first, but
						// the animation would not play properly without it.
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cidle_noweapon");
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("cwalk_noweapon");
						g_pIdle[iPlayerIndex].m_eIdleType = CWalkNoWeapon;
					}
				}
				else
				{
					if (g_pIdle[iPlayerIndex].m_eIdleType != RunNoWeapon)
					{
						// No idea on why the idle animation has to be played first, but
						// the animation would not play properly without it.
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("idle_noweapon");
						g_pIdle[iPlayerIndex].m_pModel.SetAnimation("run_noweapon");
						g_pIdle[iPlayerIndex].m_eIdleType = RunNoWeapon;
					}
				}
			}
		}
		
		// TODO: What is the best way to hide the player?
		if (pPlayer.GetRenderFx() != RENDERFX_FADE_FAST)
			pPlayer.SetRenderFx(RENDERFX_FADE_FAST);
		if (pPlayer.GetRenderMode() != g_pIdle[iPlayerIndex].m_pModel.GetRenderMode())
			g_pIdle[iPlayerIndex].m_pModel.SetRenderMode(pPlayer.GetRenderMode());
		if (pPlayer.GetSkin() != g_pIdle[iPlayerIndex].m_pModel.GetSkin())
			g_pIdle[iPlayerIndex].m_pModel.SetSkin(pPlayer.GetSkin());
		// TODO:
		// Set bodygroup and other black mesa specific variables
		// "cheek_depth", "cheek_fat_max", "cheek_fat_min", ect.
		
		int r, g, b, a;
		pPlayer.GetRenderColor(r, g, b, a);
		g_pIdle[iPlayerIndex].m_pModel.SetRenderColor(r, g, b, a);

		for (int i = 0; i < 24; ++i)
		{
			g_pIdle[iPlayerIndex].m_pModel.SetPoseParameter(i, pPlayer.GetPoseParameter(i));
		}
	}
	else
	{
		// TODO: What is the best way to hide the player?
		if (pPlayer.GetRenderFx() != RENDERFX_NONE)
			pPlayer.SetRenderFx(RENDERFX_NONE);
	}
}