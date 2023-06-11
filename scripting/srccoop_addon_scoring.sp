#include <sourcemod>
#include <sdkhooks>
#include <clientprefs>

#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1

#define COLOR_MURDER		 "\x07CC00FF"
#define COLOR_MURDER_VICTIM  "\x0700B1AF"
#define COLOR_KILL_SCORE_ENT "\x071BE5BD"
#define COLOR_CRIMSON		 "\x07E31919"
#define COLOR_WHITE			 "\x07FFFFFF"

#define MAX_DAMAGE_AWARDED_PER_HIT	500.0
#define MAX_COMBO					50

#define MENUITEM_TOGGLE_KILLFEED "ToggleKillfeed"

StringMap hTrie;
Handle hDmgTimer;
StringMap hDmgTrie;
StringMap hComboTimeTrie;
ArrayList lNpcDamageTracker;

Cookie pKillfeedEnabledCookie;
ConVar pConvarKillfeedDefault;

public Plugin myinfo =
{
	name = "SourceCoop Scoring",
	author = "Rock & Alienmario",
	description = "Player scores for killing NPCs + chat killfeed",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

public void OnPluginStart()
{
	PopulateEntityNameMap();
	InitComboTracker();
	HookEvent("entity_killed", Event_EntKilled);
	pKillfeedEnabledCookie = new Cookie("sourcecoop_killfeed_enabled", "Killfeed", CookieAccess_Protected);
	pConvarKillfeedDefault = CreateConVar("sourcecoop_killfeed_default", "0", "Sets the default setting of the killfeed player preference.", _, true, 0.0, true, 1.0);
	
	InitSourceCoopAddon();
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
	{
		TopMenu pCoopMenu = GetCoopTopMenu();
		TopMenuObject pMenuCategory = pCoopMenu.FindCategory(COOPMENU_CATEGORY_OTHER);
		if (pMenuCategory != INVALID_TOPMENUOBJECT)
		{
			pCoopMenu.AddItem(MENUITEM_TOGGLE_KILLFEED, MyMenuHandler, pMenuCategory);
		}
	}
}

public void MyMenuHandler(TopMenu topmenu, TopMenuAction action, TopMenuObject topobj_id, int param, char[] buffer, int maxlength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxlength, GetCookieBool(pKillfeedEnabledCookie, param) ? "Disable killfeed" : "Enable killfeed");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if(AreClientCookiesCached(param))
		{
			if(GetCookieBool(pKillfeedEnabledCookie, param))
			{
				SetCookieBool(pKillfeedEnabledCookie, param, false);
				Msg(param, "Killfeed disabled.");
			}
			else
			{
				SetCookieBool(pKillfeedEnabledCookie, param, true);
				Msg(param, "Killfeed enabled.");
			}
		}
		topmenu.Display(param, TopMenuPosition_LastCategory);
	}
}

public void OnClientCookiesCached(int client)
{
	if(!IsCookieSet(pKillfeedEnabledCookie, client))
	{
		// new player - set the default
		SetCookieBool(pKillfeedEnabledCookie, client, pConvarKillfeedDefault.BoolValue);
	}
}

// name fix copied from Alienmario's HL2MP deathnotices
public void EntityNameFix(char[] szName)
{
	GetTrieString(hTrie, szName, szName, 32);
}

