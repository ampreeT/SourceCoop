#pragma newdecls required
#pragma semicolon 1

enum struct CCoopPortal
{
	float m_vecPosition[3];
	CParticleSystem m_pParticle;

	void Initialize()
	{
		this.m_pParticle = NULL_CBASEENTITY;
	}

	bool Spawn()
	{
		this.m_pParticle = CParticleSystem.Create(Conf.PORTAL_PARTICLE);
		if (this.m_pParticle.IsValid())
		{
			float mins[3], maxs[3];
			mins[0] = mins[1] = mins[2] = -Conf.PORTAL_TOUCH_SIZE;
			maxs[0] = maxs[1] = maxs[2] = Conf.PORTAL_TOUCH_SIZE;
			this.m_pParticle.SetMins(mins);
			this.m_pParticle.SetMaxs(maxs);
			this.m_pParticle.Teleport(this.m_vecPosition);
			this.m_pParticle.Spawn();
			this.m_pParticle.Activate();
			this.m_pParticle.SetSolidType(SOLID_BBOX);
			this.m_pParticle.m_usSolidFlags = FSOLID_TRIGGER;
			this.m_pParticle.SetCollisionGroup(COLLISION_GROUP_DEBRIS_TRIGGER);
			SDKHook(this.m_pParticle.entindex, SDKHook_StartTouchPost, Hook_TouchPortal);
			return true;
		}
		return false;
	}

	bool IsSpawned()
	{
		return this.m_pParticle.IsValid();
	}

	void Despawn()
	{
		this.m_pParticle.Kill();
		this.m_pParticle = NULL_CBASEENTITY;
	}

	bool Overlaps(CCoopPortal pOther)
	{
		float minsA[3], maxsA[3], minsB[3], maxsB[3];
		minsA[0] = minsA[1] = minsA[2] = minsB[0] = minsB[1] = minsB[2] = -Conf.PORTAL_TOUCH_SIZE;
		maxsA[0] = maxsA[1] = maxsA[2] = maxsB[0] = maxsB[1] = maxsB[2] = Conf.PORTAL_TOUCH_SIZE;
		AddVectors(minsA, this.m_vecPosition, minsA);
		AddVectors(maxsA, this.m_vecPosition, maxsA);
		AddVectors(minsB, pOther.m_vecPosition, minsB);
		AddVectors(maxsB, pOther.m_vecPosition, maxsB);

		return IsBoxIntersectingBox(minsA, maxsA, minsB, maxsB);
	}
}

static int g_iCheckpointSerial;

enum struct CCoopSpawnEntry
{
	int m_iSerial;						// Serial number for identification purposes. While position in the checkpoint list may change, this number always stays the same.

	// -------- Teleport target ----------
	bool m_bHasPosition;				// m_vecPosition null indicator. Not setting a position allows CPs to adopt player's in pre-activation.
	float m_vecPosition[3];				// Absolute origin or a relative one to follow entity (if set).
	bool m_bHasAngles;					// m_vecAngles null indicator. Not setting angles allows CPs to adopt player's in pre-activation.
	float m_vecAngles[3];				// Absolute angles or relative ones to follow entity (if set).
	CBaseEntity m_pFollowEnt;			// Follow entity. If set, players will teleport relative to its center.
	
	// ----------- Activation ------------
	bool m_bActivating;					// In the process of activation (before ActivatePost, after ActivatePre). At this point, either position or follow entity is set and delay timer has started.
	CBasePlayer m_pActivator;			// The player who activated us, if any.
	float m_flDelay;					// Delay from trigger event until full activation.
	bool m_bBringAllPlayers;			// Teleports all players upon activation.
	float m_flBringAllGraceRadius;		// Grace radius from destination, where players are not teleported by bringall.
	ArrayList m_pPortalList;			// Spawnable portal teleporters.
	bool m_bRespawnPlayers;				// Whether activation respawns players in survival mode.
	
	// ----------- Other options ----------
	bool m_bSpawnPlayers;				// Whether players spawn at this checkpoint (default). False allows creation of portal-only checkpoints.
	
	// ------------- Triggers -------------
	// Entity output
	CBaseEntity m_pTriggerEnt;			// Trigger entity.
	char m_szOutputName[MAX_FORMAT];	// Output name for trigger entity.
	// Entity touched
	CBaseEntity m_pTouchEnt;			// Touch trigger entity.
	// Distance
	float m_flRadius;					// Min unobstructed distance to player, squared for optimization.
	
	void Initialize()
	{
		this.m_iSerial = g_iCheckpointSerial++;
		this.m_pActivator = NULL_CBASEENTITY;
		this.m_pTriggerEnt = NULL_CBASEENTITY;
		this.m_pTouchEnt = NULL_CBASEENTITY;
		this.m_pFollowEnt = NULL_CBASEENTITY;
		this.m_pPortalList = new ArrayList(sizeof(CCoopPortal));
		this.m_bSpawnPlayers = true;
	}
	
