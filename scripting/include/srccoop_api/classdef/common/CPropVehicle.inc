#pragma newdecls required
#pragma semicolon 1

methodmap CPropVehicle < CBaseProp
{
	public CPropVehicle(const int iEntIndex = -1)
	{
		return view_as<CPropVehicle>(CBaseProp(iEntIndex));
	}
}
