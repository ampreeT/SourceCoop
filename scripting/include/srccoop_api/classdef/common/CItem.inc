#pragma newdecls required
#pragma semicolon 1

enum CItem_SpawnFlags
{
	SF_ITEM_HARDRESPAWN = (1 << 2),
}

methodmap CItem < CBaseAnimating
{
	public CItem(const int iEntIndex = -1)
	{
		return view_as<CItem>(CBaseAnimating(iEntIndex));
	}

	property CItem_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CItem_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CItem_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
