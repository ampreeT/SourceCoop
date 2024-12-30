#pragma newdecls required
#pragma semicolon 1

methodmap CParticleSystem < CBaseEntity
{
	public CParticleSystem(const int iEntIndex = -1)
	{
		return view_as<CParticleSystem>(CBaseEntity(iEntIndex));
	}
	public static CParticleSystem Create(const char[] szEffectName, bool bStartActive = true)
	{
		CParticleSystem pParticle = CParticleSystem(CreateEntityByName("info_particle_system"));
		if (pParticle != NULL_CBASEENTITY)
		{
			pParticle.SetEffectName(szEffectName);
			if (bStartActive)
			{
				pParticle.SetStartActive(bStartActive);
			}
		}
		return pParticle;
	}
	
	public bool IsActive()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bActive") != 0;
	}
	public bool GetStartActive()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bStartActive") != 0;
	}
	public void SetStartActive(const bool bStartsActive)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bStartActive", bStartsActive);
	}
	public bool IsWeatherEffect()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bWeatherEffect") != 0;
	}
	public void SetWeatherEffect(const bool bWeatherEffect)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bWeatherEffect", bWeatherEffect);
	}
	public bool GetOrientationFollowsViewer()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bOrientationFollowsViewer") != 0;
	}
	public void SetOrientationFollowsViewer(const bool bOrientationFollowsViewer)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bOrientationFollowsViewer", bOrientationFollowsViewer);
	}
	public float GetStartTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flStartTime");
	}
	public void SetStartTime(const float flStartTime)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flStartTime", flStartTime);
	}
	public bool GetEffectName(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "m_iszEffectName", szBuffer, iMaxLength) != 0;
	}
	public int SetEffectName(const char[] szEffectName)
	{
		return SetEntPropString(this.entindex, Prop_Data, "m_iszEffectName", szEffectName); 
	}
	public void Start()
	{
		this.AcceptInput("Start");
	}
	public void Stop()
	{
		this.AcceptInput("Stop");
	}
	public void DestroyImmediately()
	{
		this.AcceptInput("DestroyImmediately");
	}
	public void Reset()
	{
		this.AcceptInput("Reset");
	}
}
