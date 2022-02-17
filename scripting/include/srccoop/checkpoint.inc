#if defined _srccoop_checkpoint_included
 #endinput
#endif
#define _srccoop_checkpoint_included

#define SND_BRINGALL "sfx.beep5"
#define SND_PORTAL "xen.energy_hit"

enum struct CCoopSpawnEntry
{
	bool m_bHasPosition;
	float m_vecPosition[3];
	bool m_bHasAngles;
	float m_vecAngles[3];
	float m_flDelay;
	CBaseEntity m_pTriggerEnt;
	CBaseEntity m_pTouchEnt;
	CBaseEntity m_pFollowEnt;
	char m_szOutputName[MAX_FORMAT];
	bool m_bHasPortal;
	float m_vecPortalPosition[3];
	bool m_bBringAllPlayers;
	float m_fBringAllGraceRadius;
	
	void Initialize()
	{
		this.m_pTriggerEnt = CBaseEntity();
		this.m_pTouchEnt = CBaseEntity();
		this.m_pFollowEnt = CBaseEntity();
	}
	
	bool TeleportPlayer(CBasePlayer pPlayer, bool bForceTeleport = false, bool bIsBringAllTP = false)
	{
		float vecPosition[3];
		float vecAngles[3];
		
		if (this.m_pFollowEnt.IsValid())
		{
			this.m_pFollowEnt.GetAbsOrigin(vecPosition);
			this.m_pFollowEnt.GetAbsAngles(vecAngles);
			
			vecPosition[0] += this.m_vecPosition[0];
			vecPosition[1] += this.m_vecPosition[1];
			vecPosition[2] += this.m_vecPosition[2];
			
			vecAngles[0] += this.m_vecAngles[0];
			vecAngles[1] += this.m_vecAngles[1];
			vecAngles[2] += this.m_vecAngles[2];
		}
		else
		{
			vecPosition = this.m_vecPosition;
			vecAngles = this.m_vecAngles;
		}
		if(bIsBringAllTP && this.m_fBringAllGraceRadius > 0)
		{
			float vecPlayerPos[3]; pPlayer.GetAbsOrigin(vecPlayerPos);
			if(GetVectorDistance(vecPosition, vecPlayerPos) <= this.m_fBringAllGraceRadius)
			{
				return false;
			}
		}
		CanTeleportResult res;
		if((res = CanTeleportPlayer(pPlayer, vecPosition)) || bForceTeleport)
		{
			if(res == CAN_TP_MUSTDUCK)
			{
				pPlayer.StartDucking();
			}
			else
			{
				pPlayer.StopDucking();
			}
			pPlayer.Teleport(vecPosition, vecAngles, view_as<float>({0.0, 0.0, 0.0}));
			return true;
		}
		return false;
	}
	
	bool ActivatePre(CBasePlayer pActivator)
	{
		bool bChanged;
		if(!this.m_pFollowEnt.IsValid())
		{
			if(!this.m_bHasPosition)
			{
				pActivator.GetAbsOrigin(this.m_vecPosition);
				this.m_bHasPosition = true;
				bChanged = true;
			}
			if(!this.m_bHasAngles)
			{
				pActivator.GetAbsAngles(this.m_vecAngles);
				this.m_bHasAngles = true;
				bChanged = true;
			}
		}
		return bChanged;
	}
	
	void ActivatePost()
	{
		if(this.m_bBringAllPlayers)
		{
			for (int pp = 0; pp <= MaxClients; pp++)
			{
				CBasePlayer pPlayer = CBasePlayer(pp);
				if(pPlayer.IsValid() && pPlayer.IsAlive())
				{
					if(this.TeleportPlayer(pPlayer, true, true))
						pPlayer.PlayGameSound(SND_BRINGALL);
				}
			}
		}
	}
}

enum struct CCoopWeaponEntry
{
	char m_szClassname[MAX_CLASSNAME];
	int m_iPrimaryAmmo;
	int m_iSecondaryAmmo;
}

