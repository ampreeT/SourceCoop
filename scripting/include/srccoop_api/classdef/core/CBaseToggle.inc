#pragma newdecls required
#pragma semicolon 1

methodmap CBaseToggle < CBaseEntity
{
	public CBaseToggle(const int iEntIndex = -1)
	{
		return view_as<CBaseToggle>(CBaseEntity(iEntIndex));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}
