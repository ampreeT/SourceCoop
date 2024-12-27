#pragma newdecls required
#pragma semicolon 1

methodmap CBM_MP_GameRules < CMultiplayRules
{
	public static int GetStateIndex(const char[] szStateProp)
	{
		if (strcmp(szStateProp, STATE_WARMUP) == 0) return STATE_WARMUP_INDEX;
		if (strcmp(szStateProp, STATE_ROUND) == 0) return STATE_ROUND_INDEX;
		if (strcmp(szStateProp, STATE_INTERMISSION) == 0) return STATE_INTERMISSION_INDEX;
		SetFailState("CBM_MP_GameRules.GetStateIndex %s", szStateProp); return -1;
	}
	
	public static void SetStateEndTime(const char[] szStateProp, const float flEndTime)
	{
		GameRules_SetPropFloat(szStateProp, flEndTime, STATE_ELEMENT_DONE_TIME);
	}
	public static void SetCurrentState(const char[] szStateProp)
	{
		GameRules_SetProp("m_nCurrentStateId", CBM_MP_GameRules.GetStateIndex(szStateProp));
	}
	public static void SetStateNextState(const char[] szStateProp, const char[] szNextStateProp)
	{
		GameRules_SetProp(szStateProp, CBM_MP_GameRules.GetStateIndex(szNextStateProp), _, STATE_ELEMENT_NEXT_STATE);
	}
	public static void SetStateIsInIntermission(const char[] szStateProp, const bool bInIntermission)
	{
		GameRules_SetProp(szStateProp, bInIntermission, _, STATE_ELEMENT_IS_IN_INTERMISSION);
	}
}
