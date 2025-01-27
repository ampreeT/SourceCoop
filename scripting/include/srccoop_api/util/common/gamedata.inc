#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Macros for loading gamedata keyvalues into configuration structs
//------------------------------------------------------

// Prefix for keys in gamedata. Define before including. 
#if !defined CONF_PREFIX
 #define CONF_PREFIX ""
#endif

// Creates a init func for the struct. Pass in a CONF_TYPE macro for each of the struct's variables.
#define CONF_INIT(%1)\
void Initialize(GameData pGameConfig, bool bAutoClose = true) {\
	%1\
	if (bAutoClose) pGameConfig.Close();\
}

#define CONF_STR(%1) GetGamedataString(pGameConfig, CONF_PREFIX...#%1, this.%1, sizeof(this.%1));
#define CONF_FLT(%1) this.%1 = GetGamedataFloat(pGameConfig, CONF_PREFIX...#%1);
#define CONF_INT(%1) this.%1 = GetGamedataInt(pGameConfig, CONF_PREFIX...#%1);
#define CONF_CLR(%1) this.%1 = GetGamedataColor(pGameConfig, CONF_PREFIX...#%1);

//------------------------------------------------------
// Gamedata stocks
//------------------------------------------------------

stock void GetGamedataString(GameData pGameConfig, const char[] szKey, char[] szVal, int len)
{
	if (!pGameConfig.GetKeyValue(szKey, szVal, len))
		SetFailState("Missing gamedata key: \"%s\"", szKey);
}

stock float GetGamedataFloat(GameData pGameConfig, const char[] szKey)
{
	char szVal[16]; float flVal;
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));

	if (!StringToFloatEx(szVal, flVal))
		SetFailState("Value for key: \"%s\" should be a float.", szKey);

	return flVal;
}

stock int GetGamedataInt(GameData pGameConfig, const char[] szKey)
{
	char szVal[16]; int iVal;
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));

	if (!StringToIntEx(szVal, iVal))
		SetFailState("Value for key: \"%s\" should be an int.", szKey);

	return iVal;
}

stock RGBA GetGamedataColor(GameData pGameConfig, const char[] szKey, const char[] delimiter = " ")
{
	char szVal[16]; char szColor[4][4];
	GetGamedataString(pGameConfig, szKey, szVal, sizeof(szVal));
	
	if (3 > ExplodeString(szVal, delimiter, szColor, sizeof(szColor), sizeof(szColor[])))
		SetFailState("Value for key: \"%s\" is not a valid color.", szKey);
	
	return RGBA(StringToInt(szColor[0]), StringToInt(szColor[1]), StringToInt(szColor[2]), StringToInt(szColor[3]));
}

stock Address GetInterface(GameData pGameConfig, const char[] szLibrary, const char[] szInterface, bool bExtractIfaceVersion = true)
{
	char szBuffer[64];
	FormatEx(szBuffer, sizeof(szBuffer), "CreateInterface:%s", szLibrary);

	StartPrepSDKCall(SDKCall_Static);
	if (!PrepSDKCall_SetFromConf(pGameConfig, SDKConf_Signature, szBuffer))
		SetFailState("Could not obtain game signature %s", szBuffer);

	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Pointer, VDECODE_FLAG_ALLOWNULL);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
	Handle pCreateIfaceCall = EndPrepSDKCall();
	if (!pCreateIfaceCall)
		SetFailState("Could not prep SDK call %s", szBuffer);

	Address pIfaceAddr;
	if (bExtractIfaceVersion)
	{
		GetGamedataString(pGameConfig, szInterface, szBuffer, sizeof(szBuffer));
		pIfaceAddr = SDKCall(pCreateIfaceCall, szBuffer, 0);
	}
	else
	{
		pIfaceAddr = SDKCall(pCreateIfaceCall, szInterface, 0);
	}
	
	pCreateIfaceCall.Close();

	return pIfaceAddr;
}