	void Close()
	{
		delete this.m_pPortalList;
	}

	bool TeleportPlayer(CBasePlayer pPlayer, bool bForceTeleport = false, bool bIsBringAllTP = false)
	{
		float vecPosition[3];
		float vecAngles[3];
		
		if (this.m_pFollowEnt.IsValid())
		{
			this.CalcFollowEntDestination(vecPosition, vecAngles);
		}
		else
		{
			vecPosition = this.m_vecPosition;
			vecAngles = this.m_vecAngles;
		}

		if (bIsBringAllTP && this.m_flBringAllGraceRadius > 0)
		{
			float vecPlayerPos[3]; pPlayer.GetAbsOrigin(vecPlayerPos);
			if (GetVectorDistance(vecPosition, vecPlayerPos) <= this.m_flBringAllGraceRadius)
			{
				return false;
			}
		}
		
		CanTeleportResult res;
		if ((res = CanTeleportPlayer(pPlayer, vecPosition)) || bForceTeleport)
		{
			if (res == CAN_TP_MUSTDUCK)
			{
				pPlayer.StartDucking();
			}
			else
			{
				pPlayer.StopDucking();
			}
			pPlayer.Teleport(vecPosition, vecAngles, vec3_origin);
			return true;
		}

		return false;
	}

	void CalcFollowEntDestination(float vecPosition[3], float vecAngles[3] = NULL_VECTOR)
	{
		this.m_pFollowEnt.WorldSpaceCenter(vecPosition);
		AddVectors(vecPosition, this.m_vecPosition, vecPosition);
		if (!IsNullVector(vecAngles))
		{
			this.m_pFollowEnt.GetAbsAngles(vecAngles);
			AddVectors(vecAngles, this.m_vecAngles, vecAngles);
		}
	}

	bool CheckRadiusTrigger(CBasePlayer pPlayer)
	{
		float vecDest[3], vecPlayer[3];

		pPlayer.GetAbsOrigin(vecPlayer);
		if (this.m_pFollowEnt.IsValid())
		{
			this.CalcFollowEntDestination(vecDest);
		}
		else
		{
			vecDest = this.m_vecPosition;
		}
		
		if (GetVectorDistance(vecDest, vecPlayer, true) < this.m_flRadius)
		{
			if (pPlayer.FVisiblePosition(vecDest, MASK_SOLID_BRUSHONLY))
			{
				return true;
			}
		}
		return false;
	}
	
	bool ActivatePre(CBasePlayer pActivator, bool bStartTimer = true)
	{
		if (this.m_bActivating)
			return false;
		
		this.m_bActivating = true;
		
		if (pActivator.IsValid())
		{
			this.m_pActivator = pActivator;
			if (!this.m_pFollowEnt.IsValid())
			{
				if (!this.m_bHasPosition)
				{
					pActivator.GetAbsOrigin(this.m_vecPosition);
					this.m_bHasPosition = true;
				}
				if (!this.m_bHasAngles)
				{
					pActivator.GetAbsAngles(this.m_vecAngles);
					this.m_bHasAngles = true;
				}
			}
		}
		
		if (bStartTimer)
		{
			CreateTimer(this.m_flDelay, Timer_ActivateCheckpoint, this.m_iSerial, TIMER_FLAG_NO_MAPCHANGE);
		}
		return true;
	}
	
	void ActivatePost(bool bIntermediate)
	{
		this.m_bActivating = false;

		// spawn portals
		for (int i = 0; i < this.m_pPortalList.Length; i++)
		{
			CCoopPortal pPortal;
			this.m_pPortalList.GetArray(i, pPortal);
			if (pPortal.Spawn())
			{
				this.m_pPortalList.SetArray(i, pPortal);
				
				// despawn any previous overlapping portals
				int j = -1;
				ArrayList pOtherPortalList;
				CCoopPortal pOtherPortal;
				while (CCoopSpawnSystem.GetCheckpointFieldByIndex(++j, CCoopSpawnEntry::m_pPortalList, pOtherPortalList))
				{
					if (this.m_pPortalList == pOtherPortalList)
						continue;
					
					for (int k = 0; k < pOtherPortalList.Length; k++)
					{
						pOtherPortalList.GetArray(k, pOtherPortal);
						if (pOtherPortal.IsSpawned() && pOtherPortal.Overlaps(pPortal))
						{
							pOtherPortal.Despawn();
							pOtherPortalList.SetArray(k, pOtherPortal);
						}
					}
				}
			}
		}
		
		if (!bIntermediate && this.m_bBringAllPlayers)
		{
			SetHudTextParams(-1.0, 0.9, 1.0, 200, 200, 200, 255, 0, 1.0, 1.0, 1.0);
			for (int i = 1; i <= MaxClients; i++)
			{
				CBasePlayer pPlayer = CBasePlayer(i);
				if (pPlayer.IsValid() && pPlayer.IsAlive() && !IsFakeClient(i))
				{
					if (this.TeleportPlayer(pPlayer, true, true))
					{
						EmitSoundToClient(i, Conf.SND_BRINGALL, .level = SNDLEVEL_NONE, .volume = Conf.SND_BRINGALL_VOL);
						ShowHudText(i, 1, "%t", "checkpoint reached");
					}
				}
			}
		}
		
		SurvivalManager.OnCheckpointActivation(this);
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
	ArrayList m_pWeaponList;
	ArrayList m_pWeaponEntityList;
	int m_iHealth;
	int m_iArmor;
	int m_iAmmoCount[MAX_AMMO_TYPES];
	char m_szActiveWeapon[MAX_CLASSNAME];
	
	void Initialize(bool bUseWeaponEntities = false)
	{
		this.m_pWeaponList = new ArrayList(sizeof(CCoopWeaponEntry));
		if (bUseWeaponEntities)
		{
			this.m_pWeaponEntityList = new ArrayList();
		}
		this.Clear();
	}

	bool IsInitialized()
	{
		return this.m_pWeaponList != null;
	}

	void Clear()
	{
		this.m_pWeaponList.Clear();
		this.m_iHealth = -1;
		this.m_iArmor = -1;
		this.m_szActiveWeapon = "";
		if (this.m_pWeaponEntityList)
		{
			this.m_pWeaponEntityList.Clear();
		}
		
		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			this.m_iAmmoCount[i] = -1;
		}
	}

