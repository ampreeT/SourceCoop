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
}
