#pragma newdecls required
#pragma semicolon 1

public Action Command_SetFeature(int iClient, int iArgs)
{
	if (iArgs == 0)
	{
		MsgReply(iClient, "------------------------------");
		StringMapSnapshot snapshot = g_pFeatureMap.Snapshot();
		for (int i = 0; i < snapshot.Length; i++)
		{
			int len = snapshot.KeyBufferSize(i);
			char[] szKey = new char[len];
			snapshot.GetKey(i, szKey, len);
			SourceCoopFeature iFeature;
			g_pFeatureMap.GetValue(szKey, iFeature);
			MsgReply(iClient, "%s : %t", szKey, CoopManager.IsFeatureEnabled(iFeature)? "On" : "Off");
		}
		MsgReply(iClient, "------------------------------");
		snapshot.Close();
	}
	else if (iArgs == 2)
	{
		char szFeature[MAX_FORMAT];
		GetCmdArg(1, szFeature, sizeof(szFeature));
		
		SourceCoopFeature feature;
		if (g_pFeatureMap.GetFeature(szFeature, feature))
		{
			char szVal[MAX_FORMAT];
			GetCmdArg(2, szVal, sizeof(szVal));
			
			if (StrEqual(szVal, "1") || IsEnableSynonym(szVal))
			{
				CoopManager.EnableFeature(feature);
				MsgReply(iClient, "%t", "feature enabled", szFeature);
			}
			else if (StrEqual(szVal, "0") || IsDisableSynonym(szVal))
			{
				CoopManager.DisableFeature(feature);
				MsgReply(iClient, "%t", "feature disabled", szFeature);
			}
		}
		else
		{
			MsgReply(iClient, "%t", "feature unknown", szFeature);
		}
		return Plugin_Handled;
	}
	
	MsgReply(iClient, "Format: sourcecoop_ft [<FEATURE> <1/0>]");
	return Plugin_Handled;
}

public Action Command_DumpMapEntities(int iArgs)
{
	if (g_szEntityString[0] == '\0')
	{
		MsgSrv("No entity data recorded for current map.");
		return Plugin_Handled;
	}
	
	char szDumpPath[PLATFORM_MAX_PATH];
	char szTime[128];
	FormatTime(szTime, sizeof(szTime), "%Y-%m-%d-%H%M%S");
	BuildPath(Path_SM, szDumpPath, sizeof(szDumpPath), "data/srccoop/dumps");
	CreateDirectory(szDumpPath, FPERM_U_READ|FPERM_U_WRITE|FPERM_U_EXEC|FPERM_G_READ|FPERM_G_WRITE|FPERM_G_EXEC|FPERM_O_READ|FPERM_O_EXEC);
	Format(szDumpPath, sizeof(szDumpPath), "%s/%s-%s.txt", szDumpPath, g_szMapName, szTime);
	
	File pDumpFile = OpenFile(szDumpPath, "w");
	if (pDumpFile != null)
	{
		pDumpFile.WriteString(g_szEntityString, false);
		CloseHandle(pDumpFile);
		MsgSrv("Dumped map entities in %s", szDumpPath);
	}
	else
	{
		MsgSrv("Failed opening file for writing: %s", szDumpPath);
	}
	return Plugin_Handled;
}

bool VerifyDefaultMapConfigCvars()
{
	char szBuffer[PLATFORM_MAX_PATH];

	g_pConvarDefaultMapConfig.GetString(szBuffer, 2);
	if (szBuffer[0] == EOS)
	{
		g_pConvarDefaultMapConfig.GetName(szBuffer, sizeof(szBuffer));
		MsgSrv("Set \"%s\" before running this command.", szBuffer);
		return false;
	}

	g_pConvarDefaultMapConfigDest.GetString(szBuffer, 2);
	if (szBuffer[0] == EOS)
	{
		g_pConvarDefaultMapConfigDest.GetName(szBuffer, sizeof(szBuffer));
		MsgSrv("Set \"%s\" before running this command.", szBuffer);
		return false;
	}

	return true;
}

