#pragma newdecls required
#pragma semicolon 1

enum CEnvBeam_SpawnFlags
{
	SF_BEAM_STARTON = 0x0001,
}

methodmap CEnvBeam < CBeam
{
	public CEnvBeam(const int iEntIndex = -1)
	{
		return view_as<CEnvBeam>(CBeam(iEntIndex));
	}
	public static CEnvBeam Create()
	{
		return CEnvBeam(CreateEntityByName("env_beam"));
	}
	
	property CEnvBeam_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CEnvBeam_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CEnvBeam_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
	public float GetDuration()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_life");
	}
	public void SetDuration(const float flDuration)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_life", flDuration);
	}
	public bool GetStartEntity(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszStartEntity", szBuffer, iMaxLength) != 0; 
	}
	public int SetStartEntity(const char[] szTargetname)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_iszStartEntity", szTargetname); 
	}
	public float GetWidth()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_boltWidth");
	}
	public void SetWidth(const float flWidth)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_boltWidth", flWidth);
	}
	public bool GetTextureName(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszSpriteName", szBuffer, iMaxLength) != 0;
	}
	public int SetTextureName(const char[] szTexturePath)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_iszSpriteName", szTexturePath); 
	}
	public float GetNoiseAmplitude()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_noiseAmplitude");
	}
	public void SetNoiseAmplitude(const float flAmplitude)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_noiseAmplitude", flAmplitude);
	}
	public int GetTextureScrollRate()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_speed");
	}
	public void SetTextureScrollRate(const int iTextureScrollRate)
	{
		SetEntProp(this.entindex, Prop_Send, "m_speed", iTextureScrollRate);
	}
}
