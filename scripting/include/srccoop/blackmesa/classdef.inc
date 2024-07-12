#if defined _srccoop_classdef_bm_included
 #endinput
#endif
#define _srccoop_classdef_bm_included

#pragma newdecls required
#pragma semicolon 1

stock void InitClassdefBM(GameData pGameConfig)
{
	/*
	char szPickupObject[] = "CBlackMesaPlayer::PickupObject";
	StartPrepSDKCall(SDKCall_Player);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Virtual, szPickupObject))
		SetFailState("Could not obtain gamedata offset %s", szPickupObject);
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
	if (!(g_pPickupObject = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szPickupObject);
	*/
}

methodmap CBlackMesaPlayer < CBasePlayer
{
	public CBlackMesaPlayer(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaPlayer>(CBasePlayer(iEntIndex));
	}
	
	public bool HasJumpModule()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule"));
	}
	public void SetJumpModule(const bool bShouldHaveJumpModule)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_bHasJumpModule", bShouldHaveJumpModule);
	}
	public void PerformForcedWeaponDeploy(const char[] szWeaponClassname)
	{
		this.AcceptInputStr("PerformForcedWeaponDeploy", szWeaponClassname);
	}
}

methodmap CEnvBeamTeam < CBeam
{
	public CEnvBeamTeam(const int iEntIndex = -1)
	{
		return view_as<CEnvBeamTeam>(CBeam(iEntIndex));
	}
	public static CEnvBeamTeam Create()
	{
		return CEnvBeamTeam(CreateEntityByName("beamteam"));
	}

	public CBlackMesaPlayer GetOwner()
	{
		return CBlackMesaPlayer(GetEntProp(this.GetEntIndex(), Prop_Data, "m_iOwnerEntIdx"));
	}
	public CBlackMesaBaseDetonator GetTripmine()
	{
		int iEntIndex = -1;
		while ((iEntIndex = FindEntityByClassname(iEntIndex, "grenade_tripmine")) != -1)
		{
			CBlackMesaBaseDetonator pTripmine = CBlackMesaBaseDetonator(iEntIndex);
			if (this == pTripmine.GetBeam())
			{
				return pTripmine;
			}
		}
		return NULL_CBASEENTITY;
	}
}

methodmap CSpriteTeam < CSprite
{
	public CSpriteTeam(const int iEntIndex = -1)
	{
		return view_as<CSpriteTeam>(CSprite(iEntIndex));
	}
	public static CSpriteTeam Create()
	{
		return CSpriteTeam(CreateEntityByName("spriteteam"));
	}

	public CBlackMesaPlayer GetOwner()
	{
		return CBlackMesaPlayer(GetEntProp(this.GetEntIndex(), Prop_Data, "m_iOwnerEntIdx"));
	}
	public CBlackMesaBaseDetonator GetTripmine()
	{
		int iEntIndex = -1;
		while ((iEntIndex = FindEntityByClassname(iEntIndex, "grenade_tripmine")) != -1)
		{
			CBlackMesaBaseDetonator pTripmine = CBlackMesaBaseDetonator(iEntIndex);
			if (this == pTripmine.GetSprite())
			{
				return pTripmine;
			}
		}
		return NULL_CBASEENTITY;
	}
}


methodmap CBlackMesaBaseDetonator < CBaseAnimating
{
	public CBlackMesaBaseDetonator(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaBaseDetonator>(CBaseAnimating(iEntIndex));
	}
	public static CBlackMesaBaseDetonator Create()
	{
		return CBlackMesaBaseDetonator(CreateEntityByName("grenade_tripmine"));
	}

	public bool IsPlacedByLevelDesigner()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlacedByLevelDesigner"));
	}
	public void SetPlacedByLevelDesigner(const bool bIsPlacedByDesigner)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bPlacedByLevelDesigner", bIsPlacedByDesigner);
	}
	public CEnvBeamTeam GetBeam()
	{
		return CEnvBeamTeam(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hBeam"));
	}
	public CSpriteTeam GetSprite()
	{
		return CSpriteTeam(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hLight"));
	}
	public CBlackMesaPlayer GetOwner()
	{
		return CBlackMesaPlayer(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hThrower"));
	}
}

methodmap CBM_MP_GameRules < CMultiplayRules
{
	public static int GetStateIndex(const char[] szStateProp)
	{
		if (strcmp(szStateProp, STATE_WARMUP) == 0) return STATE_WARMUP_INDEX;
		if (strcmp(szStateProp, STATE_ROUND) == 0) return STATE_ROUND_INDEX;
		if (strcmp(szStateProp, STATE_INTERMISSION) == 0) return STATE_INTERMISSION_INDEX;
		SetFailState("CBM_MP_GameRules.GetStateIndex %s", szStateProp); return -1;
	}
	
	public static void SetStateEndTime(const char[] szStateProp, float flEndTime)
	{
		GameRules_SetPropFloat(szStateProp, flEndTime, STATE_ELEMENT_DONE_TIME);
	}
	
	public static void SetCurrentState(const char[] szStateProp)
	{
		GameRules_SetProp("m_nCurrentStateId", CBM_MP_GameRules.GetStateIndex(szStateProp));
	}
	
	public static void SetStateNextState(const char[] szStateProp, const char[] szNextStateProp)
	{
		GameRules_SetProp(szStateProp, CBM_MP_GameRules.GetStateIndex(szNextStateProp), _, STATE_ELEMENT_NEXT_STATE);
	}
	
	public static void SetStateIsInIntermission(const char[] szStateProp, bool bInIntermission)
	{
		GameRules_SetProp(szStateProp, bInIntermission, _, STATE_ELEMENT_IS_IN_INTERMISSION);
	}
}
