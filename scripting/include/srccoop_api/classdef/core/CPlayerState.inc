#pragma newdecls required
#pragma semicolon 1

/**
 * The player state is an embedded class inside of player that is shared with the engine.
 * The Engine uses some of this data for responding to external server queries.
 */
methodmap CPlayerState
{
	public int GetScore()
	{
		return LoadFromAddress(view_as<Address>(this) + g_iCPlayerState_frags, NumberType_Int32);
	}
	public void SetScore(int iScore)
	{
		StoreToAddress(view_as<Address>(this) + g_iCPlayerState_frags, iScore, NumberType_Int32);
	}
}