#pragma newdecls required
#pragma semicolon 1

static Handle g_pGlobalEntityGetIndex;
static Handle g_pGlobalEntityGetState;
static Handle g_pGlobalEntityGetName;
static Handle g_pGlobalEntityGetCounter;
static Handle g_pGlobalEntityGetMap;
static Handle g_pGlobalEntitySetState;
static Handle g_pGlobalEntitySetCounter;
static Handle g_pGlobalEntityAdd;

methodmap CGlobalState
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szGlobalEntity_GetIndex[] = "GlobalEntity_GetIndex";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetIndex))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetIndex);
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntityGetIndex = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetIndex);
		}

		char szGlobalEntity_GetState[] = "GlobalEntity_GetState";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetState))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetState);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntityGetState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetState);
		}

		char szGlobalEntity_GetName[] = "GlobalEntity_GetName";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetName))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetName);
		else
		{	
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
			if (!(g_pGlobalEntityGetName = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetName);
		}

		char szGlobalEntity_GetCounter[] = "GlobalEntity_GetCounter";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetCounter))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetCounter);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntityGetCounter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetCounter);
		}

		char szGlobalEntity_GetMap[] = "GlobalEntity_GetMap";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_GetMap))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_GetMap);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_String, SDKPass_Pointer);
			if (!(g_pGlobalEntityGetMap = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_GetMap);
		}

		char szGlobalEntity_SetState[] = "GlobalEntity_SetState";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_SetState))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_SetState);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntitySetState = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_SetState);
		}

		char szGlobalEntity_SetCounter[] = "GlobalEntity_SetCounter";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_SetCounter))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_SetCounter);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntitySetCounter = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_SetCounter);
		}

		char szGlobalEntity_Add[] = "GlobalEntity_Add";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szGlobalEntity_Add))
			LogMessage("Could not obtain gamedata signature %s", szGlobalEntity_Add);
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGlobalEntityAdd = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGlobalEntity_Add);
		}
	}
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
	public static bool SetState(const int iGlobalIndex, const GLOBALESTATE eState)
	{
		if (CGlobalState.IsValidIndex(iGlobalIndex))
		{
			SDKCall(g_pGlobalEntitySetState, iGlobalIndex, eState);
			return true;
		}
		
		return false;
	}
	public static bool SetStateByName(const char[] szGlobalName, const GLOBALESTATE eState)
	{
		int iGlobalIndex = CGlobalState.GetIndex(szGlobalName);
		if (iGlobalIndex != -1)
		{
			CGlobalState.SetState(iGlobalIndex, eState);
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
	public static int Add(const char[] szGlobalName, const char[] szMapName, const GLOBALESTATE eState)
	{
		return SDKCall(g_pGlobalEntityAdd, szGlobalName, szMapName, eState);
	}
}
