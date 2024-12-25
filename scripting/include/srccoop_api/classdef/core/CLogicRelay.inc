#pragma newdecls required
#pragma semicolon 1

methodmap CLogicRelay < CLogicalEntity
{
	public CLogicRelay(const int iEntIndex = -1)
	{
		return view_as<CLogicRelay>(CLogicalEntity(iEntIndex));
	}
	public static CLogicRelay Create()
	{
		return CLogicRelay(CreateEntityByName("logic_relay"));
	}

	public bool WillRemoveOnFire()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);       // SF_REMOVE_ON_FIRE 0x001
	}
	public void SetRemoveOnFire(const bool bRemoveOnFire)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bRemoveOnFire); // SF_REMOVE_ON_FIRE 0x001
	}
}
