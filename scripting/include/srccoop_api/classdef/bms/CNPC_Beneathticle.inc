#pragma newdecls required
#pragma semicolon 1

methodmap CNPC_Beneathticle < CAI_BaseNPC
{
	public CNPC_Beneathticle(const int iEntIndex = -1)
	{
		return view_as<CNPC_Beneathticle>(CAI_BaseNPC(iEntIndex));
	}
	public static CNPC_Beneathticle Create()
	{
		return CNPC_Beneathticle(CreateEntityByName("npc_beneathticle"));
	}

	public float GetTongueLength()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flTongueLength");
	}
	public void SetTongueLength(const float flTongueLength)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flTongueLength", flTongueLength);

		static const int g_iTonguePoseIndex = 0;
		static const float g_flMaxTonguePoseDistance = 3000.0;
		this.SetPoseParameter(g_iTonguePoseIndex, Clamp(flTongueLength / g_flMaxTonguePoseDistance, 0.0, 1.0));
	}
	public float GetTongueLengthOverride()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flTongueLengthOverride");
	}
	public void SetTongueLengthOverride(const float flTongueLengthOverride)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flTongueLengthOverride", flTongueLengthOverride);
	}
	public float GetTonguePullSpeed()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flBeneathiclePullSpeed");
	}
	public void SetTonguePullSpeed(const float flTonguePullSpeed)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flBeneathiclePullSpeed", flTonguePullSpeed);
	}
	public float GetVictimHeight()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flVictimHeight");
	}
	public void SetVictimHeight(const float flVictimHeight)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flVictimHeight", flVictimHeight);
	}
    public CBaseEntity GetPrey()
    {
        return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hPrey"));
    }
    public void SetPrey(const CBaseEntity pEntity)
    {
        SetEntPropEnt(this.entindex, Prop_Data, "m_hPrey", pEntity.entindex);
    }
    public bool IsPullingPrey()
    {
        return GetEntProp(this.entindex, Prop_Data, "m_bPullingPrey") != 0;
    }
    public void SetPullingPrey(const bool bPullingPrey)
    {
        SetEntProp(this.entindex, Prop_Data, "m_bPullingPrey", bPullingPrey);
    }
}