public Action Command_MakeConfigs(int iArgs)
{
	if (iArgs < 1)
	{
		char szCmd[32]; GetCmdArg(0, szCmd, sizeof(szCmd));
		ServerCommand("help %s", szCmd);
		return Plugin_Handled;
	}

	if (!VerifyDefaultMapConfigCvars())
	{
		return Plugin_Handled;
	}

	DirectoryListing dir = OpenDirectory("maps", true);
	if (!dir)
	{
		MsgSrv("Unable to enumerate maps folder.");
		return Plugin_Handled;
	}

	char szBuffer[PLATFORM_MAX_PATH];
	char szMapFilter[MAX_MAPNAME * 2];
	GetCmdArg(1, szMapFilter, sizeof(szMapFilter));
	bool bDryRun = !GetCmdArgInt(2);
	int count;
	FileType ft;
	CoopConfigLocation ccl;
	char szSrcCoopConfig[PLATFORM_MAX_PATH];

	while (dir.GetNext(szBuffer, sizeof(szBuffer), ft))
	{
		int len = strlen(szBuffer); 
		if (ft == FileType_File && StrEndsWithEx(szBuffer, len, ".bsp", false))
		{
			szBuffer[len - 4] = EOS;

			if (!StrEqualsWildcard(szMapFilter, szBuffer, false))
				continue;

			if (!CoopManager.GetMapConfig(szBuffer, szSrcCoopConfig, ccl, false))
			{
				if (!bDryRun)
				{
					if (!CoopManager.CreateDefaultConfig(szBuffer, szSrcCoopConfig))
					{
						dir.Close();
						MsgSrv("Aborted processing.");
						return Plugin_Handled;
					}
				}
				count++;
			}
		}
	}
	dir.Close();
	
	if (bDryRun)
	{
		char szTemplate[PLATFORM_MAX_PATH], szDest[PLATFORM_MAX_PATH];
		g_pConvarDefaultMapConfig.GetString(szTemplate, sizeof(szTemplate));
		g_pConvarDefaultMapConfigDest.GetString(szDest, sizeof(szDest));
		GetCmdArg(0, szBuffer, sizeof(szBuffer));

		PrintToServer("\nYou are about to create %d new map configs.\n Template: %s\n Destination dir: %s"
				   ..."\nExecute the following command to continue:\n>>> %s %s 1 <<<\n",
				count, szTemplate, szDest,
				szBuffer, szMapFilter);
	}
	else
	{
		if (count)
			MsgSrv("Success. Created %d configs.", count);
		else
			MsgSrv("No new configs were created!");
	}
	return Plugin_Handled;
}

public Action Command_ImportConfigs(int iArgs)
{
	if (iArgs < 4)
	{
		char szCmd[32]; GetCmdArg(0, szCmd, sizeof(szCmd));
		ServerCommand("help %s", szCmd);
		return Plugin_Handled;
	}

	char szType[32];
	char szMapFilter[MAX_MAPNAME * 2];
	GetCmdArg(1, szType, sizeof(szType));
	GetCmdArg(2, szMapFilter, sizeof(szMapFilter));
	bool bUpdateConfigs = !!GetCmdArgInt(3);
	bool bCreateConfigs = !!GetCmdArgInt(4);
	bool bDryRun = !GetCmdArgInt(5);

	if (bCreateConfigs && !VerifyDefaultMapConfigCvars())
	{
		return Plugin_Handled;
	}

	ImportSource src;
	if (StrEqual(szType, "stripper", false))
	{
		src = IMPORT_STRIPPER;
	}
	else
	{
		MsgSrv("Unsupported import type: \"%s\"", szType);
		return Plugin_Handled;
	}

	MsgSrv("Import started%s", bDryRun? " [dry run]": "");

	int iUpdateCount, iCreateCount, iErrors;
	if (!ImportConfigs(src, szMapFilter, bDryRun, bUpdateConfigs, bCreateConfigs, iUpdateCount, iCreateCount, iErrors))
	{
		return Plugin_Handled;
	}

	if (bDryRun)
	{
		char szCmd[32]; GetCmdArg(0, szCmd, sizeof(szCmd));

		PrintToServer(
			"\nYou are about to import %d %s configs (of which %d will be created).%s"
			... "\nExecute the following command to continue:\n>>> %s %s %s %d %d 1 <<<\n",
			(iUpdateCount + iCreateCount), szType, iCreateCount,
			iErrors? "\nIt is recommended to fix errors above before proceeding with the live run." : "",
			szCmd, szType, szMapFilter, bUpdateConfigs, bCreateConfigs);
	}
	else
	{
		PrintToServer("\n - Import finished -\n Successful imports: %d\n Updated configs: %d\n New configs: %d\n Errors: %d\n",
			(iUpdateCount + iCreateCount), iUpdateCount, iCreateCount, iErrors);
	}

	return Plugin_Handled;
}

