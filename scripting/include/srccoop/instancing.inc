#if defined _srccoop_instancing_included
 #endinput
#endif
#define _srccoop_instancing_included

#pragma newdecls required
#pragma semicolon 1

enum struct ItemInstancingManager
{
	StringMap m_pPickedUpItems[MAXPLAYERS+1];
	
	void Initialize()
	{
		DHookAddEntityListener(ListenType_Deleted, Hook_Instancing_ItemDeleted);
	}
	
	void OnClientPutInServer(int client)
	{
		this.m_pPickedUpItems[client] = new StringMap();
	}
	
	void OnClientDisconnect(int client)
	{
		delete this.m_pPickedUpItems[client];
	}
	
	void OnPlayerSpawned(CBasePlayer pPlayer)
	{
		if(!CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS_NORESET))
		{
			this.m_pPickedUpItems[pPlayer.GetEntIndex()].Clear();
		}
	}
	
	void AddPickedUpItem(CBasePlayer pPlayer, CBaseEntity pItem)
	{
		char key[32]; IntToString(view_as<int>(pItem), key, sizeof(key));
		this.m_pPickedUpItems[pPlayer.GetEntIndex()].SetValue(key, true);
	}
	
	bool HasPickedUpItem(CBasePlayer pPlayer, CBaseEntity pItem)
	{
		char key[32]; bool _val; IntToString(view_as<int>(pItem), key, sizeof(key));
		return this.m_pPickedUpItems[pPlayer.GetEntIndex()].GetValue(key, _val);
	}
	
	void ClearPickedUpItem(CBaseEntity pItem)
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
	CBaseEntity pItem = CBaseEntity(iEntIndex);
	
	#if defined SRCCOOP_BLACKMESA
	DHookEntity(hkOnTryPickUp, false, iEntIndex, _, Hook_Instancing_OnTryPickUpPre);
	DHookEntity(hkOnTryPickUp, true, iEntIndex, _, Hook_Instancing_OnTryPickUpPost);
	#endif

	if(pItem.IsClassname("item_healthkit"))
	{
		// this spawns second entity item_healthkit_ragdoll_clone
		SDKHook(iEntIndex, SDKHook_SetTransmit, Hook_Instancing_NeverTransmit);
	}
	else
	{
		SDKHook(iEntIndex, SDKHook_SetTransmit, Hook_Instancing_ItemTransmit);
		SDKHook(iEntIndex, SDKHook_Use, Hook_Instancing_Use);
	}
	pItem.SetSpawnFlags(pItem.GetSpawnFlags() & ~SF_ITEM_HARDRESPAWN);
	DispatchKeyValue(iEntIndex, "respawntime", "0");
}

// Do not allow to to touch-pickup consumed items
public MRESReturn Hook_Instancing_OnTryPickUpPre(int _this, Handle hReturn, Handle hParams)
{
	CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
	if(pPlayer.IsClassPlayer())
	{
		CBaseEntity pItem = CBaseEntity(_this);
		if(g_pInstancingManager.HasPickedUpItem(pPlayer, pItem))
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// Store this newly consumed item
public MRESReturn Hook_Instancing_OnTryPickUpPost(int _this, Handle hReturn, Handle hParams)
{
	bool bPickedUp = DHookGetReturn(hReturn);
	if(bPickedUp)
	{
		CBasePlayer pPlayer = CBasePlayer(DHookGetParam(hParams, 1));
		if(pPlayer.IsClassPlayer())
		{
			CBaseEntity pItem = CBaseEntity(_this);
			g_pInstancingManager.AddPickedUpItem(pPlayer, pItem);
		}
	}
	return MRES_Ignored;
}

// Do not transmit picked up items. This is not optimal - better option would be to show some effect/as translucent
public Action Hook_Instancing_ItemTransmit(int entity, int client)
{
	if(g_pInstancingManager.HasPickedUpItem(CBasePlayer(client), CBaseEntity(entity)))
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

public Action Hook_Instancing_Use(int entity, int activator, int caller, UseType type, float value)
{
	if(g_pInstancingManager.HasPickedUpItem(CBasePlayer(caller), CBaseEntity(entity)))
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
	g_pInstancingManager.ClearPickedUpItem(CBaseEntity(entity));
}