enum struct CCoopEquipment
{
	Array_t m_pWeaponList;
	int m_iHealth;
	int m_iArmor;
	int m_iAmmoCount[MAX_AMMO_TYPES];
	char m_szActiveWeapon[MAX_CLASSNAME];
	
	void Initialize()
	{
		this.m_pWeaponList = new Array_t(CreateArray(sizeof(CCoopWeaponEntry)));
	}
	void Clear()
	{
		this.m_pWeaponList.Clear();
		this.m_iHealth = -1;
		this.m_iArmor = -1;
		
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			this.m_iAmmoCount[i] = -1;
		}
		
		strcopy(this.m_szActiveWeapon, MAX_CLASSNAME, "");
	}
}

enum struct EquipmentManager 
{
    StringMap m_smEquipmentMap;


    void Initialize()
    {
        this.m_smEquipmentMap = new StringMap();
    }

    void Clear()
    {
        this.m_smEquipmentMap.Clear();
    }

    void StoreEquipment(char[] steamId, CCoopEquipment equipment){
        this.m_smEquipmentMap.SetArray(steamId, equipment, sizeof(equipment), true);
    }

    bool GetEquipment(char[] steamId, CCoopEquipment equipment){
        return this.m_smEquipmentMap.GetArray(steamId, equipment, sizeof(equipment));
    }
}

EquipmentManager g_pEquipmentManager;

enum struct CCoopSpawn
{
	Array_t m_pCheckpointList;
	int m_iCurrentCheckpoint;
	CCoopEquipment m_pDefaultEquipment;
	bool m_bStrictOrder;
	
	void Clear()
	{
		this.m_iCurrentCheckpoint = -1;
		this.m_pCheckpointList.Clear();
		this.m_pDefaultEquipment.Clear();
	}
	
	void Initialize()
	{
		PrecacheScriptSound(SND_BRINGALL);
		PrecacheScriptSound(SND_PORTAL);
		this.m_pCheckpointList = new Array_t(CreateArray(sizeof(CCoopSpawnEntry)));
		this.m_pDefaultEquipment.Initialize();
	}
	
	bool GetCheckpointByIndex(int iIndex, CCoopSpawnEntry pCheckpoint)
	{
		if(0 <= iIndex < this.m_pCheckpointList.Length)
		{
			this.m_pCheckpointList.GetArray(iIndex, pCheckpoint);
			return true;
		}
		return false;
	}
	
	bool GetCurrentCheckpoint(CCoopSpawnEntry pCheckpoint)
	{
		return this.GetCheckpointByIndex(this.m_iCurrentCheckpoint, pCheckpoint);
	}
	
	int FindCheckpointByOutput(CBaseEntity pCaller, const char[] szOutput, CCoopSpawnEntry pCheckpoint, int iStartIndex = 0)
	{
		CCoopSpawnEntry pEntry;
		for (int i = iStartIndex; i < this.m_pCheckpointList.Length; i++)
		{
			this.m_pCheckpointList.GetArray(i, pEntry);
			if (pCaller == pEntry.m_pTriggerEnt && StrEqual(szOutput, pEntry.m_szOutputName))
			{
				pCheckpoint = pEntry;
				return i;
			}
		}
		return -1;
	}
	
	int FindCheckpointByTouchEnt(CBaseEntity pTouchEnt, CCoopSpawnEntry pCheckpoint, int iStartIndex = 0)
	{
		CCoopSpawnEntry pEntry;
		for (int i = iStartIndex; i < this.m_pCheckpointList.Length; i++)
		{
			this.m_pCheckpointList.GetArray(i, pEntry);
			if (pTouchEnt == pEntry.m_pTouchEnt)
			{
				pCheckpoint = pEntry;
				return i;
			}
		}
		return -1;
	}
	
