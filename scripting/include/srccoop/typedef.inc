#if defined _srccoop_typedef_included
 #endinput
#endif
#define _srccoop_typedef_included

#pragma newdecls required
#pragma semicolon 1

#define ENTITYSTRING_LENGTH 2097152

#define MAX_FORMAT 256
#define MAX_KEY 32
#define MAX_VALUE 1024

enum OperatingSystem
{
    OS_Unknown = -1,
    OS_Windows,
    OS_Linux,
    OS_Mac
}