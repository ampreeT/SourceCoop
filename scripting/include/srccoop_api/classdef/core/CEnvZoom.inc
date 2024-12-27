#pragma newdecls required
#pragma semicolon 1

enum CEnvZoom_SpawnFlags
{
	ENV_ZOOM_OVERRIDE = (1 << 0),
}

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
	
	property CEnvZoom_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CEnvZoom_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CEnvZoom_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
}
