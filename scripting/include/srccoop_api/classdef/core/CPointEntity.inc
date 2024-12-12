#pragma newdecls required
#pragma semicolon 1

methodmap CPointEntity < CBaseEntity
{
	public CPointEntity(const int iEntIndex = -1)
	{
		return view_as<CPointEntity>(CBaseEntity(iEntIndex));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}
