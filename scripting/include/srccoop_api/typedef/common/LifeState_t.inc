#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// m_lifeState values
//------------------------------------------------------
enum LifeState_t
{
	LIFE_ALIVE = 0,			// alive
	LIFE_DYING = 1,			// playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD = 2,			// dead. lying still.
	LIFE_RESPAWNABLE = 3,
	LIFE_DISCARDBODY = 4,
}
