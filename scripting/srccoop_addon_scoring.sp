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

static Handle hTrie = INVALID_HANDLE;

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
	PopulateEntityNameMap();
	HookEvent("entity_killed", Event_EntKilled, EventHookMode_Pre);
}

// name fix copied from Alienmario's HL2MP deathnotices
public void EntityNameFix(char[] szName) {
	GetTrieString(hTrie, szName, szName, 32);
}

public void PopulateEntityNameMap() {
	if(hTrie == INVALID_HANDLE){
		hTrie = CreateTrie();

		// Weapons
		SetTrieString(hTrie, "weapon_crowbar", "Crowbar");
		SetTrieString(hTrie, "weapon_crossbow", "Crossbow");
		SetTrieString(hTrie, "weapon_357", "Revolver");
		SetTrieString(hTrie, "weapon_shotgun", "Shotgun");
		SetTrieString(hTrie, "weapon_mp5", "MP5");
		SetTrieString(hTrie, "weapon_glock", "Glock");
		SetTrieString(hTrie, "weapon_tau", "Tau Cannon");
		SetTrieString(hTrie, "grenade_hornet", "Hive Hand");
		SetTrieString(hTrie, "weapon_gluon", "Gluon Gun");
		SetTrieString(hTrie, "grenade_mp5_contact", "MP5 Grenade");
		SetTrieString(hTrie, "grenade_tripmine", "Tripmine");
		SetTrieString(hTrie, "grenade_frag", "Frag Grenade");
		SetTrieString(hTrie, "grenade_bolt", "Explosive Bolt");
		SetTrieString(hTrie, "grenade_satchel", "Satchel Charge");
		SetTrieString(hTrie, "grenade_rpg", "Rocket Launcher");
		SetTrieString(hTrie, "grenade_tow", "Mounted Rocket Launcher");
		SetTrieString(hTrie, "func_50cal", "50 Cal Mounted Gun");

		// NPCS
		SetTrieString(hTrie, "npc_human_scientist", "a Friendly Scientist");
		SetTrieString(hTrie, "npc_human_scientist_female", "a Friendly Scientist");
		SetTrieString(hTrie, "npc_human_security", "a Friendly Security Guard");
		SetTrieString(hTrie, "npc_barnacle", "Barnacle");
		SetTrieString(hTrie, "npc_vortigaunt", "Vortigaunt");
		SetTrieString(hTrie, "npc_alien_slave", "Vortigaunt");
		SetTrieString(hTrie, "npc_alien_grunt", "Alien Grunt");
		SetTrieString(hTrie, "npc_alien_grunt_melee", "Alien Grunt");
		SetTrieString(hTrie, "npc_alien_grunt_unarmored", "Unarmored Alien Grunt");
		SetTrieString(hTrie, "npc_alien_grunt_elite", "Elite Alien Grunt");
		SetTrieString(hTrie, "npc_human_assassin", "Ninja");
		SetTrieString(hTrie, "npc_human_medic", "HECU Medic");
		SetTrieString(hTrie, "npc_human_grunt", "HECU Grunt");
		SetTrieString(hTrie, "npc_human_commander", "HECU Commander");
		SetTrieString(hTrie, "npc_human_grenadier", "HECU Grenadier");
		SetTrieString(hTrie, "npc_headcrab", "Headcrab"); 
		SetTrieString(hTrie, "npc_houndeye", "Houndeye");
		SetTrieString(hTrie, "npc_zombie_scientist", "Scientist Zombie");
		SetTrieString(hTrie, "npc_zombie_scientist_torso", "Half a Scientist Zombie");
		SetTrieString(hTrie, "npc_zombie_security", "Barney Zombie");
		SetTrieString(hTrie, "npc_zombie_grunt", "HECU Zombie");
		SetTrieString(hTrie, "npc_zombie_grunt_torso", "Half a HECU Zombie");
		SetTrieString(hTrie, "npc_zombie_hev", "HEV Zombie");
		SetTrieString(hTrie, "npc_bullsquid", "Bullsquid");
		SetTrieString(hTrie, "npc_sentry_ground", "Ground Turret");
		SetTrieString(hTrie, "npc_sentry_ceiling", "Ceiling Turret");
		SetTrieString(hTrie, "npc_sniper", "Sniper");
		SetTrieString(hTrie, "npc_apache", "Apache");
		SetTrieString(hTrie, "npc_ichthyosaur", "a Dumb Fish");
		SetTrieString(hTrie, "npc_snark", "Snark");
		SetTrieString(hTrie, "npc_crow", "Crow");
		SetTrieString(hTrie, "npc_abrams", "Abrams Tank");
		SetTrieString(hTrie, "npc_lav", "Light Tank");
		SetTrieString(hTrie, "npc_osprey", "Osprey");
		SetTrieString(hTrie, "npc_xortEB", "Vortigaunt");
		SetTrieString(hTrie, "npc_xort", "Friendly Vortigaunt");
		SetTrieString(hTrie, "npc_alien_controller", "Alien Controller");
		SetTrieString(hTrie, "npc_xontroller", "Alien Controller");
		SetTrieString(hTrie, "npc_gargantua", "Gargantua");

		

		// misc
		SetTrieString(hTrie, "prop_physics", "a physics prop");
	}
}

