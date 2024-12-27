#pragma newdecls required
#pragma semicolon 1

enum CSprite_SpawnFlags
{
	SF_SPRITE_STARTON = 0x0001,
	SF_SPRITE_ONCE = 0x0002,
	SF_SPRITE_TEMPORARY = 0x8000,
}

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

	property CSprite_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CSprite_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CSprite_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
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
	public float GetMaxFrame()
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
	public void SetAttachment(const CBaseEntity pEntity, const int iAttachment)
	{
		if (pEntity != NULL_CBASEENTITY)
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
		return !this.HasEffects(EF_NODRAW);
	}
}
