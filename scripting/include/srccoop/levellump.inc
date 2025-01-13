#pragma newdecls required
#pragma semicolon 1

// Temporary brush entity model placeholders to be identified and fixed up at entity spawn 
#define BRUSH_FIXUP_MDL "models/error.mdl" /* actual model that will be set on fixed up ents */
#define BRUSH_FIXUP_IDENT "$BRUSHFIXUP$" /* identifier that the entities with customized brush models will have their "model" property start with */
#define BRUSH_FIXUP_IDENT_LEN 12
#define BRUSH_FIXUP_SEPARATOR " "

/**
 * Lump of a single key and value.
 */
enum struct CEntityKeyLump
{
	char m_szKey[MAX_KEY];
	char m_szValue[MAX_VALUE];
}

/**
 * Output data. This is an exploded variant of CEntityKeyLump's value string, specifically for working with output keys.
 */
enum struct CEntityOutputLump
{
	char m_szTargetEntity[MAX_VALUE];
	char m_szInputName[MAX_VALUE];
	char m_szParameter[MAX_VALUE];
	float m_flDelay;
	int m_iTimesToFire;
	
	bool LoadFromEntityKeyLump(const CEntityKeyLump pEntityKeyLump)
	{
		char buffers[5][MAX_VALUE];
		if (sizeof(buffers) != ExplodeString(pEntityKeyLump.m_szValue, OUTPUT_SEPARATOR, buffers, sizeof(buffers), sizeof(buffers[])))
		{
			return false;
		}
		if (!StringToFloatEx(buffers[3], this.m_flDelay))
		{
			return false;
		}
		if (!StringToIntEx(buffers[4], this.m_iTimesToFire))
		{
			return false;
		}
		this.m_szTargetEntity = buffers[0];
		this.m_szInputName = buffers[1];
		this.m_szParameter = buffers[2];
		return true;
	}
	
	void SaveToEntityKeyLump(CEntityKeyLump pEntityKeyLump)
	{
		FormatEx(pEntityKeyLump.m_szValue, sizeof(pEntityKeyLump.m_szValue), "%s%s%s%s%s%s%.2f%s%d",
			this.m_szTargetEntity, OUTPUT_SEPARATOR,
			this.m_szInputName, OUTPUT_SEPARATOR,
			this.m_szParameter, OUTPUT_SEPARATOR,
			this.m_flDelay, OUTPUT_SEPARATOR,
			this.m_iTimesToFire);
	}
	
	/**
	 * Tests for equality with pOther, allows partial matching.
	 * Supports regex matching of strings - with /patterns/ inside of this object and raw data in pOther.
	 */
	bool Matches(const CEntityOutputLump pOther, bool bMatchTarget, bool bMatchInput, bool bMatchParameter, bool bMatchDelay, bool bMatchTTF)
	{
		if (bMatchTarget && !StrEqualsRegex(this.m_szTargetEntity, pOther.m_szTargetEntity, false)
			|| bMatchInput && !StrEqualsRegex(this.m_szInputName, pOther.m_szInputName, false)
			|| bMatchParameter && !StrEqualsRegex(this.m_szParameter, pOther.m_szParameter, false)
			|| bMatchDelay && this.m_flDelay != pOther.m_flDelay
			|| bMatchTTF && this.m_iTimesToFire != pOther.m_iTimesToFire)
		{
			return false;
		}
		return true;
	}
}

/**
 * Holds all key-value lumps (properties) of an entity.
 */
methodmap CEntityInfoLump < ArrayList
{
	public CEntityInfoLump()
	{
		return view_as<CEntityInfoLump>(new ArrayList(sizeof(CEntityKeyLump)));
	}

	/**
	 * Adds a new property at the end.
	 */
	public void AddProperty(const CEntityKeyLump pKeyInfo)
	{
		this.PushArray(pKeyInfo);
	}

	/**
	 * Removes matching properties. Supports regex in key and value.
	 * @param pKeyInfo     The property to match by
	 * @param bMatchValue  Requires values to match beside keys.
	 */
	public void RemoveProperty(const CEntityKeyLump pKeyInfo, bool bMatchValue)
	{
		for (int i = (this.Length - 1); i >= 0; i--)
		{
			CEntityKeyLump pThisKeyInfo;
			this.GetArray(i, pThisKeyInfo);

			if (StrEqualsRegex(pKeyInfo.m_szKey, pThisKeyInfo.m_szKey, false))
			{
				if (!bMatchValue || StrEqualsRegex(pKeyInfo.m_szValue, pThisKeyInfo.m_szValue, false))
				{
					this.Erase(i);
				}
			}
		}
	}

	/**
	 * Returns last property found for given key.
	 */
	public bool GetProperty(const char[] szKey, CEntityKeyLump pKeyInfo)
	{
		for (int i = (this.Length - 1); i >= 0; i--)
		{
			this.GetArray(i, pKeyInfo);
			if (strcmp(pKeyInfo.m_szKey, szKey, false) == 0)
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * Sets given property key/value.
	 * Note: If there are duplicate keys, only the last one is updated.
	 * @param bInsert  If missing, the property is added at the end.
	 */
	public void SetProperty(const CEntityKeyLump pKeyInfo, bool bInsert = true)
	{
		CEntityKeyLump temp;
		for (int i = (this.Length - 1); i >= 0; i--)
		{
			this.GetArray(i, temp);
			if (strcmp(temp.m_szKey, pKeyInfo.m_szKey, false) == 0)
			{
				this.SetArray(i, pKeyInfo);
				return;
			}
		}
		if (bInsert)
		{
			this.AddProperty(pKeyInfo);
		}
	}

	/**
	 * Tests that all contained properties are equal in value with pOther.
	 * Supports regex matching of keys/values - with /patterns/ inside of this object and raw data in pOther.
	 */
	public bool Matches(const CEntityInfoLump pOther)
	{
		for (int i = 0; i < this.Length; i++)
		{
			bool bFoundMatch = false;
			
			CEntityKeyLump pThisKeyInfo;
			this.GetArray(i, pThisKeyInfo);
			
			for (int j = 0; j < pOther.Length; j++)
			{
				CEntityKeyLump pOtherKeyInfo;
				pOther.GetArray(j, pOtherKeyInfo);
				
				if (StrEqualsRegex(pThisKeyInfo.m_szKey, pOtherKeyInfo.m_szKey, false)
				&& StrEqualsRegex(pThisKeyInfo.m_szValue, pOtherKeyInfo.m_szValue, false))
				{
					bFoundMatch = true;
					break;
				}
			}
			
			if (!bFoundMatch)
			{
				return false;
			}
		}
		
		return true;
	}
	
	/**
	 * Attempts to predict whether this entity would've gotten targeted in the input/output system,
	 * given the specified target string.
	 * @param szTarget 			the target string
	 * @param pCallingEntity 	entity doing the targeting
	 */
	public bool MatchesAsIOTarget(const char[] szTarget, const CEntityInfoLump pCallingEntity)
	{
		if (strcmp(szTarget, "!self", false) == 0)
		{
			return this.Matches(pCallingEntity);
		}

		CEntityKeyLump pProperty;
		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pProperty);
			if (strcmp(pProperty.m_szKey, "classname", false) == 0 && strcmp(pProperty.m_szValue, szTarget, false) == 0)
			{
				return true;
			}
			if (strcmp(pProperty.m_szKey, "targetname", false) == 0 && strcmp(pProperty.m_szValue, szTarget, false) == 0)
			{
				return true;
			}
		}

		return false;
	}
	
	/**
	 * Attempts to match this entity lump with runtime CBaseEntity instance.
	 * @return  True if it's likely that both objects refer to the same entity.
	 */
	public bool MatchesActualEntity(CBaseEntity pEntity)
	{
		CEntityKeyLump pProperty;

		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pProperty);

			// check these 3 properties for now, return false if any dont match
			if (StrEqual(pProperty.m_szKey, "hammerid", false))
			{
				if (StringToInt(pProperty.m_szValue) != pEntity.GetHammerID())
				{
					return false;
				}
			}
			else if (StrEqual(pProperty.m_szKey, "classname", false))
			{
				if (!pEntity.IsClassname(pProperty.m_szValue))
				{
					return false;
				}
			}
			else if (StrEqual(pProperty.m_szKey, "targetname", false))
			{
				char szTargetname[MAX_VALUE];
				pEntity.GetTargetname(szTargetname, sizeof(szTargetname));
				if (!StrEqual(pProperty.m_szValue, szTargetname, false))
				{
					return false;
				}
			}
		}
		return true;
	}
}

