enum struct ItemInstancingManager
{
	StringMap m_pPickedUpItems[MAXPLAYERS+1];
	
	void Initialize()
	{
	}
	
	void OnClientPutInServer(int client)
	{
		this.m_pPickedUpItems[client] = new StringMap();
	}
	
	void OnClientDisconnect(int client)
	{
		delete this.m_pPickedUpItems[client];
	}
	
	void OnPlayerSpawned(CBlackMesaPlayer pPlayer)
	{
		this.m_pPickedUpItems[pPlayer.GetEntIndex()].Clear();
	}
	
	void AddPickedUpItem(CBlackMesaPlayer pPlayer, CBaseEntity pItem)
	{
		char key[32]; IntToString(view_as<int>(pItem), key, sizeof(key));
		this.m_pPickedUpItems[pPlayer.GetEntIndex()].SetValue(key, true);
	}
	
	bool HasPickedUpItem(CBlackMesaPlayer pPlayer, CBaseEntity pItem)
	{
		char key[32]; bool _val; IntToString(view_as<int>(pItem), key, sizeof(key));
		return this.m_pPickedUpItems[pPlayer.GetEntIndex()].GetValue(key, _val);
	}
	
	bool ClearPickedUpItem(CBaseEntity pItem)
	{
		char key[32]; IntToString(view_as<int>(pItem), key, sizeof(key));
		for (int i = 0; i <= MaxClients; i++) {
			if(this.m_pPickedUpItems[i] != null)
			{
				this.m_pPickedUpItems[i].Remove(key);
			}
		}
	}
}

ItemInstancingManager g_pInstancingManager;

public void Hook_Instancing_ItemSpawn(int iEntIndex)
{
	DispatchKeyValue(iEntIndex, "respawntime", "0");
	HookSingleEntityOutput(iEntIndex, "OnPlayerPickup", Hook_Instancing_OnPlayerPickup, false);
	SDKHook(iEntIndex, SDKHook_Touch, Hook_Instancing_ItemTouch);
	SDKHook(iEntIndex, SDKHook_SetTransmit, Hook_Instancing_ItemTransmit);
	DHookAddEntityListener(ListenType_Deleted, Hook_Instancing_ItemDeleted);
}

public Action Hook_Instancing_OnPlayerPickup(const char[] output, int caller, int activator, float delay)
{
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(activator);
	if(pPlayer.IsClassPlayer())
	{
		CBaseEntity pItem = CBaseEntity(caller);
		g_pInstancingManager.AddPickedUpItem(pPlayer, pItem);
	}
}

public Action Hook_Instancing_ItemTouch(int entity, int other)
{
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(other);
	if(pPlayer.IsClassPlayer())
	{
		CBaseEntity pItem = CBaseEntity(entity);
		if(g_pInstancingManager.HasPickedUpItem(pPlayer, pItem))
		{
			return Plugin_Stop;
		}
	}
	return Plugin_Continue;
}

public Action Hook_Instancing_ItemTransmit(int entity, int client)
{
	if(g_pInstancingManager.HasPickedUpItem(CBlackMesaPlayer(client), CBaseEntity(entity)))
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

public void Hook_Instancing_ItemDeleted(int entity)
{
	g_pInstancingManager.ClearPickedUpItem(CBaseEntity(entity));
}