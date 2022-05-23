#if defined _srccoop_globals_included
 #endinput
#endif
#define _srccoop_globals_included

// ----------------------------
// Convars
// ----------------------------
ConVar g_pConvarCoopTeam;
ConVar g_pConvarDisableTeamSelect;
ConVar g_pConvarCoopRespawnTime;
ConVar g_pConvarWaitPeriod;
ConVar g_pConvarEndWaitPeriod;
ConVar g_pConvarEndWaitFactor;
ConVar g_pConvarHomeMap;
ConVar g_pConvarEndWaitDisplayMode;
ConVar g_pConVarNextStuck;

// Existing ConVars
ConVar mp_friendlyfire;
ConVar mp_flashlight;
ConVar sv_always_run;

// ----------------------------
// SDK Calls
// ----------------------------
Handle g_pCreateEngineInterface;
Handle g_pCreateServerInterface;
Handle g_pCreateServerRagdoll;

// ----------------------------
// DHooks
// ----------------------------
DynamicHook hkFAllowFlashlight;
DynamicHook hkIsMultiplayer;
DynamicHook hkPlayerSpawn;
DynamicHook hkRestoreWorld;
DynamicHook hkRespawnPlayers;
DynamicHook hkIRelationType;
DynamicHook hkIsPlayerAlly;
DynamicHook hkProtoSniperSelectSchedule;
DynamicHook hkFindNamedEntity;
DynamicHook hkFindNamedEntityClosest;
DynamicHook hkSetModel;
DynamicHook hkAcceptInput;
DynamicHook hkOnTryPickUp;
DynamicHook hkThink;
DynamicHook hkChangeTeam;
DynamicHook hkShouldCollide;
DynamicHook hkIchthyosaurIdleSound;
DynamicHook hkHandleAnimEvent;
DynamicHook hkRunAI;
// DynamicHook hkOnTakeDamage;
DynamicHook hkLevelInit;
DynamicHook hkEvent_Killed;
DynamicHook hkKeyValue_char;
DynamicDetour hkSetSuitUpdate;
DynamicDetour hkUTIL_GetLocalPlayer;
DynamicDetour hkResolveNames;
DynamicDetour hkCanSelectSchedule;
DynamicDetour hkPickup_ForcePlayerToDropThisObject;
DynamicDetour hkSetPlayerAvoidState;

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
float g_flNextStuck[MAXPLAYERS+1];
bool g_bPostTeamSelect[MAXPLAYERS+1];
EngineVersion g_Engine;
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