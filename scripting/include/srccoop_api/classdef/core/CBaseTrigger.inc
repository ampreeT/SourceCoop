#pragma newdecls required
#pragma semicolon 1

methodmap CBaseTrigger < CBaseToggle
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(CBaseToggle(iEntIndex));
	}

	public bool IsEnabled()
	{
		return !view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled"));
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled", !bEnabled);
	}
}
