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
#include <srccoop/kv_shared.inc>
#include <srccoop/manager.inc>
#include <srccoop/entitypatch.inc>
#include <srccoop/instancing.inc>
#include <srccoop/playerpatch.inc>
#include <srccoop/menu.inc>


//------------------------------------------------------
// Natives
//------------------------------------------------------
public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	CreateNative("GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SetPlayerCanSpawn", Native_SetPlayerCanSpawn);
	CreateNative("PlayerCanSpawn", Native_PlayerCanSpawn);
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

public any Native_SetPlayerCanSpawn(Handle plugin, int numParams)
{
	if (numParams < 1) ThrowNativeError(0,"You must pass a CBasePlayer as first parameter!");
	
	CBasePlayer pPlayer = CBasePlayer(GetNativeCell(1));
	if (!pPlayer.IsValid())
	{
		return 0;
	}
	
	if (numParams > 1) g_bPlayerCanSpawn[pPlayer.GetEntIndex()] = view_as<bool>(GetNativeCell(2));
	else g_bPlayerCanSpawn[pPlayer.GetEntIndex()] = true;
	
	if (g_bPlayerCanSpawn[pPlayer.GetEntIndex()])
	{
		char szSteamID[32];
		GetClientAuthId(pPlayer.GetEntIndex(), AuthId_Steam2, szSteamID, sizeof(szSteamID));
		int iFindInArray = g_pDeadPlayerIDs.FindString(szSteamID);
		if (iFindInArray != -1) g_pDeadPlayerIDs.Erase(iFindInArray);
	}
	
	return 0;
}

public any Native_PlayerCanSpawn(Handle plugin, int numParams)
{
	if (numParams < 1) ThrowNativeError(0,"You must pass a CBasePlayer as first parameter!");
	
	CBasePlayer pPlayer = CBasePlayer(GetNativeCell(1));
	if (!pPlayer.IsValid())
	{
		ThrowNativeError(0,"Passed invalid CBasePlayer to CanSpawn %i!", pPlayer.GetEntIndex());
	}
	
	return g_bPlayerCanSpawn[pPlayer.GetEntIndex()];
}