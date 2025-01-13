#pragma newdecls required
#pragma semicolon 1

/* File copy from SMLIB
 * @param source		Input file
 * @param destination	Output file
 * @return				True on success, false otherwise
 */
stock bool FileCopy(const char[] source, const char[] destination, bool use_valve_fs=false, const char[] valve_path_id="GAME")
{
	File file_source = OpenFile(source, "rb", use_valve_fs, valve_path_id);

	if (file_source == null)
	{
		return false;
	}

	File file_destination = OpenFile(destination, "wb", use_valve_fs, valve_path_id);

	if (file_destination == null)
	{
		file_source.Close();
		return false;
	}

	int buffer[32];
	int cache;

	while (!file_source.EndOfFile())
	{
		cache = file_source.Read(buffer, sizeof(buffer), 1);
		file_destination.Write(buffer, cache, 1);
	}

	delete file_source;
	delete file_destination;

	return true;
}

/**
 * This is a workaround for opening existing files, potentially located in the custom folder, with the Valve filesystem, for both reading and writing.
 * Normally, opening an existing file with Valve FS using any writable modes ("r+", "a", ..) will only look for files in root game folder.
 * This function currently accounts for the custom folder only, other additional Valve search paths are ignored.
 * 
 * https://github.com/alliedmodders/sourcemod/issues/2175
 */
stock File OpenFileEx(const char[] file, const char[] mode, bool use_valve_fs=false, const char[] valve_path_id="GAME")
{
	if (use_valve_fs && !StrEqual(mode, "r", false) && !StrEqual(mode, "rb", false) && !StrEqual(mode, "rt", false))
	{
		char szFullPath[PLATFORM_MAX_PATH];
		if (ResolveValveFile(file, szFullPath, sizeof(szFullPath)))
		{
			return OpenFile(szFullPath, mode);
		}
	}
	return OpenFile(file, mode, use_valve_fs, valve_path_id);
}

/**
 * Should only be used as last resort.
 * See OpenFileEx
 */
stock bool ResolveValveFile(const char[] szFile, char[] szFullPath, int maxLen)
{
	DirectoryListing dir = OpenDirectory("custom");
	if (dir)
	{
		char buffer[PLATFORM_MAX_PATH];
		FileType ft;
		
		while (dir.GetNext(buffer, sizeof(buffer), ft))
		{
			if (ft == FileType_Directory && !StrEqual(buffer, ".") && !StrEqual(buffer, ".."))
			{
				Format(buffer, sizeof(buffer), "custom/%s/%s", buffer, szFile);
				if (FileExists(buffer))
				{
					dir.Close();
					Format(szFullPath, maxLen, buffer);
					return true;
				}
			}
		}

		dir.Close();
	}
	return false;
}
