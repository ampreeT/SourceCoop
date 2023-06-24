#if defined _srccoop_api_included
	#endinput
#endif
#define _srccoop_api_included

#pragma newdecls required
#pragma semicolon 1

#if !defined SRCCOOP_CUSTOM_COMPILE
	#if defined SRCCOOP_HL2DM
	#else
		// SRCCOOP_BLACKMESA is the fallback if no other definition is passed
		#define SRCCOOP_BLACKMESA
	#endif
#endif


#include <topmenus>

#include <srccoop/typedef>
#include <srccoop/typedef_game>
#include <srccoop/classdef>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/blackmesa/typedef>
#include <srccoop/blackmesa/classdef>
#endif
#include <srccoop/utils>
#include <srccoop/features>

#define SRCCOOP_VERSION "1.2"
#define SRCCOOP_URL "https://github.com/ampreeT/SourceCoop"

/* The library name registered by the core plugin */
#define SRCCOOP_LIBRARY "SRCCOOP"

/* Predefined TopMenu category for player settings */
#define COOPMENU_CATEGORY_PLAYER "PlayerSettings"

/* Predefined TopMenu category for sound settings */
#define COOPMENU_CATEGORY_SOUNDS "SoundSettings"

/* Predefined TopMenu category for voting actions */
#define COOPMENU_CATEGORY_VOTING "Voting"

/* Predefined TopMenu category for other settings */
#define COOPMENU_CATEGORY_OTHER "Other"

stock void InitSourceCoopAddon()
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_GAMEDATA_NAME);
	if (pGameConfig == null)
	{
		SetFailState("Couldn't load game config %s", SRCCOOP_GAMEDATA_NAME);
	}
	InitClassdef(pGameConfig);
	CloseHandle(pGameConfig);
}

/**
 * Global callback after map config is loaded in OnMapStart.
 *
 * @param kv          The configuration file imported into keyvalues. You must call kv.Rewind() first to ensure consistency.
 * @param location    From where the config was loaded.
 */
forward void OnCoopMapConfigLoaded(KeyValues kv, CoopConfigLocation location);

/**
 * Global callback for when a coop map actually starts,
 * that is potentially after the player wait timer completes.
 */
forward void OnCoopMapStart();

/**
 * Called when the core plugin creates server-side player ragoll on death.
 * The timing of which can vary by game.
 */
forward void OnPlayerRagdollCreated(CBasePlayer pPlayer, CBaseAnimating pRagdoll);

/**
 * Retrieves the handle to the coop TopMenu.
 *
 * @return              Handle to the TopMenu
 */
native TopMenu GetCoopTopMenu();

/**
 * Returns whether current map is a coop map.
 *
 */
native bool IsCurrentMapCoop();

/**
 * Respawns a dead player.
 * @return    true if player was dead and respawned
 * @error     Invalid CBasePlayer
 */
native bool SurvivalRespawn(CBasePlayer pPlayer);

/**
 * Gets the survival mode.
 * 
 * @return    Current survival mode (enum)
 */
native SurvivalMode GetSurvivalMode();

/**
 * Teleports a player to active checkpoint.
 * 
 * @error     Invalid CBasePlayer
 * @return    true for success, false if there is no checkpoint
 */
native bool TeleportToCurrentCheckpoint(CBasePlayer pPlayer);

/**
 * Checks if a SourceCoop feature is enabled.
 */
native bool IsCoopFeatureEnabled(SourceCoopFeature iFeatureBits);