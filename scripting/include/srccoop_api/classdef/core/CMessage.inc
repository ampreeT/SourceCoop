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
}
