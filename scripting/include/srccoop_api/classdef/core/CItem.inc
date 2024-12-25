#pragma newdecls required
#pragma semicolon 1

methodmap CItem < CBaseAnimating
{
	public CItem(const int iEntIndex = -1)
	{
		return view_as<CItem>(CBaseAnimating(iEntIndex));
	}

	public bool CanHardRespawn()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 2);			// SF_ITEM_HARDRESPAWN (1 << 2)
	}
	public void SetHardRespawn(const bool bRespawnEnabled)
	{
		// if set in BM, item respawns at regular interval
		SetSpawnFlagOnEntity(this.GetEntIndex(), 2, bRespawnEnabled);	// SF_ITEM_HARDRESPAWN (1 << 2)
	}
}
