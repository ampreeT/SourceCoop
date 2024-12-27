#pragma newdecls required
#pragma semicolon 1

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public CBaseAnimatingOverlay(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimatingOverlay>(CBaseAnimating(iEntIndex));
	}
} 
