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

	public bool IsEnabled()
	{
		return !GetEntProp(this.entindex, Prop_Data, "m_bDisabled");
	}
	public void GetTopPosition(float vec3Buffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecPlayerMountPositionTop", vec3Buffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vec3Buffer, vecLocalOrigin, vec3Buffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent != NULL_CBASEENTITY)
		{
			// Technically this should be using m_rgflCoordinateFrame which is a const matrix3x4_t, but if we just add AbsOrigin, it *should* be accurate enough...
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vec3Buffer, vecLocalOrigin, vec3Buffer);
		}
	}
	public void GetBottomPosition(float vecBuffer[3])
	{
		GetEntPropVector(this.entindex, Prop_Data, "m_vecPlayerMountPositionBottom", vecBuffer);
		float vecLocalOrigin[3];
		this.GetOrigin(vecLocalOrigin);
		AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		
		CBaseEntity pParent = this.GetParent();
		if (pParent != NULL_CBASEENTITY)
		{
			pParent.GetAbsOrigin(vecLocalOrigin);
			AddVectors(vecBuffer, vecLocalOrigin, vecBuffer);
		}
	}
}
