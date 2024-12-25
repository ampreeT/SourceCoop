#pragma newdecls required
#pragma semicolon 1

methodmap CTriggerPush < CBaseTrigger
{
	public CTriggerPush(const int iEntIndex = -1)
	{
		return view_as<CTriggerPush>(CBaseTrigger(iEntIndex));
	}

	public float GetPushSpeed()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpeed");
	}
	public void SetPushSpeed(const float flSpeed)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpeed", flSpeed);
	}
	public void GetPushDirection(float vecBuffer[3])
	{
		GetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPushDir", vecBuffer);
	}
	public void SetPushDirection(const float vec3Direction[3])
	{
		SetEntPropVector(this.GetEntIndex(), Prop_Data, "m_vecPushDir", vec3Direction);
	}
}