static int g_iMissingWeaponSpawnCount = 0;
static Handle g_hMissingWeaponTimer;

// Intended to be used on the map `bm_c1a3a` to test missing weapons upon player spawns.
// Increase `engine_no_focus_sleep` to `350` on the client, then unfocus the window to reproduce.
//
public Action Command_Debug_MissingWeapons(int iArgs)
{
	if (g_hMissingWeaponTimer == null)
	{
		g_iMissingWeaponSpawnCount = 0;
		g_hMissingWeaponTimer = CreateTimer(0.5, Timer_Debug_MissingWeapons, _, TIMER_REPEAT);
		MsgAll("Infinite missing weapon timer started");
	}
	else
	{
		delete g_hMissingWeaponTimer;
		MsgAll("Infinite missing weapon timer stopped");
	}
	
	return Plugin_Handled;
}

public Action Command_DebugSetPos(int iClient, int iArgs)
{
	if (iArgs != 4)
	{
		if (iClient)
			MsgReply(iClient, "Usage: sc_debug_setpos <player> <x> <y> <z>");
		else
			MsgSrv("Usage: sc_debug_setpos <player> <x> <y> <z>");
		return Plugin_Handled;
	}
	
	char szTarget[65];
	GetCmdArg(1, szTarget, sizeof(szTarget));

	float vecPos[3];
	vecPos[0] = GetCmdArgFloat(2);
	vecPos[1] = GetCmdArgFloat(3);
	vecPos[2] = GetCmdArgFloat(4);

	char szTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS], iTargetCount;
	bool bIsML;
	
	if ((iTargetCount = ProcessTargetString(
		szTarget,
		iClient,
		iTargets,
		sizeof(iTargets),
		COMMAND_FILTER_NO_BOTS,
		szTargetName,
		sizeof(szTargetName),
		bIsML)) <= 0)
	{
		ReplyToTargetError(iClient, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTarget = iTargets[i];
		CBasePlayer pTarget = CBasePlayer(iTarget);
		if (pTarget.IsValid())
		{
			pTarget.Teleport(vecPos, NULL_VECTOR, NULL_VECTOR);
		}
	}
	
	if (iClient)
		MsgReply(iClient, "Teleported %s to %.2f %.2f %.2f.", szTargetName, vecPos[0], vecPos[1], vecPos[2]);
	else
		MsgSrv("Teleported %s to %.2f %.2f %.2f.", szTargetName, vecPos[0], vecPos[1], vecPos[2]);

	return Plugin_Handled;
}

public Action Command_DebugSetAng(int iClient, int iArgs)
{
	if (iArgs != 4)
	{
		if (iClient)
			MsgReply(iClient, "Usage: sc_debug_setang <player> <pitch> <yaw> <roll>");
		else
			MsgSrv("Usage: sc_debug_setang <player> <pitch> <yaw> <roll>");
		return Plugin_Handled;
	}
	
	char szTarget[65];
	GetCmdArg(1, szTarget, sizeof(szTarget));

	float vecAng[3];
	vecAng[0] = GetCmdArgFloat(2);
	vecAng[1] = GetCmdArgFloat(3);
	vecAng[2] = GetCmdArgFloat(4);

	char szTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS], iTargetCount;
	bool bIsML;
	
	if ((iTargetCount = ProcessTargetString(
		szTarget,
		iClient,
		iTargets,
		sizeof(iTargets),
		COMMAND_FILTER_NO_BOTS,
		szTargetName,
		sizeof(szTargetName),
		bIsML)) <= 0)
	{
		ReplyToTargetError(iClient, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTarget = iTargets[i];
		CBasePlayer pTarget = CBasePlayer(iTarget);
		if (pTarget.IsValid())
		{
			pTarget.Teleport(NULL_VECTOR, vecAng, NULL_VECTOR);
		}
	}
	
	if (iClient)
		MsgReply(iClient, "Set %s angles to %.2f %.2f %.2f.", szTargetName, vecAng[0], vecAng[1], vecAng[2]);
	else
		MsgSrv("Set %s angles to %.2f %.2f %.2f.", szTargetName, vecAng[0], vecAng[1], vecAng[2]);

	return Plugin_Handled;
}

