#pragma newdecls required
#pragma semicolon 1

methodmap CLogicAuto < CBaseEntity
{
	public CLogicAuto(const int iEntIndex = -1)
	{
		return view_as<CLogicAuto>(CBaseEntity(iEntIndex));
	}
	public static CLogicAuto Create()
	{
		return CLogicAuto(CreateEntityByName("logic_auto"));
	}

	public bool WillFireOnce()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);   // SF_AUTO_FIREONCE 0x01
	}
	public void SetFireOnce(const bool bFireOnce)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bFireOnce); // SF_AUTO_FIREONCE 0x01
	}
}
