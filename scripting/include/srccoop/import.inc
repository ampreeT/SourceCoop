#if defined _srccoop_import_included
 #endinput
#endif
#define _srccoop_import_included

#pragma newdecls required
#pragma semicolon 1

enum ImportSource
{
	IMPORT_STRIPPER
}

bool ImportConfigs(ImportSource src, const char[] szMapFilter, bool bCreate, bool bDryRun, int &count, int &createCount, int &errors)
{
	if (src == IMPORT_STRIPPER)
	{
		return ImportConfigsDir(src, "addons/stripper/maps", szMapFilter, bCreate, bDryRun, count, createCount, errors);
	}
	ThrowError("ImportConfigs not implemented for this import source."); return false;
}

bool ImportConfig(ImportSource src, const char[] szPath, File pSrcCoopConfig, char[] szError, int maxErrorLen)
{
	if (src == IMPORT_STRIPPER)
	{
		return ImportStripperConfig(szPath, pSrcCoopConfig, szError, maxErrorLen);
	}
	ThrowError("ImportConfig not implemented for this import source."); return false;
}

bool ImportConfigsDir(ImportSource src, const char[] szDir, const char[] szMapFilter, bool bCreate, bool bDryRun, int &count, int &createCount, int &errors)
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

		if (!ImportUtil.FileFilter(src, szFileName, szMapName))
			continue;
	
		if (!StrEqualsWildcard(szMapFilter, szMapName, false))
			continue;

		bool bCreated;
		if (!CoopManager.FindMapConfig(szMapName, szSrcCoopConfig, ccl, false))
		{
			if (!bCreate)
			{
				MsgSrv("Found config for \"%s\", but no SourceCoop config. Run with <CREATE> set to 1 to remedy this.", szMapName);
				continue;
			}
			if (bDryRun)
			{
				continue;
			}
			if (!CoopManager.FindMapConfig(szMapName, szSrcCoopConfig, ccl, true))
			{
				errors++;
				MsgSrv("Couldn't find SourceCoop config nor create default for \"%s\".", szMapName);
				continue;
			}
			bCreated = true;
		}

		File pSrcCoopConfig = OpenFile(szSrcCoopConfig, "r+", CoopManager.ConfigUsesValveFS(ccl));
		if (!pSrcCoopConfig)
		{
			errors++;
			MsgSrv("Unable to read SourceCoop config \"%s\" (ValveFS=%d)", szSrcCoopConfig, CoopManager.ConfigUsesValveFS(ccl));
			continue;
		}
		
		Format(szFileName, sizeof(szFileName), "%s/%s", szDir, szFileName);
		if (bDryRun || ImportConfig(src, szFileName, pSrcCoopConfig, szError, sizeof(szError)))
		{
			count++;
			if (bCreated)
			{
				createCount++;
				MsgSrv("- Create: %s", szSrcCoopConfig);
			}
			else
			{
				MsgSrv("- Update: %s", szSrcCoopConfig);
			}
			pSrcCoopConfig.Close();
		}
		else
		{
			errors++;
			pSrcCoopConfig.Close();
			MsgSrv("Error importing map config from \"%s\" to \"%s\": %s", szFileName, szSrcCoopConfig, szError);
			if (bCreated)
			{
				MsgSrv("Cleaning up.");
				DeleteFile(szSrcCoopConfig, CoopManager.ConfigUsesValveFS(ccl));
			}
		}
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

bool ImportStripperConfig(const char[] szStripperConfig, File pSrcCoopConfig, char[] szError, int maxErrorLen)
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
	bool bError;

	while (!pStripperCfg.EndOfFile())
	{
		iLine++;

		if (!pLineBuilder.AppendFileLine(pStripperCfg))
		{
			Format(szError, maxErrorLen, "Unable to read Stripper config @ line %d", iLine);
			bError = true; break;
		}

		char[] szLine = new char[pLineBuilder.BufferSize];
		pLineBuilder.ToString(szLine).Reset();
		TrimString(szLine);

		if (strncmp(szLine, "//", 2) == 0 || szLine[0] == ';' || szLine[0] == '#' || szLine[0] == '\0')
			continue;
		
		int iIndent = iLevel + 2; // offset for entity section

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
		bError = true; break;
	}

	if (!bError)
	{
		char[] szOut = new char[pOutBuilder.BufferSize];
		pOutBuilder.ToString(szOut);
		bError = !ImportUtil.ImportAsNode("entity_import_stripper", true, szOut, pSrcCoopConfig, szError, maxErrorLen);
	}

	pStripperCfg.Close();
	pLineBuilder.Close();
	pOutBuilder.Close();
	pKvDataRegex.Close();

	return !bError;
}

// --------------------------------------------------------------------------------------------------------------------------------
// Import utils
// --------------------------------------------------------------------------------------------------------------------------------

methodmap ImportUtil
{
	public static bool FileFilter(ImportSource src, const char[] szFileName, char szMapName[MAX_MAPNAME])
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
			MsgSrv("Map name in \"%s\" exceeds limit of %d characters and won't be imported.", szFileName, MAX_MAPNAME - 1);
			return false;
		}
		
		strcopy(szMapName, len, szFileName);
		return true;
	}

	public static bool ImportAsNode(const char[] szNode, bool bReplaceNode, const char[] szNodeContent, File pSrcCoopConfig, char[] szError, int maxErrorLen)
	{
		StringBuilder sb = new StringBuilder();
		if (!sb.AppendFile(pSrcCoopConfig))
		{
			sb.Close();
			Format(szError, maxErrorLen, "Unable to read from SourceCoop config");
			return false;
		}
		
		int iConfigLen = sb.BufferSize;
		char[] szConfig = new char[iConfigLen];
		sb.ToString(szConfig);

		PrintToServer(szConfig);

		// This regex enters the next (or first) child from given position. Matching stops at opening { brace.
		// /^(?:\/\/.*|"(?:\\.|[^"])*"|\s|[^}{"])*?{/
		Regex pEnterChildNode = new Regex("^(?:\\/\\/.*|\"(?:\\\\.|[^\"])*\"|\\s|[^}{\"])*?{");

		// This regex consists of OR'd groups each representing a part of KV syntax. The <braces> group is able to recurse itself.
		// The target node is searched only at current depth, as in kv.JumpToKey(). Matching stops at the start of the specified node (positive lookeahead).
		// /^(?:(?<comments>\/\/.*)|(?="?nodename"?)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>"(?:\\.|[^"])*")|(?<misc>\s|[^"{}]))*/
		char szGoToNode[256] = "^(?:(?<comments>\\/\\/.*)|(?=\"?%s\"?)|(?<braces>{(?:(?&comments)|(?&braces)|(?&quotes)|(?&misc))*})|(?<quotes>\"(?:\\\\.|[^\"])*\")|(?<misc>\\s|[^\"{}]))*";
		Format(szGoToNode, sizeof(szGoToNode), szGoToNode, szNode);
		Regex pGoToNode = new Regex(szGoToNode);

		sb.Close();
		return true;
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