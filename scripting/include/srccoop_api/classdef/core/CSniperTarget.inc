#pragma newdecls required
#pragma semicolon 1

methodmap CSniperTarget < CPointEntity
{
	public CSniperTarget(const int iEntIndex = -1)
	{
		return view_as<CSniperTarget>(CPointEntity(iEntIndex));
	}

	public bool IsInterruptable()
	{
	    return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 1);      // SF_SNIPERTARGET_NOINTERRUPT 2
	}
	public void SetInterrupt(const bool bInterrupt)
	{
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 1, !bInterrupt);   // SF_SNIPERTARGET_NOINTERRUPT 2
	}
}
