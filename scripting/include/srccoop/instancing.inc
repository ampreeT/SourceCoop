#pragma newdecls required
#pragma semicolon 1

static StringMap g_pPickedUpItems[MAXPLAYERS + 1];

methodmap ItemInstancingManager
{
	public static void Initialize()
	{
		DHookAddEntityListener(ListenType_Deleted, Hook_Instancing_ItemDeleted);
	}

	public static void OnClientPutInServer(const int iClient)
	{
		g_pPickedUpItems[iClient] = new StringMap();
	}
	public static void OnClientDisconnect(const int iClient)
	{
		delete g_pPickedUpItems[iClient];
	}
	public static void OnPlayerSpawned(const CBasePlayer pPlayer)
	{
		ItemInstancingManager.ClearAllPickedUpItemsForPlayer(pPlayer);
	}
	public static void OnItemSpawnPost(const CItem pItem)
	{
		if (!CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS))
			return;

		#if defined SRCCOOP_BLACKMESA
		DHookEntity(hkOnTryPickUp, false, pItem.entindex, _, Hook_Instancing_OnTryPickUpPre);
		DHookEntity(hkOnTryPickUp, true, pItem.entindex, _, Hook_Instancing_OnTryPickUpPost);

		char szClassname[MAX_CLASSNAME];
		pItem.GetClassname(szClassname, sizeof(szClassname));
		if (strcmp(szClassname, "item_healthkit", false) == 0 || strcmp(szClassname, "item_healthvial", false) == 0)
		{
			// Always hide the original healthkit model.
			SDKHook(pItem.entindex, SDKHook_SetTransmit, Hook_Instancing_Item_NeverTransmit);

			// Fix up the tranmission on the healthkit ragdoll.
			CItem_HealthKit pHealthKit = view_as<CItem_HealthKit>(pItem);
			CRagdollProp pRagdoll = pHealthKit.GetRagdollMimic();
			if (pRagdoll != NULL_CBASEENTITY)
			{
				pHealthKit.HookOutput("OnPlayerPickup", Hook_Instancing_HealthKit_OnPlayerPickup);
				SDKHook(pRagdoll.entindex, SDKHook_SetTransmit, Hook_Instancing_Item_Transmit);
				SDKHook(pRagdoll.entindex, SDKHook_Use, Hook_Instancing_Item_Use);
			}
		}
		else
		#endif
		{
			SDKHook(pItem.entindex, SDKHook_SetTransmit, Hook_Instancing_Item_Transmit);
			SDKHook(pItem.entindex, SDKHook_Use, Hook_Instancing_Item_Use);
		}
		
		pItem.m_spawnflags &= ~SF_ITEM_HARDRESPAWN;
		pItem.KeyValue("respawntime", "0");
	}
	
	public static void AddPickedUpItem(const CBasePlayer pPlayer, const CBaseEntity pItem)
	{
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(pItem.entref, szKey, sizeof(szKey));
		g_pPickedUpItems[pPlayer.entindex].SetValue(szKey, true);
	}
	public static bool HasPickedUpItem(const CBasePlayer pPlayer, const CBaseEntity pItem)
	{
		if (!g_pPickedUpItems[pPlayer.entindex]) // filter sourceTV
			return false;
		
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(pItem.entref, szKey, sizeof(szKey));
		return g_pPickedUpItems[pPlayer.entindex].ContainsKey(szKey);
	}
	public static void ClearPickedUpItem(const CBaseEntity pItem)
	{
		char szKey[MAX_CHARS_INT(INT_MIN)];
		IntToString(pItem.entref, szKey, sizeof(szKey));
		for (int i = 0; i <= MaxClients; i++)
		{
			if (g_pPickedUpItems[i] == null)
				continue;
			
			g_pPickedUpItems[i].Remove(szKey);
		}
	}
	public static void ClearAllPickedUpItemsForPlayer(const CBasePlayer pPlayer)
	{
		if (CoopManager.IsFeatureEnabled(FT_INSTANCE_ITEMS_NORESET))
			return;
		g_pPickedUpItems[pPlayer.entindex].Clear();
	}
}

