#if defined _srccoop_api_included
	#endinput
#endif
#define _srccoop_api_included

#pragma newdecls required
#pragma semicolon 1

#if !defined SRCCOOP_CUSTOM_COMPILE

	#if defined SRCCOOP_HL2DM
	
	#else
	
		#if !defined SRCCOOP_BLACKMESA
			// SRCCOOP_BLACKMESA is the fallback if no other definition is passed
			#define SRCCOOP_BLACKMESA
		#endif

	#endif

#endif

#include <dhooks>
#include <sdkhooks>
#include <sdktools>
#include <sourcescramble>

#include <srccoop_api/typedef>
#include <srccoop_api/util>
#include <srccoop_api/classdef>