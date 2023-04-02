#if defined _srccoop_levellump_included
 #endinput
#endif
#define _srccoop_levellump_included

#pragma newdecls required
#pragma semicolon 1

#define OUTPUT_SEPARATOR "\27"
#define BRUSH_FIXUP_MDL "models/error.mdl"
#define BRUSH_FIXUP_IDENT "$BRUSHFIXUP$"
#define BRUSH_FIXUP_IDENT_LEN 12
#define BRUSH_FIXUP_SEPARATOR " "

enum struct CEntityKeyLump
{
	char m_szKey[MAX_KEY];
	char m_szValue[MAX_VALUE];
}

enum struct CEntityOutputLump
{
	char m_szTargetEntity[MAX_VALUE];
	char m_szInputName[MAX_VALUE];
	char m_szParameter[MAX_VALUE];
	float m_flDelay;
	int m_iTimesToFire;
	
	bool LoadFromEntityKeyLump(CEntityKeyLump pEntityKeyLump)
	{
		char buffers[5][MAX_VALUE];
		if(ExplodeString(pEntityKeyLump.m_szValue, OUTPUT_SEPARATOR, buffers, sizeof(buffers), sizeof(buffers[])) != sizeof(buffers))
		{
			return false;
		}
		if(!StringToFloatEx(buffers[3], this.m_flDelay))
		{
			return false;
		}
		if(!StringToIntEx(buffers[4], this.m_iTimesToFire))
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
		this.m_szTargetEntity, OUTPUT_SEPARATOR, this.m_szInputName, OUTPUT_SEPARATOR, this.m_szParameter, OUTPUT_SEPARATOR, this.m_flDelay, OUTPUT_SEPARATOR, this.m_iTimesToFire);
	}
	
	bool Matches(CEntityOutputLump pOther, bool bMatchTarget, bool bMatchInput, bool bMatchParameter, bool bMatchDelay, bool bMatchTTF)
	{
		if(bMatchTarget && !StrEqualsRegex(this.m_szTargetEntity, pOther.m_szTargetEntity, false)
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

methodmap CEntityInfoLump < ArrayList
{
	public CEntityInfoLump(ArrayList pHandle)
	{
		return view_as<CEntityInfoLump>(pHandle);
	}
	
	public bool Matches(CEntityInfoLump rhs)
	{
		for (int i = 0; i < this.Length; i++)
		{
			bool bGotEntry = false;
			
			CEntityKeyLump pThisKeyInfo;
			if (this.GetArray(i, pThisKeyInfo, sizeof(pThisKeyInfo)))
			{
				for (int j = 0; j < rhs.Length; j++)
				{
					CEntityKeyLump rhsKeyInfo;
					if (rhs.GetArray(j, rhsKeyInfo, sizeof(rhsKeyInfo)))
					{
						if (StrEqualsRegex(pThisKeyInfo.m_szKey, rhsKeyInfo.m_szKey, false) && StrEqualsRegex(pThisKeyInfo.m_szValue, rhsKeyInfo.m_szValue, false))
						{
							bGotEntry = true;
							break;
						}
					}
				}
			}
			
			if (!bGotEntry)
			{
				return false;
			}
		}
		
		return true;
	}
		
	public bool MatchesAsIOTarget(char[] szTarget, CEntityInfoLump pCallingEntity)
	{
		if(strcmp(szTarget, "!self") == 0)
		{
			return this.Matches(pCallingEntity);
		}
		for (int i = 0; i < this.Length; i++)
		{
			CEntityKeyLump pThisKeyInfo;
			if (this.GetArray(i, pThisKeyInfo, sizeof(pThisKeyInfo)))
			{
				if(strcmp(pThisKeyInfo.m_szKey, "classname") == 0 && strcmp(pThisKeyInfo.m_szValue, szTarget) == 0)
				{
					return true;
				}
				if(strcmp(pThisKeyInfo.m_szKey, "targetname") == 0 && strcmp(pThisKeyInfo.m_szValue, szTarget) == 0)
				{
					return true;
				}
			}
		}
		return false;
	}
	
	public bool MatchesActualEntity(CBaseEntity pEntity)
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityKeyLump pThisKeyInfo;
			if (this.GetArray(i, pThisKeyInfo, sizeof(pThisKeyInfo)))
			{
				// check these 3 properties for now, return false if any dont match
				if(StrEqual(pThisKeyInfo.m_szKey, "hammerid"))
				{
					if(StringToInt(pThisKeyInfo.m_szValue) != pEntity.GetHammerID())
					{
						return false;
					}
				}
				if(StrEqual(pThisKeyInfo.m_szKey, "classname"))
				{
					if(!pEntity.IsClassname(pThisKeyInfo.m_szValue))
					{
						return false;
					}
				}
				if(StrEqual(pThisKeyInfo.m_szKey, "targetname"))
				{
					char szBuffer[MAX_CLASSNAME];
					pEntity.GetTargetname(szBuffer, sizeof(szBuffer));
					if(!StrEqual(pThisKeyInfo.m_szValue, szBuffer))
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	
	public void RemoveOrReplace(CEntityKeyLump pKeyInfo, bool replace)
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityKeyLump pThisKeyInfo;
			if (this.GetArray(i, pThisKeyInfo, sizeof(pThisKeyInfo)))
			{
				if (StrEqualsRegex(pKeyInfo.m_szKey, pThisKeyInfo.m_szKey, false))
				{
					this.Erase(i);
					break;
				}
			}
		}
		if(replace)
		{
			this.PushArray(pKeyInfo, sizeof(pKeyInfo));
		}
	}
	
	public bool GetKey(const char[] szKey, CEntityKeyLump pKeyInfo)
	{
		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pKeyInfo);
			if (strcmp(pKeyInfo.m_szKey, szKey, false) == 0)
			{
				return true;
			}
		}
		return false;
	}
}

