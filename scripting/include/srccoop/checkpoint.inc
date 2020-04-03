#define COOP_EQUIPMENT_DEFAULT_HEALTH 100
#define COOP_EQUIPMENT_DEFAULT_ARMOR 0

enum struct CCoopSpawnEntry
{
	float m_vecPosition[3];
	float m_vecAngles[3];
	float m_flDelay;
	CBaseEntity m_pTriggerEnt;
	CBaseEntity m_pFollowEnt;
	bool m_bHasPortal;
	float m_vecPortalPosition[3];
	
	void Initialize()
	{
		this.m_pTriggerEnt = CBaseEntity(-1);
		this.m_pFollowEnt = CBaseEntity(-1);
	}
	
	void TeleportPlayer(CBlackMesaPlayer pPlayer)
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
		pPlayer.Teleport(vecPosition, vecAngles, view_as<float>({0.0, 0.0, 0.0}));
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
	bool m_bHasSuit;
	int m_iHealth;
	int m_iArmor;
	int m_iAmmoCount[MAX_AMMO_TYPES];
	char m_szActiveWeapon[MAX_CLASSNAME];
	
	void Initialize()
	{
		this.m_pWeaponList = new Array_t(CreateArray(sizeof(CCoopWeaponEntry)));
	}
	bool IsValid()
	{
		return this.m_pWeaponList.IsValid();
	}
	void Clear()
	{
		this.m_pWeaponList.Clear();
		this.m_bHasSuit = false;
		this.m_iHealth = COOP_EQUIPMENT_DEFAULT_HEALTH;
		this.m_iArmor = COOP_EQUIPMENT_DEFAULT_ARMOR;
		
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			this.m_iAmmoCount[i] = 0;
		}
		
		strcopy(this.m_szActiveWeapon, MAX_CLASSNAME, "");
	}
}

enum struct CCoopSpawn
{
	CCoopSpawnEntry pInitialSpawnEntry;
	CCoopSpawnEntry pCurrentSpawnEntry;
	Array_t m_pCheckpointList;
	EntityOutput m_fnCheckpointCallback;
	CCoopEquipment m_pDefaultEquipment;
	
	void Clear()
	{
		CCoopSpawnEntry pNewEntry;
		pNewEntry.Initialize();
		this.pInitialSpawnEntry = pNewEntry;
		this.pCurrentSpawnEntry = pNewEntry;
		this.m_pCheckpointList.Clear();
		this.m_pDefaultEquipment.Clear();
	}
	void Initialize()
	{
		this.m_fnCheckpointCallback = Callback_Checkpoint;
		this.m_pCheckpointList = new Array_t(CreateArray(sizeof(CCoopSpawnEntry)));
		if (!this.m_pCheckpointList.IsValid())
			ThrowError("Was unable to create CCoopSpawnEntry on initialization");
		
		this.m_pDefaultEquipment.Initialize();
		if (!this.m_pDefaultEquipment.IsValid())
			ThrowError("Was unable to create default equipment on initialization");
	}
	void SpawnPlayerEquipment(CBlackMesaPlayer pPlayer)
	{
		pPlayer.SetHealth(this.m_pDefaultEquipment.m_iHealth);
		pPlayer.SetArmor(this.m_pDefaultEquipment.m_iArmor);
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			pPlayer.SetAmmoFromIndex(i, this.m_pDefaultEquipment.m_iAmmoCount[i]);
		}
		for (int i = 0; i < this.m_pDefaultEquipment.m_pWeaponList.Length; i++)
		{
			CCoopWeaponEntry pWeaponEntry;
			if (this.m_pDefaultEquipment.m_pWeaponList.GetArray(i, pWeaponEntry, sizeof(pWeaponEntry)))
			{
				pPlayer.GiveItem(pWeaponEntry.m_szClassname);
			}
		}
		
