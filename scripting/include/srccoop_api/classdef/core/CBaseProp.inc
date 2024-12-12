#pragma newdecls required
#pragma semicolon 1

methodmap CBaseProp < CBaseAnimating
{
	public CBaseProp(const int iEntIndex = -1)
	{
		return view_as<CBaseProp>(CBaseAnimating(iEntIndex));
	}
}
