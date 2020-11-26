#if defined _srccoop_included
	#endinput
#endif
#define _srccoop_included

#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <regex>
#include <dhooks>
#include <topmenus>

#pragma newdecls required
#pragma semicolon 1

#include <logdebug>

#include <srccoop/typedef.inc>
#include <srccoop/typedef_game.inc>
#include <srccoop/features.inc>
#include <srccoop/classdef.inc>
#include <srccoop/utils.inc>
#include <srccoop_api>

#include <srccoop/globals.inc>
#include <srccoop/levellump.inc>
#include <srccoop/checkpoint.inc>
#include <srccoop/manager.inc>
#include <srccoop/entitypatch.inc>
#include <srccoop/playerpatch.inc>
#include <srccoop/instancing.inc>
#include <srccoop/menu.inc>


//------------------------------------------------------
// Natives
//------------------------------------------------------
public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	CreateNative("GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("IsCurrentMapCoop", Native_IsCurrentMapCoop);
	RegPluginLibrary(SRCCOOP_LIBRARY);
	return APLRes_Success;
}

public any Native_GetCoopTopMenu(Handle plugin, int numParams)
{
	return g_pCoopTopMenu;
}

public any Native_IsCurrentMapCoop(Handle plugin, int numParams)
{
	return g_pCoopManager.m_bIsCoopMap;
}
