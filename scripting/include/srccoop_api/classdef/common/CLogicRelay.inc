#pragma newdecls required
#pragma semicolon 1

enum CLogicRelay_SpawnFlags
{
	SF_REMOVE_ON_FIRE = 0x001,
}

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

	property CLogicRelay_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CLogicRelay_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CLogicRelay_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
