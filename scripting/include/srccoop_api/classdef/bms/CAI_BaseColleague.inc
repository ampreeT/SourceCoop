#pragma newdecls required
#pragma semicolon 1

static int g_iOffset_bIsProvoked = -1;

methodmap CAI_BaseColleague < CAI_BaseNPC // actually CNPC_PlayerCompanion
{
	public CAI_BaseColleague(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseColleague>(CAI_BaseNPC(iEntIndex));
	}
	public static void InitClassdef(const GameData hGameConfig)
	{
		g_iOffset_bIsProvoked = GetGamedataInt(hGameConfig, "CAI_BaseColleague::m_bProvoked");
	}

	public bool IsProvoked()
	{
		return LoadFromAddress(this.address + g_iOffset_bIsProvoked, NumberType_Int8);
	}
	public void SetProvoked(bool bIsProvoked)
	{
		StoreToAddress(this.address + g_iOffset_bIsProvoked, bIsProvoked, NumberType_Int8);
	}

}
