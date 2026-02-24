#pragma newdecls required
#pragma semicolon 1

methodmap CNpc_Snark < CBaseCombatCharacter
{
	public CNpc_Snark(const int iEntIndex = -1)
	{
		return view_as<CNpc_Snark>(CBaseEntity(iEntIndex));
	}
	public static CNpc_Snark Create()
	{
		return CNpc_Snark(CreateEntityByName("npc_snark"));
	}

	public float GetLifeTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fLifeTime");
	}
	
	public void SetLifeTime(const float flLifeTime)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fLifeTime", flLifeTime);
	}

	public int GetSnarkOwner()
	{
		return GetEntPropEnt(this.entindex, Prop_Data, "m_hOwner");
	}

	public void SetSnarkOwner(int iEntIndex)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hOwner", iEntIndex);
	}
}