#pragma newdecls required
#pragma semicolon 1

/**
 * The player state is an embedded class inside of player that is shared with the engine.
 * The Engine uses some of this data for responding to external server queries.
 */
methodmap CPlayerState
{
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public int GetScore()
	{
		return LoadFromAddress(this.GetAddress() + g_iCPlayerState_frags, NumberType_Int32);
	}
	public void SetScore(int iScore)
	{
		StoreToAddress(this.GetAddress() + g_iCPlayerState_frags, iScore, NumberType_Int32);
	}
}
