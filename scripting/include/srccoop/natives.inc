#if defined _srccoop_natives_included
 #endinput
#endif
#define _srccoop_natives_included


public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	CreateNative("GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SurvivalRespawn", Native_SurvivalRespawn);
	CreateNative("GetSurvivalMode", Native_GetSurvivalMode);
	CreateNative("TeleportToCurrentCheckpoint", Native_TeleportToCurrentCheckpoint);
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