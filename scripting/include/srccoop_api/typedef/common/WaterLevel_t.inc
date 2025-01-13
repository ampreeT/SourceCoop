#pragma newdecls required
#pragma semicolon 1

//-----------------------------------------------------------------------------
// m_nWaterLevel enumeration
// Purpose: Identifies how submerged in water a player is.
//-----------------------------------------------------------------------------
enum WaterLevel_t
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
}
