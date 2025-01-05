#pragma newdecls required
#pragma semicolon 1

methodmap CBeam < CBaseEntity
{
	public CBeam(const int iEntIndex = -1)
	{
		return view_as<CBeam>(CBaseEntity(iEntIndex));
	}
	public static CBeam Create()
	{
		return CBeam(CreateEntityByName("beam"));
	}

	public int GetAttachmentCount()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nNumBeamEnts");
	}
	public void SetAttachmentCount(const int iAttachmentCount)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nNumBeamEnts", iAttachmentCount);
	}
	public BeamType_t GetBeamType()
	{
		return view_as<BeamType_t>(GetEntProp(this.entindex, Prop_Send, "m_nBeamType"));
	}
	public int SetBeamType(const BeamType_t eBeamtype)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nBeamType", eBeamtype);
	}
	public CBaseEntity GetAttachment(const int iIndex)
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Send, "m_hAttachEntity", iIndex));
	}
	public void SetAttachment(const int iIndex, CBaseEntity pEntity)
	{
		SetEntPropEnt(this.entindex, Prop_Send, "m_hAttachEntity", pEntity.entindex, iIndex);
	}
}