static void Hook_Instancing_ItemDeleted(int iEntity)
{
	ItemInstancingManager.ClearPickedUpItem(CBaseEntity(iEntity));
}

#if defined SRCCOOP_BLACKMESA
// Do not allow to touch-pickup consumed items.
//
static MRESReturn Hook_Instancing_OnTryPickUpPre(int _this, DHookReturn hReturn, DHookParam hParams)
{
	CBaseEntity pEntity = CBaseEntity(DHookGetParam(hParams, 1));
	if (pEntity != NULL_CBASEENTITY && pEntity.IsPlayer())
	{
		CBasePlayer pPlayer = view_as<CBasePlayer>(pEntity);
		CBaseEntity pItem = CBaseEntity(_this);
		if (ItemInstancingManager.HasPickedUpItem(pPlayer, pItem))
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;
}

// Store this newly consumed item.
//
static MRESReturn Hook_Instancing_OnTryPickUpPost(int _this, DHookReturn hReturn, DHookParam hParams)
{
	bool bPickedUp = DHookGetReturn(hReturn);
	if (bPickedUp)
	{
		CBaseEntity pEntity = CBaseEntity(DHookGetParam(hParams, 1));
		if (pEntity.IsPlayer())
		{
			CBasePlayer pPlayer = view_as<CBasePlayer>(pEntity);
			CBaseEntity pItem = CBaseEntity(_this);
			ItemInstancingManager.AddPickedUpItem(pPlayer, pItem);
		}
	}
	return MRES_Ignored;
}
#endif

static Action Hook_Instancing_Item_Use(int iEntIndex, int iActivator, int iCaller, UseType eType, float flValue)
{
	if (ItemInstancingManager.HasPickedUpItem(CBasePlayer(iCaller), CBaseEntity(iEntIndex)))
	{
		return Plugin_Handled;
	}
	return Plugin_Continue;
}

#if defined SRCCOOP_BLACKMESA
// Never transmit the original healthkit object as it will be visible in the world.
//
static Action Hook_Instancing_Item_NeverTransmit(int iEntity, int iClient)
{
	return Plugin_Stop;
}
#endif

// Do not transmit picked up items.
//
// TODO: This is not optimal - better option would be to show some effect/as translucent.
//
static Action Hook_Instancing_Item_Transmit(int iEntIndex, int iClient)
{
	if (ItemInstancingManager.HasPickedUpItem(CBasePlayer(iClient), CBaseEntity(iEntIndex)))
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

// Fixes parented objects transmitting while the parent item is not transmitting.
//
public Action Hook_Instancing_ItemParent_Transmit(int iEntIndex, int iClient)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	CBaseEntity pParent = pEntity.GetParent();
	if (pParent != NULL_CBASEENTITY && ItemInstancingManager.HasPickedUpItem(CBasePlayer(iClient), pParent))
	{
		return Plugin_Stop;
	}
	return Plugin_Continue;
}

#if defined SRCCOOP_BLACKMESA
// Fixes healthkits not being dropped on pickup unlike other items.
//
static Action Hook_Instancing_HealthKit_OnPlayerPickup(const char[] szOutput, int iCaller, int iActivator, float flDelay)
{
	CBaseEntity pActivator = CBaseEntity(iActivator);
	CItem_HealthKit pHealthKit = CItem_HealthKit(iCaller);
	if (pActivator != NULL_CBASEENTITY && pHealthKit != NULL_CBASEENTITY && pActivator.IsPlayer())
	{
		CBasePlayer pPlayer = view_as<CBasePlayer>(pActivator);
		CBaseEntity pCarriedObject = pPlayer.GetCarriedObject();
		CRagdollProp pRagdoll = pHealthKit.GetRagdollMimic();
		
		if (pRagdoll != NULL_CBASEENTITY && pRagdoll == pCarriedObject)
		{
			pPlayer.ForceDropOfCarriedPhysObjects();
		}

		if (pRagdoll != NULL_CBASEENTITY)
		{
			ItemInstancingManager.AddPickedUpItem(pPlayer, pRagdoll);
		}
	}
	return Plugin_Continue;
}
#endif
