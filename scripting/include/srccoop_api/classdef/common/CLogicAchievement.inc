#pragma newdecls required
#pragma semicolon 1

methodmap CLogicAchievement < CLogicalEntity
{
	public CLogicAchievement(const int iEntIndex = -1)
	{
		return view_as<CLogicAchievement>(CLogicalEntity(iEntIndex));
	}
	public static CLogicAchievement Create()
	{
		return CLogicAchievement(CreateEntityByName("logic_achievement"));
	}

	public bool GetAchievementEventID(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszAchievementEventID", szBuffer, iMaxLength) != 0;
	}
}