public Action Event_EntKilled(Event event, const char[] name, bool dontBroadcast)
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

	EntityNameFix(szInflictorClass);

	if (pAttacker.IsClassPlayer())
	{
		CBlackMesaPlayer pClient = view_as<CBlackMesaPlayer>(pAttacker.GetEntIndex());
		if (pKilled.IsClassNPC())
		{
			CAI_BaseNPC pNPC = CAI_BaseNPC(pKilled.GetEntIndex());
			// A player killed an NPC
			pClient.GetName(szAttackerName, sizeof(szAttackerName));

			// Killing friendly NPCs should be punished!
			if ((StrContains(szKilledName, "human_scientist", false) > -1) || 
				(StrContains(szKilledName, "human_security", false) > -1))
			{
				EntityNameFix(szKilledName);
				PrintToChatAll("%s%s%s brutally murdered %s%s%s in cold blood with %s%s", COLOR_MURDER, szAttackerName, COLOR_CRIMSON, COLOR_MURDER_VICTIM, szKilledName, COLOR_CRIMSON, COLOR_MURDER, szInflictorClass);
				pClient.ModifyScore(-10);
			}
			// Monsters should reward points tho
			else
			{	
				EntityNameFix(szKilledName);
				char szMessage[254];

				// Check to see if the npc that was killed was targetting another player
				char szSavedName[32];
				char szSavedNameColor[] = COLOR_KILL_SCORE_ENT;
				StrCat(szMessage, sizeof(szMessage), "%s%s%s killed %s%s%s with %s%s");

				// Did the NPC that was killed have an enemy targeted?
				CBaseEntity pKilledEnemy = pNPC.GetEnemy();

				int iPointsToAward = 1;
				if(pKilledEnemy.IsValid())
				{
					// Was it a player different from the killer?
					if (pKilledEnemy.IsClassPlayer() && pKilledEnemy.GetEntIndex() != pClient.GetEntIndex())
					{
						CBlackMesaPlayer pTargetClient = CBlackMesaPlayer(pKilledEnemy.GetEntIndex()); 
						iPointsToAward += 1;
						pTargetClient.GetName(szSavedName, sizeof(szSavedName));
						StrCat(szMessage, sizeof(szMessage), "%s saving %s%s");
					}
					// Or an NPC
					else if (pKilledEnemy.IsClassNPC())
					{
						// Was the NPC friendly?
						pKilledEnemy.GetClassname(szSavedName, sizeof(szSavedName));
						
						if ((StrContains(szSavedName, "human_scientist", false) > -1) || 
							(StrContains(szSavedName, "human_security", false) > -1))
						{
							iPointsToAward += 1;
							szSavedNameColor = COLOR_MURDER_VICTIM;
						}
						else
							szSavedNameColor = COLOR_CRIMSON;

						EntityNameFix(szSavedName);
						StrCat(szMessage, sizeof(szMessage), "%s saving %s%s");
					}
				}


				PrintToChatAll(szMessage, COLOR_KILL_SCORE_ENT, szAttackerName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szKilledName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szInflictorClass, COLOR_MURDER, szSavedNameColor, szSavedName);
				pClient.ModifyScore(iPointsToAward);
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
			EntityNameFix(szAttackerName);

			char szMessage[254];
			StrCat(szMessage, sizeof(szMessage), "%s%s%s was killed by %s%s");

			if (strcmp(szAttackerName, szInflictorClass, false) != 0)
				StrCat(szMessage, sizeof(szMessage), "%s with %s%s");
			
			
			PrintToChatAll(szMessage, COLOR_CRIMSON, szKilledName, COLOR_WHITE, COLOR_CRIMSON, szAttackerName, COLOR_WHITE, COLOR_CRIMSON, szInflictorClass);
		}
	}

	return Plugin_Continue;
}