#pragma newdecls required
#pragma semicolon 1

methodmap CFuncLadder < CBaseEntity
{
	public CFuncLadder(const int iEntIndex = -1)
	{
		return view_as<CFuncLadder>(CBaseEntity(iEntIndex));
	}
	public static CFuncLadder Create()
	{
		return CFuncLadder(CreateEntityByName("func_ladder"));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public bool IsEnabled()
	{
		return !GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled");
	}
	public void GetTopPosition(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPlayerMountPositionTop", vecBuffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent.IsValid())
		{
			// Technically this should be using m_rgflCoordinateFrame which is a const matrix3x4_t, but if we just add AbsOrigin, it *should* be accurate enough...
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		}
	}
	public void GetBottomPosition(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPlayerMountPositionBottom", vecBuffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent.IsValid())
		{
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		}
	}
}
