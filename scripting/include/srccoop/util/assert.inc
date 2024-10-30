#if defined _srccoop_util_assert_included
 #endinput
#endif
#define _srccoop_util_assert_included

#pragma newdecls required
#pragma semicolon 1

/*
* Assertion utility - only compiled into debug code
* Params: 0 => condition, 1 => message, 2+ => format arguments
*/

#if defined DEBUG

 #define assert(%0,%1) if (!(%0)) SetFailState(%1);
 #define assert1(%0,%1,%2) if (!(%0)) SetFailState(%1,%2);
 #define assert2(%0,%1,%2,%3) if (!(%0)) SetFailState(%1,%2,%3);
 #define assert3(%0,%1,%2,%3,%4) if (!(%0)) SetFailState(%1,%2,%3,%4);

#else

 #define assert(%0,%1)
 #define assert1(%0,%1,%2)
 #define assert2(%0,%1,%2,%3)
 #define assert3(%0,%1,%2,%3,%4)
 
#endif