	void AddCheckpoint(const CCoopSpawnEntry pCheckpoint)
	{
		int iIndex = this.m_pCheckpointList.PushArray(pCheckpoint);
		bool bHasTrigger;
		
		if (pCheckpoint.m_pTriggerEnt.IsValid())
		{
			pCheckpoint.m_pTriggerEnt.HookOutput(pCheckpoint.m_szOutputName, Hook_OutputCheckpoint, true);
			bHasTrigger = true;
		}
		if (pCheckpoint.m_pTouchEnt.IsValid())
		{
			SDKHook(pCheckpoint.m_pTouchEnt.GetEntIndex(), SDKHook_StartTouchPost, Hook_TouchCheckpoint);
			bHasTrigger = true;
		}
		
		if(!bHasTrigger)
		{
			this.ActivateCheckpoint(iIndex);
		}
	}
	
	bool ActivateCheckpoint(int iIndex, CBasePlayer pActivator = view_as<CBasePlayer>(-1))
	{
		if(this.m_iCurrentCheckpoint < iIndex < this.m_pCheckpointList.Length)
		{
			CCoopSpawnEntry pCheckpoint;
			
			if(this.m_bStrictOrder)
			{
				for (int i = this.m_iCurrentCheckpoint+1; ; i++)
				{
					this.m_pCheckpointList.GetArray(i, pCheckpoint);
					
					if(pCheckpoint.m_bHasPortal)
					{
						this.CreatePortal(pCheckpoint.m_vecPortalPosition);
					}
					
					if(i == iIndex)
					{
						this.m_iCurrentCheckpoint = i;
						pCheckpoint.ActivatePost();
						return true;
					}
				}
			}
			else
			{
				this.m_iCurrentCheckpoint++;
				this.m_pCheckpointList.SwapAt(iIndex, this.m_iCurrentCheckpoint);
				this.m_pCheckpointList.GetArray(this.m_iCurrentCheckpoint, pCheckpoint);
				
				if(pCheckpoint.m_bHasPortal)
				{
					this.CreatePortal(pCheckpoint.m_vecPortalPosition);
				}
				pCheckpoint.ActivatePost();
				return true;
			}
		}
		return false;
	}
	
