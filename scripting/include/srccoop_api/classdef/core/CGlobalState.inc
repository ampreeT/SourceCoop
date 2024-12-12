#pragma newdecls required
#pragma semicolon 1

methodmap CGlobalState
{
	public static int GetIndex(const char[] szGlobalname)
	{
		return SDKCall(g_pGlobalEntityGetIndex, szGlobalname);
	}
	public static GLOBALESTATE GetStateByIndex(const int iGlobalIndex)
	{
		return SDKCall(g_pGlobalEntityGetState, iGlobalIndex);
	}
	public static GLOBALESTATE GetState(const char[] szGlobalname)
	{
		return CGlobalState.GetStateByIndex(CGlobalState.GetIndex(szGlobalname));
	}
	public static int GetName(const int iGlobalIndex, const char[] szGlobalName, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetName, szGlobalName, iMaxLength, iGlobalIndex);
	}
	public static bool IsValidIndex(const int iGlobalIndex)
	{
		return CGlobalState.GetName(iGlobalIndex, NULL_STRING, 0) != -1;
	}
	public static int GetCounter(const int iGlobalIndex)
	{
		return SDKCall(g_pGlobalEntityGetCounter, iGlobalIndex);
	}
	public static int GetMap(const int iGlobalIndex, const char[] szGlobalMap, const int iMaxLength)
	{
		return SDKCall(g_pGlobalEntityGetMap, szGlobalMap, iMaxLength, iGlobalIndex);
	}
	public static bool SetState(const int iGlobalIndex, GLOBALESTATE state)
	{
		if (CGlobalState.IsValidIndex(iGlobalIndex))
		{
			SDKCall(g_pGlobalEntitySetState, iGlobalIndex, state);
			return true;
		}
		
		return false;
	}
	public static bool SetStateByName(const char[] szGlobalName, const GLOBALESTATE state)
	{
		int iGlobalIndex = CGlobalState.GetIndex(szGlobalName);
		if (iGlobalIndex != -1)
		{
			CGlobalState.SetState(iGlobalIndex, state);
			return true;
		}
		
		return false;
	}
	public static bool SetCounter(const int iGlobalIndex, const int iCounter)
	{
		if (CGlobalState.IsValidIndex(iGlobalIndex))
		{
			SDKCall(g_pGlobalEntitySetCounter, iGlobalIndex, iCounter);
			return true;
		}
		
		return false;
	}
	public static bool SetCounterByName(const char[] szGlobalName, const int iCounter)
	{
		int iGlobalIndex = CGlobalState.GetIndex(szGlobalName);
		if (iGlobalIndex != -1)
		{
			CGlobalState.SetCounter(iGlobalIndex, iCounter);
			return true;
		}
		
		return false;
	}
	// Returns index to global state
	public static int Add(const char[] szGlobalName, const char[] szMapName, GLOBALESTATE state)
	{
		return SDKCall(g_pGlobalEntityAdd, szGlobalName, szMapName, state);
	}
}
