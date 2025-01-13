#pragma newdecls required
#pragma semicolon 1

/**
 * Return nearest alive player if exists, otherwise return nearest player that passes requirements.
 */
stock CBasePlayer GetNearestPlayerPreferAlive(CBaseEntity pOriginEntity, bool bNoSpectators = false)
{
	return GetNearestPlayer(pOriginEntity, false, bNoSpectators, true);
}

stock CBasePlayer GetNearestPlayerPreferAliveEx(float vecPos[3], bool bNoSpectators = false)
{
	return GetNearestPlayerEx(vecPos, false, bNoSpectators, true);
}

/**
 * Return nearest alive player.
 */
stock CBasePlayer GetNearestPlayerAlive(CBaseEntity pOriginEntity)
{
	return GetNearestPlayer(pOriginEntity, true, true, false);
}

stock CBasePlayer GetNearestPlayerAliveEx(float vecPos[3])
{
	return GetNearestPlayerEx(vecPos, true, true, false);
}

/**
 * Return nearest player that passes requirements.
 */
stock CBasePlayer GetNearestPlayer(CBaseEntity pOriginEntity, bool bAliveOnly = true, bool bNoSpectators = true, bool bPreferAlive = false)
{
	float vecEntityPosition[3];
	pOriginEntity.GetAbsOrigin(vecEntityPosition);
	return GetNearestPlayerEx(vecEntityPosition, bAliveOnly, bNoSpectators, bPreferAlive);
}

stock CBasePlayer GetNearestPlayerEx(float vecPos[3], bool bAliveOnly = true, bool bNoSpectators = true, bool bPreferAlive = false)
{
	CBasePlayer pBestPlayer = NULL_CBASEENTITY;
	float flBestDistance = FLT_MAX;
	bool bFoundAlive;
	
	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || IsFakeClient(i))
			continue;

		bool bAlive = IsPlayerAlive(i);
		if (bAliveOnly)
		{
			if (!bAlive)
				continue;
		}
		else if (bNoSpectators && GetClientTeam(i) == TEAM_SPECTATOR)
		{
			continue;
		}
		
		if (bPreferAlive)
		{
			if (bFoundAlive)
			{
				if (!bAlive)
					continue;
			}
			else if (bAlive)
			{
				bFoundAlive = true;
				flBestDistance = FLT_MAX;
			}
		}
		
		CBasePlayer pPlayer = CBasePlayer(i);
		float vecPlayerPosition[3];
		pPlayer.GetAbsOrigin(vecPlayerPosition);

		float flDistance = GetVectorDistance(vecPos, vecPlayerPosition, true);
		if (flDistance < flBestDistance)
		{
			pBestPlayer = pPlayer;
			flBestDistance = flDistance;
		}
	}
	return pBestPlayer;
}

stock int GetRealClientCount(bool bInGameOnly, bool bAliveOnly = false, bool bNoSpectators = false)
{
	int clients;
	for (int i = 1; i <= MaxClients; i++)
	{
		if ((bInGameOnly? IsClientInGame(i) : IsClientConnected(i))
			&& !IsFakeClient(i)
			&& (!bAliveOnly || IsPlayerAlive(i))
			&& (!bNoSpectators || GetClientTeam(i) != TEAM_SPECTATOR))
		{
 			clients++;
 		}
 	}
	return clients;
}

enum CanTeleportResult
{
	CAN_TP_NO,
	CAN_TP_YES,
	CAN_TP_MUSTDUCK		//🦆//
}