methodmap CEntityListLump < ArrayList
{
	public CEntityListLump(ArrayList pHandle)
	{
		return view_as<CEntityListLump>(pHandle);
	}
	
	public void Clear()
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityInfoLump pEntityInfo = this.Get(i);
			delete pEntityInfo;
		}
		ClearArray(this);
	}
}

enum struct CEntityOutputHook
{
	CEntityListLump m_pEntityList;
	char m_szOutputName[MAX_VALUE];
}

methodmap COutputHookLump < ArrayList
{
	public COutputHookLump(ArrayList pHandle)
	{
		return view_as<COutputHookLump>(pHandle);
	}
	
	public void Clear()
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityOutputHook pOutputHook; this.GetArray(i, pOutputHook);
			pOutputHook.m_pEntityList.Close();
		}
		ClearArray(this);
	}
	
	// Remove hooks whose entities are not contained in the list (f.e. deleted when parsing)
	public void Validate(CEntityListLump pValidEntityList)
	{
		for (int i = 0; i < this.Length; i++)
		{
			CEntityOutputHook pOutputHook; this.GetArray(i, pOutputHook);
			for (int j = 0; j < pOutputHook.m_pEntityList.Length; j++)
			{
				CEntityInfoLump pEntityInfo = pOutputHook.m_pEntityList.Get(j);
				if(pValidEntityList.FindValue(pEntityInfo) == -1)
				{
					pOutputHook.m_pEntityList.Erase(j--);
				}
			}
			if(!pOutputHook.m_pEntityList.Length)
			{
				pOutputHook.m_pEntityList.Close();
				this.Erase(i--);
			}
		}
	}
}

