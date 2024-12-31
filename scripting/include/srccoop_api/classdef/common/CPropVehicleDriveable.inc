#pragma newdecls required
#pragma semicolon 1

methodmap CPropVehicleDriveable < CBaseAnimating
{
	public CPropVehicleDriveable(const int iEntIndex = -1)
	{
		return view_as<CPropVehicleDriveable>(CBaseAnimating(iEntIndex));
	}
}
