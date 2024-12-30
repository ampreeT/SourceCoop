#pragma newdecls required
#pragma semicolon 1

methodmap CWorld < CBaseEntity
{
	public CWorld()
	{
		return view_as<CWorld>(CBaseEntity(0));
	}

	public bool IsValid()
	{
		return true;
	}
	public bool GetChapterTitle(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszChapterTitle", szBuffer, iMaxLength) != 0;
	}
}
