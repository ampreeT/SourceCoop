#pragma newdecls required
#pragma semicolon 1

methodmap CTraceRay < Handle
{
	public CTraceRay(const float vecPosition[3], const float vecRayType[3], const int iTraceMask, const RayType eRayType, const TraceEntityFilter fnFilter, const any pData)
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
