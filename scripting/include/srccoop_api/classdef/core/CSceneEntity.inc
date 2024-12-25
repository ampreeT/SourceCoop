#pragma newdecls required
#pragma semicolon 1

methodmap CSceneEntity < CPointEntity
{
	public CSceneEntity(const int iEntIndex = -1)
	{
		return view_as<CSceneEntity>(CBaseEntity(iEntIndex));
	}
	public static CSceneEntity Create()
	{
		// Could also be a "scripted_scene`.
		return CSceneEntity(CreateEntityByName("logic_choreographed_scene"));
	}

	public float GetCurrentTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCurrentTime");
	}
	public void SetCurrentTime(const float flCurrentTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCurrentTime", flCurrentTime);
	}
	public float GetForceClientTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flForceClientTime");
	}
	public void SetForceClientTime(const float flForceClientTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flForceClientTime", flForceClientTime);
	}
}