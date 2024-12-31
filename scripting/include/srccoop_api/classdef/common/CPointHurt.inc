#pragma newdecls required
#pragma semicolon 1

methodmap CPointHurt < CPointEntity
{
	public CPointHurt(const int iEntIndex = -1)
	{
		return view_as<CPointHurt>(CPointEntity(iEntIndex));
	}
	public static CPointHurt Create()
	{
		return CPointHurt(CreateEntityByName("point_hurt"));
	}
	
	public int GetDamageType()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bitsDamageType");
	}
}
