#pragma newdecls required
#pragma semicolon 1

// =======================================
//  Movement goals 
//	Used both to store the current movment goal in m_routeGoalType and to or/and with route
// =======================================
enum GoalType_t
{
	GOALTYPE_NONE,
	GOALTYPE_TARGETENT,
	GOALTYPE_ENEMY,
	GOALTYPE_PATHCORNER,
	GOALTYPE_LOCATION,
	GOALTYPE_LOCATION_NEAREST_NODE,
	GOALTYPE_FLANK,
	GOALTYPE_COVER,
	
	GOALTYPE_INVALID
}
