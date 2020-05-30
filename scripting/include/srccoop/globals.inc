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
//Handle hkIsDeathmatch;
Handle hkIsMultiplayer;
Handle hkIRelationType;
Handle hkIsPlayerAlly;
Handle hkProtoSniperSelectSchedule;
Handle hkFindNamedEntity;
Handle hkFindNamedEntityClosest;
Handle hkSetModel;
Handle hkAcceptInput;
//Handle hkSetSuitUpdate;
Handle hkOnTryPickUp;
Handle hkThink;
Handle hkChangeTeam;
Handle hkShouldCollide;
Handle hkUTIL_GetLocalPlayer;
Handle hkResolveNames;

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
bool g_bTempDontHookEnts;
bool g_bIsMultiplayerOverride = true;
bool g_bMapStarted;

ArrayList g_pPostponedSpawns;