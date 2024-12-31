#pragma newdecls required
#pragma semicolon 1

enum CChangelevel_SpawnFlags
{
	SF_CHANGELEVEL_NOTOUCH = 0x0002,
}

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

	property CChangelevel_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CChangelevel_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CChangelevel_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
	public void GetMapName(char[] szBuffer, const int iMaxLength)
	{
		GetEntPropString(this.entindex, Prop_Data, "m_szMapName", szBuffer, iMaxLength);
	}
}
