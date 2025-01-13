#pragma newdecls required
#pragma semicolon 1

methodmap CRevertSaved < CBaseEntity
{
	public CRevertSaved(const int iEntIndex = -1)
	{
		return view_as<CRevertSaved>(CBaseEntity(iEntIndex));
	}
	public static CRevertSaved Create()
	{
		return CRevertSaved(CreateEntityByName("player_loadsaved"));
	}

	public float GetDuration()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_Duration");
	}
	public void SetDuration(float flDuration)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_Duration", flDuration);
	}
}
