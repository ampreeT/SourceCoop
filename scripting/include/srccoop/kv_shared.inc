#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------------------------------------------------------------
// This exists for sharing functionality between checkpoint.inc and levellump.inc
// Has to be structured this way since sourcepawn doesn't have forward declaration and parses all but functions top to bottom.
//------------------------------------------------------------------------------------------------------------

/**
 * Macro for evaluating conditions while parsing keyvalues. Should be called from a loop iterating over keys.
 *
 * @param 1	the current section name
 * @param 2	keyvalues handle
 * @param 3	code to run for parsing contents of the evaluated block
 */
#define EVAL_CONDITIONS(%1,%2,%3)\
if(StrEqual(%1, "#if", false))\
{\
	if(ParseConditions(%2))\
	{\
		if(%2.JumpToKey("#then"))\
		{\
			%3;%2.GoBack();continue;\
		}\
		else continue;\
	}\
	else\
	{\
		if(%2.JumpToKey("#else"))\
		{\
			%3;%2.GoBack();continue;\
		}\
		else continue;\
	}\
}

#include <srccoop/checkpoint>
#include <srccoop/levellump>

/**
 * The actual function which evaluates conditions as it's parsing them.
 *
 * @param	kv
 * @param	anyPass
 *		true ==> OR
 *		false ==> AND
 */
bool ParseConditions(KeyValues kv, bool anyPass = false)
{
	if (kv.GotoFirstSubKey(false))
	{
		char szCondition[MAX_KEY], szValue[MAX_VALUE];
		bool bPassed = anyPass? false:true;
		do
		{
			if (kv.GetSectionName(szCondition, sizeof(szCondition)))
			{
				kv.GetString(NULL_STRING, szValue, sizeof(szValue));
				bool bResult = !anyPass;
				
				if (kv.GetDataType(NULL_STRING) == KvData_None)
				{
					if (StrEqual(szCondition, "#any", false))
					{
						bResult = ParseConditions(kv, true);
					}
					else if (StrEqual(szCondition, "#all", false) || StrEqual(szCondition, "#if", false))
					{
						bResult = ParseConditions(kv, false);
					}
				}
				else
				{
					if (szCondition[0] == '!')
					{
						strcopy(szCondition, sizeof(szCondition), szCondition[1]);
						bResult = !TestCondition(szCondition, szValue);
					}
					else
					{
						bResult = TestCondition(szCondition, szValue);
					}
				}
				
				if (anyPass) 
				{
					if (bResult)
					{
						bPassed = true;
						break;
					}
				}
				else
				{
					if (!bResult)
					{
						bPassed = false;
						break;
					}
				}
			}
		}
		while (kv.GotoNextKey(false));
		
		kv.GoBack();
		if (bPassed)
		{
			return true;
		}
	}
	return false;
}

bool TestCondition(const char szCondition[MAX_KEY], const char szValue[MAX_VALUE])
{
	if (StrEqual(szCondition, "globalstate", false))
	{
		return CGlobalState.GetState(szValue) == GLOBAL_ON;
	}
	else if (StrEqual(szCondition, "prevmap", false))
	{
		return StrEqualsRegex(szValue, g_szPrevMapName, false);
	}
	else if (StrEqual(szCondition, "os", false))
	{
		return (
			StrEqual(szValue, "linux", false) && g_serverOS == OS_Linux
			|| StrEqual(szValue, "windows", false) && g_serverOS == OS_Windows
		);
	}
	// if no condition types match, check edt defines
	else
	{
		char szDefineKey[MAX_KEY], szDefineValue[MAX_VALUE];
		UpperCaseString(szCondition, szDefineKey, sizeof(szDefineKey));
		return (
			g_pLevelLump.m_EdtDefinesMap.GetString(szDefineKey, szDefineValue, sizeof(szDefineValue))
			&& StrEqual(szValue, szDefineValue, false)
		);
	}
}

/**
 * Extracts context flags from a string and replaces the string with the remaining value.
 * Context flags are simple characters in a row at the beginning of the string.
 * They are prefixed with ~ (tilda) and end with a space, followed by the actual value.
 * Example: "~fd value" will return flags ['f', 'd']
 * 
 * If a ~ character is needed at the beginning of the string, double tilda "~~etc" can be used instead.
 * This implies space and tilda cannot be used for a context flag.
 * 
 * @return              Number of flags extracted
 */
int ExtractContextFlags(char[] szInput, int maxLength, int[] flags, int maxFlags)
{
	if (szInput[0] == '~')
	{
		// escaping
		if (szInput[1] == '~')
		{
			strcopy(szInput, maxLength, szInput[1]);
			return 0;
		}

		for (int i = 1; i < maxLength; i++)
		{
			if (szInput[i] == EOS)
			{
				strcopy(szInput, maxLength, szInput[i]);
				return i - 1;
			}
			if (szInput[i] == ' ')
			{
				strcopy(szInput, maxLength, szInput[i + 1]);
				return i - 1;
			}
			if (i > maxFlags)
			{
				ThrowError("At most %d context flags are allowed! @ \"%s\"", maxFlags, szInput);
			}
			flags[i - 1] = szInput[i];
		}
	}
	return 0;
}