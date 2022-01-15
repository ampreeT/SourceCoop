#if defined _srccoop_api_included
	#endinput
#endif
#define _srccoop_api_included

#include <topmenus>

#include <srccoop/typedef.inc>
#include <srccoop/typedef_game.inc>
#include <srccoop/classdef.inc>
#include <srccoop/utils.inc>

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
 * Global callback for when a coop map actually starts,
 * that is potentially after the player wait timer completes.
 */
forward void OnCoopMapStart();

/**
 * Allows other sources or plugins set whether a player can spawn
 * 
 */
native void PlayerSetCanSpawn(CBasePlayer pPlayer, bool bSetCanSpawn = true);

/**
 * Allows other sources or plugins check whether a player can spawn
 * 
 */
native bool PlayerCanSpawn(CBasePlayer pPlayer);