#pragma newdecls required
#pragma semicolon 1

// ----------------------------
// Convars
// ----------------------------
ConVar g_pConvarCoopTeam;
ConVar g_pConvarCoopRespawnTime;
ConVar g_pConvarStartWaitPeriod;
ConVar g_pConvarStartWaitMode;
ConVar g_pConvarEndWaitPeriod;
ConVar g_pConvarEndWaitFactor;
ConVar g_pConvarHomeMap;
ConVar g_pConvarEndWaitDisplayMode;
ConVar g_pConvarValidateSteamIds;
ConVar g_pConvarDefaultMapConfig;
ConVar g_pConvarDefaultMapConfigDest;

#if defined GAMEPATCH_TEAMSELECT_UI
ConVar g_pConvarDisableTeamSelect;
#endif

#if defined SRCCOOP_BLACKMESA
ConVar g_pConvarCleanHud;
#endif

// Existing ConVars.
ConVar mp_friendlyfire;
ConVar mp_flashlight;
ConVar mp_forcerespawn;

// Existing Black Mesa ConVars.

#if defined SRCCOOP_BLACKMESA
ConVar sv_always_run;
ConVar sv_speed_sprint;
ConVar sv_speed_walk;
ConVar sv_jump_long_enabled;
ConVar sv_long_jump_manacost;
#endif

// ----------------------------
// SDK Calls
// ----------------------------

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
DynamicHook hkUpdateOnRemove;
DynamicHook hkEvent_Killed;
DynamicHook hkKeyValue_char;

#if defined SRCCOOP_BLACKMESA
DynamicDetour hkGiveDefaultItems;
#endif

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

#if defined ENTPATCH_NPC_RUNTASK
DynamicHook hkBaseNpcRunTask;
#endif

#if defined ENTPATCH_PLAYER_ALLY
DynamicHook hkIsPlayerAlly;
#endif

#if defined ENTPATCH_SNIPER
DynamicHook hkProtoSniperSelectSchedule;
#endif

#if defined ENTPATCH_NPC_DIALOGUE
DynamicHook hkFindNamedEntity;
DynamicHook hkFindNamedEntityClosest;
DynamicDetour hkExpresserHostDoModifyOrAppendCriteria;
#endif

#if defined SRCCOOP_BLACKMESA
DynamicHook hkOnTryPickUp;
#endif

#if defined ENTPATCH_BM_ICHTHYOSAUR
DynamicHook hkIchthyosaurIdleSound;
#endif

#if defined ENTPATCH_FUNC_TRACKAUTOCHANGE || defined ENTPATCH_FUNC_TRACKTRAIN
DynamicHook hkBlocked;
#endif

#if defined PLAYERPATCH_RESTORE_MP_FORCERESPAWN
DynamicHook hkForceRespawn;
#endif

#if defined PLAYERPATCH_OVERRIDE_DEATH_OBSMODE
DynamicHook hkStartObserverMode;
#endif

#if defined PLAYERPATCH_HITREG
DynamicHook hkPlayerWeaponShootPosition;
#endif

#if defined ENTPATCH_BM_SP_WEAPONS
DynamicHook hkBaseCombatWeaponItemPostFrame;
DynamicHook hkBaseCombatWeaponDeploy;
DynamicHook hkBaseCombatWeaponPrimaryAttack;
DynamicHook hkBaseCombatWeaponGetPrimaryAttackActivity;
DynamicHook hkBaseCombatWeaponGetDrawActivity;
DynamicHook hkCrossbowFireBolt;
DynamicDetour hkToggleIronsights;
DynamicDetour hkTauFireBeam;
DynamicDetour hkParamsManagerInitInstances;
#endif

#if defined PLAYERPATCH_SUIT_SOUNDS
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

#if defined ENTPATCH_NPC_SLEEP
DynamicDetour hkBaseNpcUpdateSleepState;
#endif

#if defined ENTPATCH_NPC_THINK_LOCALPLAYER
DynamicDetour hkPhysics_RunThinkFunctions;
#endif

#if defined ENTPATCH_BM_DISSOLVE
DynamicDetour hkDissolve;
#endif

#if defined GAMEPATCH_UTIL_FINDCLIENT
DynamicDetour hkUtilFindClientInPVSGuts;
DynamicDetour hkUtilFindClientInPVS;
DynamicDetour hkUtilFindClientInVisibilityPVS;
#endif

#if defined ENTPATCH_SCRIPTED_SEQUENCE
DynamicDetour hkScriptedSequenceStartScript;
#endif

#if defined GAMEPATCH_PREDICTED_EFFECTS
DynamicDetour hkIgnorePredictionCull;
DynamicHook hkDispatchEffect;
#endif

#if defined ENTPATCH_BM_PROP_CHARGERS
DynamicHook hkPropChargerThink;
#endif

#if defined SRCCOOP_HL2DM && defined PLAYERPATCH_SERVERSIDE_RAGDOLLS
DynamicHook hkCreateRagdollEntity;
#endif

#if defined SRCCOOP_BLACKMESA
DynamicDetour hkAccumulatePose;
DynamicDetour hkTestGroundMove;
#endif

// ----------------------------
// MemPatches
// ----------------------------
ArrayList g_pCoopModeMemPatchList;

// ----------------------------
// Plugin variables
// ----------------------------
char g_szMapName[MAX_MAPNAME];
char g_szPrevMapName[MAX_MAPNAME];
char g_szEntityString[ENTITYSTRING_LENGTH];
char g_szSteamIds[MAXPLAYERS+1][32];
bool g_bTempDontHookEnts;
public bool g_bMapStarted;
int g_iPlayerCount;
bool g_bPostTeamSelect[MAXPLAYERS+1];
int g_iAddButtons[MAXPLAYERS+1];
int g_bUnblockRespawn[MAXPLAYERS+1];
SpawnOptions g_pSpawnOptions;
OperatingSystem g_serverOS;
FeatureMap g_pFeatureMap;

ArrayList g_pPostponedSpawns;

// Represents the return value of the IsMultiplayer hook.
bool g_bIsMultiplayerOverride = true;

// Counts the # of overrides to `gpGlobals->maxclients`.
int g_iMaxClientsOverrides = 0;

// The entity which UTIL_GetLocalPlayer hook will return the closest player to, alternatively containing the player index itself.
// Note: Real stack size is 1 less, because first entry starts at index 1.
CBaseEntity g_pLocalPlayerEntity[32] = {NULL_CBASEENTITY, ...};
int g_iLocalPlayerStackSkips[sizeof(g_pLocalPlayerEntity)];
int g_iLocalPlayerStackPointer;

// ----------------------------
// Plugin API
// ----------------------------
GlobalForward g_CoopMapStartFwd;
GlobalForward g_CoopMapConfigLoadedFwd;
GlobalForward g_OnPlayerRagdollCreatedFwd;
GlobalForward g_OnCoopMapEndFwd;