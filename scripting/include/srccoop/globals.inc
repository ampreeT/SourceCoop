#if defined _srccoop_globals_included
 #endinput
#endif
#define _srccoop_globals_included

// ----------------------------
// Convars
// ----------------------------
ConVar g_pConvarCoopEnabled;
ConVar g_pConvarCoopTeam;
ConVar g_pConvarCoopRespawnTime;
ConVar g_pConvarWaitPeriod;
ConVar g_pConvarEndWaitPeriod;
ConVar g_pConvarEndWaitFactor;

// ----------------------------
// SDK Calls
// ----------------------------
Handle g_pCreateEngineInterface;
Handle g_pCreateServerInterface;

//Handle g_pPickupObject;
//Handle g_pSendWeaponAnim;

// ----------------------------
// DHooks
// ----------------------------
Handle hkFAllowFlashlight;
Handle hkIsMultiplayer;
Handle hkRestoreWorld;
Handle hkRespawnPlayers;
Handle hkIRelationType;
Handle hkIsPlayerAlly;
Handle hkProtoSniperSelectSchedule;
Handle hkFindNamedEntity;
Handle hkFindNamedEntityClosest;
Handle hkSetModel;
Handle hkAcceptInput;
Handle hkSetSuitUpdate;
Handle hkOnTryPickUp;
Handle hkThink;
Handle hkChangeTeam;
Handle hkShouldCollide;
Handle hkIchthyosaurIdleSound;
Handle hkUTIL_GetLocalPlayer;
Handle hkResolveNames;
Handle hkCanSelectSchedule;
Handle hkPickup_ForcePlayerToDropThisObject;
Handle hkHandleAnimEvent;
Handle hkRunAI;
//Handle hkSetPlayerAvoidState;

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
bool g_bTempDontHookEnts;
bool g_bMapStarted;
EngineVersion g_Engine;
OperatingSystem g_serverOS;
FeatureMap g_pFeatureMap;
int g_iEnabledFeatures;

ArrayList g_pPostponedSpawns;

// represents the return value of the IsMultiplayer hook
bool g_bIsMultiplayerOverride = true; 

// the entity which UTIL_GetLocalPlayer hook will return the closest player to
CBaseEntity g_pLocalPlayerCallingEntity = view_as<CBaseEntity>(-1);

// ----------------------------
// Plugin API
// ----------------------------
GlobalForward g_CoopMapStartFwd;