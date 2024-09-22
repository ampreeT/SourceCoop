#if defined _srccoop_util_assert_included
 #endinput
#endif
#define _srccoop_util_assert_included

#pragma newdecls required
#pragma semicolon 1

/*
* Assertion utility - only compiled into debug code
* Params: 1 => condition, 2 => message + format arguments
*/

#if defined DEBUG

 #define assert(%1,%2) if (!(%1)) SetFailState(%2);

#else

 #define assert(%1,%2)
 
#endif