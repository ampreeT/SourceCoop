#if defined _srccoop_checkpoint_included
 #endinput
#endif
#define _srccoop_checkpoint_included

#define SND_BRINGALL "sfx.beep5"
#define SND_PORTAL "xen.energy_hit"

enum struct CCoopSpawnEntry
{
	float m_vecPosition[3];
	float m_vecAngles[3];
	float m_flDelay;
	CBaseEntity m_pTriggerEnt;
	CBaseEntity m_pFollowEnt;
	char m_szOutputName[MAX_FORMAT];
	bool m_bHasPortal;
	float m_vecPortalPosition[3];
	bool m_bBringAllPlayers;
	float m_fBringAllGraceRadius;
	
	void Initialize()
	{
		this.m_pTriggerEnt = CBaseEntity(-1);
		this.m_pFollowEnt = CBaseEntity(-1);
	}
	
	bool TeleportPlayer(CBlackMesaPlayer pPlayer, bool bForceTeleport = false, bool bIsBringAllTP = false)
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
			if(res == YES_MUSTDUCK)
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
	void Clear()
	{
		this.m_pWeaponList.Clear();
		this.m_bHasSuit = false;
		this.m_iHealth = -1;
		this.m_iArmor = -1;
		
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			this.m_iAmmoCount[i] = -1;
		}
		
		strcopy(this.m_szActiveWeapon, MAX_CLASSNAME, "");
	}
}

enum struct CCoopSpawn
{
	Array_t m_pCheckpointList;
	int m_iCurrentCheckpoint;
	CCoopEquipment m_pDefaultEquipment;
	
	void Clear()
	{
		this.m_iCurrentCheckpoint = 0;
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
		if(this.m_pCheckpointList.Length > iIndex)
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
	int FindCheckpointByOutput(CBaseEntity pCaller, const char[] szOutput, CCoopSpawnEntry pCheckpoint)
	{
		CCoopSpawnEntry pEntry;
		for (int i = this.m_iCurrentCheckpoint+1; i < this.m_pCheckpointList.Length; i++)
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
	void AddCheckpoint(CCoopSpawnEntry pCheckpoint)
	{
		int iIndex = this.m_pCheckpointList.PushArray(pCheckpoint);
		if (pCheckpoint.m_pTriggerEnt.IsValid())
		{
			pCheckpoint.m_pTriggerEnt.HookOutput(pCheckpoint.m_szOutputName, Callback_Checkpoint, true);
		}
		else
		{
			this.ActivateCheckpoint(iIndex);
		}
	}
	bool ActivateCheckpoint(int iIndex)
	{
		if(this.m_iCurrentCheckpoint < iIndex < this.m_pCheckpointList.Length)
		{
			for (int i = this.m_iCurrentCheckpoint+1; ; i++)
			{
				CCoopSpawnEntry pCheckpoint; this.m_pCheckpointList.GetArray(i, pCheckpoint);
				
				if(pCheckpoint.m_bHasPortal)
				{
					this.CreatePortal(pCheckpoint.m_vecPortalPosition);
				}
				if(i == iIndex)
				{
					this.m_iCurrentCheckpoint = i;
					if(pCheckpoint.m_bBringAllPlayers)
					{
						for (int pp = 0; pp <= MaxClients; pp++) {
							CBlackMesaPlayer pPlayer = CBlackMesaPlayer(pp);
							if(pPlayer.IsValid() && pPlayer.IsAlive())
							{
								if(pCheckpoint.TeleportPlayer(pPlayer, true, true))
									pPlayer.PlayGameSound(SND_BRINGALL);
							}
						}
					}
					return true;
				}
			}
		}
		return false;
	}
	void SpawnPlayerEquipment(CBlackMesaPlayer pPlayer)
	{
		if(this.m_pDefaultEquipment.m_iHealth > 0)
			pPlayer.SetHealth(this.m_pDefaultEquipment.m_iHealth);
		if(this.m_pDefaultEquipment.m_iArmor > 0)
			pPlayer.SetArmor(this.m_pDefaultEquipment.m_iArmor);
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			if(this.m_pDefaultEquipment.m_iAmmoCount[i] >= 0)
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
			pPlayer.SetSuit(true);
		}
	}
	void SpawnPlayer(CBlackMesaPlayer pPlayer, bool bSpawnEquipment = true)
	{
		CCoopSpawnEntry pCheckpoint;
		// pickup possible spawn items and make sure player spawned in if second teleport is blocked
		if(this.GetCheckpointByIndex(0, pCheckpoint))
		{
			pCheckpoint.TeleportPlayer(pPlayer, true);
			if(this.m_iCurrentCheckpoint > 0)
			{
				RequestFrame(RequestSpawnPlayer, pPlayer);
			}
		}
		if(bSpawnEquipment) RequestFrame(RequestSpawnEquipment, pPlayer);
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
		if (kv.GotoFirstSubKey(true))
		{
			do
			{
				char szSectionName[MAX_KEY];
				if (kv.GetSectionName(szSectionName, sizeof(szSectionName)))
				{
					if(StrEqual(szSectionName, "#if", false))
					{
						if(ParseConditions(kv) && kv.JumpToKey("#then"))
						{
							this.ParseCheckpoint(kv);
							kv.GoBack();
						}
						continue;
					}
					
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

CCoopSpawn g_SpawnSystem;

public void RequestSpawnPlayer(CBlackMesaPlayer pPlayer)
{
	if(pPlayer.IsValid() && pPlayer.IsAlive())
	{
		CCoopSpawnEntry pCheckpoint;
		if(g_SpawnSystem.GetCurrentCheckpoint(pCheckpoint))
		{
			pCheckpoint.TeleportPlayer(pPlayer);
		}
	}
}

public void RequestSpawnEquipment(CBlackMesaPlayer pPlayer)
{
	if(pPlayer.IsValid() && pPlayer.IsAlive())
	{
		g_SpawnSystem.SpawnPlayerEquipment(pPlayer);
	}
}

public void Callback_Checkpoint(const char[] szName, int iCaller, int iActivator, float flDelay)
{
	CCoopSpawnEntry pCheckpoint; int iIndex;
	if((iIndex = g_SpawnSystem.FindCheckpointByOutput(CBaseEntity(iCaller), szName, pCheckpoint)) != -1)
	{
		CreateTimer(pCheckpoint.m_flDelay, Callback_CheckpointTimer, iIndex, TIMER_FLAG_NO_MAPCHANGE);
	}
}

public Action Callback_CheckpointTimer(Handle timer, int iCheckpointIndex)
{
	g_SpawnSystem.ActivateCheckpoint(iCheckpointIndex);
}

public void Hook_TouchPortal(int entity, int other)
{
	CBlackMesaPlayer pPlayer = CBlackMesaPlayer(other);
	if(pPlayer.IsClassPlayer())
	{
		CCoopSpawnEntry pCheckpoint;
		if(g_SpawnSystem.GetCurrentCheckpoint(pCheckpoint) && pCheckpoint.TeleportPlayer(pPlayer))
		{
			pPlayer.PlayGameSound(SND_PORTAL);
		}
	}
}