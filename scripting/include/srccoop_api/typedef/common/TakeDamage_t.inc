#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Settings for m_takedamage
//------------------------------------------------------
enum TakeDamage_t
{
	DAMAGE_NO = 0,
	DAMAGE_EVENTS_ONLY = 1,	// Call damage functions, but don't modify health
	DAMAGE_YES = 2,
	DAMAGE_AIM = 3,
}