	void Close()
	{
		delete this.m_pWeaponList;
		delete this.m_pWeaponEntityList;
	}

	void AddItem(const char[] szClassname, int iPrimaryAmmo = -1, int iSecondaryAmmo = -1)
	{
		CCoopWeaponEntry pWeaponEntry;
		strcopy(pWeaponEntry.m_szClassname, sizeof(pWeaponEntry.m_szClassname), szClassname);
		pWeaponEntry.m_iPrimaryAmmo = iPrimaryAmmo;
		pWeaponEntry.m_iSecondaryAmmo = iSecondaryAmmo;
		this.m_pWeaponList.PushArray(pWeaponEntry);
	}

	void Equip(CBasePlayer pPlayer)
	{
		if (this.m_iHealth > 0)
		{
			pPlayer.SetHealth(this.m_iHealth);
		}

		if (this.m_iArmor >= 0)
		{
			pPlayer.SetArmor(this.m_iArmor);	
		}
		
		int size;
		if (this.m_pWeaponEntityList)
		{
			size = this.m_pWeaponEntityList.Length;
			for (int i = 0; i < size; i++)
			{
				CBaseCombatWeapon pWeapon = this.m_pWeaponEntityList.Get(i);
				if (pWeapon.IsValid())
				{
					pPlayer.EquipWeapon(pWeapon);
				}
			}
		}

		size = this.m_pWeaponList.Length;
		for (int i = 0; i < size; i++)
		{
			CCoopWeaponEntry pWeaponEntry;
			this.m_pWeaponList.GetArray(i, pWeaponEntry);
			
			if (strncmp(pWeaponEntry.m_szClassname, "weapon_", 7, false) == 0)
			{
				CBaseCombatWeapon pWeapon = pPlayer.GiveWeapon(pWeaponEntry.m_szClassname);
				if (pWeapon.IsValid())
				{
					if (pWeapon.GetPrimaryAmmoType() != -1)
					{
						pWeapon.SetPrimaryAmmo(pWeaponEntry.m_iPrimaryAmmo);
					}
					if (pWeapon.GetSecondaryAmmoType() != -1)
					{
						pWeapon.SetSecondaryAmmo(pWeaponEntry.m_iSecondaryAmmo);
					}
				}
			}
			else
			{
				pPlayer.GiveItem(pWeaponEntry.m_szClassname);
			}
		}

		for (int i = 0; i < MAX_AMMO_TYPES; i++)
		{
			if (this.m_iAmmoCount[i] >= 0)
				pPlayer.SetAmmoFromIndex(view_as<AmmoType_t>(i), this.m_iAmmoCount[i]);
		}

		if (this.m_szActiveWeapon[0] != '\0')
		{
			CBaseCombatWeapon pWeapon = pPlayer.GetWeapon(this.m_szActiveWeapon);
			if (pWeapon != NULL_CBASEENTITY)
				pPlayer.WeaponSwitch(pWeapon);
		}
	}