	void StripPlayer(CBasePlayer pPlayer)
	{
		if (IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT))
		{
			pPlayer.RemoveAllItems(!IsFeatureEnabled(FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT));
		}
	}
	
	void SpawnPlayerEquipment(CBasePlayer pPlayer)
	{
		float flStartTime = GetEngineTime();
		CCoopEquipment existingEquipment;
		existingEquipment.Initialize();

		if(strlen(g_szSteamIds[pPlayer.GetEntIndex()]) > 0 && g_pEquipmentManager.GetEquipment(g_szSteamIds[pPlayer.GetEntIndex()], existingEquipment)){
			this.EquipPlayer(pPlayer, existingEquipment);
			return;
		}

		this.EquipPlayer(pPlayer, this.m_pDefaultEquipment);
		LogDebug("SpawnPlayerEquipment took %f", GetEngineTime() - flStartTime);
	}

	void EquipPlayer(CBasePlayer pPlayer, CCoopEquipment equipment)
	{
		if(equipment.m_iHealth > 0)
			pPlayer.SetHealth(equipment.m_iHealth);
		if(equipment.m_iArmor > 0)
			pPlayer.SetArmor(equipment.m_iArmor);
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			if(equipment.m_iAmmoCount[i] >= 0)
				pPlayer.SetAmmoFromIndex(i, equipment.m_iAmmoCount[i]);
		}
		for (int i = 0; i < equipment.m_pWeaponList.Length; i++)
		{
			CCoopWeaponEntry pWeaponEntry;
			if (equipment.m_pWeaponList.GetArray(i, pWeaponEntry, sizeof(pWeaponEntry)))
			{
				pPlayer.GiveItem(pWeaponEntry.m_szClassname);
			}
		}
	}

	void SavePlayerEquipment()
	{
		float flStartTime = GetEngineTime();
		for(int iClient = 0; iClient < MAXPLAYERS + 1; iClient++)
		{
			if(strlen(g_szSteamIds[iClient]) == 0)
				continue;

			CBasePlayer pPlayer = CBasePlayer(iClient);
			if(!pPlayer.IsValid() || !pPlayer.IsInGame())
				continue;
			
			CCoopEquipment playerEquipment;
			playerEquipment.Initialize();

			playerEquipment.m_iHealth = pPlayer.GetHealth();
			playerEquipment.m_iArmor = pPlayer.GetArmor();

			for(int iWeapon = 0; iWeapon < MAX_WEAPONS; iWeapon++)
			{
				CBaseCombatWeapon pWeapon = pPlayer.GetWeaponFromIndex(iWeapon);
				char weaponClassName[MAX_CLASSNAME];
				if(pWeapon.IsValid() && pWeapon.GetClassname(weaponClassName, sizeof(weaponClassName)))
				{
					if(pPlayer.HasWeapon(weaponClassName))
					{
						CCoopWeaponEntry pWeaponEntry;
						strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), weaponClassName);
						pWeaponEntry.m_iPrimaryAmmo = -1;
						pWeaponEntry.m_iSecondaryAmmo = -1;
						playerEquipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
						Msg(iClient, weaponClassName);
					}
				}
			}

			for(int iAmmo = 0; iAmmo < MAX_AMMO_TYPES; iAmmo++)
			{
				playerEquipment.m_iAmmoCount[iAmmo] = pPlayer.GetAmmoFromIndex(iAmmo);
			}

			g_pEquipmentManager.StoreEquipment(g_szSteamIds[iClient], playerEquipment);

			LogDebug("SavePlayerEquipment took %f", GetEngineTime() - flStartTime);
		}
	}
	
	void SpawnPlayer(CBasePlayer pPlayer, bool bSpawnEquipment = true)
	{
		CCoopSpawnEntry pCheckpoint;
		if (this.GetCurrentCheckpoint(pCheckpoint))
		{
			pCheckpoint.TeleportPlayer(pPlayer, true);
		}
		if(bSpawnEquipment)
		{
			if (g_Engine != Engine_BlackMesa) // bm uses game_player_equip
			{
				this.StripPlayer(pPlayer);
			}
			this.SpawnPlayerEquipment(pPlayer);
		}
	}
	
	void CreatePortal(const float vecPosition[3])
	{
		CBaseEntity pParticle = CBaseEntity(CreateEntityByName("info_particle_system"));
		if (pParticle.IsValid())
		{
			pParticle.SetKeyValueStr("effect_name", "teleport_lambda_entrance");
			pParticle.SetKeyValueStr("start_active", "1");
			#define PORTAL_TOUCH_SIZE 16.0
			float mins[3] = {-PORTAL_TOUCH_SIZE, -PORTAL_TOUCH_SIZE, -PORTAL_TOUCH_SIZE};
			float maxs[3] = {PORTAL_TOUCH_SIZE, PORTAL_TOUCH_SIZE, PORTAL_TOUCH_SIZE};
			pParticle.SetMins(mins);
			pParticle.SetMaxs(maxs);
			pParticle.Teleport(vecPosition);
			pParticle.Spawn();
			pParticle.Activate();
			pParticle.SetSolidType(SOLID_BBOX);
			pParticle.SetSolidFlags(FSOLID_TRIGGER);
			pParticle.SetCollisionGroup(COLLISION_GROUP_DEBRIS_TRIGGER);
			SDKHook(pParticle.GetEntIndex(), SDKHook_StartTouchPost, Hook_TouchPortal);
		}
	}
	
	void ParseConfigFile(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szType[MAX_KEY];
				if (kv.GetSectionName(szType, sizeof(szType)))
				{
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
					}
					else if (strcmp(szType, "checkpoint", false) == 0)
					{
						this.ParseCheckpoint(kv);
					}
					else if (strcmp(szType, "equipment", false) == 0)
					{
						this.ParseEquipment(kv);
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}

	void ParseCheckpoint(KeyValues kv)
	{
		if(view_as<bool>(kv.GetNum("use_map_spawnpoint")))
		{
			this.AddSpawnPointFromMap();
		}
		if(view_as<bool>(kv.GetNum("use_map_autosave")))
		{
			this.m_bStrictOrder = false;
			this.AddCheckpointsFromMap();
		}
		else
		{
			this.m_bStrictOrder = view_as<bool>(kv.GetNum("strict_order", 1));
		}
		
		if (kv.GotoFirstSubKey(true))
		{
			char szSectionName[MAX_KEY];
			char szValue[MAX_VALUE];
			char szKey[MAX_KEY];
			
			do
			{
				if (kv.GetSectionName(szSectionName, sizeof(szSectionName)))
				{
					EVAL_CONDITIONS(szSectionName, kv, this.ParseCheckpoint(kv))
					CCoopSpawnEntry pEntry;
					pEntry.Initialize();
					
					if (kv.GotoFirstSubKey(false))
					{
						do
						{
							if (kv.GetSectionName(szKey, sizeof(szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
							{
								char szVector[3][MAX_FORMAT];
								kv.GetString(NULL_STRING, szValue, sizeof(szValue));
								
								if (strcmp(szKey, "output", false) == 0)
								{
									strcopy(pEntry.m_szOutputName, sizeof(pEntry.m_szOutputName), szValue);
								}
								else if (strcmp(szKey, "delay", false) == 0)
								{
									if (!StringToFloatEx(szValue, pEntry.m_flDelay))
										ThrowError("Delay parameter is not a float type");
								}
								else if (strcmp(szKey, "origin", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									if (!StringToFloatEx(szVector[0], pEntry.m_vecPosition[0]))
										ThrowError("Origin x parameter is not a float");
									if (!StringToFloatEx(szVector[1], pEntry.m_vecPosition[1]))
										ThrowError("Origin y parameter is not a float");
									if (!StringToFloatEx(szVector[2], pEntry.m_vecPosition[2]))
										ThrowError("Origin z parameter is not a float");
									pEntry.m_bHasPosition = true;
								}
								else if (strcmp(szKey, "angles", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									if (!StringToFloatEx(szVector[0], pEntry.m_vecAngles[0]))
										ThrowError("Angles x parameter is not a float");
									if (!StringToFloatEx(szVector[1], pEntry.m_vecAngles[1]))
										ThrowError("Angles y parameter is not a float");
									if (!StringToFloatEx(szVector[2], pEntry.m_vecAngles[2]))
										ThrowError("Angles z parameter is not a float");
									pEntry.m_bHasAngles = true;
								}
								else if (strcmp(szKey, "followid", false) == 0)
								{
									if (!(pEntry.m_pFollowEnt = this.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find followid entity %s", szValue);
								}
								else if (strcmp(szKey, "triggerid", false) == 0)	
								{
									if (!(pEntry.m_pTriggerEnt = this.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find triggerid entity %s", szValue);
								}
								else if (strcmp(szKey, "touchid", false) == 0)
								{
									if (!(pEntry.m_pTouchEnt = this.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find touchid entity %s", szValue);
								}
								else if (strcmp(szKey, "portal", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									if (!StringToFloatEx(szVector[0], pEntry.m_vecPortalPosition[0]))
										ThrowError("Portal x parameter is not a float");
									if (!StringToFloatEx(szVector[1], pEntry.m_vecPortalPosition[1]))
										ThrowError("Portal y parameter is not a float");
									if (!StringToFloatEx(szVector[2], pEntry.m_vecPortalPosition[2]))
										ThrowError("Portal z parameter is not a float");
									pEntry.m_bHasPortal = true;
								}
								else if (strcmp(szKey, "bringall", false) == 0)
								{
									pEntry.m_bBringAllPlayers = !!StringToInt(szValue);
								}
								else if (strcmp(szKey, "bringall_radius", false) == 0)
								{
									pEntry.m_fBringAllGraceRadius = StringToFloat(szValue);
								}
								else
								{
									ThrowError("Unknown checkpoint parameter type %s", szKey);
								}
							}
						}
						while (kv.GotoNextKey(false));
						kv.GoBack();
					}
					this.AddCheckpoint(pEntry);
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
	}
	
	CBaseEntity FindEntity(const char szValue[MAX_VALUE]) // accepts a int hammerid or a string targetname
	{
		int iHammerID;
		if (StringToIntEx(szValue, iHammerID))
		{
			CBaseEntity pEntity = CBaseEntity(FindEntityByHammerID(iHammerID));
			if(pEntity.IsValid())
			{
				return pEntity;
			}
		}
		return CBaseEntity(FindEntityByTargetname(-1, szValue));
	}
	
	void AddSpawnPointFromMap()
	{
		CCoopSpawnEntry pEntry; pEntry.Initialize();
		
		int ent = -1;
		if ((ent = FindEntityByClassname(ent, "info_player_start")) != -1)
		{
			pEntry.m_pFollowEnt = CBaseEntity(ent);
			this.AddCheckpoint(pEntry);
		}
	}
	
	void AddCheckpointsFromMap()
	{
		CCoopSpawnEntry pEntry; pEntry.Initialize();
		
		int ent = -1;
		while ((ent = FindEntityByClassname(ent, "trigger_autosave")) != -1)
		{
			pEntry.m_pTouchEnt = CBaseEntity(ent);
			this.AddCheckpoint(pEntry);
		}
	}
	
	void ParseEquipment(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szKey[MAX_KEY];
				char szValue[MAX_VALUE];
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, this.ParseEquipment(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						kv.GetString(NULL_STRING, szValue, sizeof(szValue));
						if (strcmp(szKey, "health", false) == 0)
						{
							if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iHealth))
								ThrowError("Health parameter is not a integer");
						}
						else if (strcmp(szKey, "armor", false) == 0)
						{
							if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iArmor))
								ThrowError("Armor parameter is not a integer");
						}
						else if (strcmp(szKey, "item", false) == 0)
						{
							CCoopWeaponEntry pWeaponEntry;
							strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), szValue);
							pWeaponEntry.m_iPrimaryAmmo = -1;
							pWeaponEntry.m_iSecondaryAmmo = -1;
							this.m_pDefaultEquipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
						}
						else
						{
							ThrowError("Could not find parameter type %s", szKey);
						}
					}
					else
					{
						if (strcmp(szKey, "ammo", false) == 0)
						{
							if (kv.GotoFirstSubKey(false))
							{
								do
								{
									char szAmmoKey[MAX_KEY];
									char szAmmoValue[MAX_VALUE];
									if (kv.GetSectionName(szAmmoKey, sizeof(szAmmoKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
									{
										kv.GetString(NULL_STRING, szAmmoValue, sizeof(szAmmoValue));
										
										if (strcmp(szAmmoKey, "9mm", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_9MM]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "357", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_357]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "bolts", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_BOLTS]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "shells", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SHELLS]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "energy", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_ENERGY]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "mp5nade", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_MP5NADE]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "rockets", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_ROCKETS]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "nades", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_NADES]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "satchels", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SATCHELS]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "tripmines", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_TRIPMINES]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "hivegun", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_HIVEGUN]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "snarks", false) == 0)
										{
											if (!StringToIntEx(szValue, this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SNARKS]))
												ThrowError("Ammo parameter %s is not a integer", szAmmoKey);
										}
										else
										{
											ThrowError("Could not find ammo parameter type %s", szAmmoKey);
										}
									}
								}
								while (kv.GotoNextKey(false));
								kv.GoBack();
							}
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	bool AddSpawnItem(const char[] szItem, bool bAllowDuplicates = false)
	{
		if(!bAllowDuplicates)
		{
			for (int i = 0; i < this.m_pDefaultEquipment.m_pWeaponList.Length; i++)
			{
				CCoopWeaponEntry pWeaponEntry;
				if (this.m_pDefaultEquipment.m_pWeaponList.GetArray(i, pWeaponEntry, sizeof(pWeaponEntry)))
				{
					// does basic checking of item_weapon vs weapon_ strings
					if(StrContains(pWeaponEntry.m_szClassname, szItem, false) != -1 || StrContains(szItem, pWeaponEntry.m_szClassname, false) != -1)
					{
						return false;
					}
				}
			}
		}
		CCoopWeaponEntry pWeaponEntry;
		strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), szItem);
		pWeaponEntry.m_iPrimaryAmmo = -1;
		pWeaponEntry.m_iSecondaryAmmo = -1;
		this.m_pDefaultEquipment.m_pWeaponList.PushArray(pWeaponEntry);
		return true;
	}
}

CCoopSpawn g_SpawnSystem;

public void Hook_OutputCheckpoint(const char[] szName, int iCaller, int iActivator, float flDelay)
{
	CBaseEntity pCaller = CBaseEntity(iCaller);
	CBaseEntity pActivator = CBaseEntity(iActivator);
	CCoopSpawnEntry pCheckpoint;
	int iIndex = g_SpawnSystem.m_iCurrentCheckpoint + 1;
	
	while ((iIndex = g_SpawnSystem.FindCheckpointByOutput(pCaller, szName, pCheckpoint, iIndex)) != -1)
	{
		if(pActivator.IsClassPlayer() && pCheckpoint.ActivatePre(view_as<CBasePlayer>(pActivator)))
		{
			g_SpawnSystem.m_pCheckpointList.SetArray(iIndex, pCheckpoint);
		}
		
		DataPack dp; CreateDataTimer(pCheckpoint.m_flDelay, Timer_Checkpoint, dp, TIMER_FLAG_NO_MAPCHANGE);
		dp.WriteCell(iIndex); dp.WriteCell(pActivator);
		
		iIndex++;
	}
}

public void Hook_TouchCheckpoint(int iEntity, int iOther)
{
	CBasePlayer pActivator = CBasePlayer(iOther);
	if (pActivator.IsClassPlayer())
	{
		CBaseEntity pTouched = CBaseEntity(iEntity);
		CCoopSpawnEntry pCheckpoint;
		int iIndex = g_SpawnSystem.m_iCurrentCheckpoint + 1;
		
		while ((iIndex = g_SpawnSystem.FindCheckpointByTouchEnt(pTouched, pCheckpoint, iIndex)) != -1)
		{
			if(pCheckpoint.ActivatePre(pActivator))
			{
				g_SpawnSystem.m_pCheckpointList.SetArray(iIndex, pCheckpoint);
			}
			
			DataPack dp; CreateDataTimer(pCheckpoint.m_flDelay, Timer_Checkpoint, dp, TIMER_FLAG_NO_MAPCHANGE);
			dp.WriteCell(iIndex); dp.WriteCell(pActivator);
			
			iIndex++;
		}

		g_SpawnSystem.SavePlayerEquipment();
		SDKUnhook(iEntity, SDKHook_StartTouchPost, Hook_TouchCheckpoint);
	}
}

public Action Timer_Checkpoint(Handle timer, DataPack dp)
{
	dp.Reset();
	int iCheckpointIndex = dp.ReadCell();
	CBasePlayer pActivator = dp.ReadCell();
	g_SpawnSystem.ActivateCheckpoint(iCheckpointIndex, pActivator);
	return Plugin_Handled;
}

public void Hook_TouchPortal(int iEntity, int iOther)
{
	CBasePlayer pPlayer = CBasePlayer(iOther);
	if (pPlayer.IsClassPlayer())
	{
		CCoopSpawnEntry pCheckpoint;
		if (g_SpawnSystem.GetCurrentCheckpoint(pCheckpoint) && pCheckpoint.TeleportPlayer(pPlayer))
		{
			pPlayer.PlayGameSound(SND_PORTAL);
		}
	}
}
