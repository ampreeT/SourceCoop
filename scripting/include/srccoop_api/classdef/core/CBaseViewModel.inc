#pragma newdecls required
#pragma semicolon 1

methodmap CBaseViewModel < CBaseAnimating
{
	public CBaseViewModel(const int iEntIndex = -1)
	{
		return view_as<CBaseViewModel>(CBaseAnimating(iEntIndex));
	}

	public void SendViewModelMatchingSequence(const int iSequence)
	{
		SDKCall(g_pSendViewModelMatchingSequence, this.GetEntIndex(), iSequence);
	}
}
