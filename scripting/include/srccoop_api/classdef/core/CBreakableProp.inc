#pragma newdecls required
#pragma semicolon 1

methodmap CBreakableProp < CBaseProp
{
	public CBreakableProp(const int iEntIndex = -1)
	{
		return view_as<CBreakableProp>(CBaseProp(iEntIndex));
	}
}
