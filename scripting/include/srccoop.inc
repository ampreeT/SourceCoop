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

#include <srccoop/typedef>
#include <srccoop/typedef_game>
#include <srccoop/features>
#include <srccoop/classdef>
#include <srccoop/utils>
#include <srccoop_api>

#include <srccoop/globals>
#include <srccoop/kv_shared>
#include <srccoop/manager_changelevel>
#include <srccoop/manager>
#include <srccoop/manager_survival>
#include <srccoop/manager_persistence>
#include <srccoop/entitypatch>
#include <srccoop/instancing>
#include <srccoop/playerpatch>
#include <srccoop/menu>


//------------------------------------------------------
// Natives
//------------------------------------------------------
public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	CreateNative("GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SurvivalRespawn", Native_SurvivalRespawn);
	CreateNative("GetSurvivalMode", Native_GetSurvivalMode);
	RegPluginLibrary(SRCCOOP_LIBRARY);
	return APLRes_Success;
}

public any Native_GetCoopTopMenu(Handle plugin, int numParams)
{
	return g_pCoopTopMenu;
}

public any Native_IsCurrentMapCoop(Handle plugin, int numParams)
{
	return g_pCoopManagerData.m_bIsCoopMap;
}

public any Native_SurvivalRespawn(Handle plugin, int numParams)
{
	CBasePlayer pPlayer = GetNativeCell(1);
	if (!pPlayer.IsValid())
		ThrowNativeError(0, "Passed CBasePlayer is invalid!");
	return SurvivalManager.RespawnPlayer(pPlayer);
}

public any Native_GetSurvivalMode(Handle plugin, int numParams)
{
	return SurvivalManager.GetMode();
}