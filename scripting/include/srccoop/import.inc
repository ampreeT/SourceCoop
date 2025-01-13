#pragma newdecls required
#pragma semicolon 1

// --------------------------------------------------------------------------------------------------------------------------------
// Import sources
// --------------------------------------------------------------------------------------------------------------------------------

enum ImportSource
{
	IMPORT_STRIPPER
}

// --------------------------------------------------------------------------------------------------------------------------------
// Import utils
// --------------------------------------------------------------------------------------------------------------------------------

methodmap ImportUtil
{
	public static bool FileFilter(ImportSource src, const char[] szFileName, char szMapName[MAX_MAPNAME], int &errors)
	{
		int len = strlen(szFileName);
		int extLen;

		if (src == IMPORT_STRIPPER)
		{
			if (!StrEndsWithEx(szFileName, len, ".cfg", false))
				return false;
			extLen = 4;
		}
		else ThrowError("ImportUtil.FileFilter not implemented for this import source.");
		
		len = len - extLen + 1;
		if (len > MAX_MAPNAME)
		{
			errors++;
			MsgSrv("Map name in \"%s\" exceeds limit of %d characters and won't be imported.", szFileName, MAX_MAPNAME - 1);
			return false;
		}
		
		strcopy(szMapName, len, szFileName);
		return true;
	}

	public static bool ImportAsNode(const char[] szNode, const char[] szNodeContent, StringBuilder pConfigBuilder, char[] szError, int maxErrorLen)
	{
		// This regex enters first child key from given position.
		// Matching stops after the opening { brace.
		//
		// Raw form:
		// /^(?:\/\/.*|"(?:\\.|[^"])*"|\s|[^}{"])*?{/
		
		Regex pEnterChildNode = new Regex("^(?:\\/\\/.*|\"(?:\\\\.|[^\"])*\"|\\s|[^}{\"])*?{");

		// This regex consists of OR'd groups each representing a part of KV syntax. The <braces> group is recursive.
		// The target node is searched at current depth, as in kv.JumpToKey().
		// Matching stops after the name of the specified node.
		//
		// Raw form:
		// /^(?:(?<comments>\/\/.*)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>"(?:\\.|[^"])*")|(?<misc>\s|[^"{}]))*?(?:(?<=\s)(?<target>entity)(?=[{\s])|"(?&target)")/
		
		char szGoToNode[256] = "^(?:(?<comments>\\/\\/.*)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>\"(?:\\\\.|[^\"])*\")|(?<misc>\\s|[^\"{}]))*?(?:(?<=\\s)(?<target>%s)(?=[{\\s])|\"(?&target)\")";
		Format(szGoToNode, sizeof(szGoToNode), szGoToNode, szNode);
		Regex pGoToNode = new Regex(szGoToNode);

		// Moves to end of current node.
		// Same as above, but without the target node mathing part at the end.
		// Matching stops before closing } brace.
		//
		// Raw form:
		// /^(?:(?<comments>\/\/.*)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>"(?:\\.|[^"])*")|(?<misc>\s|[^"{}]))*+/
		
		Regex pGoToEnd = new Regex("^(?:(?<comments>\\/\\/.*)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>\"(?:\\\\.|[^\"])*\")|(?<misc>\\s|[^\"{}]))*+");

		char[] szConfig = new char[pConfigBuilder.BufferSize];
		pConfigBuilder.ToString(szConfig).Reset();

		int offset;
		if (!ImportUtil.RegexAdvance(pEnterChildNode, szConfig, offset))
		{
			Format(szError, maxErrorLen, "SourceCoop config root element not found");
		}
		else
		{
			if (ImportUtil.RegexAdvance(pGoToNode, szConfig, offset))
			{
				// found requested node

				if (ImportUtil.RegexAdvance(pEnterChildNode, szConfig, offset))
				{
					pConfigBuilder.AppendEx(szConfig, offset);
					pConfigBuilder.Append("\n");
					pConfigBuilder.Append(szNodeContent);
					pConfigBuilder.Append("\t");
					if (ImportUtil.RegexAdvance(pGoToEnd, szConfig, offset))
					{
						pConfigBuilder.Append(szConfig[offset]);
					}
					else
					{
						Format(szError, maxErrorLen, "Unable to traverse SourceCoop config (#3)");
					}
				}
				else
				{
					Format(szError, maxErrorLen, "Unable to traverse SourceCoop config (#1)");
				}
			}
			else
			{
				// requested node missing
				
				if (ImportUtil.RegexAdvance(pGoToEnd, szConfig, offset))
				{
					pConfigBuilder.AppendEx(szConfig, offset);
					pConfigBuilder.Append("\t\"");
					pConfigBuilder.Append(szNode);
					pConfigBuilder.Append("\"\n\t{\n");
					pConfigBuilder.Append(szNodeContent);
					pConfigBuilder.Append("\t}\n");
					pConfigBuilder.Append(szConfig[offset]);
				}
				else
				{
					Format(szError, maxErrorLen, "Unable to traverse SourceCoop config (#2)");
				}
			}
		}
		
		pEnterChildNode.Close();
		pGoToNode.Close();
		pGoToEnd.Close();
		return (szError[0] == EOS);
	}

	public static bool RegexAdvance(Regex regex, const char[] szStr, int &offset = 0)
	{
		int ret = regex.Match(szStr[offset]);
		if (ret != -1 && regex.MatchCount())
		{
			offset += regex.MatchOffset();
			return true;
		}
		return false;
	}

	public static void AppendLine(StringBuilder sb, const char[] szText, int indent)
	{
		ImportUtil.Indent(sb, indent);
		sb.Append(szText);
		sb.Append("\n");
	}

	public static void Indent(StringBuilder sb, int indent)
	{
		for (int i = 0; i < indent; i++)
			sb.Append("\t");
	}
}

