// Convars
ConVar g_pConvarCoopEnabled;
ConVar g_pConvarWaitPeriod;

// SDK Calls
Handle g_pCreateEngineInterface;
Handle g_pCreateServerInterface;

Handle g_pPickupObject;
Handle g_pSendWeaponAnim;
Handle g_pGameShutdown;

// DHooks
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
Handle hkSetSuitUpdate;

// Adresses
//Address g_VEngineServer;
Address g_ServerGameDLL;

// Plugin variables
char g_szMapName[MAX_MAPNAME];
bool g_bTempDontHookEnts;
bool g_bIsMultiplayerOverride = true;