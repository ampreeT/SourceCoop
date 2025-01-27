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

#define assert_eq(%0, %1) if ((%0) != (%1)) SetFailState("assertion `left == right` failed\n\t left: %i (0x%x)\n\tright: %i (0x%x)", %0, %1, %0, %1)
#define assert_eq_str(%0, %1) if (strcmp((%0), (%1), true) == 0) SetFailState("assertion `left == right` failed\n\t left: %s\n\tright: %s", %0, %1, %0, %1)

#else

#define assert(%0,%1)
#define assert1(%0,%1,%2)
#define assert2(%0,%1,%2,%3)
#define assert3(%0,%1,%2,%3,%4)

#define assert_eq(%0, %1)
#define assert_eq_str(%0, %1)

#endif