public void PopulateEntityNameMap()
{
	if(hTrie == null) {
		hTrie = new StringMap();

		// Weapons
		SetTrieString(hTrie, "weapon_crowbar", "Crowbar");
		SetTrieString(hTrie, "weapon_crossbow", "Crossbow");
		SetTrieString(hTrie, "weapon_357", "Revolver");
		SetTrieString(hTrie, "weapon_shotgun", "Shotgun");
		SetTrieString(hTrie, "weapon_mp5", "MP5");
		SetTrieString(hTrie, "weapon_glock", "Glock");
		SetTrieString(hTrie, "weapon_tau", "Tau Cannon");
		SetTrieString(hTrie, "weapon_gluon", "Gluon Gun");
		SetTrieString(hTrie, "weapon_assassin_glock", "Silenced Glock");
		SetTrieString(hTrie, "projectile_electric_ball", "Electric Orb");
		SetTrieString(hTrie, "projectile_electrocluster_chunk", "Electrocluster");

		// Projectiles & Grenades
		SetTrieString(hTrie, "grenade_hornet", "Hive Hand");
		SetTrieString(hTrie, "grenade_mp5_contact", "MP5 Grenade");
		SetTrieString(hTrie, "grenade_tripmine", "Tripmine");
		SetTrieString(hTrie, "grenade_frag", "Frag Grenade");
		SetTrieString(hTrie, "grenade_bolt", "Explosive Bolt");
		SetTrieString(hTrie, "grenade_satchel", "Satchel Charge");
		SetTrieString(hTrie, "grenade_rpg", "Rocket Launcher");
		SetTrieString(hTrie, "grenade_tow", "Mounted Rocket Launcher");
		SetTrieString(hTrie, "grenade_spit", "Toxic Spit");

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
		SetTrieString(hTrie, "npc_tentacle", "Outer-Space Octopus");

		// Xen
		SetTrieString(hTrie, "npc_houndeye_knockback", "Armored Houndeye");
		SetTrieString(hTrie, "npc_houndeye_suicide", "Baby Houndeye");
		SetTrieString(hTrie, "npc_beneathticle", "Beneathticle");
		SetTrieString(hTrie, "npc_protozoan", "Floaty Boi");
		SetTrieString(hTrie, "npc_xentree", "Xen Tree");
		SetTrieString(hTrie, "npc_xenturret", "Xen Turret"); 
		SetTrieString(hTrie, "npc_headcrab_baby", "Baby Headcrab"); 
		SetTrieString(hTrie, "npc_bullsquid_melee", "Melee Bullsquid"); 
		SetTrieString(hTrie, "npc_gonarch", "The Gonarch");   
		SetTrieString(hTrie, "npc_xen_grun", "Xen Grunt");  
		SetTrieString(hTrie, "npc_nihalinth", "The Nihalinth");  

		// misc
		SetTrieString(hTrie, "prop_physics", "a physics prop");
		SetTrieString(hTrie, "func_50cal", "50 Cal Mounted Gun");
	}
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

	EntityNameFix(szInflictorClass);

	if (pAttacker.IsClassPlayer())
	{
		CBasePlayer pClient = view_as<CBasePlayer>(pAttacker.GetEntIndex());
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
				PrintToChatKillfeed("%s%s%s brutally murdered %s%s%s in cold blood with %s%s", COLOR_MURDER, szAttackerName, COLOR_CRIMSON, COLOR_MURDER_VICTIM, szKilledName, COLOR_CRIMSON, COLOR_MURDER, szInflictorClass);
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
						CBasePlayer pTargetClient = CBasePlayer(pKilledEnemy.GetEntIndex()); 
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

				PrintToChatKillfeed(szMessage, COLOR_KILL_SCORE_ENT, szAttackerName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szKilledName, COLOR_WHITE, COLOR_KILL_SCORE_ENT, szInflictorClass, COLOR_MURDER, szSavedNameColor, szSavedName);
				pClient.ModifyScore(iPointsToAward);

				// Loop through clients, printing in chat the amount of damage each player inflicted to this NPC.
				float[] attackers = new float[MaxClients + 1];
				float fTotalDamage = 0.0;
				lNpcDamageTracker.GetArray(pKilled.GetEntIndex(), attackers);

				// Gotta calculate the damage total first to see if it's worthy of being broadcast in chat...
				for(int iClientIndex = 0; iClientIndex <= MaxClients; iClientIndex++) {
					float fDamageInflicted = attackers[iClientIndex];
					CBasePlayer pDamagerDealer = CBasePlayer(iClientIndex);
					if(!pDamagerDealer.IsValid() || fDamageInflicted <= 0) continue;
					fTotalDamage += fDamageInflicted;
				}
				for(int iClientIndex = 0; iClientIndex <= MaxClients; iClientIndex++) {
					float fDamageInflicted = attackers[iClientIndex];
					CBasePlayer pDamagerDealer = CBasePlayer(iClientIndex);
					if(!pDamagerDealer.IsValid() || fDamageInflicted <= 0) continue;
				
					char szDmgDealerName[32];
					pDamagerDealer.GetName(szDmgDealerName, sizeof(szDmgDealerName));
					
					// Damage combo
					
					float fPlrDmgLast;

					if(!GetTrieValue(hDmgTrie, szDmgDealerName, fPlrDmgLast))
					{
						fPlrDmgLast = 0.0;
					}

					float fNewDmgCombo = fPlrDmgLast + fDamageInflicted;
					
					if(GetCookieBool(pKillfeedEnabledCookie, iClientIndex)) {
						PrintCenterText(iClientIndex, "COMBO +%.0f (%.0f)", fDamageInflicted, fNewDmgCombo);
					}
					
					if(fTotalDamage >= 100)
						PrintToChatKillfeed("%s%s%s dealt %s%.0f%s damage.", COLOR_KILL_SCORE_ENT, szDmgDealerName, COLOR_WHITE, COLOR_MURDER, fDamageInflicted, COLOR_MURDER_VICTIM);

					// Add damage to combo, reset combo counter to 6000 ms
					SetTrieValue(hDmgTrie, szDmgDealerName, fNewDmgCombo);
					SetTrieValue(hComboTimeTrie, szDmgDealerName, 6000);
				}
			}
		}
	}
	else if (pAttacker.IsClassNPC())
	{
		if (pKilled.IsClassPlayer())
		{
			// An NPC killed a player...
			CBasePlayer pClient = view_as<CBasePlayer>(pKilled.GetEntIndex());
			pClient.GetName(szKilledName, sizeof(szKilledName));
			EntityNameFix(szAttackerName);

			char szMessage[254];
			StrCat(szMessage, sizeof(szMessage), "%s%s%s was killed by %s%s");

			if (strcmp(szAttackerName, szInflictorClass, false) != 0)
				StrCat(szMessage, sizeof(szMessage), "%s with %s%s");
			
			
			PrintToChatKillfeed(szMessage, COLOR_CRIMSON, szKilledName, COLOR_WHITE, COLOR_CRIMSON, szAttackerName, COLOR_WHITE, COLOR_CRIMSON, szInflictorClass);
		}
	}
}

