#pragma newdecls required
#pragma semicolon 1

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
    
	public bool WillPlayBootSequence()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 17);			    // SF_BOOTSEQUENCE (1 << 17)
	}
	public void SetPlayBootSequence(const bool bPlayBootSequence)
	{
		// if set in BM, item respawns at regular interval
		SetSpawnFlagOnEntity(this.GetEntIndex(), 17, bPlayBootSequence);    // SF_BOOTSEQUENCE (1 << 17)
	}
}
