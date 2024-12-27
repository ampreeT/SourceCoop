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
}