// Taken from basechat.sp
void SendDialogToOne(int iClientIndex, int iRed, int iGreen, int iBlue, const char[] szText, any ...)
{
	char szMessage[100];
	VFormat(szMessage, sizeof(szMessage), szText, 6);	
	
	KeyValues kv = new KeyValues("Stuff", "title", szMessage);
	kv.SetColor("color", iRed, iGreen, iBlue, 255);
	kv.SetNum("level", 1);
	kv.SetNum("time", 10);
	
	CreateDialog(iClientIndex, kv, DialogType_Msg);

	delete kv;
}

// Adaptation of PrintToChatAll with killfeed enable check
void PrintToChatKillfeed(const char[] format, any ...)
{
	char buffer[254];

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && GetCookieBool(pKillfeedEnabledCookie, i))
		{
			SetGlobalTransTarget(i);
			VFormat(buffer, sizeof(buffer), format, 2);
			PrintToChat(i, "%s", buffer);
		}
	}
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if(pEntity.IsClassNPC())
	{
		float[] clients = new float[MaxClients + 1];
		for(int iClientIndex = 0; iClientIndex <= MaxClients; iClientIndex++) {
			clients[iClientIndex] = 0.0;
		}

		// Each NPC needs an array of damage that clients have done to them
		lNpcDamageTracker.SetArray(iEntIndex, clients);
		
		// Hook NPC damage
		SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_OnNpcTakeDamage);
	}
}

public void InitComboTracker()
{
	if(hDmgTimer == null)
	{
		lNpcDamageTracker = CreateArray(MaxClients + 1, GetMaxEntities());
		hDmgTimer = CreateTimer(0.1, Timer_DamageUpdate, _, TIMER_REPEAT);
		hDmgTrie = new StringMap();
		hComboTimeTrie = new StringMap();
	}
}

