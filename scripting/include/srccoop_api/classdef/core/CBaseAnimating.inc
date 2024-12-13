#pragma newdecls required
#pragma semicolon 1

methodmap CBaseAnimating < CBaseEntity
{
	public CBaseAnimating(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimating>(CBaseEntity(iEntIndex));
	}
	public static CBaseAnimating Create(const char[] szClassname)
	{
		return CBaseAnimating(CreateEntityByName(szClassname));
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetSkin()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nSkin");
	}
	public int SetSkin(const int iSkin)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nSkin", iSkin);
	}
	public int GetBody()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nBody");
	}
	public int SetBody(const int iBody)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nBody", iBody);
	}
	public float GetFadeMinDist()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_fadeMinDist");
	}
	public void SetFadeMinDist(const float flFadeMinDist)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_fadeMinDist", flFadeMinDist);
	}
	public float GetFadeMaxDist()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_fadeMaxDist");
	}
	public void SetFadeMaxDist(const float flFadeMaxDist)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_fadeMaxDist", flFadeMaxDist);
	}
	public void SetFadeDistance(const float flFadeMinDist, const float flFadeMaxDist)
	{
		this.SetFadeMinDist(flFadeMinDist);
		this.SetFadeMaxDist(flFadeMaxDist);
	}
	public float GetFadeScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFadeScale");
	}
	public void SetFadeScale(const float flFadeScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flFadeScale", flFadeScale);
	}
	public float GetModelScale()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale");
	}
	public void SetModelScale(const float flModelScale)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Send, "m_flModelScale", flModelScale);
	}
	public int GetSequence()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence");
	}
	public void SetSequence(const int iSequence)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nSequence", iSequence);
	}
	public void GetSequenceLinearMotion(const int iSequence, float vecBuffer[3])
	{
		SDKCall(g_pGetSequenceLinearMotion, this, iSequence, vecBuffer);
	}
	public float GetPlaybackRate()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPlaybackRate");
	}
	public void SetPlaybackRate(const float flPlaybackRate)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPlaybackRate", flPlaybackRate);
	}
	public float GetCycle()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCycle");
	}
	public void SetCycle(const float flCycle)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flCycle", flCycle);
	}
	public int LookupAttachment(const char[] szName)
	{
		return LookupEntityAttachment(this.GetEntIndex(), szName);
	}
	public bool GetAttachment(const int iAttachment, float vecOrigin[3], float vecAngles[3])
	{
		return GetEntityAttachment(this.GetEntIndex(), iAttachment, vecOrigin, vecAngles);
	}
	public int GetForceBone()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_nForceBone");
	}
	public void SetForceBone(const int iForceBone)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_nForceBone", iForceBone);
	}
	#if defined SRCCOOP_BLACKMESA
	public int SelectWeightedSequence(const int iActivity)
	{
		// TODO:
		// This is currently only implemented in Black Mesa but it may be required for others.
		// Reimplement the logic from scratch.
		return SDKCall(g_pSelectWeightedSequence, this.GetEntIndex(), iActivity);
	}
	#endif
	public float GetPoseParameter(const int iIndex)
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPoseParameter", iIndex);
	}
	public void SetPoseParameter(const int iIndex, const float flPoseParameter)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flPoseParameter", flPoseParameter, iIndex);
	}
	public bool IsSequenceFinished()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_bSequenceFinished") != 0;
	}
	public void SetSequenceFinished(const bool bFinished)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bSequenceFinished", bFinished);
	}
	public float GetAnimTime()
	{
		return GetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flAnimTime");
	}
	public void SetAnimTime(const float flAnimTime)
	{
		SetEntPropFloat(this.GetEntIndex(), Prop_Data, "m_flAnimTime", flAnimTime);
	}
}