public Action Command_DebugGetPos(int iClient, int iArgs)
{
	if (iArgs != 1)
	{
		if (iClient)
			MsgReply(iClient, "Usage: sc_debug_getpos <player>");
		else
			MsgSrv("Usage: sc_debug_getpos <player>");
		return Plugin_Handled;
	}
	
	char szTarget[65];
	GetCmdArg(1, szTarget, sizeof(szTarget));

	char szTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS], iTargetCount;
	bool bIsML;

	if ((iTargetCount = ProcessTargetString(
		szTarget,
		iClient,
		iTargets,
		sizeof(iTargets),
		COMMAND_FILTER_NO_BOTS,
		szTargetName,
		sizeof(szTargetName),
		bIsML)) <= 0)
	{
		ReplyToTargetError(iClient, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTarget = iTargets[i];
		CBasePlayer pTarget = CBasePlayer(iTarget);
		if (!pTarget.IsValid())
			continue;

		float vecPos[3];
		pTarget.GetAbsOrigin(vecPos);

		if (iClient)
			MsgReply(iClient, "%N => %.2f %.2f %.2f", iTarget, vecPos[0], vecPos[1], vecPos[2]);
		else
			MsgSrv("%N => %.2f %.2f %.2f", iTarget, vecPos[0], vecPos[1], vecPos[2]);
	}

	return Plugin_Handled;
}

public Action Command_DebugGetAng(int iClient, int iArgs)
{
	if (iArgs != 1)
	{
		if (iClient)
			MsgReply(iClient, "Usage: sc_debug_getang <player>");
		else
			MsgSrv("Usage: sc_debug_getang <player>");
		return Plugin_Handled;
	}
	
	char szTarget[65];
	GetCmdArg(1, szTarget, sizeof(szTarget));

	char szTargetName[MAX_TARGET_LENGTH];
	int iTargets[MAXPLAYERS], iTargetCount;
	bool bIsML;

	if ((iTargetCount = ProcessTargetString(
		szTarget,
		iClient,
		iTargets,
		sizeof(iTargets),
		COMMAND_FILTER_NO_BOTS,
		szTargetName,
		sizeof(szTargetName),
		bIsML)) <= 0)
	{
		ReplyToTargetError(iClient, iTargetCount);
		return Plugin_Handled;
	}

	for (int i = 0; i < iTargetCount; i++)
	{
		int iTarget = iTargets[i];
		CBasePlayer pTarget = CBasePlayer(iTarget);
		if (!pTarget.IsValid())
			continue;

		float vecAng[3];
		pTarget.GetEyeAngles(vecAng);

		if (iClient)
			MsgReply(iClient, "%N => %.2f %.2f %.2f", iTarget, vecAng[0], vecAng[1], vecAng[2]);
		else
			MsgSrv("%N => %.2f %.2f %.2f", iTarget, vecAng[0], vecAng[1], vecAng[2]);
	}

	return Plugin_Handled;
}

static Action Timer_Debug_MissingWeapons(Handle hTimer)
{
	for (int i = 1; i <= MaxClients; ++i)
	{
		CBasePlayer pPlayer = CBasePlayer(i);
		if (pPlayer == NULL_CBASEENTITY || !pPlayer.IsAlive() || pPlayer.IsBot())
			continue;

		// Check player state here.
		if (!pPlayer.HasWeapon("weapon_glock") || !pPlayer.HasWeapon("weapon_shotgun"))
		{
			MsgAll("Missing weapons after %i spawns", g_iMissingWeaponSpawnCount);
			g_hMissingWeaponTimer = null;
			return Plugin_Stop;
		}
		
		pPlayer.Suicide();
		pPlayer.Spawn();
	}

	g_iMissingWeaponSpawnCount += 1;

	return Plugin_Continue;
}
