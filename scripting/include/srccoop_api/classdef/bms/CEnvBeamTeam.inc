#pragma newdecls required
#pragma semicolon 1

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
		return CBlackMesaPlayer(GetEntProp(this.entindex, Prop_Data, "m_iOwnerEntIdx"));
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
