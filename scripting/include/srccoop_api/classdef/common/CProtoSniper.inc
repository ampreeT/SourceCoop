#pragma newdecls required
#pragma semicolon 1

methodmap CProtoSniper < CAI_BaseNPC
{
	public CProtoSniper(const int iEntIndex = -1)
	{
		return view_as<CProtoSniper>(CAI_BaseNPC(iEntIndex));
	}
	public static CProtoSniper Create()
	{
		return CProtoSniper(CreateEntityByName("npc_sniper"));
	}
	
	public bool IsSweepHighestPriority()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bSweepHighestPriority") != 0;
	}
	public CSniperTarget GetSweepTarget()
	{
		return CSniperTarget(GetEntPropEnt(this.entindex, Prop_Data, "m_hSweepTarget"));
	}
	public bool IsWeaponLoaded()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_fWeaponLoaded") != 0;
	}
	public bool IsEnabled()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_fEnabled") != 0;
	}
}
