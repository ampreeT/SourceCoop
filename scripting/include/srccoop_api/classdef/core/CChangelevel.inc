#pragma newdecls required
#pragma semicolon 1

methodmap CChangelevel < CBaseTrigger
{
	public CChangelevel(const int iEntIndex = -1)
	{
		return view_as<CChangelevel>(CBaseTrigger(iEntIndex));
	}
	public static CChangelevel Create()
	{
		return CChangelevel(CreateEntityByName("trigger_changelevel"));
	}

	public void GetMapName(char[] szBuffer, const int iMaxLength)
	{
		GetEntPropString(this.GetEntIndex(), Prop_Data, "m_szMapName", szBuffer, iMaxLength);
	}
	public bool IsTouchEnabled()
	{
		return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 1);			// SF_CHANGELEVEL_NOTOUCH 0x0002
	}
	public void SetTouchEnabled(const bool bTouchEnabled)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 1, !bTouchEnabled);	// SF_CHANGELEVEL_NOTOUCH 0x0002
	}
}