/**
 * A list of CEntityInfoLump objects.
 */
methodmap CEntityListLump < ArrayList
{
	public CEntityListLump()
	{
		return view_as<CEntityListLump>(new ArrayList());
	}

	// cast
	public CEntityInfoLump Get(int index)
	{
		return view_as<ArrayList>(this).Get(index);
	}
	
	public void Clear()
	{
		for (int i = 0; i < this.Length; i++)
		{
			delete this.Get(i);
		}
		view_as<ArrayList>(this).Clear();
	}
}

/**
 * Evaluated entity list paired with an output name.
 * Used at runtime for the delayed outputs feature.
 */
enum struct CEntityOutputHook
{
	CEntityListLump m_pEntityList;
	char m_szOutputName[MAX_VALUE];

	void Close()
	{
		delete this.m_pEntityList;
	}
}

/**
 * Collection of CEntityOutputHook.
 */
methodmap COutputHookList < ArrayList
{
	public COutputHookList()
	{
		return view_as<COutputHookList>(new ArrayList(sizeof(CEntityOutputHook)));
	}
	
	public void Clear()
	{
		CEntityOutputHook pOutputHook;
		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pOutputHook);
			pOutputHook.Close();
		}
		view_as<ArrayList>(this).Clear();
	}
	
	/**
	 * Removes hooks or their entities which are no longer contained in the list (f.e. deleted when parsing).
	 */
	public void Validate(const CEntityListLump pValidEntityList)
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityOutputHook pOutputHook;
			this.GetArray(i, pOutputHook);

			// loop all of output-hook's entities
			for (int j = 0; j < pOutputHook.m_pEntityList.Length; j++)
			{
				CEntityInfoLump pEntityInfo = pOutputHook.m_pEntityList.Get(j);
				// delete those not found in valid list
				if (pValidEntityList.FindValue(pEntityInfo) == -1)
				{
					pOutputHook.m_pEntityList.Erase(j--);
				}
			}
			
			// if all of output-hook's entities got deleted, it's no longer valid
			if (!pOutputHook.m_pEntityList.Length)
			{
				pOutputHook.Close();
				this.Erase(i--);
			}
		}
	}
}

/**
 * Collection of allowed map names (or regexes of) for transitions.
 */
methodmap CNextMapLump < ArrayList
{
	public CNextMapLump()
	{
		return view_as<CNextMapLump>(new ArrayList(ByteCountToCells(MAX_MAPNAME)));
	}
	
	public bool IsInMapList(const char[] szMapName)
	{
		for (int i = 0; i < this.Length; i++)
		{
			char szNextMap[MAX_MAPNAME];
			if (this.GetString(i, szNextMap, sizeof(szNextMap)) && StrEqualsRegex(szNextMap, szMapName, false))
			{
				return true;
			}
		}
		return false;
	}
}

/**
 * Level metadata for modifying a server convar
 */
enum struct CServerConsoleCommand
{
	ConVar m_pConvar;
	char m_szValue[256];
	int m_iFlagsAdd;
	bool m_bHooked;
	bool m_bLate;
}

/**
 * Collection of CServerConsoleCommand
 */
methodmap CServerConsoleCommandList < ArrayList
{
	public CServerConsoleCommandList()
	{
		return view_as<CServerConsoleCommandList>(new ArrayList(sizeof(CServerConsoleCommand)));
	}
}

/**
 * Level metadata for modifying a client convar
 */
enum struct CClientConsoleCommand
{
	char m_szConvar[128];
	char m_szValue[256];
	ConVar m_pConvar;
	bool m_bApplyOnSpawn;
}

/**
 * Collection of CClientConsoleCommand
 */
methodmap CClientConsoleCommandList < ArrayList
{
	public CClientConsoleCommandList()
	{
		return view_as<CClientConsoleCommandList>(new ArrayList(sizeof(CClientConsoleCommand)));
	}
}

/**
 * Handles parsing, updating and saving the level's entity string alongside processing edt configs.
 */
