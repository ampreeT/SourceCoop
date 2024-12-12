#pragma newdecls required
#pragma semicolon 1

methodmap AI_CriteriaSet
{
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
