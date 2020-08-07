#include <sourcemod>
#include <srccoop_api>
#include <regex>

#define COLOR_MURDER		 "\x07CC00FF"
#define COLOR_MURDER_VICTIM  "\x0700B1AF"
#define COLOR_KILL_SCORE_ENT "\x071BE5BD"
#define COLOR_CRIMSON		 "\x07E31919"
#define COLOR_WHITE			 "\x07FFFFFF"

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Scoring",
	author = "Rock & Alienmario",
	description = "Player scores for killing NPCs + chat killfeed",
	version = "1.0.0",
	url = "https://github.com/ampreeT/SourceCoop"
};

public void OnPluginStart()
{
	HookEvent("entity_killed", Event_EntKilled);
}

public void Event_EntKilled(Event event, const char[] name, bool dontBroadcast)
{
	CBaseEntity pKilled = CBaseEntity(event.GetInt("entindex_killed"));
	CBaseEntity pAttacker = CBaseEntity(event.GetInt("entindex_attacker"));
	CBaseEntity pInflictor = CBaseEntity(event.GetInt("entindex_inflictor"));

	char szKilledName[32];
	pKilled.GetClassname(szKilledName, sizeof(szKilledName));
	char szAttackerName[32];
	pAttacker.GetClassname(szAttackerName, sizeof(szAttackerName));
	char szInflictorClass[32];
	pInflictor.GetClassname(szInflictorClass, sizeof(szInflictorClass));

	if (pAttacker.IsClassPlayer())
	{
		CBlackMesaPlayer pClient = view_as<CBlackMesaPlayer>(pAttacker.GetEntIndex());
		if (pKilled.IsClassNPC())
		{
			// A player killed an NPC
			pClient.GetName(szAttackerName, sizeof(szAttackerName));

			// Killing friendly NPCs should be punished!
			if ((StrContains(szKilledName, "human_scientist", false) > -1) || 
				(StrContains(szKilledName, "human_security", false) > -1))
			{
				PrintToChatAll("%s%s%s brutally murdered %s%s%s in cold blood with %s%s", COLOR_MURDER, szAttackerName, COLOR_CRIMSON, COLOR_MURDER_VICTIM, szKilledName, COLOR_CRIMSON, COLOR_MURDER, szInflictorClass);
				pClient.ModifyScore(-10);
			}
			// Monsters should reward points tho
			else
			{
				PrintToChatAll("%s%s%s killed %s%s%s with %s%s", COLOR_KILL_SCORE_ENT, szAttackerName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szKilledName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szInflictorClass);
				pClient.ModifyScore(1);
			}
		}
	}
	else if (pAttacker.IsClassNPC())
	{
		if (pKilled.IsClassPlayer())
		{
			// An NPC killed a player...
			CBlackMesaPlayer pClient = view_as<CBlackMesaPlayer>(pKilled.GetEntIndex());
			pClient.GetName(szKilledName, sizeof(szKilledName));
			if (strcmp(szAttackerName, szInflictorClass, false) != 0)
			{
				PrintToChatAll("%s%s%s was killed by %s%s%s with %s%s", COLOR_CRIMSON, szKilledName, COLOR_WHITE, COLOR_CRIMSON, szAttackerName, COLOR_WHITE, COLOR_CRIMSON, szInflictorClass);
			}
			// If an NPC doesn't use a weapon, don't print the weapon name
			else
			{
				PrintToChatAll("%s%s%s was killed by %s%s", COLOR_CRIMSON, szKilledName, COLOR_WHITE, COLOR_CRIMSON, szAttackerName);
			}
		}
	}
}