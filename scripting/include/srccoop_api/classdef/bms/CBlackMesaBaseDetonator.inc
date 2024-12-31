#pragma newdecls required
#pragma semicolon 1

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
		return GetEntProp(this.entindex, Prop_Data, "m_bPlacedByLevelDesigner") != 0;
	}
	public void SetPlacedByLevelDesigner(const bool bIsPlacedByDesigner)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bPlacedByLevelDesigner", bIsPlacedByDesigner);
	}
	public CEnvBeamTeam GetBeam()
	{
		return CEnvBeamTeam(GetEntPropEnt(this.entindex, Prop_Data, "m_hBeam"));
	}
	public CSpriteTeam GetSprite()
	{
		return CSpriteTeam(GetEntPropEnt(this.entindex, Prop_Data, "m_hLight"));
	}
	public CBlackMesaPlayer GetOwner()
	{
		return CBlackMesaPlayer(GetEntPropEnt(this.entindex, Prop_Data, "m_hThrower"));
	}
}

