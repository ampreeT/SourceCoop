#pragma newdecls required
#pragma semicolon 1

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
		return CBlackMesaPlayer(GetEntProp(this.entindex, Prop_Data, "m_iOwnerEntIdx"));
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
