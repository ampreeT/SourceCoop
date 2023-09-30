#if defined _srccoop_natives_included
 #endinput
#endif
#define _srccoop_natives_included

#pragma newdecls required
#pragma semicolon 1

void RegNatives()
{
	CreateNative("SC_GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("SC_IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SC_SurvivalRespawn", Native_SurvivalRespawn);
	CreateNative("SC_GetSurvivalMode", Native_GetSurvivalMode);
	CreateNative("SC_TeleportToCurrentCheckpoint", Native_TeleportToCurrentCheckpoint);
	CreateNative("SC_IsCoopFeatureEnabled", Native_IsCoopFeatureEnabled);
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
