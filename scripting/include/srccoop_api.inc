#if defined _srccoop_api_included
	#endinput
#endif
#define _srccoop_api_included

#include <srccoop/typedef.inc>
#include <srccoop/classdef.inc>
#include <srccoop/utils.inc>

#include <topmenus>

/* The library name registered by the core plugin */
#define SRCCOOP_LIBRARY "SRCCOOP"

/* Predefined TopMenu category for player settings */
#define COOPMENU_CATEGORY_SETTINGS "Player settings"

/* Predefined TopMenu category for sound settings */
#define COOPMENU_CATEGORY_SOUND_SETTINGS "Sound settings"

/**
 * Retrieves the handle to the coop TopMenu.
 *
 * @return              Handle to the TopMenu,
 */
native TopMenu GetCoopTopMenu();
