#if defined _srccoop_kv_shared_included
 #endinput
#endif
#define _srccoop_kv_shared_included

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
				
				if(kv.GetDataType(NULL_STRING) == KvData_None)
				{
					if(StrEqual(szCondition, "#any", false))
					{
						bResult = ParseConditions(kv, true);
					}
					else if(StrEqual(szCondition, "#all", false) || StrEqual(szCondition, "#if", false))
					{
						bResult = ParseConditions(kv, false);
					}
				}
				else
				{
					if(szCondition[0] == '!')
					{
						Format(szCondition, sizeof(szCondition), szCondition[1]);
						bResult = !TestCondition(szCondition, szValue);
					}
					else
					{
						bResult = TestCondition(szCondition, szValue);
					}
				}
				
				if(anyPass) 
				{
					if(bResult)
					{
						bPassed = true;
						break;
					}
				}
				else
				{
					if(!bResult)
					{
						bPassed = false;
						break;
					}
				}
			}
		}
		while (kv.GotoNextKey(false));
		
		kv.GoBack();
		if(bPassed)
		{
			return true;
		}
	}
	return false;
}

bool TestCondition(char szCondition[MAX_KEY], char szValue[MAX_VALUE])
{
	if(StrEqual(szCondition, "globalstate", false))
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
		char szDefineValue[MAX_VALUE];
		return (
			g_pLevelLump.m_EdtDefinesMap.GetString(szCondition, szDefineValue, sizeof(szDefineValue))
			&& StrEqual(szValue, szDefineValue, false)
		);
	}
}