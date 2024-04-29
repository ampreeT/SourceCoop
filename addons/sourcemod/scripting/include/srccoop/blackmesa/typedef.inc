#if defined _srccoop_typedef_bm_included
 #endinput
#endif
#define _srccoop_typedef_bm_included

#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Gamerules gamestates
//------------------------------------------------------
#define STATE_WARMUP "m_StateWarmup"
#define STATE_ROUND "m_StateRound"
#define STATE_INTERMISSION "m_StateIntermission"

enum
{
	STATE_WARMUP_INDEX = 0,
	STATE_INTERMISSION_INDEX,
	STATE_ROUND_INDEX
}

enum
{
	STATE_ELEMENT_DONE_TIME = 0,
	STATE_ELEMENT_NEXT_STATE,
	STATE_ELEMENT_IS_IN_INTERMISSION
}
