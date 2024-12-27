#pragma newdecls required
#pragma semicolon 1

enum CItemSuit_SpawnFlags
{
	SF_BOOTSEQUENCE = (1 << 17),
}

// In BMS, the name of this class is `CItem_Suit`.
methodmap CItemSuit < CItem
{
	public CItemSuit(const int iEntIndex = -1)
	{
		return view_as<CItemSuit>(CItem(iEntIndex));
	}
	public static CItemSuit Create()
	{
		return CItemSuit(CreateEntityByName("item_suit"));
	}
    
	property CItemSuit_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CItemSuit_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CItemSuit_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
