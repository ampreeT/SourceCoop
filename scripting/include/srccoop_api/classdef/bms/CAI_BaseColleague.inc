#pragma newdecls required
#pragma semicolon 1

methodmap CAI_BaseColleague < CAI_BaseNPC // actually CNPC_PlayerCompanion
{
	public CAI_BaseColleague(const int iEntIndex = -1)
	{
		return view_as<CAI_BaseColleague>(CAI_BaseNPC(iEntIndex));
	}
}
