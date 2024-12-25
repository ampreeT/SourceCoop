#pragma newdecls required
#pragma semicolon 1

methodmap CMessage < CPointEntity
{
	public CMessage(const int iEntIndex = -1)
	{
		return view_as<CMessage>(CPointEntity(iEntIndex));
	}
	public static CMessage Create()
	{
		return CMessage(CreateEntityByName("env_message"));
	}

	public bool GetMessage(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszMessage", szBuffer, iMaxLength) != 0;
	}
	public void SetMessage(const char[] szMessage)
	{
		SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszMessage", szMessage);
	}
	public void ShowMessage()
	{
		this.AcceptInputStr("ShowMessage");
	}
	public bool WillShowOnce()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);	// SF_MESSAGE_ONCE 0x0001
	}
	public void SetShowOnce(const bool bShowOnce)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bShowOnce);	// SF_MESSAGE_ONCE 0x0001
	}
	public bool WillShowAllPlayers()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 1);			// SF_MESSAGE_ALL 0x0002
	}
	public void SetShowAllPlayers(const bool bShowAllPlayers)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 1, bShowAllPlayers);	// SF_MESSAGE_ALL 0x0002
	}
}