methodmap CNextMapLump < ArrayList
{
	public CNextMapLump(ArrayList pHandle)
	{
		return view_as<CNextMapLump>(pHandle);
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

enum struct CGlobalLevelLump
{
	CEntityListLump m_pEntityList;
	CNextMapLump m_pNextMapList;
	ArrayList m_pModifiedCvarList;
	COutputHookLump m_pOutputHookList;
	IntroType m_iIntroType;
	StringMap m_EdtDefinesMap;
	
	void Initialize()
	{
		this.m_pEntityList = new CEntityListLump(CreateArray(sizeof(CEntityInfoLump)));
		this.m_pNextMapList = new CNextMapLump(CreateArray(MAX_MAPNAME));
		this.m_pModifiedCvarList = CreateArray();
		this.m_pOutputHookList = new COutputHookLump(CreateArray(sizeof(CEntityOutputHook)));
		this.m_EdtDefinesMap = new StringMap();
	}
	
	void Clear()
	{
		this.m_pNextMapList.Clear();
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
				if(bParsingValue)
				{
					n = SplitByChar(szMapEntities[++i], '"', pEntityInfoKey.m_szValue, sizeof(pEntityInfoKey.m_szValue));
					pEntityInfo.PushArray(pEntityInfoKey, sizeof(pEntityInfoKey));
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
				
				pEntityInfo = new CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
			}
			else if (szMapEntities[i] == '}')
			{
				if (!pEntityInfo)
					ThrowError("Did not have valid CEntityInfoLump when finding '}'");
				
				this.m_pEntityList.Push(pEntityInfo);
				pEntityInfo = null;
			}
		}
		
		LogDebug("CGlobalLevelLump.ParseMapEntities took %f", GetEngineTime() - flStartTime);
	}
	
	int ToString(char szBuffer[ENTITYSTRING_LENGTH])	// absolutely do not care for bounds checking, this 2 mb buffer is big enough for everything
	{
		float flStartTime = GetEngineTime();
		
		int n = 0;
		for (int i = 0; i < this.m_pEntityList.Length; i++)
		{
			CEntityInfoLump pEntityInfo = this.m_pEntityList.Get(i);
			if (!pEntityInfo) // should be never invalid
				return -1;
			if (pEntityInfo.Length <= 0) // never write if it has nothing in it to begin with
				continue;
			
			szBuffer[n] = '{';
			szBuffer[n + 1] = '\n';
			n += 2;
			
			for (int j = 0; j < pEntityInfo.Length; j++)
			{
				CEntityKeyLump pEntityInfoKey;
				if (pEntityInfo.GetArray(j, pEntityInfoKey, sizeof(pEntityInfoKey)))
				{
					static char szKeyValue[2048];
					int iLength = Format(szKeyValue, sizeof(szKeyValue), "\t\"%s\"\t\"%s\"\n", pEntityInfoKey.m_szKey, pEntityInfoKey.m_szValue);
					strcopy(szBuffer[n], iLength + 1, szKeyValue);
					n += iLength;
				}
			}
			
			szBuffer[n] = '}';
			szBuffer[n + 1] = (i == (this.m_pEntityList.Length - 1)) ? '\0' : '\n'; // if this doesn't end with a null terminator, the token will search for '{' and crash the server
			n += 2;
		}
		
		LogDebug("CGlobalLevelLump.ToString took %f", GetEngineTime() - flStartTime);
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
					if (kv.GetDataType(NULL_STRING) != KvData_None)
					{
						if (strcmp(szType, "nextmap", false) == 0)
						{
							char szNextMapName[MAX_MAPNAME];
							kv.GetString(NULL_STRING, szNextMapName, sizeof(szNextMapName));
							this.m_pNextMapList.PushString(szNextMapName);
						}
						else if (strcmp(szType, "intro_type", false) == 0)
						{
							char szIntroType[16];
							kv.GetString(NULL_STRING, szIntroType, sizeof(szIntroType));
							if(strcmp(szIntroType, "fade", false) == 0)
							{
								this.m_iIntroType = INTRO_FADE;
							}
							else if(strcmp(szIntroType, "none", false) == 0)
							{
								this.m_iIntroType = INTRO_NONE;
							}
							else if(strcmp(szIntroType, "freeze", false) == 0)
							{
								this.m_iIntroType = INTRO_FREEZE;
							}
						}
					}
					else
					{
						if (strcmp(szType, "define", false) == 0)
						{
							this.ParseDefines(kv);
						}
						else if (strcmp(szType, "features", false) == 0)
						{
							this.ParseFeatures(kv);
						}
						else if (strcmp(szType, "console", false) == 0)
						{
							this.ParseConsole(kv);
						}
						else if (strcmp(szType, "entity", false) == 0 || StrContains(szType, "entity_base", false) == 0)
						{
							LogDebug("Processing \"%s\" block for entity lump modifications", szType);
							this.ParseEntityEdits(kv);
						}
						else if (strcmp(szType, "equipment", false) == 0)
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
		
		LogDebug("CGlobalLevelLump.ParseConfigFile took %f", GetEngineTime() - flStartTime);
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
					
					if(StrEqual(szKey, "lookup", false))
					{
						if (kv.GotoFirstSubKey(false))
						{
							CEntityInfoLump pEntityInfo = new CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
							do
							{
								CEntityKeyLump pEntityKeyInfo;
								if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
								{
									kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
									pEntityInfo.PushArray(pEntityKeyInfo);
								}
							}
							while (kv.GotoNextKey(false));
							kv.GoBack();
							
							for (int i = 0; i < this.m_pEntityList.Length; i++)
							{
								CEntityInfoLump pThisEntityInfo = this.m_pEntityList.Get(i);
								if (pEntityInfo.Matches(pThisEntityInfo))
								{
									CEntityKeyLump pClassKey;
									if(pThisEntityInfo.GetKey("classname", pClassKey))
									{
										g_SpawnSystem.AddSpawnItem(pClassKey.m_szValue, true);
										pThisEntityInfo.Close();
										this.m_pEntityList.Erase(i);
										i--;
									}
								}
							}
							pEntityInfo.Close();
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
					
					if(kv.GetDataType(NULL_STRING) != KvData_None)
					{
						char szValue[MAX_FORMAT];
						kv.GetString(NULL_STRING, szValue, sizeof(szValue));
						this.m_EdtDefinesMap.SetString(szKey, szValue, true);
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
					
					if(kv.GetDataType(NULL_STRING) != KvData_None)
					{
						SourceCoopFeature feature;
						if (g_pFeatureMap.GetFeature(szKey, feature))
						{
							if(kv.GetNum(NULL_STRING))
							{
								CoopManager.EnableFeature(feature);
							}
							else
							{
								CoopManager.DisableFeature(feature);
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
			do
			{
				char szKey[MAX_KEY];
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
						
						char szValue[MAX_VALUE];
						kv.GetString(NULL_STRING, szValue, sizeof(szValue));
						ConVar pConvar = FindConVar(szKey);
						if (pConvar != null)
						{
							this.m_pModifiedCvarList.Push(pConvar);
							pConvar.SetString(szValue);
						}
					}
				}
			}
			while (kv.GotoNextKey(false));
			kv.GoBack();
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
					if(!add && !remove && !modify) {
						ThrowError("Unknown entity action \"%s\", expected one of add | remove | modify", szAction);
					}
					
					if (add)
					{
						if (kv.GotoFirstSubKey(false))
						{
							CEntityInfoLump pEntityInfo = new CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
							do
							{
								CEntityKeyLump pEntityKeyInfo;
								if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
								{
									kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
									pEntityInfo.PushArray(pEntityKeyInfo);
								}
							}
							while (kv.GotoNextKey(false));
							kv.GoBack();
							this.m_pEntityList.Push(pEntityInfo);
							
							// alow to directly use modify actions here
							if (kv.GotoFirstSubKey(true))
							{
								CEntityListLump pEntityWrapperList = new CEntityListLump(CreateArray(sizeof(CEntityInfoLump)));
								pEntityWrapperList.Push(pEntityInfo);
								do
								{
									char szModifyType[MAX_KEY];
									if (kv.GetSectionName(szModifyType, sizeof(szModifyType)))
									{
										this.ParseModifyAction(kv, szModifyType, pEntityWrapperList);
									}
								}
								while (kv.GotoNextKey(true));
								kv.GoBack();
								pEntityWrapperList.Close();
							}
						}
					}
					else if (remove)
					{
						if (kv.GotoFirstSubKey(false))
						{
							CEntityInfoLump pEntityInfo = new CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
							do
							{
								CEntityKeyLump pEntityKeyInfo;
								if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
								{
									kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
									pEntityInfo.PushArray(pEntityKeyInfo);
								}
							}
							while (kv.GotoNextKey(false));
							kv.GoBack();
							
							for (int i = 0; i < this.m_pEntityList.Length; i++)
							{
								CEntityInfoLump pThisEntityInfo = this.m_pEntityList.Get(i);
								if (pEntityInfo.Matches(pThisEntityInfo))
								{
									pThisEntityInfo.Close();
									this.m_pEntityList.Erase(i);
									i--;
								}
							}
							pEntityInfo.Close();
						}
					}
					else if (modify)
					{
						if (kv.GotoFirstSubKey(false))
						{
							CEntityListLump pTempEvaluatedList = this.EvaluateEntityList(kv);
							if (pTempEvaluatedList.Length && kv.GotoFirstSubKey(true))
							{
								do
								{
									char szModifyType[MAX_KEY];
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
			}
			while (kv.GotoNextKey(true));
			kv.GoBack();
		}
	}
	
	CEntityListLump EvaluateEntityList(KeyValues kv)
	{
		CEntityInfoLump pEvaulationEntity = new CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
		CEntityListLump pTempEvaluatedList = new CEntityListLump(CreateArray(sizeof(CEntityInfoLump)));
		
		do
		{
			CEntityKeyLump pEntityKeyInfo;
			if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
			{
				kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
				pEvaulationEntity.PushArray(pEntityKeyInfo, sizeof(pEntityKeyInfo));
			}
		}
		while (kv.GotoNextKey(false));
		kv.GoBack();
		
		for (int i = 0; i < this.m_pEntityList.Length; i++)
		{
			CEntityInfoLump pThisEntityInfo = this.m_pEntityList.Get(i);
			if (pEvaulationEntity.Matches(pThisEntityInfo))
			{
				pTempEvaluatedList.Push(pThisEntityInfo);
			}
		}
		pEvaulationEntity.Close();
		return pTempEvaluatedList;
	}
	
	void ParseModifyAction(KeyValues kv, char szModifyType[MAX_KEY], CEntityListLump pTempEvaluatedList)
	{
		bool add = IsAddSynonym(szModifyType) || IsReplaceSynonym(szModifyType); // Add and replace do the same thing; pre-existing keys are always removed to prevent duplication
		bool remove = IsRemoveSynonym(szModifyType);
		bool functions = strcmp(szModifyType, "functions", false) == 0;
		bool outputs = strcmp(szModifyType, "outputs", false) == 0;
		bool flags = strcmp(szModifyType, "flags", false) == 0;
		if(!add && !remove && !functions && !outputs && !flags)
		{
			ThrowError("Unknown entity modify action \"%s\", expected one of add | remove | functions | outputs | flags", szModifyType);
		}
		
		if (kv.GotoFirstSubKey(false))
		{
			do
			{
				if(functions)
				{
					this.ParseFunction(kv, pTempEvaluatedList);
				}
				else if(outputs)
				{
					this.ParseOutputs(kv, pTempEvaluatedList);
				}
				else if(flags)
				{
					this.ParseFlags(kv, pTempEvaluatedList);
				}
				else // add, remove
				{
					CEntityKeyLump pKeyInfo;
					if (kv.GetSectionName(pKeyInfo.m_szKey, sizeof(pKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
					{
						kv.GetString(NULL_STRING, pKeyInfo.m_szValue, sizeof(pKeyInfo.m_szValue));
						for (int i = 0; i < pTempEvaluatedList.Length; i++)
						{
							CEntityInfoLump pEditedEntityInfo = pTempEvaluatedList.Get(i);
							pEditedEntityInfo.RemoveOrReplace(pKeyInfo, add);
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
			if(kv.GetDataType(NULL_STRING) != KvData_None)
			{
				// inline functions
				kv.GetString(NULL_STRING, szValue, sizeof(szValue));
				if(StrEqual(szFunction, "delay_output", false))
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
				if(StrEqual(szFunction, "copy_model", false))
				{
					if(kv.GotoFirstSubKey(false))
					{
						CEntityListLump pTargetEvaluatedList;
						pTargetEvaluatedList = this.EvaluateEntityList(kv);
						if(!pTargetEvaluatedList.Length)
						{
							ThrowError("Found 0 entities for copy model matching (expects 1+)");
						}
						CEntityKeyLump pModelKey, pOriginKey;
						for (int i = 0; i < pTargetEvaluatedList.Length; i++)
						{
							CEntityInfoLump pTargetEnt = pTargetEvaluatedList.Get(i);
							if(pTargetEnt.GetKey("model", pModelKey))
							{
								bool bHasOrigin = pTargetEnt.GetKey("origin", pOriginKey);
								for (int k = 0; k < pTempEvaluatedList.Length; k++)
								{
									CEntityInfoLump pTempEnt = pTempEvaluatedList.Get(k);
									pTempEnt.RemoveOrReplace(pModelKey, true);
									if(bHasOrigin)
									{
										pTempEnt.RemoveOrReplace(pOriginKey, true);
									}
								}
								break;
							}
							else ThrowError("Target entity for copy_model has no model property");
						}
						pTargetEvaluatedList.Close();
					}
				}
				else if(StrEqual(szFunction, "set_model", false))
				{
					float vecFrom[3], vecTo[3];
					if(kv.GetDataType("from") != KvData_None && kv.GetDataType("to") != KvData_None)
					{
						kv.GetVector("from", vecFrom);
						kv.GetVector("to", vecTo);
					}
					else if(kv.GetDataType("origin") != KvData_None && kv.GetDataType("size") != KvData_None)
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
						CEntityInfoLump pEnt = pTempEvaluatedList.Get(k);
						pEnt.RemoveOrReplace(pModelKey, true);
					}
				}
				else bFound = false;
			}
			
			if(!bFound)
			{
				ThrowError("Unknown function %s", szFunction);
			}
		}
	}
	
	void ParseFlags(KeyValues kv, CEntityListLump pTempEvaluatedList)
	{
		char szFlagName[MAX_KEY], szAction[MAX_KEY];
		if (kv.GetDataType(NULL_STRING) == KvData_None && kv.GetSectionName(szFlagName, sizeof(szFlagName)))
		{
			if (kv.GotoFirstSubKey(false))
			{
				do
				{
					if (kv.GetDataType(NULL_STRING) == KvData_Int && kv.GetSectionName(szAction, sizeof(szAction)))
					{
						bool add = IsAddSynonym(szAction) || IsEnableSynonym(szAction);
						bool remove = IsRemoveSynonym(szAction) || IsDisableSynonym(szAction);
						if(!add && !remove)
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
										if(add)
										{
											iFlags |= iActionValue;
										}
										else if(remove)
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
							if(!bFoundFlag && add)
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
		if (kv.GetSectionName(szAction, sizeof(szAction)))
		{
			LogDebug("Performing output \"%s\" action for %d entities", szAction, pTempEvaluatedList.Length);
			
			bool bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF;
			if(IsAddSynonym(szAction))
			{
				for (int i = 0; i < pTempEvaluatedList.Length; i++)
				{
					CEntityInfoLump pEditedEntityInfo = pTempEvaluatedList.Get(i);
					CEntityOutputLump pNewOutputLump; CEntityKeyLump pNewKeyInfo;
					
					this.ParseOutputKeys(kv, pNewOutputLump, pNewKeyInfo.m_szKey, sizeof(pNewKeyInfo.m_szKey), bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF);
					if(!bFoundOutput) ThrowError("Cannot add output - missing entry for \"output\"");
					if(!bFoundTarget) ThrowError("Cannot add output - missing entry for \"target\"");
					if(!bFoundInput) ThrowError("Cannot add output - missing entry for \"input\"");
					if(!bFoundTTF) pNewOutputLump.m_iTimesToFire = 1;
					
					pNewOutputLump.SaveToEntityKeyLump(pNewKeyInfo);
					pEditedEntityInfo.PushArray(pNewKeyInfo);
				}
				return;
			}
			
			bool modify = IsModifySynonym(szAction);
			bool remove = IsRemoveSynonym(szAction);
			if(!modify && !remove)
			{
				ThrowError("Unknown outputs action \"%s\", expected one of add | remove | modify", szAction);
			}
			
			CEntityOutputLump pMatchOutputLump;
			this.ParseOutputKeys(kv, pMatchOutputLump, szOutputName, sizeof(szOutputName), bFoundOutput, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF);
			
			// evaluate entities matching target if specified
			CEntityListLump pTargetEvaluatedList;
			if(kv.GetDataType("target") == KvData_None && kv.JumpToKey("target", false))
			{
				if(kv.GotoFirstSubKey(false)) {
					pTargetEvaluatedList = this.EvaluateEntityList(kv);
					LogDebug("Found %d entities for output target matching", pTargetEvaluatedList.Length);
				}
				kv.GoBack();
			}
			
			for (int i = 0; i < pTempEvaluatedList.Length; i++)
			{
				CEntityInfoLump pEditedEntityInfo = pTempEvaluatedList.Get(i);
				for (int j = 0; j < pEditedEntityInfo.Length; j++)
				{
					CEntityKeyLump pEditedKeyInfo; pEditedEntityInfo.GetArray(j, pEditedKeyInfo, sizeof(pEditedKeyInfo));
					if(bFoundOutput && !StrEqualsRegex(szOutputName, pEditedKeyInfo.m_szKey, false))
					{
						continue; // avoid parsing output from value unless necessary
					}
					
					CEntityOutputLump pEntityOutputLump;
					if(pEntityOutputLump.LoadFromEntityKeyLump(pEditedKeyInfo))
					{
						if(pMatchOutputLump.Matches(pEntityOutputLump, bFoundTarget, bFoundInput, bFoundParameter, bFoundDelay, bFoundTTF))
						{
							// skip any outputs that dont match the target entity
							if(pTargetEvaluatedList)
							{
								bool bFoundOne;
								for (int k = 0; k < pTargetEvaluatedList.Length; k++)
								{
									CEntityInfoLump pTargetEntityInfo = pTargetEvaluatedList.Get(k);
									if(pTargetEntityInfo.MatchesAsIOTarget(pEntityOutputLump.m_szTargetEntity, pEditedEntityInfo))
									{
										bFoundOne = true;
										break;
									}
								}
								if(!bFoundOne)
									continue;
							}
							if(modify)
							{
								if (kv.GotoFirstSubKey(true))
								{
									do
									{
										if (kv.GetSectionName(szKey, sizeof(szKey)) && IsReplaceSynonym(szKey))
										{
											LogDebug("Replacing output %s %s", pEditedKeyInfo.m_szKey, pEditedKeyInfo.m_szValue);
											this.ParseOutputKeys(kv, pEntityOutputLump, pEditedKeyInfo.m_szKey, sizeof(pEditedKeyInfo.m_szKey));
											pEntityOutputLump.SaveToEntityKeyLump(pEditedKeyInfo);
											LogDebug(".....With output %s %s", pEditedKeyInfo.m_szKey, pEditedKeyInfo.m_szValue);
											pEditedEntityInfo.SetArray(j, pEditedKeyInfo);
										}
									}
									while (kv.GotoNextKey(true));
									kv.GoBack();
								}
							}
							else if (remove)
							{
								pEditedEntityInfo.Erase(j--);
							}
						}
					}
				}
			}
			pTargetEvaluatedList.Close();
		}
	}
	
	void ParseOutputKeys(KeyValues kv, CEntityOutputLump pOutputLump, char[] szOutputName, int iOutputNameLength,
		bool& bFoundOutput = false, bool& bFoundTarget = false, bool& bFoundInput = false, bool& bFoundParameter = false, bool& bFoundDelay = false, bool& bFoundTTF = false)
	{
		bFoundOutput = kv.GetDataType("output") != KvData_None;
		if(bFoundOutput)
		{
			kv.GetString("output", szOutputName, iOutputNameLength);
		}
		bFoundTarget = kv.GetDataType("target") != KvData_None;
		if(bFoundTarget)
		{
			kv.GetString("target", pOutputLump.m_szTargetEntity, sizeof(pOutputLump.m_szTargetEntity));
		}
		bFoundInput = kv.GetDataType("input") != KvData_None;
		if(bFoundInput)
		{
			kv.GetString("input", pOutputLump.m_szInputName, sizeof(pOutputLump.m_szInputName));
		}
		bFoundParameter = kv.GetDataType("parameter") != KvData_None;
		if(bFoundParameter)
		{
			kv.GetString("parameter", pOutputLump.m_szParameter, sizeof(pOutputLump.m_szParameter));
		}
		bFoundDelay = kv.GetDataType("delay") != KvData_None;
		if(bFoundDelay)
		{
			pOutputLump.m_flDelay = kv.GetFloat("delay");
		}
		bFoundTTF = kv.GetDataType("timestofire") != KvData_None;
		if(bFoundTTF)
		{
			pOutputLump.m_iTimesToFire = kv.GetNum("timestofire");
		}
	}

	ArrayList GetOutputHooksForEntity(CBaseEntity pEntity)
	{
		ArrayList pResolvedOutputHookList = new ArrayList(sizeof(CEntityOutputHook));
		for (int i = 0; i < this.m_pOutputHookList.Length; i++)
		{
			CEntityOutputHook pOutputHook; this.m_pOutputHookList.GetArray(i, pOutputHook);
			CEntityListLump pEntityList = pOutputHook.m_pEntityList;
			for (int j = 0; j < pEntityList.Length; j++)
			{
				CEntityInfoLump pHookedEntityInfo = pEntityList.Get(j);
				if(pHookedEntityInfo.MatchesActualEntity(pEntity))
				{
					pResolvedOutputHookList.PushArray(pOutputHook);
				}
			}
		}
		return pResolvedOutputHookList;
	}
	
	void RevertConvars()
	{
		for (int i = 0; i < this.m_pModifiedCvarList.Length; i++)
		{
			ConVar pConvar = this.m_pModifiedCvarList.Get(i);
			pConvar.RestoreDefault();
			delete pConvar;
		}
		this.m_pModifiedCvarList.Clear();
	}
}

CGlobalLevelLump g_pLevelLump;

public Action Hook_FixupBrushModels(int iEntIndex)
{
	SDKUnhook(iEntIndex, SDKHook_Spawn, Hook_FixupBrushModels);
	
	CBaseEntity pThis = CBaseEntity(iEntIndex);
	static char szModelName[PLATFORM_MAX_PATH];
	pThis.GetModelName(szModelName, sizeof(szModelName));
	if(strncmp(szModelName, BRUSH_FIXUP_IDENT, BRUSH_FIXUP_IDENT_LEN) == 0)
	{
		if(PrecacheModel(BRUSH_FIXUP_MDL))
		{
			pThis.SetModelName(BRUSH_FIXUP_MDL);
			
			DataPack pack = new DataPack();
			pack.WriteCell(pThis);
			pack.WriteString(szModelName);
			pack.Reset();
			RequestFrame(FixupBrushModelsPost, pack);
		} else {
			ThrowError("Couldn't precache model %s", BRUSH_FIXUP_MDL);
		}
	}
	return Plugin_Continue;
}

public void FixupBrushModelsPost(DataPack pack)
{
	CBaseEntity pThis = pack.ReadCell();
	if(pThis.IsValid())
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
		pThis.SetEffects(pThis.GetEffects() | EF_NODRAW);
	}
	delete pack;
}