	void Fill(CBasePlayer pPlayer)
	{
		this.m_iHealth = pPlayer.GetHealth();
		this.m_iArmor = pPlayer.GetArmor();

		CBaseCombatWeapon pActiveWeapon = pPlayer.GetActiveWeapon();
		if (pActiveWeapon.IsValid())
		{
			pActiveWeapon.GetClassname(this.m_szActiveWeapon, sizeof(this.m_szActiveWeapon));
		}

		if (this.m_pWeaponEntityList)
		{
			for (int i = 0; i < MAX_WEAPONS; i++)
			{
				CBaseCombatWeapon pWeapon = pPlayer.GetWeaponFromIndex(i);
				if (pWeapon.IsValid())
				{
					this.m_pWeaponEntityList.Push(pWeapon);
				}
			}
		}
		else
		{
			char szClassname[MAX_CLASSNAME];
			for (int i = 0; i < MAX_WEAPONS; i++)
			{
				CBaseCombatWeapon pWeapon = pPlayer.GetWeaponFromIndex(i);
				if (pWeapon.IsValid() && pWeapon.GetClassname(szClassname, sizeof(szClassname)))
				{
					this.AddItem(szClassname, pWeapon.GetPrimaryAmmo(), pWeapon.GetSecondaryAmmo());
				}
			}
		}

		if (pPlayer.HasSuit())
		{
			this.AddItem("item_suit");
		}

		#if defined SRCCOOP_BLACKMESA
		if (view_as<CBlackMesaPlayer>(pPlayer).HasJumpModule())
		{
			this.AddItem("item_longjump");
		}
		#endif // SRCCOOP_BLACKMESA

		for (int iAmmoType = 0; iAmmoType < MAX_AMMO_TYPES; ++iAmmoType)
		{
			this.m_iAmmoCount[iAmmoType] = pPlayer.GetAmmoFromIndex(view_as<AmmoType_t>(iAmmoType));
		}
	}
}

enum struct CCoopSpawnSystemData
{
	ArrayList m_pCheckpointList;			// holds all of maps's spawn points / checkpoints
	int m_iCurrentCheckpoint;				// most recently activated checkpoint index, or -1 for none
	int m_iSpawnAtCheckpoint;				// most recently activated checkpoint index that allows spawning, or -1 for none
	CCoopEquipment m_pDefaultEquipment;		// holds map's equipment configuration
	bool m_bOrdered;						// whether checkpoints are specified in order of progression
	bool m_bRespawnOnLogicAutosave;			// use logic_autosave for survival respawns
	bool m_bDefaultGameEquipment;			// allow equipping of game's default items
}

CCoopSpawnSystemData g_pSpawnSystemData;

// local shortcut for data:
#define data g_pSpawnSystemData

