#pragma newdecls required
#pragma semicolon 1

static Handle g_pSendViewModelMatchingSequence;

methodmap CBaseViewModel < CBaseAnimating
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szSendViewModelMatchingSequence[] = "CBaseViewModel::SendViewModelMatchingSequence";
		StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szSendViewModelMatchingSequence))
			LogMessage("Could not obtain gamedata offset %s", szSendViewModelMatchingSequence);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pSendViewModelMatchingSequence = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szSendViewModelMatchingSequence);
		}
	}
	public CBaseViewModel(const int iEntIndex = -1)
	{
		return view_as<CBaseViewModel>(CBaseAnimating(iEntIndex));
	}

	public void SendViewModelMatchingSequence(const int iSequence)
	{
		SDKCall(g_pSendViewModelMatchingSequence, this.entindex, iSequence);
	}
}
