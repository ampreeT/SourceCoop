#if defined _srccoop_included
	#endinput
#endif
#define _srccoop_included

#if defined SRCCOOP_CUSTOM_COMPILE
	// custom compile predefined games
#else
	// compile all games by default, they are still checked at runtime
	#define SRCCOOP_BLACKMESA
#endif

#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <regex>
#include <dhooks>
#include <topmenus>

#pragma newdecls required
#pragma semicolon 1

#include <logdebug>

#include <srccoop/typedef>
#include <srccoop/typedef_game>
#include <srccoop/features>
#include <srccoop/classdef>
#if defined SRCCOOP_BLACKMESA
#include <srccoop/blackmesa/typedef>
#include <srccoop/blackmesa/classdef>
#endif
#include <srccoop/utils>
#include <srccoop_api>

#include <srccoop/globals>
#include <srccoop/kv_shared>
#include <srccoop/manager_changelevel>
#include <srccoop/manager>
#include <srccoop/manager_survival>
#include <srccoop/manager_persistence>
#include <srccoop/entitypatch>
#include <srccoop/blackmesa/entitypatch>
#include <srccoop/instancing>
#include <srccoop/playerpatch>
#include <srccoop/menu>
#include <srccoop/commands>
#include <srccoop/natives>