// --------------------------------------------------------------------------------------------------------------------------------
// Import boilerplate
// --------------------------------------------------------------------------------------------------------------------------------

/**
 * Batch imports configs from given ImportSource to SourceCoop map configs according to provided parameters.
 * @param src             Import source
 * @param szMapFilter     Map-name filter, supports wildcards.
 * @param bDryRun         Dry run does not modify actual files, merely reports stats and checks for potential issues.
 * @param bUpdate         Update existing SC configs?
 * @param bCreate         Create missing SC configs?
 * @param updated         Updated SC configs counter
 * @param created         Created SC configs counter
 * @param errors          Error counter
 * @return                If halted early false, otherwise true.
 */
bool ImportConfigs(ImportSource src, const char[] szMapFilter, bool bDryRun, bool bUpdate, bool bCreate, int &updated, int &created, int &errors)
{
	if (src == IMPORT_STRIPPER)
	{
		return ImportConfigsDir(src, "addons/stripper/maps", szMapFilter, bDryRun, bUpdate, bCreate, updated, created, errors);
	}
	ThrowError("ImportConfigs not implemented for this import source."); return false;
}

/**
 * Imports a config from source file into provided StringBuilder.
 * @param src             Import source
 * @param szPath          Import source file path
 * @param pConfigBuilder  String builder pre-loaded with SourceCoop config to update.
 * @param szError         Error message
 * @param maxErrorLen     Error message max length
 * @return                True on success. If false, szError contains detailed message.
 */
bool ImportConfig(ImportSource src, const char[] szPath, StringBuilder pConfigBuilder, char[] szError, int maxErrorLen)
{
	if (src == IMPORT_STRIPPER)
	{
		return ImportStripperConfig(szPath, pConfigBuilder, szError, maxErrorLen);
	}
	ThrowError("ImportConfig not implemented for this import source."); return false;
}

/**
 * Enumerates a directory importing found files. For import sources with per-map config files.
 * @return If halted early false, otherwise true.
 */