methodmap CCoopSpawnSystem
{
	public static void Initialize()
	{
		data.m_pCheckpointList = new ArrayList(sizeof(CCoopSpawnEntry));
		data.m_pDefaultEquipment.Initialize();
	}
	
	public static void Clear()
	{
		data.m_iCurrentCheckpoint = -1;
		data.m_iSpawnAtCheckpoint = -1;
		data.m_pDefaultEquipment.Clear();
		data.m_bDefaultGameEquipment = false;

		CCoopSpawnEntry pEntry;
		for (int i = 0; i < data.m_pCheckpointList.Length; i++)
		{
			data.m_pCheckpointList.GetArray(i, pEntry);
			pEntry.Close();
		}
		data.m_pCheckpointList.Clear();
	}
		
	public static void OnClientPutInServer(int client)
	{
		SDKHook(client, SDKHook_VPhysicsUpdatePost, Hook_CheckpointRadius);
	}
	
	public static bool GetCheckpointByIndex(int iIndex, CCoopSpawnEntry pCheckpoint)
	{
		if (0 <= iIndex < data.m_pCheckpointList.Length)
		{
			data.m_pCheckpointList.GetArray(iIndex, pCheckpoint);
			return true;
		}
		return false;
	}

	public static bool GetCheckpointFieldByIndex(int iIndex, int iBlock, any &val)
	{
		if (0 <= iIndex < data.m_pCheckpointList.Length)
		{
			val = data.m_pCheckpointList.Get(iIndex, iBlock);
			return true;
		}
		return false;
	}
	
	public static bool GetCurrentCheckpoint(CCoopSpawnEntry pCheckpoint)
	{
		return CCoopSpawnSystem.GetCheckpointByIndex(data.m_iCurrentCheckpoint, pCheckpoint);
	}

	public static bool GetCurrentSpawnCheckpoint(CCoopSpawnEntry pCheckpoint)
	{
		return CCoopSpawnSystem.GetCheckpointByIndex(data.m_iSpawnAtCheckpoint, pCheckpoint);
	}
	
	public static int CountCheckpoints(bool bRemaining, bool bRespawnOnly = false, bool bSpawnOnly = false)
	{
		int iStartIndex = bRemaining? data.m_iCurrentCheckpoint + 1 : 0;
		int count;
		
		CCoopSpawnEntry pEntry;
		for (int i = iStartIndex; i < data.m_pCheckpointList.Length; i++)
		{
			data.m_pCheckpointList.GetArray(i, pEntry);
			if (bRespawnOnly && !pEntry.m_bRespawnPlayers)
				continue;
			if (bSpawnOnly && !pEntry.m_bSpawnPlayers)
				continue;
			count++;
		}
		return count;
	}

	public static int FindCheckpointByOutput(CBaseEntity pCaller, const char[] szOutput, CCoopSpawnEntry pCheckpoint, int iStartIndex = 0)
	{
		CCoopSpawnEntry pEntry;
		for (int i = iStartIndex; i < data.m_pCheckpointList.Length; i++)
		{
			data.m_pCheckpointList.GetArray(i, pEntry);
			if (pCaller == pEntry.m_pTriggerEnt && StrEqual(szOutput, pEntry.m_szOutputName, false))
			{
				pCheckpoint = pEntry;
				return i;
			}
		}
		return -1;
	}
	
	public static int FindCheckpointByTouchEnt(CBaseEntity pTouchEnt, int iStartIndex = 0)
	{
		return CCoopSpawnSystem.FindCheckpointGeneric(pTouchEnt, CCoopSpawnEntry::m_pTouchEnt, iStartIndex);
	}

	public static int FindCheckpointByFollowEnt(CBaseEntity pFollowEnt, int iStartIndex = 0)
	{
		return CCoopSpawnSystem.FindCheckpointGeneric(pFollowEnt, CCoopSpawnEntry::m_pFollowEnt, iStartIndex);
	}

	public static int FindCheckpointBySerial(int iSerial, int iStartIndex = 0)
	{
		return CCoopSpawnSystem.FindCheckpointGeneric(iSerial, CCoopSpawnEntry::m_iSerial, iStartIndex);
	}

	public static int FindCheckpointGeneric(any val, int iBlock, int iStartIndex = 0)
	{
		for (int i = iStartIndex; i < data.m_pCheckpointList.Length; i++)
		{
			if (data.m_pCheckpointList.Get(i, iBlock) == val)
			{
				return i;
			}
		}
		return -1;
	}
	
	public static int AddCheckpoint(const CCoopSpawnEntry pCheckpoint)
	{
		int iIndex = data.m_pCheckpointList.PushArray(pCheckpoint);
		bool bHasTrigger;
		
		if (pCheckpoint.m_pTriggerEnt.IsValid())
		{
			pCheckpoint.m_pTriggerEnt.HookOutput(pCheckpoint.m_szOutputName, Hook_OutputCheckpoint, true);
			bHasTrigger = true;
		}
		if (pCheckpoint.m_pTouchEnt.IsValid())
		{
			SDKHook(pCheckpoint.m_pTouchEnt.entindex, SDKHook_StartTouchPost, Hook_TouchCheckpoint);
			bHasTrigger = true;
		}
		if (pCheckpoint.m_flRadius > 0.0)
		{
			bHasTrigger = true;
		}
		
		if (!bHasTrigger)
		{
			CCoopSpawnSystem.ActivateCheckpoint(iIndex);
		}
		return iIndex;
	}
	
	public static bool ActivateCheckpoint(int iIndex)
	{
		if (data.m_iCurrentCheckpoint < iIndex < data.m_pCheckpointList.Length)
		{
			CCoopSpawnEntry pCheckpoint;
			
			if (data.m_bOrdered)
			{
				for (int i = data.m_iCurrentCheckpoint + 1; ; i++)
				{
					data.m_pCheckpointList.GetArray(i, pCheckpoint);
					if (pCheckpoint.m_bSpawnPlayers)
					{
						data.m_iSpawnAtCheckpoint = i;
					}
					
					pCheckpoint.ActivatePost(i != iIndex);
					data.m_pCheckpointList.SetArray(i, pCheckpoint);

					if (i == iIndex)
					{
						data.m_iCurrentCheckpoint = i;
						return true;
					}
				}
			}
			else // un-ordered
			{
				data.m_iCurrentCheckpoint++;
				data.m_pCheckpointList.SwapAt(iIndex, data.m_iCurrentCheckpoint);
				data.m_pCheckpointList.GetArray(data.m_iCurrentCheckpoint, pCheckpoint);
				if (pCheckpoint.m_bSpawnPlayers)
				{
					data.m_iSpawnAtCheckpoint = data.m_iCurrentCheckpoint;
				}

				pCheckpoint.ActivatePost(false);
				data.m_pCheckpointList.SetArray(data.m_iCurrentCheckpoint, pCheckpoint);
				return true;
			}
		}
		return false;
	}
	
	public static void SpawnPlayerEquipment(CBasePlayer pPlayer)
	{
		#if defined SRCCOOP_HL2DM
		if (!data.m_bDefaultGameEquipment)
		{
			pPlayer.RemoveAllItems();
		}
		#endif

		CCoopEquipment pSavedEquipment;
		if (data.m_iCurrentCheckpoint == 0 && EquipmentManager.GetEquipment(pPlayer, pSavedEquipment))
		{
			pSavedEquipment.Equip(pPlayer);
		}
		else
		{
			data.m_pDefaultEquipment.Equip(pPlayer);
		}
	}
	
	public static void TeleportPlayerOnSpawn(CBasePlayer pPlayer)
	{
		CCoopSpawnEntry pCheckpoint;
		if (CCoopSpawnSystem.GetCurrentSpawnCheckpoint(pCheckpoint))
		{
			pCheckpoint.TeleportPlayer(pPlayer, true);
		}
	}
	
	public static void ParseConfigFile(KeyValues kv)
	{
		PrecacheSound(Conf.SND_BRINGALL);
		PrecacheSound(Conf.SND_PORTAL);
		
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szType[MAX_KEY];
				if (kv.GetSectionName(szType, sizeof(szType)))
				{
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						continue;
					}
					LogDebug("Post-load processing \"%s\" block", szType);
					if (StrContains(szType, "checkpoint", false) == 0)
					{
						CCoopSpawnSystem.ParseCheckpoint(kv);
					}
					else if (StrContains(szType, "equipment", false) == 0)
					{
						CCoopSpawnSystem.ParseEquipment(kv);
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}

	public static void ParseCheckpoint(KeyValues kv)
	{
		if (view_as<bool>(kv.GetNum("use_map_spawnpoint")))
		{
			CCoopSpawnSystem.AddSpawnPointFromMap();
		}
		if (view_as<bool>(kv.GetNum("use_map_autosave")))
		{
			data.m_bOrdered = false;
			CCoopSpawnSystem.AddCheckpointsFromMap();
		}
		else
		{
			data.m_bOrdered = view_as<bool>(kv.GetNum("strict_order", 1));
		}
		data.m_bRespawnOnLogicAutosave = view_as<bool>(kv.GetNum("respawn_on_logic_autosave", 1));
		
		if (kv.GotoFirstSubKey(true))
		{
			char szSectionName[MAX_KEY];
			char szValue[MAX_VALUE];
			char szKey[MAX_KEY];
			
			do
			{
				if (kv.GetSectionName(szSectionName, sizeof(szSectionName)))
				{
					EVAL_CONDITIONS(szSectionName, kv, CCoopSpawnSystem.ParseCheckpoint(kv))

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
										ThrowError("Delay parameter should be a float");
								}
								else if (strcmp(szKey, "radius", false) == 0)
								{
									if (!StringToFloatEx(szValue, pEntry.m_flRadius))
										ThrowError("Radius parameter should be a float");
									if (pEntry.m_flRadius > 0.0)
										pEntry.m_flRadius *= pEntry.m_flRadius;
								}
								else if (strcmp(szKey, "origin", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									if (!StringToFloatEx(szVector[0], pEntry.m_vecPosition[0]))
										ThrowError("Origin x parameter should be a float");
									if (!StringToFloatEx(szVector[1], pEntry.m_vecPosition[1]))
										ThrowError("Origin y parameter should be a float");
									if (!StringToFloatEx(szVector[2], pEntry.m_vecPosition[2]))
										ThrowError("Origin z parameter should be a float");
									pEntry.m_bHasPosition = true;
								}
								else if (strcmp(szKey, "angles", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									if (!StringToFloatEx(szVector[0], pEntry.m_vecAngles[0]))
										ThrowError("Angles x parameter should be a float");
									if (!StringToFloatEx(szVector[1], pEntry.m_vecAngles[1]))
										ThrowError("Angles y parameter should be a float");
									if (!StringToFloatEx(szVector[2], pEntry.m_vecAngles[2]))
										ThrowError("Angles z parameter should be a float");
									pEntry.m_bHasAngles = true;
								}
								else if (strcmp(szKey, "followid", false) == 0)
								{
									if (!(pEntry.m_pFollowEnt = CCoopSpawnSystem.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find followid entity \"%s\"", szValue);
									hkUpdateOnRemove.HookEntity(Hook_Post, pEntry.m_pFollowEnt.entindex, Hook_FollowEntityRemoved);
								}
								else if (strcmp(szKey, "triggerid", false) == 0)	
								{
									if (!(pEntry.m_pTriggerEnt = CCoopSpawnSystem.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find triggerid entity \"%s\"", szValue);
								}
								else if (strcmp(szKey, "touchid", false) == 0)
								{
									if (!(pEntry.m_pTouchEnt = CCoopSpawnSystem.FindEntity(szValue)).IsValid())
										ThrowError("Couldn't find touchid entity \"%s\"", szValue);
								}
								else if (strcmp(szKey, "portal", false) == 0 && ExplodeString(szValue, " ", szVector, 3, MAX_FORMAT, false) == 3)
								{
									CCoopPortal pPortal;
									pPortal.Initialize();
									if (!StringToFloatEx(szVector[0], pPortal.m_vecPosition[0]))
										ThrowError("Portal x parameter should be a float");
									if (!StringToFloatEx(szVector[1], pPortal.m_vecPosition[1]))
										ThrowError("Portal y parameter should be a float");
									if (!StringToFloatEx(szVector[2], pPortal.m_vecPosition[2]))
										ThrowError("Portal z parameter should be a float");
									pEntry.m_pPortalList.PushArray(pPortal);
								}
								else if (strcmp(szKey, "bringall", false) == 0)
								{
									pEntry.m_bBringAllPlayers = !!StringToInt(szValue);
								}
								else if (strcmp(szKey, "bringall_radius", false) == 0)
								{
									pEntry.m_flBringAllGraceRadius = StringToFloat(szValue);
								}
								else if (strcmp(szKey, "spawn", false) == 0)
								{
									pEntry.m_bSpawnPlayers = !!StringToInt(szValue);
								}
								else if (strcmp(szKey, "respawn", false) == 0)
								{
									pEntry.m_bRespawnPlayers = !!StringToInt(szValue);
								}
								else
								{
									ThrowError("Unknown checkpoint parameter type \"%s\"", szKey);
								}
							}
						}
						while (kv.GotoNextKey(false));
						kv.GoBack();
					}
					LogDebug("Adding checkpoint \"%s\"", szSectionName);
					CCoopSpawnSystem.AddCheckpoint(pEntry);
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
	}
	
	public static CBaseEntity FindEntity(const char szValue[MAX_VALUE]) // accepts a int hammerid or a string targetname
	{
		int iHammerID;
		if (StringToIntEx(szValue, iHammerID))
		{
			CBaseEntity pEntity = CBaseEntity(FindEntityByHammerID(iHammerID));
			if (pEntity.IsValid())
			{
				return pEntity;
			}
		}
		return CBaseEntity(FindEntityByTargetname(-1, szValue));
	}
	
	public static void AddSpawnPointFromMap()
	{	
		int ent = -1;
		if ((ent = FindEntityByClassname(ent, "info_player_start")) != -1)
		{
			CCoopSpawnEntry pEntry;
			pEntry.Initialize();
			pEntry.m_pFollowEnt = CBaseEntity(ent);
			CCoopSpawnSystem.AddCheckpoint(pEntry);
		}
	}
	
	public static void AddCheckpointsFromMap()
	{
		int ent = -1;
		while ((ent = FindEntityByClassname(ent, "trigger_autosave")) != -1)
		{
			CCoopSpawnEntry pEntry;
			pEntry.Initialize();
			pEntry.m_pTouchEnt = CBaseEntity(ent);
			CCoopSpawnSystem.AddCheckpoint(pEntry);
		}
	}
	
	// ToDo: all games support
	public static void ParseEquipment(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szKey[MAX_KEY];
				char szValue[MAX_VALUE];
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, CCoopSpawnSystem.ParseEquipment(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						kv.GetString(NULL_STRING, szValue, sizeof(szValue));
						if (strcmp(szKey, "health", false) == 0)
						{
							if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iHealth))
								ThrowError("Health parameter is not a integer");
						}
						else if (strcmp(szKey, "armor", false) == 0)
						{
							if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iArmor))
								ThrowError("Armor parameter is not a integer");
						}
						else if (strcmp(szKey, "item", false) == 0)
						{
							CCoopSpawnSystem.AddSpawnItem(szValue, true);
						}
						else if (strcmp(szKey, "defaults", false) == 0)
						{
							data.m_bDefaultGameEquipment = !!StringToInt(szValue);
							
							#if defined SRCCOOP_BLACKMESA
							// to simulate classic multiplayer, we need the suit added
							if (data.m_bDefaultGameEquipment)
							{
								CCoopSpawnSystem.AddSpawnItem("item_suit");
							}
							#endif
						}
						else
						{
							ThrowError("Could not find equipment parameter type \"%s\"", szKey);
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
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_9MM]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "357", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_357]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "bolts", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_BOLTS]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "shells", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_SHELLS]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "energy", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_ENERGY]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "mp5nade", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_MP5NADE]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "rockets", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_ROCKETS]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "nades", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_NADES]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "satchels", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_SATCHELS]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "tripmines", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_TRIPMINES]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "hivegun", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_HIVEGUN]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else if (strcmp(szAmmoKey, "snarks", false) == 0)
										{
											if (!StringToIntEx(szValue, data.m_pDefaultEquipment.m_iAmmoCount[AMMO_SNARKS]))
												ThrowError("Ammo parameter \"%s\" is not a integer", szAmmoKey);
										}
										else
										{
											ThrowError("Could not find ammo parameter type \"%s\"", szAmmoKey);
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
	
	public static bool AddSpawnItem(const char[] szItem, bool bAllowDuplicates = false)
	{
		if (!bAllowDuplicates)
		{
			for (int i = 0; i < data.m_pDefaultEquipment.m_pWeaponList.Length; i++)
			{
				CCoopWeaponEntry pWeaponEntry;
				if (data.m_pDefaultEquipment.m_pWeaponList.GetArray(i, pWeaponEntry, sizeof(pWeaponEntry)))
				{
					// does basic checking of item_weapon vs weapon_ strings
					if (StrContains(pWeaponEntry.m_szClassname, szItem, false) != -1 || StrContains(szItem, pWeaponEntry.m_szClassname, false) != -1)
					{
						return false;
					}
				}
			}
		}
		data.m_pDefaultEquipment.AddItem(szItem);
		return true;
	}
}

