enum struct CEntityKeyLump
{
	char m_szKey[MAX_KEY];
	char m_szValue[MAX_VALUE];
}

methodmap CEntityInfoLump < Array_t
{
	public CEntityInfoLump(Handle pHandle = null)
	{
		return view_as<CEntityInfoLump>(pHandle);
	}
	
	public bool IsEqual(CEntityInfoLump rhs)
	{
		for (int i = 0; i < this.Size(); i++)
		{
			bool bGotEntry = false;
			
			CEntityKeyLump pThisKeyInfo;
			if (this.GetArray(i, pThisKeyInfo, sizeof(pThisKeyInfo)))
			{
				for (int j = 0; j < rhs.Size(); j++)
				{
					CEntityKeyLump rhsKeyInfo;
					if (rhs.GetArray(j, rhsKeyInfo, sizeof(rhsKeyInfo)))
					{
						if (strcmp(pThisKeyInfo.m_szKey, rhsKeyInfo.m_szKey, false) == 0 && strcmp(pThisKeyInfo.m_szValue, rhsKeyInfo.m_szValue, false) == 0)
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
}

methodmap CEntityListLump < Array_t
{
	public CEntityListLump(Handle pHandle = null)
	{
		return view_as<CEntityListLump>(pHandle);
	}
	
	public void Clear()
	{
		for (int i = 0; i < this.Size(); i++)
		{
			CEntityInfoLump pEntityInfo = this.GetCell(i);
			if (pEntityInfo.IsValid())
			{
				pEntityInfo.Close();
			}
		}
		ClearArray(this.GetHandle());
	}
}

methodmap CNextMapLump < Array_t
{
	public CNextMapLump(Handle pHandle = null)
	{
		return view_as<CNextMapLump>(pHandle);
	}
	
	public bool IsInMapList(const char[] szMapName)
	{
		for (int i = 0; i < this.Size(); i++)
		{
			char szNextMap[MAX_MAPNAME];
			if (this.GetString(i, szNextMap, sizeof(szNextMap)) && strcmp(szNextMap, szMapName) == 0)
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
	
	void Initialize()
	{
		this.m_pEntityList = CEntityListLump(CreateArray(sizeof(CEntityInfoLump)));
		if (!this.m_pEntityList.IsValid())
			ThrowError("Was unable to create CEntityListLump on initialization");
		
		this.m_pNextMapList = CNextMapLump(CreateArray(MAX_MAPNAME));
		if (!this.m_pNextMapList.IsValid())
			ThrowError("Was unable to create Array_t for m_pNextMapList on initialization");
	}
	bool IsCoopMap(const char[] szMapName)
	{
		char szMapPath[MAX_FORMAT];
		return view_as<bool>(BuildPath(Path_SM, szMapPath, sizeof(szMapPath), "data/srccoop/%s.edt", szMapName) && FileExists(szMapPath));
	}
	void ParseMapEntities(const char szMapEntities[2097152])
	{
		this.m_pEntityList.Clear();
		Regex pRegex = new Regex("\"(.*)\" \"(.*)\"");
		if (pRegex == null)
			ThrowError("pRegex is null when attempting to create pattern");
		
		CEntityInfoLump pEntityInfo = CEntityInfoLump();
		int iMapEntitiesLength = strlen(szMapEntities);
		
		for (int i = 0; i < iMapEntitiesLength; i++)
		{
			if (szMapEntities[i] == '\n' || szMapEntities[i] == '\0')
			{
				continue;
			}
			else if (szMapEntities[i] == '{')
			{
				if (pEntityInfo.IsValid())
					ThrowError("Found '{' when expecting '}'");
				
				pEntityInfo = CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
			}
			else if (szMapEntities[i] == '}')
			{
				if (!pEntityInfo.IsValid())
					ThrowError("Did not have valid CEntityInfoLump when finding '}'");
				
				this.m_pEntityList.PushCell(pEntityInfo);
				pEntityInfo = CEntityInfoLump();
			}
			else
			{
				char szKeyValue[2048];
				int n = SplitNewline(szMapEntities[i], sizeof(szKeyValue), szKeyValue);
				i += n;
				
				if (pRegex.Match(szKeyValue) == -1 || pRegex.MatchCount() == 0)
					ThrowError("Regex match failed when parsing: %s", szKeyValue);
				
				CEntityKeyLump pEntityInfoKey;
				pRegex.GetSubString(1, pEntityInfoKey.m_szKey, sizeof(pEntityInfoKey.m_szKey));
				pRegex.GetSubString(2, pEntityInfoKey.m_szValue, sizeof(pEntityInfoKey.m_szValue));
				pEntityInfo.PushArray(pEntityInfoKey, sizeof(pEntityInfoKey));
			}
		}
		
		delete pRegex;
	}
	int ToString(char szBuffer[2097152])	// absolutely do not care for bounds checking, this 2 mb buffer is big enough for everything
	{
		int n = 0;
		for (int i = 0; i < this.m_pEntityList.Size(); i++)
		{
			CEntityInfoLump pEntityInfo = this.m_pEntityList.GetCell(i);
			if (!pEntityInfo.IsValid())		// should be never invalid
				return -1;
			if (pEntityInfo.Size() <= 0)	// never write if it has nothing in it to begin with
				continue;
			
			szBuffer[n] = '{';
			szBuffer[n + 1] = '\n';
			n += 2;
			
			for (int j = 0; j < pEntityInfo.Size(); j++)
			{
				CEntityKeyLump pEntityInfoKey;
				if (pEntityInfo.GetArray(j, pEntityInfoKey, sizeof(pEntityInfoKey)))
				{
					char szKeyValue[2048];
					int iLength = Format(szKeyValue, sizeof(szKeyValue), "\t\"%s\"\t\"%s\"\n", pEntityInfoKey.m_szKey, pEntityInfoKey.m_szValue);
					strcopy(szBuffer[n], iLength + 1, szKeyValue);
					n += iLength;
				}
			}
			
			szBuffer[n] = '}';
			szBuffer[n + 1] = (i == (this.m_pEntityList.Size() - 1)) ? '\0' : '\n';	// if this doesn't end with a null terminator, the token will search for '{' and crash the server
			n += 2;
		}
		
		return n;
	}
	void ParseConfigFile(const char[] szMapName)
	{
		this.m_pNextMapList.Clear();
		
		char szMapPath[MAX_FORMAT];
		if (!(BuildPath(Path_SM, szMapPath, sizeof(szMapPath), "data/srccoop/%s.edt", szMapName) && FileExists(szMapPath)))
			ThrowError("Couldn't find map file %s", szMapName);

		KeyValues kv = new KeyValues(szMapName);
		if (kv == null)
			ThrowError("Couldn't create KeyValues for %s", szMapName);
		
		char szRoot[MAX_MAPNAME];
		if (!(kv.ImportFromFile(szMapPath) && kv.GetSectionName(szRoot, sizeof(szRoot)) && strcmp(szRoot, szMapName) == 0))
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
						if (strcmp(szType, "nextmap", false) == 0)
						{
							char szNextMapName[MAX_MAPNAME];
							kv.GetString(NULL_STRING, szNextMapName, sizeof(szNextMapName));
							this.m_pNextMapList.PushString(szNextMapName);
						}
					}
					else
					{
						if (strcmp(szType, "console", false) == 0)	// is this needed? this is kinda risky
						{
							if (kv.GotoFirstSubKey(false))
							{
								do
								{
									char szKey[MAX_KEY];
									if (kv.GetSectionName(szKey, sizeof(szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
									{
										char szValue[MAX_VALUE];
										kv.GetString(NULL_STRING, szValue, sizeof(szValue));
										ServerCommand("%s %s", szKey, szValue);
									}
								}
								while (kv.GotoNextKey(false));
								kv.GoBack();
							}
						}
						else if (strcmp(szType, "entity") == 0)
						{
							if (kv.GotoFirstSubKey(true))
							{
								do
								{
									char szAction[MAX_KEY];
									if (kv.GetSectionName(szAction, sizeof(szAction)))
									{
										if (strcmp(szAction, "create", false) == 0)
										{
											if (kv.GotoFirstSubKey(false))
											{
												CEntityInfoLump pEntityInfo = CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
												if (!pEntityInfo.IsValid())
													ThrowError("Was unable to create CEntityInfoLump for entity creation");
												do
												{
													CEntityKeyLump pEntityKeyInfo;
													if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
													{
														kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
														pEntityInfo.PushArray(pEntityKeyInfo, sizeof(pEntityKeyInfo));
													}
												}
												while (kv.GotoNextKey(false));
												kv.GoBack();
												this.m_pEntityList.PushCell(pEntityInfo);
											}
										}
										// removing the entity from the entrylist seems to break this
										/*
										else if (strcmp(szAction, "delete") == 0)
										{
											if (kv.GotoFirstSubKey(false))
											{
												CEntityInfoLump pEntityInfo = CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
												if (!pEntityInfo.IsValid())
													ThrowError("Was unable to create CEntityInfoLump for entity deletion");
												
												do
												{
													CEntityKeyLump pEntityKeyInfo;
													if (kv.GetSectionName(pEntityKeyInfo.m_szKey, sizeof(pEntityKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
													{
														kv.GetString(NULL_STRING, pEntityKeyInfo.m_szValue, sizeof(pEntityKeyInfo.m_szValue));
														pEntityInfo.PushArray(pEntityKeyInfo, sizeof(pEntityKeyInfo));
													}
												}
												while (kv.GotoNextKey(false));
												
												for (int i = 0; i < this.Size(); i++)
												{
													CEntityInfoLump pThisEntityInfo = this.GetCell(i);
													if (pEntityInfo.IsEqual(pThisEntityInfo))
													{
														PrintToServer("Removing entry %d", i);
														pThisEntityInfo.Close();
														this.Remove(i);
														i--;
													}
												}
												
												pEntityInfo.Close();
												kv.GoBack();
											}
										}
										*/
										else if (strcmp(szAction, "modify", false) == 0)
										{
											if (kv.GotoFirstSubKey(false))
											{
												CEntityInfoLump pEvaulationEntity = CEntityInfoLump(CreateArray(sizeof(CEntityKeyLump)));
												if (!pEvaulationEntity.IsValid())
													ThrowError("Was unable to create CEntityInfoLump for entity modify evaulation");
												
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
												
												CEntityListLump pTempEvaluatedList = CEntityListLump(CreateArray(sizeof(CEntityInfoLump)));
												if (!pTempEvaluatedList.IsValid())
													ThrowError("Was unable to create CEntityListLump for entity modify temp list");
												
												for (int i = 0; i < this.m_pEntityList.Size(); i++)
												{
													CEntityInfoLump pThisEntityInfo = this.m_pEntityList.GetCell(i);
													if (pEvaulationEntity.IsEqual(pThisEntityInfo))
													{
														pTempEvaluatedList.PushCell(pThisEntityInfo);
													}
												}
												
												if (kv.GotoFirstSubKey(true))
												{
													do
													{
														char szModifyType[MAX_KEY];
														if (kv.GetSectionName(szModifyType, sizeof(szModifyType)))
														{
															if (strcmp(szModifyType, "add", false) == 0)
															{
																if (kv.GotoFirstSubKey(false))
																{
																	do
																	{
																		CEntityKeyLump pAddKeyInfo;
																		if (kv.GetSectionName(pAddKeyInfo.m_szKey, sizeof(pAddKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
																		{
																			kv.GetString(NULL_STRING, pAddKeyInfo.m_szValue, sizeof(pAddKeyInfo.m_szValue));
																			
																			for (int i = 0; i < pTempEvaluatedList.Size(); i++)
																			{
																				CEntityInfoLump pAddEntityInfo = pTempEvaluatedList.GetCell(i);
																				pAddEntityInfo.PushArray(pAddKeyInfo, sizeof(pAddKeyInfo));
																			}
																		}
																	}
																	while (kv.GotoNextKey(false));
																	kv.GoBack();
																}
															}
															else if (strcmp(szModifyType, "remove", false) == 0)
															{
																if (kv.GotoFirstSubKey(false))
																{
																	do
																	{
																		CEntityKeyLump pRemoveKeyInfo;
																		if (kv.GetSectionName(pRemoveKeyInfo.m_szKey, sizeof(pRemoveKeyInfo.m_szKey)) && kv.GetDataType(NULL_STRING) != KvData_None)
																		{
																			kv.GetString(NULL_STRING, pRemoveKeyInfo.m_szValue, sizeof(pRemoveKeyInfo.m_szValue));
																			for (int i = 0; i < pTempEvaluatedList.Size(); i++)
																			{
																				CEntityInfoLump pRemoveEntityInfo = pTempEvaluatedList.GetCell(i);
																				for (int j = 0; j < pRemoveEntityInfo.Size(); j++)
																				{
																					CEntityKeyLump pThisKeyInfo;
																					if (pRemoveEntityInfo.GetArray(j, pThisKeyInfo, sizeof(pThisKeyInfo)))
																					{
																						if (strcmp(pRemoveKeyInfo.m_szKey, pThisKeyInfo.m_szKey, false) == 0 && strcmp(pRemoveKeyInfo.m_szValue, pThisKeyInfo.m_szValue, false) == 0)
																						{
																							pRemoveEntityInfo.Remove(j);
																							break;
																						}
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
												
												pEvaulationEntity.Close();
												pTempEvaluatedList.Close();
											}
										}
									}
								}
								while (kv.GotoNextKey(true));
								kv.GoBack();
							}
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

public int SplitNewline(const char[] szSource, const int nSize, char[] szBuffer)
{
	int i = 0;
	while (szSource[i] != '\n' && szSource[i] != '\0')
		i++;
	strcopy(szBuffer, ((i + 1) < nSize) ? (i + 1) : nSize, szSource);	// strcopy inconvinently includes the null terminator
	return (i < nSize) ? i : nSize;
}