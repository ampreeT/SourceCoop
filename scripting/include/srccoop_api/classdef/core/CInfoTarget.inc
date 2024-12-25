#pragma newdecls required
#pragma semicolon 1

methodmap CInfoTarget < CPointEntity
{
	public CInfoTarget(const int iEntIndex = -1)
	{
		return view_as<CInfoTarget>(CPointEntity(iEntIndex));
	}
	public static CInfoTarget Create()
	{
		return CInfoTarget(CreateEntityByName("info_target"));
	}

	public bool IsForcedToTransmitToAllClients()
	{
        return this.HasEFlags(EFL_FORCE_CHECK_TRANSMIT);
	}
    // Ripped from CInfoTarget::Spawn().
    // TODO: should this be moved to `CBaseEntity`?
	public void SetForceTransmitToAllClients(const bool bTransmitToAllClients)
	{
		bTransmitToAllClients ? this.AddEFlags(EFL_FORCE_CHECK_TRANSMIT) : this.RemoveEFlags(EFL_FORCE_CHECK_TRANSMIT);
	}
}