public void Hook_OutputCheckpoint(const char[] szName, int iCaller, int iActivator, float flDelay)
{
	CBaseEntity pCaller = CBaseEntity(iCaller);
	CBaseEntity pActivator = CBaseEntity(iActivator);
	int iIndex = data.m_iCurrentCheckpoint + 1;
	
	CCoopSpawnEntry pCheckpoint;
	while ((iIndex = CCoopSpawnSystem.FindCheckpointByOutput(pCaller, szName, pCheckpoint, iIndex)) != -1)
	{
		if (pCheckpoint.ActivatePre(view_as<CBasePlayer>(pActivator)))
		{
			data.m_pCheckpointList.SetArray(iIndex, pCheckpoint);
		}
		iIndex++;
	}
}

public void Hook_TouchCheckpoint(int iEntity, int iOther)
{
	CBasePlayer pOther = CBasePlayer(iOther);
	if (pOther.IsPlayer())
	{
		CBaseEntity pTouched = CBaseEntity(iEntity);
		int iIndex = data.m_iCurrentCheckpoint + 1;
		
		CCoopSpawnEntry pCheckpoint;
		while ((iIndex = CCoopSpawnSystem.FindCheckpointByTouchEnt(pTouched, iIndex)) != -1)
		{
			data.m_pCheckpointList.GetArray(iIndex, pCheckpoint);
			if (pCheckpoint.ActivatePre(pOther))
			{
				data.m_pCheckpointList.SetArray(iIndex, pCheckpoint);
			}
			iIndex++;
		}

		SDKUnhook(iEntity, SDKHook_StartTouchPost, Hook_TouchCheckpoint);
	}
}

