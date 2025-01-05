#pragma newdecls required
#pragma semicolon 1

enum NPC_STATE
{
	NPC_STATE_INVALID = -1,
	NPC_STATE_NONE = 0,
	NPC_STATE_IDLE,
	NPC_STATE_ALERT,
	NPC_STATE_COMBAT,
	NPC_STATE_SCRIPT,
	NPC_STATE_PLAYDEAD,
	NPC_STATE_PRONE,				// When in clutches of barnacle
	NPC_STATE_DEAD
}
