#pragma newdecls required
#pragma semicolon 1

enum CSniperTarget_SpawnFlags
{
	SF_SNIPERTARGET_NOINTERRUPT = 2,
}

methodmap CSniperTarget < CPointEntity
{
	public CSniperTarget(const int iEntIndex = -1)
	{
		return view_as<CSniperTarget>(CPointEntity(iEntIndex));
	}

	property CSniperTarget_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CSniperTarget_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CSniperTarget_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