bool ImportConfigsDir(ImportSource src, const char[] szDir, const char[] szMapFilter, bool bDryRun, bool bUpdate, bool bCreate, int &updated, int &created, int &errors)
{
	DirectoryListing dir = OpenDirectory(szDir, false);
	if (!dir)
	{
		MsgSrv("Unable to enumerate \"%s\" folder.", szDir);
		return false;
	}

	FileType ft;
	CoopConfigLocation ccl;
	char szFileName[PLATFORM_MAX_PATH];
	char szMapName[MAX_MAPNAME];
	char szSrcCoopConfig[PLATFORM_MAX_PATH];
	char szError[128];

	while (dir.GetNext(szFileName, sizeof(szFileName), ft))
	{
		if (ft != FileType_File)
			continue;

		if (!ImportUtil.FileFilter(src, szFileName, szMapName, errors))
			continue;
	
		if (!StrEqualsWildcard(szMapFilter, szMapName, false))
			continue;

		bool bCreated;
		if (CoopManager.GetMapConfig(szMapName, szSrcCoopConfig, ccl, false))
		{
			if (!bUpdate)
				continue;
		}
		else
		{
			if (!bCreate)
			{
				MsgSrv("WARN: Found config for \"%s\", but no matching SourceCoop config. Run with <CREATE> set to 1 to auto-create missing configs.", szMapName);
				continue;
			}
			if (bDryRun)
			{
				MsgSrv("- Create: %s", szMapName);
				created++;
				continue;
			}
			if (!CoopManager.GetMapConfig(szMapName, szSrcCoopConfig, ccl, true))
			{
				errors++;
				MsgSrv("ERROR: Couldn't find SourceCoop config nor create default for \"%s\".", szMapName);
				continue;
			}
			bCreated = true;
		}

		if (CoopManager.ConfigUsesValveFS(ccl))
		{
			ResolveValveFile(szSrcCoopConfig, szSrcCoopConfig, sizeof(szSrcCoopConfig));
		}

		File pSrcCoopConfig = OpenFile(szSrcCoopConfig, "r+");
		if (!pSrcCoopConfig)
		{
			errors++;
			MsgSrv("ERROR: Unable to read SourceCoop config \"%s\"", szSrcCoopConfig);
			continue;
		}

		szError = "";
		Format(szFileName, sizeof(szFileName), "%s/%s", szDir, szFileName);
		StringBuilder pConfigBuilder = new StringBuilder();

		if (!pConfigBuilder.AppendFile(pSrcCoopConfig))
		{
			Format(szError, sizeof(szError), "Unable to read from SourceCoop config");
		}
		else
		{
			if (ImportConfig(src, szFileName, pConfigBuilder, szError, sizeof(szError)))
			{
				if (!bDryRun)
				{
					if (!pSrcCoopConfig.Seek(0, SEEK_SET) || !pConfigBuilder.ToFile(pSrcCoopConfig))
					{
						Format(szError, sizeof(szError), "Unable to write to SourceCoop config");
					}
				}
			}
		}

		if (szError[0] == EOS)
		{
			if (bCreated)
			{
				created++;
				MsgSrv("- Create: %s", szSrcCoopConfig);
			}
			else
			{
				updated++;
				MsgSrv("- Update: %s", szSrcCoopConfig);
			}
			pSrcCoopConfig.Close();
		}
		else
		{
			errors++;
			pSrcCoopConfig.Close();
			MsgSrv("ERROR: Importing map config from \"%s\" to \"%s\": %s", szFileName, szSrcCoopConfig, szError);
			if (bCreated)
			{
				MsgSrv("Undoing config file.");
				if (!DeleteFile(szSrcCoopConfig))
				{
					MsgSrv("Config cleanup failed.");
				}
			}
		}
		pConfigBuilder.Close();
	}
	dir.Close();
	return true;
}

// --------------------------------------------------------------------------------------------------------------------------------
// Stripper importer
// --------------------------------------------------------------------------------------------------------------------------------

#define STRIPPER_TOK_OBRACE                (1 << 0)
#define STRIPPER_TOK_CBRACE                (1 << 1)
#define STRIPPER_TOK_FILTER                (1 << 2)
#define STRIPPER_TOK_ADD                   (1 << 3)
#define STRIPPER_TOK_MODIFY                (1 << 4)
#define STRIPPER_TOK_MATCH                 (1 << 5)
#define STRIPPER_TOK_REPLACE               (1 << 6)
#define STRIPPER_TOK_DELETE                (1 << 7)
#define STRIPPER_TOK_INSERT                (1 << 8)
#define STRIPPER_TOK_KVDATA                (1 << 9)

#define STRIPPER_ELEMENTS_ROOT             (STRIPPER_TOK_FILTER|STRIPPER_TOK_ADD|STRIPPER_TOK_MODIFY)
#define STRIPPER_ELEMENTS_MODIFY           (STRIPPER_TOK_MATCH|STRIPPER_TOK_REPLACE|STRIPPER_TOK_DELETE|STRIPPER_TOK_INSERT)

