#pragma newdecls required
#pragma semicolon 1

methodmap CEnvBeam < CBeam
{
	public CEnvBeam(const int iEntIndex = -1)
	{
		return view_as<CEnvBeam>(CBaseEntity(iEntIndex));
	}
	public static CEnvBeam Create()
	{
		return CEnvBeam(CreateEntityByName("env_beam"));
	}
	
	public float GetDuration()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_life");
	}
	public void SetDuration(const float flDuration)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_life", flDuration);
	}
	public bool GetStartEntity(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszStartEntity", szBuffer, iMaxLength)); 
	}
	public int SetStartEntity(const char[] szTargetname)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszStartEntity", szTargetname); 
	}
	public float GetWidth()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_boltWidth");
	}
	public void SetWidth(const float flWidth)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_boltWidth", flWidth);
	}
	public bool GetTextureName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszSpriteName", szBuffer, iMaxLength));
	}
	public int SetTextureName(const char[] szTexturePath)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszSpriteName", szTexturePath); 
	}
	public float GetNoiseAmplitude()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_noiseAmplitude");
	}
	public void SetNoiseAmplitude(const float flAmplitude)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_noiseAmplitude", flAmplitude);
	}
	public int GetTextureScrollRate()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_speed");
	}
	public void SetTextureScrollRate(const int iTextureScrollRate)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_speed", iTextureScrollRate);
	}
}