public Action Timer_DamageUpdate(Handle hTimer)
{
	for(int iClientIndex = 0; iClientIndex <= MaxClients; iClientIndex++)
	{
		CBasePlayer pPlayer = CBasePlayer(iClientIndex);
		if(!pPlayer.IsValid()) continue;
		char szPlayerName[32];
		pPlayer.GetName(szPlayerName, sizeof(szPlayerName));
		
		int iComboMsLeft = 0;
		float fDamageDone = 0.0;

		bool bPlayerHasCombo = GetTrieValue(hDmgTrie, szPlayerName, fDamageDone) &&
							   GetTrieValue(hComboTimeTrie, szPlayerName, iComboMsLeft);
		
		if(bPlayerHasCombo) {
			// subtract 100ms from timer
			iComboMsLeft -= 100;
			SetTrieValue(hComboTimeTrie, szPlayerName, iComboMsLeft);
			if(fDamageDone <= 0) continue;

			// Timer is up!
			if(iComboMsLeft <= 0) {
				// Reset combo back to zero and notify player of their final combo
				SetTrieValue(hDmgTrie, szPlayerName, 0.0);
				

				// Calculate bonus score for combo
				float fDmgMinus100Clamped = fDamageDone - 100.0 > 0.0 ? fDamageDone - 100.0 : 0.0;
				int iScoreToGive = RoundFloat(Pow(fDmgMinus100Clamped/60.0,1.25));
				bool bMaxCombo = false;
				
				// Clamp max score to MAX_COMBO (in case of obscene combo bonuses)
				if(iScoreToGive >= MAX_COMBO) {
					iScoreToGive = MAX_COMBO;
					bMaxCombo = true;
				}

				if(GetCookieBool(pKillfeedEnabledCookie, iClientIndex)) {
					if(iScoreToGive > 0) {
						SendDialogToOne(iClientIndex, 255, 50, 50, "Bonus Points +%d", iScoreToGive);
						PrintToChat(iClientIndex, "%sFinal Combo: %s(%.0f Dmg for +%d Points)%s!",
							COLOR_WHITE, COLOR_KILL_SCORE_ENT, fDamageDone, iScoreToGive, COLOR_WHITE);
						if(bMaxCombo)
							PrintCenterText(iClientIndex, "MAXIMUM COMBO COMPLETE");
						else
							PrintCenterText(iClientIndex, "COMBO COMPLETE");
					} else {
						PrintCenterText(iClientIndex, "COMBO OVER");
					}
				}

				pPlayer.ModifyScore(iScoreToGive);

				if(fDamageDone >= 400) {
					if(bMaxCombo)
						PrintToChatKillfeed("%s%s%s GOT A MAX COMBO OF %s%.0f%s DAMAGE! GRANTING THEM A SCORE BONUS OF %s%d",
						COLOR_KILL_SCORE_ENT, szPlayerName, COLOR_MURDER, COLOR_KILL_SCORE_ENT, fDamageDone, COLOR_MURDER, COLOR_KILL_SCORE_ENT, iScoreToGive);
					else
						PrintToChatKillfeed("%s%s%s GOT A %s%.0f%s DAMAGE COMBO! GRANTING THEM A SCORE BONUS OF %s%d",
						COLOR_KILL_SCORE_ENT, szPlayerName, COLOR_MURDER, COLOR_KILL_SCORE_ENT, fDamageDone, COLOR_MURDER, COLOR_KILL_SCORE_ENT, iScoreToGive);
				}
					
			}
		}
	}
	return Plugin_Continue;
}

public Action Hook_OnNpcTakeDamage(int iVictim, int &iAttacker, int &iInflictor, float &fDamage, int &iDamageType, int &iWeapon, float vfDamageForce[3], float vfDamagePosition[3])
{
	CBaseEntity pAttacker = CBaseEntity(iAttacker);

	// If the damage was done by a player, add it to the NPC's damage list
	if(pAttacker.IsClassPlayer()) {
		float[] attackers = new float[MaxClients + 1];
		lNpcDamageTracker.GetArray(iVictim, attackers);
		
		float fDamageToAward = fDamage;

		// DOESNT WORK:
		// CBaseCombatCharacter pVictim = CBaseCombatCharacter(iVictim);
		// int iHealth = pVictim.GetHealth();

		// WORKAROUND:
		int iHealth = GetEntProp(iVictim, Prop_Data, "m_iHealth", 1);

		// Prevent overkill damage from being recorded
		if(fDamage > iHealth)
			fDamageToAward = float(iHealth);
		
		// No negative damage
		if(fDamageToAward <= 0) fDamageToAward = 0.0;

		// Cap the max damage awarded from any given source to prevent obscene numbers due to edge cases
		if(fDamageToAward > MAX_DAMAGE_AWARDED_PER_HIT)
			fDamageToAward = MAX_DAMAGE_AWARDED_PER_HIT;

		attackers[iAttacker] += fDamageToAward;

		lNpcDamageTracker.SetArray(iVictim, attackers);
	}

	return Plugin_Continue;
}