#pragma newdecls required
#pragma semicolon 1

methodmap CRagdollProp < CBaseAnimating 
{
	public CRagdollProp(const int iEntIndex = -1)
	{
		return view_as<CRagdollProp>(CBaseAnimating(iEntIndex));
	}
}