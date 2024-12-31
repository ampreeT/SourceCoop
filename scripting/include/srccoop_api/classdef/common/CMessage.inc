#pragma newdecls required
#pragma semicolon 1

enum CMessage_SpawnFlags
{
	SF_MESSAGE_ONCE = 0x0001,
	SF_MESSAGE_ALL = 0x0002,
}

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

	property CMessage_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CMessage_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CMessage_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
	public bool GetMessage(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszMessage", szBuffer, iMaxLength) != 0;
	}
	public void SetMessage(const char[] szMessage)
	{
		SetEntPropString(this.entindex, Prop_Data, "m_iszMessage", szMessage);
	}
	public void ShowMessage()
	{
		this.AcceptInputStr("ShowMessage");
	}
}
