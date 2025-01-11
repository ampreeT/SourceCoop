#pragma newdecls required
#pragma semicolon 1

methodmap CTraceRay < Handle
{
	public CTraceRay(const float vecPosition[3], const float vecRayType[3], const int iTraceMask, const RayType eRayType, const TraceEntityFilter fnFilter, const any pData = -1)
	{
		return view_as<CTraceRay>(TR_TraceRayFilterEx(vecPosition, vecRayType, iTraceMask, eRayType, fnFilter, pData));
	}

	public bool StartedSolid()
	{
		return TR_StartSolid(this);
	}
	public float GetFraction()
	{
		return TR_GetFraction(this);
	}
	public CBaseEntity GetEntity()
	{
		return CBaseEntity(TR_GetEntityIndex(this));
	}
}

stock bool TraceEntityFilter_IgnoreData(int iEntIndex, int iMask, any pData)
{
	return (iEntIndex != pData);
}

stock bool TraceEntityFilter_IgnorePlayers(int iEntIndex, int iMask, any pData)
{
	CBasePlayer pPlayer = CBasePlayer(iEntIndex);
	if (pPlayer.IsValid())
		return false;
	return (iEntIndex != pData);
}