stock CanTeleportResult CanTeleportPlayer(CBasePlayer pPlayer, float vecOrigin[3], bool bExact = false, bool bDuckCheck = true, bool bOnlyVaried = false)
{
	if (!bOnlyVaried)
	{
		bool bDidHit;

		TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
		bDidHit = TR_DidHit();
		if (!bDidHit) return CAN_TP_YES;
		
		if (bDuckCheck)
		{
			TR_TraceHullFilter(vecOrigin, vecOrigin, view_as<float>(VEC_DUCK_HULL_MIN), view_as<float>(VEC_DUCK_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
			bDidHit = TR_DidHit();
			if (!bDidHit) return CAN_TP_MUSTDUCK;
		}
	}
	
	if (!bExact)
	{
		// try varying the positions in a radius
		static const float yaw[] = {0.0, 45.0, 90.0, 135.0, 180.0, 225.0, 270.0, 315.0};
		static const float distance[] = {25.0, 50.0};
		static const float zDistance[] = {0.0, 25.0, 50.0};
		
		for (int z = 0; z < sizeof(zDistance); z++)
		{
			for (int d = 0; d < sizeof(distance); d++)
			{
				for (int y = 0; y < sizeof(yaw); y++)
				{
					if (CanTeleportPlayer_Offset(vecOrigin, yaw[y], distance[d], zDistance[z]))
					{
						return CAN_TP_YES;
					}
				}
			}
		}
	}
	
	return CAN_TP_NO;
}

static stock bool CanTeleportPlayer_Offset(float vecOrigin[3], float yaw, float distance, float z)
{
	float newPos[3], angles[3];
	newPos = vecOrigin; angles[1] = yaw;
	
	GetAngleVectors(angles, angles, NULL_VECTOR, NULL_VECTOR);
	newPos[0] += angles[0] * distance;
	newPos[1] += angles[1] * distance;
	newPos[2] += z;
	
	//TE_SetupGlowSprite(newPos, PrecacheModel("sprites/orangeglow1.vmt"), 60.0, 0.1, 255); TE_SendToAll();
	
	TR_TraceHullFilter(newPos, newPos, view_as<float>(VEC_HULL_MIN), view_as<float>(VEC_HULL_MAX), MASK_PLAYERSOLID|CONTENTS_HITBOX, CanTeleportPlayer_TraceFilter);
	if (!TR_DidHit())
	{
		vecOrigin = newPos;
		return true;
	}

  	return false;
}

static stock bool CanTeleportPlayer_TraceFilter(int entity, int mask, any data)
{
	CBaseEntity pEntity = CBaseEntity(entity);
	if (pEntity.IsPlayer() || pEntity.IsPickupItem())
	{
		return false;
	}
	return true;
}

stock float PlayerDotProductToEntity(CBasePlayer pPlayer, CBaseEntity pEnt)
{
	float entOrigin[3];
	pEnt.GetAbsOrigin(entOrigin);
	return PlayerDotProductToPoint(pPlayer, entOrigin);
}

stock float PlayerDotProductToEntityCenter(CBasePlayer pPlayer, CBaseEntity pEnt)
{
	float vecCenter[3];
	pEnt.WorldSpaceCenter(vecCenter);
	return PlayerDotProductToPoint(pPlayer, vecCenter);
}

stock float PlayerDotProductToPoint(CBasePlayer pPlayer, const float vecPoint[3])
{
	float angles[3], origin[3], fwd[3], targetDir[3];
	pPlayer.GetEyeAngles(angles);
	GetAngleVectors(angles, fwd, NULL_VECTOR, NULL_VECTOR);

	pPlayer.GetEyePosition(origin);
	MakeVectorFromPoints(origin, vecPoint, targetDir);
	NormalizeVector(targetDir, targetDir);

	return GetVectorDotProduct(fwd, targetDir);
}

stock void SetClientGamerules(const char[] szGamerulesClass)
{
	int iTable = FindStringTable("GameRulesCreation");
	if (iTable == INVALID_STRING_TABLE)
	{
		ThrowError("Could not find string table GameRulesCreation !");
	}
	SetStringTableData(iTable, 0, szGamerulesClass, strlen(szGamerulesClass) + 1);
}

stock int GetClientGamerules(char[] szGamerulesClass, int iMaxLength)
{
	int iTable = FindStringTable("GameRulesCreation");
	if (iTable == INVALID_STRING_TABLE)
	{
		ThrowError("Could not find string table GameRulesCreation !");
	}
	return GetStringTableData(iTable, 0, szGamerulesClass, iMaxLength);
}