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
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bSweepHighestPriority"));
	}
	public CBaseEntity GetSweepTarget()
	{
		return CBaseEntity(GetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hSweepTarget"));
	}
	public bool IsWeaponLoaded()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fWeaponLoaded"));
	}
	public bool IsEnabled()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_fEnabled"));
	}
}