enum struct CGlobalLevelLump
{
	CEntityListLump m_pEntityList;
	CNextMapLump m_pNextMapList;
	CServerConsoleCommandList m_pServerCvarList;
	CClientConsoleCommandList m_pClientCvarList;
	COutputHookList m_pOutputHookList;
	IntroType m_iIntroType;
	StringMap m_EdtDefinesMap;
	
	void Initialize()
	{
		this.m_pEntityList = new CEntityListLump();
		this.m_pNextMapList = new CNextMapLump();
		this.m_pServerCvarList = new CServerConsoleCommandList();
		this.m_pClientCvarList = new CClientConsoleCommandList();
		this.m_pOutputHookList = new COutputHookList();
		this.m_EdtDefinesMap = new StringMap();
	}
	
	void Clear()
	{
		this.m_pNextMapList.Clear();
		this.m_pServerCvarList.Clear();
		this.m_pClientCvarList.Clear();
		this.m_pOutputHookList.Clear();
		this.m_iIntroType = INTRO_NONE;
		this.m_EdtDefinesMap.Clear();
	}
	
	void ParseMapEntities(const char szMapEntities[ENTITYSTRING_LENGTH])
	{
		this.m_pEntityList.Clear();
		float flStartTime = GetEngineTime();
		
		CEntityInfoLump pEntityInfo;
		CEntityKeyLump pEntityInfoKey;
		bool bParsingValue;
		int n;
		
		for (int i = 0; szMapEntities[i] != '\0'; i++)
		{
			if (szMapEntities[i] == '"')
			{
				if (bParsingValue)
				{
					n = SplitByChar(szMapEntities[++i], '"', pEntityInfoKey.m_szValue, sizeof(pEntityInfoKey.m_szValue));
					pEntityInfo.PushArray(pEntityInfoKey);
				}
				else
				{
					n = SplitByChar(szMapEntities[++i], '"', pEntityInfoKey.m_szKey, sizeof(pEntityInfoKey.m_szKey));
				}
				if (n == -1)
				{
					ThrowError("Did not find closing \" for keyvalue");
				}
				i += n;
				bParsingValue = !bParsingValue;
			}
			else if (bParsingValue)
			{
				continue;
			}
			else if (szMapEntities[i] == '{')
			{
				if (pEntityInfo)
					ThrowError("Found '{' when expecting '}'");
				
				pEntityInfo = new CEntityInfoLump();
			}
			else if (szMapEntities[i] == '}')
			{
				if (!pEntityInfo)
					ThrowError("Did not have valid entity info when finding '}'");
				
				this.m_pEntityList.Push(pEntityInfo);
				pEntityInfo = null;
			}
		}
		
		LogDebug("Parsed %d entities in %fs", this.m_pEntityList.Length, GetEngineTime() - flStartTime);
	}
	
	int ToString(char szBuffer[ENTITYSTRING_LENGTH])	// absolutely do not care for bounds checking, this 2 mb buffer is big enough for everything
	{
		float flStartTime = GetEngineTime();
		szBuffer = "";
		
		int n = 0;
		for (int i = 0; i < this.m_pEntityList.Length; i++)
		{
			CEntityInfoLump pEntityInfo = this.m_pEntityList.Get(i);

			if (!pEntityInfo) // should be never invalid
				return -1;
			if (pEntityInfo.Length <= 0) // never write if it has nothing in it to begin with
				continue;
			
			szBuffer[n++] = '{';
			szBuffer[n++] = '\n';
			
			for (int j = 0; j < pEntityInfo.Length; j++)
			{
				CEntityKeyLump pEntityInfoKey;
				if (pEntityInfo.GetArray(j, pEntityInfoKey))
				{
					static char szKeyValue[2048];
					int iLength = FormatEx(szKeyValue, sizeof(szKeyValue), "\t\"%s\"\t\"%s\"\n", pEntityInfoKey.m_szKey, pEntityInfoKey.m_szValue);
					strcopy(szBuffer[n], iLength + 1, szKeyValue);
					n += iLength;
				}
			}
			
			szBuffer[n] = '}';
			// if this doesn't end with a null terminator, the token will search for '{' and crash the server
			szBuffer[n + 1] = (i == (this.m_pEntityList.Length - 1)) ? '\0' : '\n';
			n += 2;
		}
		
		LogDebug("Saving %d entities to string took %fs", this.m_pEntityList.Length, GetEngineTime() - flStartTime);
		return n;
	}
	
