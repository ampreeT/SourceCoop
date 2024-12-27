#pragma newdecls required
#pragma semicolon 1

methodmap CBaseFlex < CBaseAnimatingOverlay
{
	public CBaseFlex(const int iEntIndex = -1)
	{
		return view_as<CBaseFlex>(CBaseAnimatingOverlay(iEntIndex));
	}
}
