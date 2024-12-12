#pragma newdecls required
#pragma semicolon 1

methodmap CSprite < CBaseEntity
{
	public CSprite(const int iEntIndex = -1)
	{
		return view_as<CSprite>(CBaseEntity(iEntIndex));
	}
	public static CSprite Create()
	{
		return CSprite(CreateEntityByName("env_sprite"));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public float GetScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale");
	}
	public void SetScale(const float flScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteScale", flScale);
	}
	public int GetBrightness()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nBrightness");
	}
	public void SetBrigtness(const int iBrightness)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nBrightness", iBrightness);
	}
	public float GetFrameRate()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate");
	}
	public void SetFrameRate(const float flFrameRate)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flSpriteFramerate", flFrameRate);
	}
	public float GetFrame()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame");
	}
	public void SetFrame(const float flFrame)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFrame", flFrame);
	}
	public float Frames()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flMaxFrame");
	}
	public float GetGlowProxySize()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize");
	}
	public void SetGlowProxySize(const float flGlowProxySize)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flGlowProxySize", flGlowProxySize);
	}
	public void SetAttachment(CBaseEntity pEntity, const int iAttachment)
	{
		if (pEntity.IsValid())
		{
			SetEntPropEnt(this.GetEntIndex(), Prop_Data, "m_hAttachedToEntity", pEntity.GetEntIndex());
			SetEntProp(this.GetEntIndex(), Prop_Data, "m_nAttachment", iAttachment);
			this.SetParent(pEntity);
		}
	}
	public void TurnOff()
	{
		this.AcceptInput("HideSprite");
	}
	public void TurnOn()
	{
		this.AcceptInput("ShowSprite");
	}
	public bool IsOn()
	{
		return !(this.GetEffects() & EF_NODRAW);
	}
}
