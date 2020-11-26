#if defined _srccoop_api_included
	#endinput
#endif
#define _srccoop_api_included

#include <srccoop/typedef.inc>
#include <srccoop/typedef_game.inc>
#include <srccoop/classdef.inc>
#include <srccoop/utils.inc>

#include <topmenus>

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