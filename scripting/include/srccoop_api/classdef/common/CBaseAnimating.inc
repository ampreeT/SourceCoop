#pragma newdecls required
#pragma semicolon 1

static Handle g_pGetSequenceLinearMotion;

#if defined SRCCOOP_BLACKMESA
static Handle g_pSelectWeightedSequence;
#endif

methodmap CBaseAnimating < CBaseEntity
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szGetSequenceLinearMotion[] = "CBaseAnimating::GetSequenceLinearMotion";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGetSequenceLinearMotion))
			LogMessage("Could not obtain gamedata signature %s", szGetSequenceLinearMotion);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Pointer, _, VENCODE_FLAG_COPYBACK);
			if (!(g_pGetSequenceLinearMotion = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetSequenceLinearMotion);
		}
		
		// This function is virtual only in Black Mesa.
		#if defined SRCCOOP_BLACKMESA
		char szSelectWeightedSequence[] = "CBaseAnimating::SelectWeightedSequence";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szSelectWeightedSequence))
			LogMessage("Could not obtain gamedata offset %s", szSelectWeightedSequence);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pSelectWeightedSequence = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSelectWeightedSequence);
		}
		#endif
	}
	public CBaseAnimating(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimating>(CBaseEntity(iEntIndex));
	}
	public static CBaseAnimating Create(const char[] szClassname)
	{
		return CBaseAnimating(CreateEntityByName(szClassname));
	}

	public int GetSkin()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nSkin");
	}
	public int SetSkin(const int iSkin)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nSkin", iSkin);
	}
	public int GetBody()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nBody");
	}
	public int SetBody(const int iBody)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nBody", iBody);
	}
	public float GetFadeMinDist()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_fadeMinDist");
	}
	public void SetFadeMinDist(const float flFadeMinDist)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_fadeMinDist", flFadeMinDist);
	}
	public float GetFadeMaxDist()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_fadeMaxDist");
	}
	public void SetFadeMaxDist(const float flFadeMaxDist)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_fadeMaxDist", flFadeMaxDist);
	}
	public void SetFadeDistance(const float flFadeMinDist, const float flFadeMaxDist)
	{
		this.SetFadeMinDist(flFadeMinDist);
		this.SetFadeMaxDist(flFadeMaxDist);
	}
	public float GetFadeScale()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flFadeScale");
	}
	public void SetFadeScale(const float flFadeScale)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flFadeScale", flFadeScale);
	}
	public float GetModelScale()
	{
		return GetEntPropFloat(this.entindex, Prop_Send, "m_flModelScale");
	}
	public void SetModelScale(const float flModelScale)
	{
		SetEntPropFloat(this.entindex, Prop_Send, "m_flModelScale", flModelScale);
	}
	public int GetSequence()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nSequence");
	}
	public void SetSequence(const int iSequence)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nSequence", iSequence);
	}
	public void GetSequenceLinearMotion(const int iSequence, float vec3Buffer[3])
	{
		SDKCall(g_pGetSequenceLinearMotion, this, iSequence, vec3Buffer);
	}
	public float GetPlaybackRate()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flPlaybackRate");
	}
	public void SetPlaybackRate(const float flPlaybackRate)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flPlaybackRate", flPlaybackRate);
	}
	public float GetCycle()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flCycle");
	}
	public void SetCycle(const float flCycle)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flCycle", flCycle);
	}
	public int LookupAttachment(const char[] szName)
	{
		return LookupEntityAttachment(this.entindex, szName);
	}
	public bool GetAttachment(const int iAttachment, float vec3Origin[3], float vec3Angles[3])
	{
		return GetEntityAttachment(this.entindex, iAttachment, vec3Origin, vec3Angles);
	}
	public int GetForceBone()
	{
		return GetEntProp(this.entindex, Prop_Send, "m_nForceBone");
	}
	public void SetForceBone(const int iForceBone)
	{
		SetEntProp(this.entindex, Prop_Send, "m_nForceBone", iForceBone);
	}
	#if defined SRCCOOP_BLACKMESA
	public int SelectWeightedSequence(const Activity eActivity)
	{
		// TODO:
		// This is currently only implemented in Black Mesa but it may be required for others.
		// Reimplement the logic from scratch.
		return SDKCall(g_pSelectWeightedSequence, this.entindex, eActivity);
	}
	#endif
	public float GetPoseParameter(const int iIndex)
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flPoseParameter", iIndex);
	}
	public void SetPoseParameter(const int iIndex, const float flPoseParameter)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flPoseParameter", flPoseParameter, iIndex);
	}
	public bool IsSequenceFinished()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_bSequenceFinished") != 0;
	}
	public void SetSequenceFinished(const bool bFinished)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bSequenceFinished", bFinished);
	}
	public float GetAnimationTime()
	{
		return GetEntPropFloat(this.entindex, Prop_Data, "m_flAnimTime");
	}
	public void SetAnimationTime(const float flAnimationTime)
	{
		SetEntPropFloat(this.entindex, Prop_Data, "m_flAnimTime", flAnimationTime);
	}
}
