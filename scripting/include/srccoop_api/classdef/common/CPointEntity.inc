#pragma newdecls required
#pragma semicolon 1

methodmap CPointEntity < CBaseEntity
{
	public CPointEntity(const int iEntIndex = -1)
	{
		return view_as<CPointEntity>(CBaseEntity(iEntIndex));
	}
}
