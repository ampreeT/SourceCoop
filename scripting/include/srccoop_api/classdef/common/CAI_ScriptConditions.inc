#pragma newdecls required
#pragma semicolon 1

methodmap CAI_ScriptConditions < CBaseEntity
{
	public CAI_ScriptConditions(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptConditions>(CBaseEntity(iEntIndex));
	}
	public static CAI_ScriptConditions Create()
	{
		return CAI_ScriptConditions(CreateEntityByName("ai_script_conditions"));
	}

	public bool IsEnabled()
	{
		return !GetEntProp(this.entindex, Prop_Data, "m_fDisabled");
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.entindex, Prop_Data, "m_fDisabled", !bEnabled);
	}
	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hTarget"));
	}
	public void SetTargetEnt(const CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hTarget", pTarget.entindex);
	}
}
