#pragma newdecls required
#pragma semicolon 1

methodmap CEnvZoom < CPointEntity
{
	public CEnvZoom(const int iEntIndex = -1)
	{
		return view_as<CEnvZoom>(CPointEntity(iEntIndex));
	}
	public static CEnvZoom Create()
	{
		return CEnvZoom(CreateEntityByName("env_zoom"));
	}
	
	public bool WillOverride()
	{
		return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);	// ENV_ZOOM_OVERRIDE (1 << 0)
	}
	public void SetOverride(const bool bOverride)
	{
		SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bOverride);	// ENV_ZOOM_OVERRIDE (1 << 0)
	}
}
