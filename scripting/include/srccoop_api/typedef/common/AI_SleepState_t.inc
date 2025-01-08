#pragma newdecls required
#pragma semicolon 1

enum AI_SleepState_t
{
	AISS_AWAKE,
	AISS_WAITING_FOR_THREAT,
	AISS_WAITING_FOR_PVS,
	AISS_WAITING_FOR_INPUT,
	AISS_AUTO_PVS,
	AISS_AUTO_PVS_AFTER_PVS, // Same as AUTO_PVS, except doesn't activate until/unless the NPC is IN the player's PVS. 
}
