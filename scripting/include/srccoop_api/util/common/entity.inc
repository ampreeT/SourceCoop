#pragma newdecls required
#pragma semicolon 1

stock int FindEntityByHammerID(int iHammerID, const char[] szClassname = "*")
{
	int startEnt = -1;
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		if (pEntity.GetHammerID() == iHammerID)
		{
			return startEnt;
		}
	}
	return -1;
}

stock int FindEntityByTargetname(int startEnt, const char[] targetname, const char[] szClassname = "*")
{
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		char szTargetname[MAX_CLASSNAME];
		if (pEntity.GetTargetname(szTargetname, sizeof(szTargetname)) && strcmp(szTargetname, targetname, false) == 0)
		{
			return startEnt;
		}
	}
	return -1;
}

stock int FindEntityByOwner(int startEnt, CBaseEntity pOwnerEntity, const char[] szClassname = "*")
{
	while ((startEnt = FindEntityByClassname(startEnt, szClassname)) != -1)
	{
		CBaseEntity pEntity = CBaseEntity(startEnt);
		if (pEntity.GetOwner() == pOwnerEntity)
		{
			return startEnt;
		}
	}
	return -1;
}