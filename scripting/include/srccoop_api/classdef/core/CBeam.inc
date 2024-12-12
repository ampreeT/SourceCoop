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

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetAttachmentCount()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nNumBeamEnts");
	}
	public void SetAttachmentCount(const int iAttachmentCount)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nNumBeamEnts", iAttachmentCount);
	}
	public int GetBeamType()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nBeamType");
	}
	public int SetBeamType(const int iBeamType)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nBeamType", iBeamType);
	}
	public CBaseEntity GetAttachment(const int iIndex)
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hAttachEntity", iIndex));
	}
	public void SetAttachment(const int iIndex, CBaseEntity pEntity)
	{
		SetEntPropEnt(this.GetEntIndex(), Prop_Send, "m_hAttachEntity", pEntity.GetEntIndex(), iIndex);
	}
}