bool ImportStripperConfig(const char[] szStripperConfig, StringBuilder pConfigBuilder, char[] szError, int maxErrorLen)
{
	File pStripperCfg = OpenFile(szStripperConfig, "rt");
	if (!pStripperCfg)
	{
		Format(szError, maxErrorLen, "Unable to read Stripper config.");
		return false;
	}

	Regex pKvDataRegex = new Regex("\"([^\"]+)\"\\s+\"([^\"]+)\"");
	StringBuilder pLineBuilder = new StringBuilder();
	StringBuilder pOutBuilder = new StringBuilder();
	int iMode[3]; // edit mode at each level, either 0 for none, one of the tokens or kvdata
	int iLevel; // parsing depth
	int iLine; // line count
	int iNext = STRIPPER_ELEMENTS_ROOT; // next expected token bits

	while (!pStripperCfg.EndOfFile())
	{
		iLine++;

		if (!pLineBuilder.AppendFileLine(pStripperCfg))
		{
			Format(szError, maxErrorLen, "Unable to read Stripper config @ line %d", iLine);
			break;
		}

		char[] szLine = new char[pLineBuilder.BufferSize];
		pLineBuilder.ToString(szLine).Reset();
		TrimString(szLine);
		
		if (szLine[0] == '\0')
			continue;
		
		int iIndent = iLevel + 2; // offset for entity section

		if (strncmp(szLine, "//", 2) == 0 || szLine[0] == ';' || szLine[0] == '#')
		{
			ImportUtil.AppendLine(pOutBuilder, szLine, iIndent);
			continue;
		}

		if (iNext & STRIPPER_TOK_OBRACE && StrEqual(szLine, "{"))
		{
			// defer writing keywords until found the opening brace
			if (iMode[iLevel] == STRIPPER_TOK_ADD || iMode[iLevel] == STRIPPER_TOK_FILTER || iMode[iLevel] & STRIPPER_ELEMENTS_MODIFY)
			{
				iNext = STRIPPER_TOK_KVDATA|STRIPPER_TOK_CBRACE;
				
				if (iMode[iLevel] == STRIPPER_TOK_ADD || iMode[iLevel] == STRIPPER_TOK_INSERT)
					ImportUtil.AppendLine(pOutBuilder, "add", iIndent);
				else if (iMode[iLevel] == STRIPPER_TOK_FILTER)
					ImportUtil.AppendLine(pOutBuilder, "remove", iIndent);
				else if (iMode[iLevel] == STRIPPER_TOK_REPLACE)
					ImportUtil.AppendLine(pOutBuilder, "replace", iIndent);
				else if (iMode[iLevel] == STRIPPER_TOK_DELETE)
					ImportUtil.AppendLine(pOutBuilder, "delete", iIndent);
			}
			else if (iMode[iLevel] == STRIPPER_TOK_MODIFY)
			{
				iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_CBRACE;
				ImportUtil.AppendLine(pOutBuilder, "modify", iIndent);
			}

			if (iMode[iLevel] != STRIPPER_TOK_MATCH)
			{
				ImportUtil.AppendLine(pOutBuilder, "{", iIndent);
			}

			iLevel++;
			continue;
		}
		if (iNext & STRIPPER_TOK_CBRACE && StrEqual(szLine, "}"))
		{
			iMode[iLevel--] = 0;

			if (iMode[iLevel] != STRIPPER_TOK_MATCH)
			{
				ImportUtil.AppendLine(pOutBuilder, "}", iIndent - 1);
			}
			
			if (iMode[iLevel] & STRIPPER_ELEMENTS_ROOT)
			{
				iNext = STRIPPER_ELEMENTS_ROOT|STRIPPER_TOK_OBRACE;
			}
			else if (iMode[iLevel] & STRIPPER_ELEMENTS_MODIFY)
			{
				iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_OBRACE|STRIPPER_TOK_CBRACE;
			}
			continue;
		}
		if (iNext & STRIPPER_TOK_KVDATA && pKvDataRegex.Match(szLine) == 3)
		{
			iMode[iLevel] = STRIPPER_TOK_KVDATA;
			
			if (iLevel == 2 && iMode[1] == STRIPPER_TOK_MATCH)
			{
				iIndent--;
			}
			ImportUtil.AppendLine(pOutBuilder, szLine, iIndent);
			continue;
		}
		if (iNext & STRIPPER_TOK_ADD && StrEqual(szLine, "add:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_ADD;
			iNext = STRIPPER_ELEMENTS_ROOT|STRIPPER_TOK_OBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_FILTER && StrEqual(szLine, "filter:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_FILTER;
			iNext = STRIPPER_ELEMENTS_ROOT|STRIPPER_TOK_OBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_MODIFY && StrEqual(szLine, "modify:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_MODIFY;
			iNext = STRIPPER_ELEMENTS_ROOT|STRIPPER_TOK_OBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_MATCH && StrEqual(szLine, "match:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_MATCH;
			iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_OBRACE|STRIPPER_TOK_CBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_REPLACE && StrEqual(szLine, "replace:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_REPLACE;
			iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_OBRACE|STRIPPER_TOK_CBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_DELETE && StrEqual(szLine, "delete:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_DELETE;
			iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_OBRACE|STRIPPER_TOK_CBRACE;
			continue;
		}
		if (iNext & STRIPPER_TOK_INSERT && StrEqual(szLine, "insert:", false))
		{
			iMode[iLevel] = STRIPPER_TOK_INSERT;
			iNext = STRIPPER_ELEMENTS_MODIFY|STRIPPER_TOK_OBRACE|STRIPPER_TOK_CBRACE;
			continue;
		}
		Format(szError, maxErrorLen, "Invalid Stripper config format @ line %d", iLine);
		break;
	}

	if (szError[0] == EOS)
	{
		char[] szNodeContent = new char[pOutBuilder.BufferSize];
		pOutBuilder.ToString(szNodeContent);
		ImportUtil.ImportAsNode("entity_import_stripper", szNodeContent, pConfigBuilder, szError, maxErrorLen);
	}

	pStripperCfg.Close();
	pLineBuilder.Close();
	pOutBuilder.Close();
	pKvDataRegex.Close();

	return (szError[0] == EOS);
}