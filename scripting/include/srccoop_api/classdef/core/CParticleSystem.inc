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
		if (pParticle.IsValid())
		{
			pParticle.SetEffectName(szEffectName);
			if (bStartActive)
			{
				pParticle.SetStartActive(bStartActive);
			}
		}
		return pParticle;
	}
	
	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public bool IsActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bActive"));
	}
	public bool GetStartActive()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive"));
	}
	public void SetStartActive(const bool bStartsActive)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bStartActive", bStartsActive);
	}
	public bool IsWeatherEffect()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect"));
	}
	public void SetWeatherEffect(const bool bWeatherEffect)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bWeatherEffect", bWeatherEffect);
	}
	public bool GetOrientationFollowsViewer()
	{
		return view_as<bool>(GetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer"));
	}
	public void SetOrientationFollowsViewer(const bool bOrientationFollowsViewer)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bOrientationFollowsViewer", bOrientationFollowsViewer);
	}
	public float GetStartTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime");
	}
	public void SetStartTime(const float flStartTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flStartTime", flStartTime);
	}
	public bool GetEffectName(char[] szBuffer, const int iMaxLength)
	{
		return view_as<bool>(GetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", szBuffer, iMaxLength));
	}
	public int SetEffectName(const char[] szEffectName)
	{
		return SetEntPropString(this.GetEntIndex(), Prop_Data, "m_iszEffectName", szEffectName); 
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