		if (this.m_pDefaultEquipment.m_bHasSuit)
		{
			CreateTimer(0.0, Callback_GivePlayerSuit, pPlayer);
		}
	}
	void SpawnPlayer(CBlackMesaPlayer pPlayer, bool bSpawnEquipment = true)
	{
		if(g_pLevelLump.m_bInstanceItems)
		{
			// pickup possible spawn items
			this.pInitialSpawnEntry.TeleportPlayer(pPlayer);
			RequestFrame(RequestSpawnPlayer, pPlayer);
		}
		else
		{
			this.pCurrentSpawnEntry.TeleportPlayer(pPlayer);
		}
		if(bSpawnEquipment) this.SpawnPlayerEquipment(pPlayer);
	}
	void AddSpawnEntry(CCoopSpawnEntry pSpawnEntry, const char[] szOutputType = "")
	{
		if (pSpawnEntry.m_pTriggerEnt.IsValid())
		{
			pSpawnEntry.m_pTriggerEnt.HookOutput(szOutputType, this.m_fnCheckpointCallback, true);
			this.m_pCheckpointList.PushArray(pSpawnEntry, sizeof(pSpawnEntry));
		}
		else
		{
			this.SetSpawnEntry(pSpawnEntry, true);
		}
	}
	void SetSpawnEntry(CCoopSpawnEntry pSpawnEntry, bool bIsInitial = false)
	{
		this.pCurrentSpawnEntry = pSpawnEntry;
		if(bIsInitial)
		{
			this.pInitialSpawnEntry = pSpawnEntry;
		}
	}
	void CreatePortal(const float vecPosition[3])
	{
		CBaseEntity pParticle = CBaseEntity(CreateEntityByName("info_particle_system"));
		if(pParticle.IsValidIndex())
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
	void EraseCheckpoints(int iCount, bool bSpawnPortals = false)
	{
		while (iCount > 0 && this.m_pCheckpointList.Length)
		{
			CCoopSpawnEntry pEntry;
			if(bSpawnPortals && this.m_pCheckpointList.GetArray(0, pEntry, sizeof(pEntry)) && pEntry.m_bHasPortal)
			{
				this.CreatePortal(pEntry.m_vecPortalPosition);
			}
			this.m_pCheckpointList.Erase(0);
			iCount--;
		}
	}
	void ParseConfigFile(const char[] szMapName)
	{
		this.Clear();
		
		char szMapPath[MAX_FORMAT];
		if (!(BuildPath(Path_SM, szMapPath, sizeof(szMapPath), "data/srccoop/%s.edt", szMapName) && FileExists(szMapPath)))
			ThrowError("Couldn't find map file %s", szMapName);

		KeyValues kv = new KeyValues(szMapName);
		if (kv == null)
			ThrowError("Couldn't create KeyValues for %s", szMapName);
		
		char szRoot[MAX_MAPNAME];
		if (!(kv.ImportFromFile(szMapPath) && kv.GetSectionName(szRoot, sizeof(szRoot)) && strcmp(szRoot, "config") == 0))
			ThrowError("Couldn't import %s into KeyValues", szMapName);
		
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
						if (kv.GotoFirstSubKey(true))
						{
							do	// ignore section name; reserved for naming purposes for front-end
							{
								char szOutputType[MAX_FORMAT];
								CCoopSpawnEntry pEntry;
								pEntry.Initialize();
								
								if (kv.GotoFirstSubKey(false))
								{
									do
									{
										int iHammerID = 0;
										char szKey[MAX_KEY];
										char szValue[MAX_VALUE];
										if (kv.GetSectionName(szKey, sizeof(szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
										{
											char szVector[3][MAX_FORMAT];
											kv.GetString(NULL_STRING, szValue, sizeof(szValue));
											
											if (strcmp(szKey, "output", false) == 0)
											{
												strcopy(szOutputType, sizeof(szOutputType), szValue);
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
											}
											else if (strcmp(szKey, "angles", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
											{
												if (!StringToFloatEx(szVector[0], pEntry.m_vecAngles[0]))
													ThrowError("Angles x parameter is not a float");
												if (!StringToFloatEx(szVector[1], pEntry.m_vecAngles[1]))
													ThrowError("Angles y parameter is not a float");
												if (!StringToFloatEx(szVector[2], pEntry.m_vecAngles[2]))
													ThrowError("Angles z parameter is not a float");
											}
											else if (strcmp(szKey, "followid", false) == 0)		// accepts a int hammerid or a string targetname
											{
												if (StringToIntEx(szValue, iHammerID))
													pEntry.m_pFollowEnt = CBaseEntity(FindEntityByHammerID(iHammerID));
												else
													pEntry.m_pFollowEnt = CBaseEntity(FindEntityByTargetname(-1, szValue));
												if (!pEntry.m_pFollowEnt.IsValid())
													ThrowError("Couldn't find followid entity %s", szValue);
											}
											else if (strcmp(szKey, "triggerid", false) == 0)	// accepts a int hammerid or a string targetname
											{
												if (StringToIntEx(szValue, iHammerID))
													pEntry.m_pTriggerEnt = CBaseEntity(FindEntityByHammerID(iHammerID));
												else
													pEntry.m_pTriggerEnt = CBaseEntity(FindEntityByTargetname(-1, szValue));
												if (!pEntry.m_pTriggerEnt.IsValid())
													ThrowError("Couldn't find triggerid entity %s", szValue);
											}
											else if (strcmp(szKey, "portal", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
											{
												pEntry.m_bHasPortal = true;
												if (!StringToFloatEx(szVector[0], pEntry.m_vecPortalPosition[0]))
													ThrowError("Portal x parameter is not a float");
												if (!StringToFloatEx(szVector[1], pEntry.m_vecPortalPosition[1]))
													ThrowError("Portal y parameter is not a float");
												if (!StringToFloatEx(szVector[2], pEntry.m_vecPortalPosition[2]))
													ThrowError("Portal z parameter is not a float");
											}
											else
											{
												ThrowError("Could not find parameter type %s", szKey);
											}
										}
									}
									while (kv.GotoNextKey(false));
									kv.GoBack();
								}
								this.AddSpawnEntry(pEntry, szOutputType);
							}
							while (kv.GotoNextKey(true));
							kv.GoBack();
						}
					}
					else if (strcmp(szType, "equipment", false) == 0)
					{
						if (kv.GotoFirstSubKey(false))
						{
							do
							{
								char szKey[MAX_KEY];
								char szValue[MAX_VALUE];
								if (kv.GetSectionName(szKey, sizeof(szKey)))
								{
									if (kv.GetDataType(NULL_STRING) != KvData_None)
									{
										kv.GetString(NULL_STRING, szValue, sizeof(szValue));
										if (strcmp(szKey, "suit", false) == 0)
										{
											int iValue = 0;
											bool bIsTrue = strcmp(szValue, "true", false) == 0;
											bool bIsFalse = strcmp(szValue, "false", false) == 0;
											
											if (bIsTrue || bIsFalse)
											{
												this.m_pDefaultEquipment.m_bHasSuit = bIsTrue;
											}
											else if (StringToIntEx(szValue, iValue))
											{
												this.m_pDefaultEquipment.m_bHasSuit = view_as<bool>(iValue);
											}
											else
											{
												ThrowError("Suit parameter is not a string or integer");
											}
										}
										else if (strcmp(szKey, "health", false) == 0)
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
										else
										{
											ThrowError("Could not find parameter type %s", szKey);
										}
									}
								}
							}
							while (kv.GotoNextKey(false));
							kv.GoBack();
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
		
		delete kv;
	}
}

CCoopSpawn g_SpawnSystem;

public Action Callback_GivePlayerSuit(Handle hTimer, CBlackMesaPlayer pPlayer)
{
	if (pPlayer.IsValid())
	{
		pPlayer.GiveItem("item_suit");
	}
}

public void RequestSpawnPlayer(CBlackMesaPlayer pPlayer)
{
	if(pPlayer.IsValid() && pPlayer.IsAlive())
	{
		g_SpawnSystem.pCurrentSpawnEntry.TeleportPlayer(pPlayer);
	}
}

int FindSpawnEntry(const CBaseEntity pCaller, CCoopSpawnEntry pEntry)
{
	CCoopSpawnEntry _pEntry;
	for (int i = 0; i < g_SpawnSystem.m_pCheckpointList.Length; i++)
	{
		if (g_SpawnSystem.m_pCheckpointList.GetArray(i, _pEntry, sizeof(_pEntry)))
		{
			if (pCaller == _pEntry.m_pTriggerEnt)
			{
				pEntry = _pEntry;
				return i;
			}
		}
	}
	return -1;
}

public void Callback_Checkpoint(const char[] szName, int iCaller, int iActivator, float flDelay)
{
	CBaseEntity pCaller = CBaseEntity(iCaller);
	CCoopSpawnEntry pEntry;
	if(FindSpawnEntry(pCaller, pEntry) != -1)
	{
		CreateTimer(pEntry.m_flDelay, Callback_CheckpointTimer, pCaller, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Action Callback_CheckpointTimer(Handle timer, CBaseEntity pCaller)
{
	if(!pCaller.IsValid())
	{
		return;
	}
	CCoopSpawnEntry pEntry;
	int iEntriesToKill;
	if((iEntriesToKill = FindSpawnEntry(pCaller, pEntry)) != -1)
	{
		g_SpawnSystem.EraseCheckpoints(iEntriesToKill + 1, true);
		g_SpawnSystem.SetSpawnEntry(pEntry);
	}
}

public void Hook_TouchPortal(int entity, int other)
{
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(other);
	if(pPlayer.IsClassPlayer())
	{
		g_SpawnSystem.SpawnPlayer(pPlayer, false);
		pPlayer.PlayGameSound("xen.energy_hit");
	}
}