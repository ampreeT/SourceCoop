#pragma newdecls required
#pragma semicolon 1

void RegNatives()
{
	CreateNative("SC_GetCoopTopMenu", Native_GetCoopTopMenu);
	CreateNative("SC_IsCurrentMapCoop", Native_IsCurrentMapCoop);
	CreateNative("SC_Respawn", Native_Respawn);
	CreateNative("SC_GetSurvivalMode", Native_GetSurvivalMode);
	CreateNative("SC_TeleportToCurrentCheckpoint", Native_TeleportToCurrentCheckpoint);
	CreateNative("SC_IsCoopFeatureEnabled", Native_IsCoopFeatureEnabled);
	CreateNative("SC_FixWeaponPrediction", Native_FixWeaponPrediction);
}

public any Native_GetCoopTopMenu(Handle plugin, int numParams)
{
	return g_pCoopTopMenu;
}

public any Native_IsCurrentMapCoop(Handle plugin, int numParams)
{
	return g_pCoopManagerData.m_bIsCoopMap;
}

public any Native_Respawn(Handle plugin, int numParams)
{
	CBasePlayer pPlayer = GetNativeCell(1);
	if (!pPlayer.IsValid())
		ThrowNativeError(0, "Passed CBasePlayer is invalid!");
	
	if (!pPlayer.IsAlive() && pPlayer.GetTeam() != TEAM_SPECTATOR)
	{
		if (GetNativeArray(2, g_pSpawnOptions, sizeof(g_pSpawnOptions)) != SP_ERROR_NONE)
			ThrowNativeError(0, "SourcePawn error in spawn options");
		
		SurvivalManager.SetDead(pPlayer, false);
		return pPlayer.Spawn();
	}
	return false;
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
	if (CCoopSpawnSystem.GetCurrentCheckpoint(pCheckpoint))
	{
		return pCheckpoint.TeleportPlayer(pPlayer, true);
	}
	return false;
}

public any Native_IsCoopFeatureEnabled(Handle plugin, int numParams)
{
	return CoopManager.IsFeatureEnabled(GetNativeCell(1));
}

public any Native_FixWeaponPrediction(Handle plugin, int numParams)
{
	CBasePlayer pPlayer = GetNativeCell(1);
	if (!pPlayer.IsValid())
		ThrowNativeError(0, "Passed CBasePlayer is invalid!");
	if (!pPlayer.IsAlive())
		ThrowNativeError(0, "Passed CBasePlayer is dead!");
	
	return FixWeaponPrediction(pPlayer);
}