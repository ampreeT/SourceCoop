#pragma newdecls required
#pragma semicolon 1

StringMap g_pPickedUpItems[MAXPLAYERS + 1];

methodmap ItemInstancingManager
{
	public static void Initialize()
	{
		DHookAddEntityListener(ListenType_Deleted, Hook_Instancing_ItemDeleted);
	}
	
	public static void OnClientPutInServer(int client)
	{
		g_pPickedUpItems[client] = new StringMap();
	}
	
	public static void OnClientDisconnect(int client)
	{
		delete g_pPickedUpItems[client];
	}
	
	public static void OnPlayerSpawned(CBasePlayer pPlayer)
	{
		if (!CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS_NORESET))
		{
			g_pPickedUpItems[pPlayer.entindex].Clear();
		}
	}
	
	public static void AddPickedUpItem(CBasePlayer pPlayer, CBaseEntity pItem)
	{
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(view_as<int>(pItem), szKey, sizeof(szKey));
		g_pPickedUpItems[pPlayer.entindex].SetValue(szKey, true);
	}
	
	public static bool HasPickedUpItem(CBasePlayer pPlayer, CBaseEntity pItem)
	{
		int client = pPlayer.entindex;
		if (!g_pPickedUpItems[client]) // filter sourceTV
			return false;
		
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(view_as<int>(pItem), szKey, sizeof(szKey));
		
		return g_pPickedUpItems[client].ContainsKey(szKey);
	}
	
	public static void ClearPickedUpItem(CBaseEntity pItem)
	{
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(view_as<int>(pItem), szKey, sizeof(szKey));
		for (int i = 0; i <= MaxClients; i++)
		{
			if (g_pPickedUpItems[i] != null)
			{
				g_pPickedUpItems[i].Remove(szKey);
			}
		}
	}
}

public void Hook_Instancing_ItemSpawn(int iEntIndex)
{
	CItem pItem = CItem(iEntIndex);
	
	#if defined SRCCOOP_BLACKMESA
	DHookEntity(hkOnTryPickUp, false, iEntIndex, _, Hook_Instancing_OnTryPickUpPre);
	DHookEntity(hkOnTryPickUp, true, iEntIndex, _, Hook_Instancing_OnTryPickUpPost);
	#endif

	if (pItem.IsClassname("item_healthkit"))
	{
		// this spawns second entity item_healthkit_ragdoll_clone
		SDKHook(iEntIndex, SDKHook_SetTransmit, Hook_Instancing_NeverTransmit);
	}
	else
	{
		SDKHook(iEntIndex, SDKHook_SetTransmit, Hook_Instancing_ItemTransmit);
		SDKHook(iEntIndex, SDKHook_Use, Hook_Instancing_Use);
	}
	pItem.m_spawnflags &= ~SF_ITEM_HARDRESPAWN;
	DispatchKeyValue(iEntIndex, "respawntime", "0");
}

// Do not allow to to touch-pickup consumed items
public MRESReturn Hook_Instancing_OnTryPickUpPre(int _this, DHookReturn hReturn, DHookParam hParams)
{
	CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
	if (pPlayer.IsPlayer())
	{
		CBaseEntity pItem = CBaseEntity(_this);
		if (ItemInstancingManager.HasPickedUpItem(pPlayer, pItem))
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// Store this newly consumed item
public MRESReturn Hook_Instancing_OnTryPickUpPost(int _this, DHookReturn hReturn, DHookParam hParams)
{
	bool bPickedUp = DHookGetReturn(hReturn);
	if (bPickedUp)
	{
		CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
		if (pPlayer.IsPlayer())
		{
			CBaseEntity pItem = CBaseEntity(_this);
			ItemInstancingManager.AddPickedUpItem(pPlayer, pItem);
		}
	}
	return MRES_Ignored;
}

// Do not transmit picked up items. This is not optimal - better option would be to show some effect/as translucent
public Action Hook_Instancing_ItemTransmit(int entity, int client)
{
	if (ItemInstancingManager.HasPickedUpItem(CBasePlayer(client), CBaseEntity(entity)))
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

public Action Hook_Instancing_Use(int entity, int activator, int caller, UseType type, float value)
{
	if (ItemInstancingManager.HasPickedUpItem(CBasePlayer(caller), CBaseEntity(entity)))
	{
		return Plugin_Handled;
	}
	return Plugin_Continue;
}

public Action Hook_Instancing_NeverTransmit(int entity, int client)
{
	return Plugin_Stop;
}

public void Hook_Instancing_ItemDeleted(int entity)
{
	ItemInstancingManager.ClearPickedUpItem(CBaseEntity(entity));
}