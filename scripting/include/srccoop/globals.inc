#if defined _srccoop_globals_included
 #endinput
#endif
#define _srccoop_globals_included

#pragma newdecls required
#pragma semicolon 1

// ----------------------------
// Convars
// ----------------------------
ConVar g_pConvarCoopTeam;
ConVar g_pConvarCoopRespawnTime;
ConVar g_pConvarWaitPeriod;
ConVar g_pConvarEndWaitPeriod;
ConVar g_pConvarEndWaitFactor;
ConVar g_pConvarHomeMap;
ConVar g_pConvarEndWaitDisplayMode;

#if defined GAMEPATCH_TEAMSELECT_UI
ConVar g_pConvarDisableTeamSelect;
#endif

// Existing ConVars
ConVar mp_friendlyfire;
ConVar mp_flashlight;

#if defined SRCCOOP_BLACKMESA
ConVar sv_always_run;
#endif

// ----------------------------
// SDK Calls
// ----------------------------
Handle g_pCreateEngineInterface;
Handle g_pCreateServerInterface;

#if defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
Handle g_pCreateServerRagdoll;
#endif

// ----------------------------
// DHooks
// ----------------------------
DynamicHook hkLevelInit;
DynamicHook hkChangeTeam;
DynamicHook hkShouldCollide;
DynamicHook hkPlayerSpawn;
DynamicHook hkSetModel;
DynamicHook hkAcceptInput;
DynamicHook hkThink;
DynamicHook hkEvent_Killed;
DynamicHook hkKeyValue_char;

#if defined GAMEPATCH_ALLOW_FLASHLIGHT
DynamicHook hkFAllowFlashlight;
#endif

#if defined GAMEPATCH_IS_MULTIPLAYER
DynamicHook hkIsMultiplayer;
#endif

#if defined GAMEPATCH_BLOCK_RESTOREWORLD
DynamicHook hkRestoreWorld;
#endif

#if defined GAMEPATCH_BLOCK_RESPAWNPLAYERS
DynamicHook hkRespawnPlayers;
#endif

#if defined ENTPATCH_RELATION_TYPE
DynamicHook hkIRelationType;
#endif

#if defined ENTPATCH_PLAYER_ALLY
DynamicHook hkIsPlayerAlly;
#endif

#if defined ENTPATCH_SNIPER
DynamicHook hkProtoSniperSelectSchedule;
#endif

#if defined ENTPATCH_FIND_NAMED_ENTITY
DynamicHook hkFindNamedEntity;
DynamicHook hkFindNamedEntityClosest;
#endif

#if defined SRCCOOP_BLACKMESA
DynamicHook hkOnTryPickUp;
#endif

#if defined ENTPATCH_BM_ICHTHYOSAUR
DynamicHook hkIchthyosaurIdleSound;
#endif

#if defined ENTPATCH_BM_XENTURRET || defined ENTPATCH_BM_NIHILANTH
DynamicHook hkHandleAnimEvent;
#endif

#if defined ENTPATCH_BM_XENTURRET || defined ENTPATCH_BM_NIHILANTH || defined ENTPATCH_BM_GONARCH
DynamicHook hkRunAI;
#endif

#if defined ENTPATCH_BM_FUNC_TRACKAUTOCHANGE || defined ENTPATCH_BM_FUNC_TRACKTRAIN
DynamicHook hkBlocked;
#endif

#if defined PLAYERPATCH_SETSUITUPDATE
DynamicDetour hkSetSuitUpdate;
#endif

#if defined GAMEPATCH_UTIL_GETLOCALPLAYER
DynamicDetour hkUTIL_GetLocalPlayer;
#endif

#if defined ENTPATCH_GOALENTITY_RESOLVENAMES
DynamicDetour hkResolveNames;
#endif

#if defined ENTPATCH_GOAL_LEAD
DynamicDetour hkCanSelectSchedule;
#endif

#if defined PLAYERPATCH_PICKUP_FORCEPLAYERTODROPTHISOBJECT
DynamicDetour hkPickup_ForcePlayerToDropThisObject;
#endif

#if defined ENTPATCH_SETPLAYERAVOIDSTATE
DynamicDetour hkSetPlayerAvoidState;
#endif

#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
DynamicHook hkCreateRagdollEntity;
#endif

#if defined SRCCOOP_BLACKMESA
DynamicDetour hkAccumulatePose;
#endif

// ----------------------------
// Adresses, Interfaces
// ----------------------------
//Address g_VEngineServer;
IServerGameDLL g_ServerGameDLL;

// ----------------------------
// Plugin variables
// ----------------------------
char g_szMapName[MAX_MAPNAME];
char g_szPrevMapName[MAX_MAPNAME];
char g_szEntityString[ENTITYSTRING_LENGTH];
char g_szSteamIds[MAXPLAYERS+1][32];
bool g_bTempDontHookEnts;
bool g_bMapStarted;
bool g_bPostTeamSelect[MAXPLAYERS+1];
OperatingSystem g_serverOS;
FeatureMap g_pFeatureMap;

ArrayList g_pPostponedSpawns;

// represents the return value of the IsMultiplayer hook
bool g_bIsMultiplayerOverride = true;

// the entity which UTIL_GetLocalPlayer hook will return the closest player to, alternatively containing the player index itself
CBaseEntity g_pLocalPlayerEntity = view_as<CBaseEntity>(-1);

// ----------------------------
// Plugin API
// ----------------------------
GlobalForward g_CoopMapStartFwd;
GlobalForward g_CoopMapConfigLoadedFwd;
GlobalForward g_OnPlayerRagdollCreatedFwd;