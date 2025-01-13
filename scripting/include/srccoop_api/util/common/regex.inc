#pragma newdecls required
#pragma semicolon 1

/**
 * Matches a regex pattern against a string if pattern is enclosed by forward slashes "/".
 * Otherwise acts as a regular equals check.
 */
stock bool StrEqualsRegex(const char[] pattern, const char[] str, bool caseSensitive = false)
{
	int patternLength = strlen(pattern);
	if (patternLength > 1 && pattern[0] == '/' && pattern[patternLength - 1] == '/')
	{
		if (str[0] == '\0')
		{
			// regexing empty string errors out..
			return false;
		}
		char[] patternStripped = new char[--patternLength];
		strcopy(patternStripped, patternLength, pattern[1]);
		int substrings = SimpleRegexMatch(str, patternStripped, caseSensitive? 0 : PCRE_CASELESS);
		return (substrings > 0);
	}
	return strcmp(pattern, str, caseSensitive) == 0;
}

/**
 * Simple * wildcard pattern matching. Best for comparing map names (think maps command), file paths (think gitignore), etc.
 * 
 * Example pattern: *pine*
 * Matched strings: pine, pineapple, spines ...
 * 
 */
stock bool StrEqualsWildcard(const char[] pattern, const char[] str, bool caseSensitive = false)
{
	int regexLen = strlen(pattern) * 2 + 3;
	char[] regex = new char[regexLen];

	EscapeRegex(pattern, regex, regexLen, "*");
	ReplaceString(regex, regexLen, "*", ".*");
	Format(regex, regexLen, "^%s$", regex);

	int substrings = SimpleRegexMatch(str, regex, caseSensitive? 0 : PCRE_CASELESS);
	return (substrings > 0);
}

/**
 * Quotes / escapes all special regex characters in a string.
 * @param allowedChars Characters that should not be escaped.
 * @return Characters written
 */
stock int EscapeRegex(const char[] str, char[] dest, int destLen, const char[] allowedChars = "")
{
	static const char specialChars[] = ".\\+*?[^]$(){}=!<>|:-#";
	int i, j;
	while (str[i] != EOS && (j + 1) < destLen)
	{
		if (FindCharInString(specialChars, str[i]) != -1 && FindCharInString(allowedChars, str[i]) == -1)
		{
			if (j + 2 >= destLen)
				break;
			dest[j++] = '\\';
		}
		dest[j++] = str[i++];
	}
	dest[j] = EOS;
	return j;
}