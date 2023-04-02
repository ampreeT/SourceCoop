#if defined _srccoop_natives_included
 #endinput
#endif
#define _srccoop_natives_included

#pragma newdecls required
#pragma semicolon 1

void RegNatives()
{
	CreateNative("GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SurvivalRespawn", Native_SurvivalRespawn);
	CreateNative("GetSurvivalMode", Native_GetSurvivalMode);
	CreateNative("TeleportToCurrentCheckpoint", Native_TeleportToCurrentCheckpoint);
	CreateNative("IsCoopFeatureEnabled", Native_IsCoopFeatureEnabled);
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

public any Native_TeleportToCurrentCheckpoint(Handle plugin, int numParams)
{
	CBasePlayer pPlayer = GetNativeCell(1);
	if (!pPlayer.IsValid())
		ThrowNativeError(0, "Passed CBasePlayer is invalid!");

	CCoopSpawnEntry pCheckpoint;
	if (g_SpawnSystem.GetCurrentCheckpoint(pCheckpoint))
	{
		return pCheckpoint.TeleportPlayer(pPlayer, true);
	}
	return false;
}

public any Native_IsCoopFeatureEnabled(Handle plugin, int numParams)
{
	return CoopManager.IsFeatureEnabled(GetNativeCell(1));
}