public void Hook_CheckpointRadius(int iEntity)
{
	int size = data.m_pCheckpointList.Length;
	for (int i = data.m_iCurrentCheckpoint + 1; i < size; i++)
	{
		float flRadius = data.m_pCheckpointList.Get(i, CCoopSpawnEntry::m_flRadius);
		if (flRadius > 0.0)
		{
			CCoopSpawnEntry pCheckpoint;
			data.m_pCheckpointList.GetArray(i, pCheckpoint);
			if (pCheckpoint.m_bActivating)
			{
				continue;
			}
			
			CBasePlayer pPlayer = CBasePlayer(iEntity);
			if (pCheckpoint.CheckRadiusTrigger(pPlayer))
			{
				if (pCheckpoint.ActivatePre(pPlayer))
				{
					data.m_pCheckpointList.SetArray(i, pCheckpoint);
				}
			}
		}
	}
}

public void Timer_ActivateCheckpoint(Handle pTimer, int iSerial)
{
	CCoopSpawnSystem.ActivateCheckpoint(CCoopSpawnSystem.FindCheckpointBySerial(iSerial));
}

public void Hook_TouchPortal(int iEntity, int iOther)
{
	CBasePlayer pOther = CBasePlayer(iOther);
	if (pOther.IsPlayer())
	{
		CCoopSpawnEntry pCheckpoint;
		if (CCoopSpawnSystem.GetCurrentCheckpoint(pCheckpoint) && pCheckpoint.TeleportPlayer(pOther))
		{
			EmitSoundToClient(iOther, Conf.SND_PORTAL, .level = SNDLEVEL_NONE, .volume = Conf.SND_PORTAL_VOL);
		}
	}
}

/**
 * Checks for follow entity removal, saves last known position.
 */
MRESReturn Hook_FollowEntityRemoved(int iEntity)
{
	CBaseEntity pEntity = CBaseEntity(iEntity);
	int iIndex;
	while ((iIndex = CCoopSpawnSystem.FindCheckpointByFollowEnt(pEntity, iIndex)) != -1)
	{
		CCoopSpawnEntry pCheckpoint;
		data.m_pCheckpointList.GetArray(iIndex, pCheckpoint);

		float vecPosition[3], vecAngles[3]; // prevent self-overwrite
		pCheckpoint.CalcFollowEntDestination(vecPosition, vecAngles);
		pCheckpoint.m_pFollowEnt = NULL_CBASEENTITY;
		pCheckpoint.m_vecPosition = vecPosition;
		pCheckpoint.m_vecAngles = vecAngles;
		pCheckpoint.m_bHasPosition = pCheckpoint.m_bHasAngles = true;

		data.m_pCheckpointList.SetArray(iIndex, pCheckpoint);
	}
	return MRES_Ignored;
}

// end shortcut
#undef data