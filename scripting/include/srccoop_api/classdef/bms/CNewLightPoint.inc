#pragma newdecls required
#pragma semicolon 1

methodmap CNewLightPoint < CBaseEntity
{
	public CNewLightPoint(const int iEntIndex = -1)
	{
		return view_as<CNewLightPoint>(CBaseEntity(iEntIndex));
	}
	
	public static CNewLightPoint Create()
	{
		return CNewLightPoint(CreateEntityByName("newlight_point"));
	}
	
	public bool GetEnabled()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bEnabled") != 0;
	}
	
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bEnabled", bEnabled);
	}
	
	public int GetStyle()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iStyle");
	}
	
	public void SetStyle(const int iStyle)
	{
		SetEntProp(this.entindex, Prop_Data, "m_iStyle", iStyle);
	}

	public int GetLightColor()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_tColor");
	}
	
	public void SetLightColor(const int r, const int g, const int b)
	{
		int col3 = (b << 16) | (g << 8) | r; //calc rgb col int value
		SetEntProp(this.entindex, Prop_Data, "m_tColor", col3);
	}

	public int GetLightColorAmbient()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_tColorAmbient");
	}
	
	public void SetLightColorAmbient(const int r, const int g, const int b)
	{
		int col3 = (b << 16) | (g << 8) | r;
		SetEntProp(this.entindex, Prop_Data, "m_tColorAmbient", col3);
	}

	public float GetIntensity()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fIntensity");
	}
	
	public void SetIntensity(const float flIntensity)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fIntensity", flIntensity);
	}

	public float GetRange()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fRange");
	}
	
	public void SetRange(const float flRange)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fRange", flRange);
	}

	public float GetSpecMultiplier()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fSpecMultiplier");
	}
	
	public void SetSpecMultiplier(const float flSpecMultiplier)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fSpecMultiplier", flSpecMultiplier);
	}
	
	public bool GetHasShadow()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bHasShadow") != 0;
	}
	
	public void SetHasShadow(const bool bHasShadow)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bHasShadow", bHasShadow);
	}
	
	public int GetLightType()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_tLightType");
	}
	
	public void SetLightType(const int iLightType)
	{
		SetEntProp(this.entindex, Prop_Data, "m_tLightType", iLightType);
	}
	
	public bool GetTexName(char[] szBuffer, const int iMaxLength)
	{
		return GetEntPropString(this.entindex, Prop_Data, "texName", szBuffer, iMaxLength) != 0;
	}
	
	public int SetTexName(const char[] szTexturePath)
	{
		return SetEntPropString(this.entindex, Prop_Data, "texName", szTexturePath); 
	}
	
	public bool GetTexLight()
	{
		return GetEntProp(this.entindex, Prop_Data, "bTexLight") != 0;
	}
	
	public void SetTexLight(const bool bTexLight)
	{
		SetEntProp(this.entindex, Prop_Data, "bTexLight", bTexLight);
	}

	public float GetShadowRadius()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fShadowRadius");
	}
	
	public void SetShadowRadius(const float flShadowRadius)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fShadowRadius", flShadowRadius);
	}
	
	public bool GetEnableGodRays()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bEnableGodRays") != 0;
	}
	
	public void SetEnableGodRays(const bool bEnableGodRays)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bEnableGodRays", bEnableGodRays);
	}

	public float GetDensity()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fDensity");
	}
	
	public void SetDensity(const float flDensity)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fDensity", flDensity);
	}

	public float GetExposure()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fExposure");
	}
	
	public void SetExposure(const float flExposure)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fExposure", flExposure);
	}

	public float GetWeight()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fWeight");
	}
	
	public void SetWeight(const float flWeight)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fWeight", flWeight);
	}
	
	public float GetGodRaysDistFactor()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fDistFactor");
	}
	
	public void SetDistFactor(const float flDistFactor)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fDistFactor", flDistFactor);
	}

	public bool GetGodRaysType()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_nGodRaysType") != 0;
	}
	
	public void SetGodRaysType(const bool iGodRaysType)
	{
		SetEntProp(this.entindex, Prop_Data, "m_nGodRaysType", iGodRaysType);
	}

	public float GetDiskInnerSizePercent()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fDiskInnerSizePercent");
	}
	
	public void SetDiskInnerSizePercent(const float flDiskInnerSizePercent)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fDiskInnerSizePercent", flDiskInnerSizePercent);
	}

	public float GetDiskRadius()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_fDiskRadius");
	}
	
	public void SetDiskRadius(const float flDiskRadius)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_fDiskRadius", flDiskRadius);
	}

	public void TurnOn()
	{
		this.AcceptInput("TurnOn");
	}
	
	public void TurnOff()
	{
		this.AcceptInput("TurnOff");
	}
	
	public void Toggle()
	{
		this.AcceptInput("Toggle");
	}
	
	public void TurnOnGodRays()
	{
		this.AcceptInput("TurnOnGodRays");
	}
	
	public void TurnOffGodRays()
	{
		this.AcceptInput("TurnOffGodRays");
	}
	
	public void SetColorLight(const float r, const float g, const float b)
	{
		float rgb[3];
		
		rgb[0] = r;
		rgb[1] = g;
		rgb[2] = b;
		
		this.AcceptInputVector("SetColorLight", rgb);
	}
	
	public void SetIntensityForLight(const float flIntensity)
	{
		//Todo: Replace with AcceptEntityFloat if it will exists 
		char szBuffer[256];
		
		FloatToString(flIntensity, szBuffer, sizeof(szBuffer));
		
		SetVariantString(szBuffer);
		AcceptEntityInput(this.entindex, "SetIntensityForLight");
	}
}