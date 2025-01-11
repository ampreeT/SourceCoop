#pragma newdecls required
#pragma semicolon 1

// Obtains the max formatted length (including the negative sign and null terminator) of a 32-bit signed integer at compile time.
// This will accept both positive and negative values.
#define MAX_CHARS_INT(%1) ( \
view_as<int>(!!((%1) & (1 << 31))) + \
1 +	\
view_as<int>(!!((%1) / 10)) + \
view_as<int>(!!((%1) / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10)) + \
view_as<int>(!!((%1) / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10 / 10)) + \
1	\
)

stock void FormatTimeLength(int seconds, char[] out, int size)
{
	int hours = seconds / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	if (hours != 0)
	{
		Format(out, size, "%dh ", hours);
	}
	if (mins != 0)
	{
		Format(out, size, "%s%dm ", out, mins);
	}
	if (secs != 0)
	{
		Format(out, size, "%s%ds", out, secs);
	}
}

stock void FormatTimeLengthLong(int seconds, char[] out, int size)
{
	int hours = seconds / 3600;
	int mins = (seconds % 3600) / 60;
	int secs = seconds % 60;
	char separator[10];
	if (hours != 0)
	{
		Format(out, size, "%d hour%s", hours, hours == 1? "" : "s");
		separator = " ";
	}
	if (mins != 0)
	{
		Format(out, size, "%s%s%d minute%s", out, separator, mins, mins == 1? "" : "s");
		separator = " ";
	}
	if (secs != 0)
	{
		Format(out, size, "%s%s%d second%s", out, mins == 0? separator : " and ", secs, secs == 1? "" : "s");
	}
}

// Like SplitString but more efficient
stock int SplitByChar(const char[] szSource, char c, char[] szBuffer, int nSize)
{
	int i = 0;
	while (szSource[i] != c)
	{
		if (szSource[i] == '\0')
		{
			return -1;
		}
		i++;
	}
	i++;
	strcopy(szBuffer, i < nSize ? i : nSize, szSource);
	return i;
}

stock void LowerCaseString(const char[] szInput, char[] szOutput, int iSize)
{
	iSize--;
	int x = 0;
	while (szInput[x] != '\0' && x < iSize)
	{
		szOutput[x] = CharToLower(szInput[x]);
		x++;
	}
	szOutput[x] = '\0';
}

stock void UpperCaseString(const char[] szInput, char[] szOutput, int iSize)
{
	iSize--;
	int x = 0;
	while (szInput[x] != '\0' && x < iSize)
	{
		szOutput[x] = CharToUpper(szInput[x]);
		x++;
	}
	szOutput[x] = '\0';
}

stock bool StrStartsWith(const char[] str, const char[] prefix, bool caseSensitive = true)
{
	return strncmp(str, prefix, strlen(prefix), caseSensitive) == 0;
}

stock bool StrEndsWith(const char[] str, const char[] suffix, bool caseSensitive = true)
{
	return StrEndsWithEx(str, strlen(str), suffix, caseSensitive);
}

stock bool StrEndsWithEx(const char[] str, int realLen, const char[] suffix, bool caseSensitive = true)
{
	int suffixLen = strlen(suffix);
	return (realLen >= suffixLen && StrEqual(str[realLen - suffixLen], suffix, caseSensitive));
}

stock bool IsAddSynonym(const char[] str)
{
	return (strcmp(str, "add", false) == 0 || strcmp(str, "create", false) == 0);
}

stock bool IsRemoveSynonym(const char[] str)
{
	return (strcmp(str, "remove", false) == 0 || strcmp(str, "delete", false) == 0);
}

stock bool IsModifySynonym(const char[] str)
{
	return (strcmp(str, "modify", false) == 0 || strcmp(str, "edit", false) == 0);
}

stock bool IsEnableSynonym(const char[] str)
{
	return (strcmp(str, "enable", false) == 0 || strcmp(str, "turnon", false) == 0 || strcmp(str, "on", false) == 0);
}

stock bool IsDisableSynonym(const char[] str)
{
	return (strcmp(str, "disable", false) == 0 || strcmp(str, "turnoff", false) == 0 || strcmp(str, "off", false) == 0);
}
