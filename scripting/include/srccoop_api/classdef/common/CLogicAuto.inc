#pragma newdecls required
#pragma semicolon 1

enum CLogicAuto_SpawnFlags
{
	SF_AUTO_FIREONCE = 0x01,
}

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

	property CLogicAuto_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CLogicAuto_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CLogicAuto_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
