#pragma newdecls required
#pragma semicolon 1

static int g_iCPlayerState_frags = -1;

/**
 * The player state is an embedded class inside of player that is shared with the engine.
 * The Engine uses some of this data for responding to external server queries.
 */
methodmap CPlayerState
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		g_iCPlayerState_frags = hGameConfig.GetOffset("CPlayerState::frags");
		if (g_iCPlayerState_frags < 0)
			SetFailState("Invalid offset CPlayerState::frags");
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public int GetScore()
	{
		return LoadFromAddress(this.GetAddress() + g_iCPlayerState_frags, NumberType_Int32);
	}
	public void SetScore(const int iScore)
	{
		StoreToAddress(this.GetAddress() + g_iCPlayerState_frags, iScore, NumberType_Int32);
	}
}
