#pragma newdecls required
#pragma semicolon 1

methodmap CLogicalEntity < CBaseEntity
{
	public CLogicalEntity(const int iEntIndex = -1)
	{
		return view_as<CLogicalEntity>(CBaseEntity(iEntIndex));
	}
	
	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
}
