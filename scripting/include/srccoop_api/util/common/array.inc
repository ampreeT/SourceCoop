#pragma newdecls required
#pragma semicolon 1

stock int FindStringInArrayEx(const char[][] array, int len, const char[] str, bool caseSensitive = true)
{
	for (int i = 0; i < len; i++)
	{
		if (StrEqual(str, array[i], caseSensitive))
		{
			return i;
		}
	}
	return -1;
}

stock int FindValueInArrayEx(const any[] array, int len, const any val)
{
	for (int i = 0; i < len; i++)
	{
		if (array[i] == val)
		{
			return i;
		}
	}
	return -1;
}

stock int FindCharInArray(const any[] array, int len, const int val, bool caseSensitive = true)
{
	int i = FindValueInArrayEx(array, len, val);
	if (!caseSensitive && i == -1)
	{
		int val2 = IsCharLower(val) ? CharToUpper(val) : CharToLower(val);
		if (val2 != val)
		{
			i = FindValueInArrayEx(array, len, val2);
		}
	}
	return i;
}
