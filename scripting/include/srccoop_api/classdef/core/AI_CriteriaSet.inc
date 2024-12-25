#pragma newdecls required
#pragma semicolon 1

static Handle g_pAppendCriteria;

methodmap AI_CriteriaSet
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szAppendCriteria[] = "AI_CriteriaSet::AppendCriteria";
		StartPrepSDKCall(SDKCall_Raw);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szAppendCriteria))
			LogMessage("Could not obtain gamedata signature %s", szAppendCriteria);
		else
		{
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
			if (!(g_pAppendCriteria = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szAppendCriteria);
		}
	}
	public AI_CriteriaSet(const Address pAddress)
	{
		return view_as<AI_CriteriaSet>(pAddress);
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public void AppendCriteria(const char[] szCriteria, const char[] szValue, const float flWeight = 1.0)
	{
		SDKCall(g_pAppendCriteria, this.GetAddress(), szCriteria, szValue, flWeight);
	}
}