	void ParseConfigFile(KeyValues kv)
	{
		float flStartTime = GetEngineTime();
		
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szType[MAX_KEY];
				if (kv.GetSectionName(szType, sizeof(szType)))
				{
					if (kv.GetDataType(NULL_STRING) != KvData_None) // simple values
					{
						if (strcmp(szType, "nextmap", false) == 0)
						{
							char szNextMapName[MAX_MAPNAME];
							kv.GetString(NULL_STRING, szNextMapName, sizeof(szNextMapName));
							this.m_pNextMapList.PushString(szNextMapName);
							LogDebug("Adding nextmap to allowlist: \"%s\"", szNextMapName);
						}
						else if (strcmp(szType, "intro_type", false) == 0)
						{
							char szIntroType[16];
							kv.GetString(NULL_STRING, szIntroType, sizeof(szIntroType));
							if (strcmp(szIntroType, "fade", false) == 0)
							{
								this.m_iIntroType = INTRO_FADE;
							}
							else if (strcmp(szIntroType, "none", false) == 0)
							{
								this.m_iIntroType = INTRO_NONE;
							}
							else if (strcmp(szIntroType, "freeze", false) == 0)
							{
								this.m_iIntroType = INTRO_FREEZE;
							}
							else LogError("Unknown intro_type \"%s\"", szIntroType);
						}
					}
					else // subsections
					{
						LogDebug("Processing \"%s\" block", szType);
						// We only check for StartsWith on subsection strings.
						// This is so subsection keys from config file hierarchy can have unique names.
						// That way, they are not forcefully merged together into 1 subtree.
						// This is especially important for sections with repetitive keys, where each key
						// should be processed separately, i.e. entity modifications, or when conditions are used inside.
						if (StrContains(szType, "define", false) == 0)
						{
							this.ParseDefines(kv);
						}
						else if (StrContains(szType, "features", false) == 0)
						{
							this.ParseFeatures(kv);
						}
						else if (StrContains(szType, "console", false) == 0)
						{
							this.ParseConsole(kv);
						}
						else if (StrContains(szType, "client_console", false) == 0)
						{
							this.ParseClientConsole(kv);
						}
						else if (StrContains(szType, "entity", false) == 0)
						{
							this.ParseEntityEdits(kv);
						}
						else if (StrContains(szType, "equipment", false) == 0)
						{
							this.ParseEquipment(kv);
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
		this.m_pOutputHookList.Validate(this.m_pEntityList);
		
		LogDebug("Processing edt file took %fs", GetEngineTime() - flStartTime);
	}
	
	void ParseEquipment(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(true))
		{
			do
			{
				char szKey[MAX_FORMAT];
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, this.ParseEquipment(kv))
					
					if (StrEqual(szKey, "lookup", false))
					{
						if (kv.GotoFirstSubKey(false))
						{
							#if defined DEBUG
							int iImportCount;
							#endif

							// Gather properties to lookup entities by
							CEntityInfoLump pLookupEntity = this.ReadEntityInfo(kv);
							kv.GoBack();
							
							// Look for matching entities in the entity list
							for (int i = (this.m_pEntityList.Length - 1); i >= 0; i--)
							{
								CEntityInfoLump pListEntity = this.m_pEntityList.Get(i);
								if (pLookupEntity.Matches(pListEntity))
								{
									// Add as equipment using their classnames
									CEntityKeyLump pClassProp;
									if (pListEntity.GetProperty("classname", pClassProp))
									{
										CCoopSpawnSystem.AddSpawnItem(pClassProp.m_szValue, true);
										// Delete imported entities from map
										pListEntity.Close();
										this.m_pEntityList.Erase(i);

										#if defined DEBUG
										iImportCount++;
										#endif
									}
								}
							}
							pLookupEntity.Close();
							
							#if defined DEBUG
							LogDebug("Equipment lookup imported %d entities", iImportCount);
							#endif
						}
					}
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
	}
	
	void ParseDefines(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szKey[MAX_FORMAT];
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, this.ParseDefines(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						char szValue[MAX_FORMAT];
						kv.GetString(NULL_STRING, szValue, sizeof(szValue));
						UpperCaseString(szKey, szKey, sizeof(szKey));
						this.m_EdtDefinesMap.SetString(szKey, szValue, true);
						LogDebug("Setting define: [\"%s\" = \"%s\"]", szKey, szValue);
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	void ParseFeatures(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				char szKey[MAX_FORMAT];
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, this.ParseFeatures(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						SourceCoopFeature feature;
						if (g_pFeatureMap.GetFeature(szKey, feature))
						{
							if (kv.GetNum(NULL_STRING))
							{
								CoopManager.EnableFeature(feature);
								LogDebug("Setting feature: [\"%s\" = On]", szKey);
							}
							else
							{
								CoopManager.DisableFeature(feature);
								LogDebug("Setting feature: [\"%s\" = Off]", szKey);
							}
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	void ParseConsole(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			char szKey[128];
			do
			{
				if (kv.GetSectionName(szKey, sizeof(szKey)))
				{
					EVAL_CONDITIONS(szKey, kv, this.ParseConsole(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						static const char FORBIDDEN[][] = {"sv_allow_point_servercommand", "rcon_password"};
						int pos = FindStringInArrayEx(FORBIDDEN, sizeof(FORBIDDEN), szKey, false);
						if (pos != -1)
						{
							LogMessage("Map %s tried to change dangerous convar %s", g_szMapName, FORBIDDEN[pos]);
							continue;
						}

						CServerConsoleCommand cmd;
						if ((cmd.m_pConvar = FindConVar(szKey)) != null)
						{
							kv.GetString(NULL_STRING, cmd.m_szValue, sizeof(cmd.m_szValue));
							
							int contextFlags[3]; // increase this when adding more flags
							ExtractContextFlags(cmd.m_szValue, sizeof(cmd.m_szValue), contextFlags, sizeof(contextFlags));
							
							if (FindCharInArray(contextFlags, sizeof(contextFlags), 'H', false) != -1)
							{
								// [H]ide convar (unavailable to console unless using sm_cvar)
								LogDebug("Hiding convar: \"%s\"", szKey);
								cmd.m_iFlagsAdd |= FCVAR_DEVELOPMENTONLY;
							}
							cmd.m_pConvar.Flags |= cmd.m_iFlagsAdd;
							
							if (FindCharInArray(contextFlags, sizeof(contextFlags), 'L', false) != -1)
							{
								// [L]ate convars are set after OnConfigsExecuted
								cmd.m_bLate = true;
							}
							else
							{
								LogDebug("Setting convar: [\"%s\" = \"%s\"]", szKey, cmd.m_szValue);
								cmd.m_pConvar.SetString(cmd.m_szValue);
							}

							if (FindCharInArray(contextFlags, sizeof(contextFlags), 'F', false) != -1)
							{
								// [F]orced convar value
								LogDebug("Preventing convar changes: \"%s\"", szKey);
								cmd.m_pConvar.AddChangeHook(Hook_ForceConvarValue);
								cmd.m_bHooked = true;
							}
							this.m_pServerCvarList.PushArray(cmd);
						}
						else
						{
							LogDebug("Convar \"%s\" not found!", szKey);
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	void ParseClientConsole(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			CClientConsoleCommand cmd;
			do
			{
				if (kv.GetSectionName(cmd.m_szConvar, sizeof(cmd.m_szConvar)))
				{
					EVAL_CONDITIONS(cmd.m_szConvar, kv, this.ParseClientConsole(kv))
					
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						kv.GetString(NULL_STRING, cmd.m_szValue, sizeof(cmd.m_szValue));
						
						int contextFlags[1]; // increase this when adding more flags
						if (ExtractContextFlags(cmd.m_szValue, sizeof(cmd.m_szValue), contextFlags, sizeof(contextFlags)))
						{
							if (FindCharInArray(contextFlags, sizeof(contextFlags), 'S', false) != -1)
							{
								// Apply on each spawn
								cmd.m_bApplyOnSpawn = true;
							}
						}

						LogDebug("Adding client convar: [\"%s\" = \"%s\"]%s", cmd.m_szConvar, cmd.m_szValue,
								cmd.m_bApplyOnSpawn? " (Apply on spawn)" : "");
						cmd.m_pConvar = FindConVar(cmd.m_szConvar);
						this.m_pClientCvarList.PushArray(cmd);
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	/**
	 * Reads entity properties at current keyvalues position into a new CEntityInfoLump instance.
	 */
	CEntityInfoLump ReadEntityInfo(KeyValues kv)
	{
		CEntityInfoLump pEntity = new CEntityInfoLump();
		CEntityKeyLump pEntityProp;
		do
		{
			if (kv.GetSectionName(pEntityProp.m_szKey, sizeof(pEntityProp.m_szKey))
				&& kv.GetDataType(NULL_STRING) != KvData_None)
			{
				kv.GetString(NULL_STRING, pEntityProp.m_szValue, sizeof(pEntityProp.m_szValue));
				pEntity.PushArray(pEntityProp);
			}
		}
		while (kv.GotoNextKey(false));
		return pEntity;
	}
	
	/**
	 * Returns a new list of evaluated entities matching the properties
	 * of sub-values found at current keyvalues position.
	 */
	CEntityListLump EvaluateEntityList(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(false))
		{
			CEntityInfoLump pEvaulationEntity = this.ReadEntityInfo(kv);
			kv.GoBack();
			
			CEntityListLump pEvaluatedList = new CEntityListLump();
			for (int i = 0; i < this.m_pEntityList.Length; i++)
			{
				CEntityInfoLump pListEntity = this.m_pEntityList.Get(i);
				if (pEvaulationEntity.Matches(pListEntity))
				{
					pEvaluatedList.Push(pListEntity);
				}
			}

			pEvaulationEntity.Close();
			return pEvaluatedList;
		}
		else
		{
			// if there are no properties to filter by, return all entities
			return view_as<CEntityListLump>(this.m_pEntityList.Clone());
		}
	}
	
	void ParseEntityEdits(KeyValues kv)
	{
		if (kv.GotoFirstSubKey(true))
		{
			do
			{
				char szAction[MAX_KEY];
				if (kv.GetSectionName(szAction, sizeof(szAction)))
				{
					EVAL_CONDITIONS(szAction, kv, this.ParseEntityEdits(kv))
					
					bool add = IsAddSynonym(szAction);
					bool remove = IsRemoveSynonym(szAction);
					bool modify = IsModifySynonym(szAction);
					if (!add && !remove && !modify)
					{
						ThrowError("Unknown entity action \"%s\", expected one of add | remove | modify", szAction);
					}
					
					if (add)
					{
						if (kv.GotoFirstSubKey(false))
						{
							CEntityInfoLump pEntityInfo = this.ReadEntityInfo(kv);
							this.m_pEntityList.Push(pEntityInfo);
							kv.GoBack();
							
							// alow direct usage of modify actions here
							if (kv.GotoFirstSubKey(true))
							{
								CEntityListLump pSingletonEntityList = new CEntityListLump();
								pSingletonEntityList.Push(pEntityInfo);
								do
								{
									char szModifyType[MAX_KEY];
									if (kv.GetSectionName(szModifyType, sizeof(szModifyType)))
									{
										this.ParseModifyAction(kv, szModifyType, pSingletonEntityList);
									}
								}
								while (kv.GotoNextKey(true));
								kv.GoBack();
								pSingletonEntityList.Close();
							}
						}
					}
					else if (remove)
					{
						CEntityInfoLump pRemoveFilter;
						if (kv.GotoFirstSubKey(false))
						{
							pRemoveFilter = this.ReadEntityInfo(kv);
							kv.GoBack();
						}
						else pRemoveFilter = new CEntityInfoLump();
						
						#if defined DEBUG
						int iEntCount = this.m_pEntityList.Length;
						#endif

						for (int i = (this.m_pEntityList.Length - 1); i >= 0; i--)
						{
							CEntityInfoLump pListEntity = this.m_pEntityList.Get(i);
							if (pRemoveFilter.Matches(pListEntity))
							{
								pListEntity.Close();
								this.m_pEntityList.Erase(i);
							}
						}
						pRemoveFilter.Close();
						
						#if defined DEBUG
						LogDebug("Deleted %d entities", iEntCount - this.m_pEntityList.Length);
						#endif
					}
					else if (modify)
					{
						CEntityListLump pTempEvaluatedList = this.EvaluateEntityList(kv);
						if (pTempEvaluatedList.Length && kv.GotoFirstSubKey(true))
						{
							char szModifyType[MAX_KEY];
							do
							{
								if (kv.GetSectionName(szModifyType, sizeof(szModifyType)))
								{
									this.ParseModifyAction(kv, szModifyType, pTempEvaluatedList);
								}
							}
							while (kv.GotoNextKey(true));
							kv.GoBack();
						}
						pTempEvaluatedList.Close();
					}
				}
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
	}
	
	void ParseModifyAction(KeyValues kv, const char szModifyType[MAX_KEY], CEntityListLump pTempEvaluatedList)
	{
		bool add = IsAddSynonym(szModifyType);
		bool remove = IsRemoveSynonym(szModifyType);
		bool set = strcmp(szModifyType, "set", false) == 0;
		bool replace = strcmp(szModifyType, "replace", false) == 0;
		bool functions = strcmp(szModifyType, "functions", false) == 0;
		bool outputs = strcmp(szModifyType, "outputs", false) == 0;
		bool flags = strcmp(szModifyType, "flags", false) == 0;
		if (!add && !remove && !set && !replace && !functions && !outputs && !flags)
		{
			ThrowError("Unknown entity modify action \"%s\", expected one of add | remove | set | replace | functions | outputs | flags", szModifyType);
		}
		
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				if (functions)
				{
					this.ParseFunction(kv, pTempEvaluatedList);
				}
				else if (outputs)
				{
					this.ParseOutputs(kv, pTempEvaluatedList);
				}
				else if (flags)
				{
					this.ParseFlags(kv, pTempEvaluatedList);
				}
				else
				{
					CEntityKeyLump pProperty;
					if (kv.GetDataType(NULL_STRING) == KvData_None || !kv.GetSectionName(pProperty.m_szKey, sizeof(pProperty.m_szKey)))
					{
						continue;
					}
					kv.GetString(NULL_STRING, pProperty.m_szValue, sizeof(pProperty.m_szValue));
					
					if (add)
					{
						for (int i = 0; i < pTempEvaluatedList.Length; i++)
						{
							pTempEvaluatedList.Get(i).AddProperty(pProperty);
						}
					}
					else if (remove)
					{
						// match values if not empty
						bool bMatchValue = pProperty.m_szValue[0] != EOS;
						for (int i = 0; i < pTempEvaluatedList.Length; i++)
						{
							pTempEvaluatedList.Get(i).RemoveProperty(pProperty, bMatchValue);
						}
					}
					else // set, replace
					{
						for (int i = 0; i < pTempEvaluatedList.Length; i++)
						{
							// set = updates or adds property
							// replace = updates pre-existing only
							pTempEvaluatedList.Get(i).SetProperty(pProperty, set);
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
		}
	}
	
	void ParseFunction(KeyValues kv, CEntityListLump pTempEvaluatedList)
	{
		char szFunction[MAX_KEY], szValue[MAX_KEY];
		bool bFound = true;

		if (kv.GetSectionName(szFunction, sizeof(szFunction)))
		{
			if (kv.GetDataType(NULL_STRING) != KvData_None)
			{
				// inline functions
				kv.GetString(NULL_STRING, szValue, sizeof(szValue));
				if (StrEqual(szFunction, "delay_output", false))
				{
					CEntityOutputHook pOutputHook;
					pOutputHook.m_pEntityList = view_as<CEntityListLump>(CloneHandle(pTempEvaluatedList));
					pOutputHook.m_szOutputName = szValue;
					this.m_pOutputHookList.PushArray(pOutputHook);
					LogDebug("Delaying output \"%s\" for %d entities", szValue, pTempEvaluatedList.Length);
				}
				else bFound = false;
			}
			else
			{
				if (StrEqual(szFunction, "copy_model", false))
				{
					CEntityListLump pTargetEvaluatedList = this.EvaluateEntityList(kv);
					if (!pTargetEvaluatedList.Length)
					{
						ThrowError("Found 0 entities for copy model matching (expected 1+)");
					}

					CEntityKeyLump pModelKey, pOriginKey;
					for (int i = 0; i < pTargetEvaluatedList.Length; i++)
					{
						CEntityInfoLump pTargetEnt = pTargetEvaluatedList.Get(i);
						if (pTargetEnt.GetProperty("model", pModelKey))
						{
							bool bHasOrigin = pTargetEnt.GetProperty("origin", pOriginKey);
							for (int k = 0; k < pTempEvaluatedList.Length; k++)
							{
								CEntityInfoLump pEvalEnt = pTempEvaluatedList.Get(k);
								pEvalEnt.SetProperty(pModelKey, true);
								if (bHasOrigin)
								{
									pEvalEnt.SetProperty(pOriginKey, true);
								}
							}
							break;
						}
						else ThrowError("Target entity for copy_model has no model property");
					}
					pTargetEvaluatedList.Close();
				}
				else if (StrEqual(szFunction, "set_model", false))
				{
					float vecFrom[3], vecTo[3];
					if (kv.GetDataType("from") != KvData_None && kv.GetDataType("to") != KvData_None)
					{
						kv.GetVector("from", vecFrom);
						kv.GetVector("to", vecTo);
					}
					else if (kv.GetDataType("origin") != KvData_None && kv.GetDataType("size") != KvData_None)
					{
						float vecOrigin[3], vecSize[3];
						kv.GetVector("origin", vecOrigin);
						kv.GetVector("size", vecSize);
						ScaleVector(vecSize, 0.5);
						SubtractVectors(vecOrigin, vecSize, vecFrom);
						AddVectors(vecOrigin, vecSize, vecTo);
					}
					else ThrowError("Function set_model requires parameters 'from' + 'to' OR 'origin' + 'size'");
					
					CEntityKeyLump pModelKey;
					pModelKey.m_szKey = "model";
					Format(pModelKey.m_szValue, sizeof(pModelKey.m_szValue), "%s%f%s%f%s%f%s%f%s%f%s%f", BRUSH_FIXUP_IDENT,
						vecFrom[0], BRUSH_FIXUP_SEPARATOR, vecFrom[1], BRUSH_FIXUP_SEPARATOR, vecFrom[2], BRUSH_FIXUP_SEPARATOR,
						vecTo[0], BRUSH_FIXUP_SEPARATOR, vecTo[1], BRUSH_FIXUP_SEPARATOR, vecTo[2]);
						
					for (int k = 0; k < pTempEvaluatedList.Length; k++)
					{
						pTempEvaluatedList.Get(k).SetProperty(pModelKey, true);
					}
				}
				else bFound = false;
			}
			
			if (!bFound)
			{
				ThrowError("Unknown function \"%s\"", szFunction);
			}
		}
	}
	
	void ParseFlags(KeyValues kv, CEntityListLump pTempEvaluatedList)
	{
		char szFlagName[MAX_KEY], szAction[MAX_KEY];
		
		if (kv.GetDataType(NULL_STRING) == KvData_None &&
			kv.GetSectionName(szFlagName, sizeof(szFlagName)))
		{
			if (kv.GotoFirstSubKey(false))
			{
				do
				{
					if (kv.GetDataType(NULL_STRING) == KvData_Int
						&& kv.GetSectionName(szAction, sizeof(szAction)))
					{
						bool add = IsAddSynonym(szAction) || IsEnableSynonym(szAction);
						bool remove = IsRemoveSynonym(szAction) || IsDisableSynonym(szAction);
						if (!add && !remove)
						{
							ThrowError("Flags block \"%s\" contains unknown action \"%s\", expected one of add | remove", szFlagName, szAction);
						}
						
						int iActionValue = kv.GetNum(NULL_STRING);
						CEntityKeyLump pKeyInfo;
						
						for (int i = 0; i < pTempEvaluatedList.Length; i++)
						{
							CEntityInfoLump pEditedEntityInfo = pTempEvaluatedList.Get(i);
							bool bFoundFlag;
							for (int j = 0; j < pEditedEntityInfo.Length; j++)
							{
								if (pEditedEntityInfo.GetArray(j, pKeyInfo, sizeof(pKeyInfo)))
								{
									if (strcmp(pKeyInfo.m_szKey, szFlagName, false) == 0)
									{
										int iFlags = StringToInt(pKeyInfo.m_szValue);
										if (add)
										{
											iFlags |= iActionValue;
										}
										else if (remove)
										{
											iFlags &= ~iActionValue;
										}
										IntToString(iFlags, pKeyInfo.m_szValue, sizeof(pKeyInfo.m_szValue));
										pEditedEntityInfo.SetArray(j, pKeyInfo);
										bFoundFlag = true;
										break;
									}
								}
							}
							if (!bFoundFlag && add)
							{
								strcopy(pKeyInfo.m_szKey, sizeof(pKeyInfo.m_szKey), szFlagName);
								IntToString(iActionValue, pKeyInfo.m_szValue, sizeof(pKeyInfo.m_szValue));
								pEditedEntityInfo.PushArray(pKeyInfo);
							}
						}
					}
				}
				while (kv.GotoNextKey(false));
				kv.GoBack();
			}
		}
	}
	
	void ParseOutputs(KeyValues kv, CEntityListLump pTempEvaluatedList)
	{
		char szAction[MAX_KEY], szKey[MAX_KEY], szOutputName[MAX_KEY];
		if (!kv.GetSectionName(szAction, sizeof(szAction)))
		{
			return;
		}

		LogDebug("Performing output \"%s\" action for %d entities", szAction, pTempEvaluatedList.Length);
		
		bool bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF;
		if (IsAddSynonym(szAction))
		{
			CEntityOutputLump pOutputLump; CEntityKeyLump pOutputProp;
			
			this.ParseOutputKeys(kv, pOutputLump, pOutputProp.m_szKey, sizeof(pOutputProp.m_szKey),
				bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF);

			if (!bFoundOutput) ThrowError("Cannot add output - missing entry for \"output\"");
			if (!bFoundTarget) ThrowError("Cannot add output - missing entry for \"target\"");
			if (!bFoundInput) ThrowError("Cannot add output - missing entry for \"input\"");
			if (!bFoundTTF) pOutputLump.m_iTimesToFire = 1;
			
			pOutputLump.SaveToEntityKeyLump(pOutputProp);
			
			for (int i = 0; i < pTempEvaluatedList.Length; i++)
			{
				pTempEvaluatedList.Get(i).AddProperty(pOutputProp);
			}
			LogDebug("Adding output %s %s", pOutputProp.m_szKey, pOutputProp.m_szValue);
			return;
		}
		
		bool modify = IsModifySynonym(szAction);
		bool remove = IsRemoveSynonym(szAction);
		if (!modify && !remove)
		{
			ThrowError("Unknown outputs action \"%s\", expected one of add | remove | modify", szAction);
		}
		
		CEntityOutputLump pMatchOutputLump;
		this.ParseOutputKeys(kv, pMatchOutputLump, szOutputName, sizeof(szOutputName),
			bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF);
		
		// evaluate entities matching target if specified
		CEntityListLump pTargetEvaluatedList;
		if (kv.GetDataType("target") == KvData_None && kv.JumpToKey("target", false))
		{
			pTargetEvaluatedList = this.EvaluateEntityList(kv);
			LogDebug("Found %d entities for output target matching", pTargetEvaluatedList.Length);
			kv.GoBack();
		}

		#if defined DEBUG
		int iTouchedOutputs;
		#endif
		
		for (int i = 0; i < pTempEvaluatedList.Length; i++)
		{
			CEntityInfoLump pCandidateEntity = pTempEvaluatedList.Get(i);
			for (int j = 0; j < pCandidateEntity.Length; j++)
			{
				CEntityKeyLump pCandidateProp;
				pCandidateEntity.GetArray(j, pCandidateProp);

				if (bFoundOutput && !StrEqualsRegex(szOutputName, pCandidateProp.m_szKey, false))
				{
					continue; // avoid parsing output from value unless necessary
				}
				
				CEntityOutputLump pCandidateOutput;
				if (pCandidateOutput.LoadFromEntityKeyLump(pCandidateProp))
				{
					if (pMatchOutputLump.Matches(pCandidateOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF))
					{
						// skip any outputs that dont match the target entity
						if (pTargetEvaluatedList)
						{
							bool bFoundOne;
							for (int k = 0; k < pTargetEvaluatedList.Length; k++)
							{
								CEntityInfoLump pTargetEntityInfo = pTargetEvaluatedList.Get(k);
								if (pTargetEntityInfo.MatchesAsIOTarget(pCandidateOutput.m_szTargetEntity, pCandidateEntity))
								{
									bFoundOne = true;
									break;
								}
							}
							if (!bFoundOne)
							{
								continue;
							}
						}
						
						if (modify)
						{
							if (kv.GotoFirstSubKey(true))
							{
								do
								{
									if (kv.GetSectionName(szKey, sizeof(szKey))
										&& (StrEqual(szKey, "replace", false) || StrEqual(szKey, "set", false)))
									{
										LogDebug("Replacing output %s %s", pCandidateProp.m_szKey, pCandidateProp.m_szValue);
										this.ParseOutputKeys(kv, pCandidateOutput, pCandidateProp.m_szKey, sizeof(pCandidateProp.m_szKey));
										pCandidateOutput.SaveToEntityKeyLump(pCandidateProp);
										LogDebug("................ %s %s", pCandidateProp.m_szKey, pCandidateProp.m_szValue);
										pCandidateEntity.SetArray(j, pCandidateProp);

										#if defined DEBUG
										iTouchedOutputs++;
										#endif
									}
								}
								while (kv.GotoNextKey(true));
								kv.GoBack();
							}
						}
						else // remove
						{
							pCandidateEntity.Erase(j--);

							#if defined DEBUG
							iTouchedOutputs++;
							#endif
						}
					}
				}
			}
		}
		
		#if defined DEBUG
		LogDebug("%s %d outputs", remove? "Removed" : "Updated", iTouchedOutputs);
		#endif

		delete pTargetEvaluatedList;
	}
	
	void ParseOutputKeys(KeyValues kv, CEntityOutputLump pOutputLump, char[] szOutputName, int iOutputNameLength,
		bool& bFoundOutput = false, bool& bFoundTarget = false, bool& bFoundInput = false, bool& bFoundParameter = false, bool& bFoundDelay = false, bool& bFoundTTF = false)
	{
		bFoundOutput = kv.GetDataType("output") != KvData_None;
		if (bFoundOutput)
		{
			kv.GetString("output", szOutputName, iOutputNameLength);
		}
		bFoundTarget = kv.GetDataType("target") != KvData_None;
		if (bFoundTarget)
		{
			kv.GetString("target", pOutputLump.m_szTargetEntity, sizeof(pOutputLump.m_szTargetEntity));
		}
		bFoundInput = kv.GetDataType("input") != KvData_None;
		if (bFoundInput)
		{
			kv.GetString("input", pOutputLump.m_szInputName, sizeof(pOutputLump.m_szInputName));
		}
		bFoundParameter = kv.GetDataType("parameter") != KvData_None;
		if (bFoundParameter)
		{
			kv.GetString("parameter", pOutputLump.m_szParameter, sizeof(pOutputLump.m_szParameter));
		}
		bFoundDelay = kv.GetDataType("delay") != KvData_None;
		if (bFoundDelay)
		{
			pOutputLump.m_flDelay = kv.GetFloat("delay");
		}
		bFoundTTF = kv.GetDataType("timestofire") != KvData_None;
		if (bFoundTTF)
		{
			pOutputLump.m_iTimesToFire = kv.GetNum("timestofire");
		}
	}

	COutputHookList GetOutputHooksForEntity(CBaseEntity pEntity)
	{
		COutputHookList pResolvedList = new COutputHookList();

		for (int i = 0; i < this.m_pOutputHookList.Length; i++)
		{
			CEntityOutputHook pOutputHook;
			this.m_pOutputHookList.GetArray(i, pOutputHook);

			for (int j = 0; j < pOutputHook.m_pEntityList.Length; j++)
			{
				if (pOutputHook.m_pEntityList.Get(j).MatchesActualEntity(pEntity))
				{
					pResolvedList.PushArray(pOutputHook);
				}
			}
		}
		return pResolvedList;
	}
	
	void ApplyLateConvars()
	{
		CServerConsoleCommand cmd;
		int len = this.m_pServerCvarList.Length;
		for (int i = 0; i < len; i++)
		{
			this.m_pServerCvarList.GetArray(i, cmd);
			if (cmd.m_bLate)
			{
				#if defined DEBUG
				char szName[128];
				cmd.m_pConvar.GetName(szName, sizeof(szName));
				LogDebug("Setting late convar: [\"%s\" = \"%s\"]", szName, cmd.m_szValue);
				#endif

				cmd.m_pConvar.SetString(cmd.m_szValue);
			}
		}
	}

	void RevertConvars()
	{
		CServerConsoleCommand cmd;
		int len = this.m_pServerCvarList.Length;
		for (int i = 0; i < len; i++)
		{
			this.m_pServerCvarList.GetArray(i, cmd);
			if (cmd.m_bHooked)
			{
				cmd.m_pConvar.RemoveChangeHook(Hook_ForceConvarValue);
			}
			cmd.m_pConvar.Flags &= ~cmd.m_iFlagsAdd;
			cmd.m_pConvar.RestoreDefault();
		}
		this.m_pServerCvarList.Clear();
	}
}

CGlobalLevelLump g_pLevelLump;

/**
 * Entity spawn hook for extracting pre-set model strings and applying resultant dimensions.
 */
public Action Hook_FixupBrushModels(int iEntIndex)
{
	SDKUnhook(iEntIndex, SDKHook_Spawn, Hook_FixupBrushModels);
	
	CBaseEntity pThis = CBaseEntity(iEntIndex);
	static char szModelName[PLATFORM_MAX_PATH];
	pThis.GetModelName(szModelName, sizeof(szModelName));
	if (strncmp(szModelName, BRUSH_FIXUP_IDENT, BRUSH_FIXUP_IDENT_LEN) == 0)
	{
		if (PrecacheModel(BRUSH_FIXUP_MDL))
		{
			pThis.SetModelName(BRUSH_FIXUP_MDL);
			
			DataPack pack = new DataPack();
			pack.WriteCell(pThis);
			pack.WriteString(szModelName);
			pack.Reset();
			RequestFrame(FixupBrushModelsPost, pack);
		}
		else ThrowError("Couldn't precache model %s", BRUSH_FIXUP_MDL);
	}
	return Plugin_Continue;
}

public void FixupBrushModelsPost(DataPack pack)
{
	CBaseEntity pThis = pack.ReadCell();
	if (pThis.IsValid())
	{
		static char szModelName[PLATFORM_MAX_PATH];
		pack.ReadString(szModelName, sizeof(szModelName));
		
		static char buffers[6][128];
		ExplodeString(szModelName[BRUSH_FIXUP_IDENT_LEN], BRUSH_FIXUP_SEPARATOR, buffers, sizeof(buffers), sizeof(buffers[]));
		float vecFrom[3], vecTo[3], vecOrigin[3], vecDistance[3], vecMins[3], vecMaxs[3];
		vecFrom[0] = StringToFloat(buffers[0]); vecTo[0] = StringToFloat(buffers[3]);
		vecFrom[1] = StringToFloat(buffers[1]); vecTo[1] = StringToFloat(buffers[4]);
		vecFrom[2] = StringToFloat(buffers[2]); vecTo[2] = StringToFloat(buffers[5]);
		
		MoveVector(vecFrom, vecTo, 0.5, vecOrigin);
		GetVectorDistanceVector(vecFrom, vecTo, vecDistance);
		vecMins = vecDistance;
		ScaleVector(vecMins, -0.5);
		vecMaxs = vecDistance;
		ScaleVector(vecMaxs, 0.5);
		
		pThis.Teleport(vecOrigin);
		pThis.SetCollisionBounds(vecMins, vecMaxs);
		pThis.SetSolidType(SOLID_BBOX);
		pThis.m_fEffects |= EF_NODRAW;
	}
	delete pack;
}

void Hook_ForceConvarValue(ConVar convar, const char[] oldValue, const char[] newValue)
{
	int index = g_pLevelLump.m_pServerCvarList.FindValue(convar, CServerConsoleCommand::m_pConvar);
	if (index == -1)
	{
		char szName[128];
		convar.GetName(szName, sizeof(szName));
		ThrowError("Hooked ConVar \"%s\" is missing from m_pServerCvarList!", szName);
	}

	CServerConsoleCommand cmd;
	g_pLevelLump.m_pServerCvarList.GetArray(index, cmd);
	if (!StrEqual(newValue, cmd.m_szValue))
	{
		cmd.m_pConvar.RemoveChangeHook(Hook_ForceConvarValue);
		cmd.m_pConvar.SetString(cmd.m_szValue);
		cmd.m_pConvar.AddChangeHook(Hook_ForceConvarValue);
	}
}