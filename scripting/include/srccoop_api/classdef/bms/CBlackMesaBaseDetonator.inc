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

