#define COOP_EQUIPMENT_DEFAULT_HEALTH 100
#define COOP_EQUIPMENT_DEFAULT_ARMOR 0

enum struct CCoopSpawnEntry
{
	float m_vecPosition[3];
	float m_vecAngles[3];
	float m_flDelay;
	CBaseEntity m_pTriggerEnt;
	CBaseEntity m_pFollowEnt;
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
		this.m_pWeaponList = Array_t(CreateArray(sizeof(CCoopSpawnEntry)));
	}
	bool IsValid()
	{
		return this.m_pWeaponList.IsValid();
	}
	void Clear()
	{
		this.m_pWeaponList.Clear();
		this.m_bHasSuit = true;
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
	float m_vecPosition[3];
	float m_vecAngles[3];
	CBaseEntity m_pFollowEnt;
	Array_t m_pCheckpointList;
	EntityOutput m_fnCheckpointCallback;
	CCoopEquipment m_pDefaultEquipment;
	
	void Clear()
	{
		this.m_vecPosition[0] = 0.0;
		this.m_vecPosition[1] = 0.0;
		this.m_vecPosition[2] = 0.0;
		
		this.m_vecAngles[0] = 0.0;
		this.m_vecAngles[1] = 0.0;
		this.m_vecAngles[2] = 0.0;
		
		this.m_pFollowEnt = CBaseEntity(-1);
		
		this.m_pCheckpointList.Clear();
		
		this.m_pDefaultEquipment.Clear();
	}
	void Initialize(const EntityOutput fnCheckpointCallback)
	{
		this.m_fnCheckpointCallback = fnCheckpointCallback;
		this.m_pCheckpointList = Array_t(CreateArray(sizeof(CCoopSpawnEntry)));
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
		for (int i = 0; i < this.m_pDefaultEquipment.m_pWeaponList.Size(); i++)
		{
			CCoopWeaponEntry pWeaponEntry;
			if (this.m_pDefaultEquipment.m_pWeaponList.GetArray(i, pWeaponEntry, sizeof(pWeaponEntry)))
			{
				pPlayer.GiveWeapon(pWeaponEntry.m_szClassname);
			}
		}
		
		if (!this.m_pDefaultEquipment.m_bHasSuit)
		{
			CreateTimer(0.0, Callback_RemovePlayerSuit, pPlayer);
		}
	}
	void SpawnPlayer(CBlackMesaPlayer pPlayer)
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
			vecPosition[0] = this.m_vecPosition[0];
			vecPosition[1] = this.m_vecPosition[1];
			vecPosition[2] = this.m_vecPosition[2];
			
			vecAngles[0] = this.m_vecAngles[0];
			vecAngles[1] = this.m_vecAngles[1];
			vecAngles[2] = this.m_vecAngles[2];
		}
		
		pPlayer.Teleport(vecPosition, vecAngles);
		this.SpawnPlayerEquipment(pPlayer);
	}
	void SetSpawnLocation(const float vecPosition[3], const float vecAngles[3], CBaseEntity pEntity = view_as<CBaseEntity>(-1))
	{
		this.m_vecPosition[0] = vecPosition[0];
		this.m_vecPosition[1] = vecPosition[1];
		this.m_vecPosition[2] = vecPosition[2];
		
		this.m_vecAngles[0] = vecAngles[0];
		this.m_vecAngles[1] = vecAngles[1];
		this.m_vecAngles[2] = vecAngles[2];
		
		this.m_pFollowEnt = pEntity;
	}
	void EraseCheckpoints(int iCount)
	{
		while (iCount > 0 && this.m_pCheckpointList.Size())
		{
			this.m_pCheckpointList.Remove(0);
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
		if (!(kv.ImportFromFile(szMapPath) && kv.GetSectionName(szRoot, sizeof(szRoot)) && strcmp(szRoot, szMapName) == 0))
			ThrowError("Couldn't import %s into KeyValues", szMapName);
		
		if (kv.GotoFirstSubKey(true))
		{
			do
			{
				char szType[MAX_KEY];
				if (kv.GetSectionName(szType, sizeof(szType)))
				{
					if (strcmp(szType, "checkpoint", false) == 0)
					{
						if (kv.GotoFirstSubKey(true))
						{
							bool bGotFirstEntry = false;
							
							do	// ignore section name; reserved for naming purposes for front-end
							{
								char szOutputType[MAX_FORMAT];
								CCoopSpawnEntry pEntry;
								
								if (kv.GotoFirstSubKey(false))
								{
									do
									{
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
												pEntry.m_flDelay = StringToFloat(szValue);
											}
											else if (strcmp(szKey, "origin", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
											{
												pEntry.m_vecPosition[0] = StringToFloat(szVector[0]);
												pEntry.m_vecPosition[1] = StringToFloat(szVector[1]);
												pEntry.m_vecPosition[2] = StringToFloat(szVector[2]);
											}
											else if (strcmp(szKey, "angles", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
											{
												pEntry.m_vecAngles[0] = StringToFloat(szVector[0]);
												pEntry.m_vecAngles[1] = StringToFloat(szVector[1]);
												pEntry.m_vecAngles[2] = StringToFloat(szVector[2]);
											}
											else if (strcmp(szKey, "followid", false) == 0)
											{
												int iHammerID = 0;
												if (StringToIntEx(szValue, iHammerID))
												{
													pEntry.m_pFollowEnt = CBaseEntity(FindEntityByHammerID(iHammerID));
												}
												else
												{
													pEntry.m_pFollowEnt = CBaseEntity(FindEntityByTargetname(-1, szValue));
												}
											}
											else if (strcmp(szKey, "triggerid", false) == 0)
											{
												int iHammerID = 0;
												if (StringToIntEx(szValue, iHammerID))
												{
													pEntry.m_pTriggerEnt = CBaseEntity(FindEntityByHammerID(iHammerID));
												}
												else
												{
													pEntry.m_pTriggerEnt = CBaseEntity(FindEntityByTargetname(-1, szValue));
												}
											}
										}
									}
									while (kv.GotoNextKey(false));
									kv.GoBack();
								}
								
								if (!bGotFirstEntry)
								{
									this.SetSpawnLocation(pEntry.m_vecPosition, pEntry.m_vecAngles, pEntry.m_pFollowEnt);
									bGotFirstEntry = true;
								}
								else if (pEntry.m_pTriggerEnt.IsValid())
								{
									pEntry.m_pTriggerEnt.HookOutput(szOutputType, this.m_fnCheckpointCallback, true);
									this.m_pCheckpointList.PushArray(pEntry, sizeof(pEntry));
								}
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
											this.m_pDefaultEquipment.m_bHasSuit = view_as<bool>(StringToInt(szValue));
										}
										else if (strcmp(szKey, "health", false) == 0)
										{
											this.m_pDefaultEquipment.m_iHealth = StringToInt(szValue);
										}
										else if (strcmp(szKey, "armor", false) == 0)
										{
											this.m_pDefaultEquipment.m_iArmor = StringToInt(szValue);
										}
										else if (strcmp(szKey, "item", false) == 0)
										{
											CCoopWeaponEntry pWeaponEntry;
											strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), szValue);
											pWeaponEntry.m_iPrimaryAmmo = -1;
											pWeaponEntry.m_iSecondaryAmmo = -1;
											this.m_pDefaultEquipment.m_pWeaponList.PushArray(pWeaponEntry, sizeof(pWeaponEntry));
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
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_9MM] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "357", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_357] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "bolts", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_BOLTS] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "shells", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SHELLS] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "energy", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_ENERGY] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "mp5nade", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_MP5NADE] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "rockets", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_ROCKETS] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "nades", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_NADES] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "satchels", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SATCHELS] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "tripmines", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_TRIPMINES] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "hivegun", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_HIVEGUN] = StringToInt(szAmmoValue);
														}
														else if (strcmp(szAmmoKey, "snarks", false) == 0)
														{
															this.m_pDefaultEquipment.m_iAmmoCount[AMMO_SNARKS] = StringToInt(szAmmoValue);
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
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
		
		delete kv;
	}
}

public Action Callback_RemovePlayerSuit(Handle hTimer, CBlackMesaPlayer pPlayer)
{
	if (pPlayer.IsValid())
	{
		pPlayer.SetSuit(false);
	}
}