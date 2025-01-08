#pragma newdecls required
#pragma semicolon 1

enum AI_Efficiency_t
{
	// Run at full tilt
	AIE_NORMAL,

	// Run decision process less often
	AIE_EFFICIENT,

	// Run decision process even less often, ignore other NPCs
	AIE_VERY_EFFICIENT,

	// Run decision process even less often, ignore other NPCs
	AIE_SUPER_EFFICIENT,

	// Don't run at all
	AIE_DORMANT,
}
