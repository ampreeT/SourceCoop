#include <clientprefs>
#include <sdkhooks>
#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1
#pragma tabsize 0

Cookie ckGoreEffectsEnabled;
ConVar cvGoreEffectsEnabled;

public Plugin myinfo =
{
	name        = "SourceCoop Gore Effects",
	author      = "Jimbab",
	description = "Client side gore effects for all weapons",
	version     = "1.0.7",
	url         = "https://github.com/ampreeT/SourceCoop"
};

enum EntityType
{
	ENTITY_HUMAN_NORMAL,
	ENTITY_TURRET,
	ENTITY_ZOMBIE_NORMAL,
	ENTITY_ALIEN_NORMAL,
	ENTITY_ALIEN_BIG,
	ENTITY_NIHILANTH
}

public void OnPluginStart()
{
	AddTempEntHook("WeaponBullets", RetraceBulletsForEffects);

	cvGoreEffectsEnabled = CreateConVar("sourcecoop_client_goreeffects_enabled", "1", "Enables Jimbab's Gore Effects");
	ckGoreEffectsEnabled = new Cookie("sourcecoop_client_goreeffects_enabled", "Client Side Gore Effects", CookieAccess_Protected);

	InitSourceCoopAddon();

	if (LibraryExists(SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
}

void OnSourceCoopStarted()
{
	TopMenu hCoopMenu = GetCoopTopMenu();
	TopMenuObject menuCategory = hCoopMenu.FindCategory(COOPMENU_CATEGORY_OTHER);

	if (menuCategory != INVALID_TOPMENUOBJECT)
		hCoopMenu.AddItem("ToggleGoreEffects", GoreEffectsMenuHandler, menuCategory);
}

public void GoreEffectsMenuHandler(TopMenu topMenu, TopMenuAction action, TopMenuObject topObjectId, int client, char[] buffer, int maxLength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxLength, GetCookieBool(ckGoreEffectsEnabled, client) ? "Disable Gore Effects" : "Enable Gore Effects");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(client))
		{
			if (GetCookieBool(ckGoreEffectsEnabled, client))
			{
				SetCookieBool(ckGoreEffectsEnabled, client, false);
				Msg(client, "Gore Effects Off");
				cvGoreEffectsEnabled.SetBool(false);
			}
			else
			{
				SetCookieBool(ckGoreEffectsEnabled, client, true);
				Msg(client, "Gore Effects On");
				cvGoreEffectsEnabled.SetBool(true);
			}
		}

		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

public void OnClientCookiesCached(int client)
{
	if (!IsCookieSet(ckGoreEffectsEnabled, client))
	{
		SetCookieBool(ckGoreEffectsEnabled, client, cvGoreEffectsEnabled.BoolValue);
	}
}

/* 
 * Figure out what we're shooting before sending the information to process
 */
public Action RetraceBulletsForEffects(const char[] szTempEntName, const int[] players, int numberOfClients, float delay)
{
	// Check that gore effects are enabled, before continuing
	if (cvGoreEffectsEnabled.BoolValue == false)
		return Plugin_Continue;

	// Get bullet origin, to use as our trace start point
	float bO[3];
	TE_ReadVector("origin", bO);

	// Get bullet rotation, to use as our trace direction
	float bA[3];
	bA[0] = TE_ReadFloat("angles[0]");
	bA[1] = TE_ReadFloat("angles[1]");
	bA[2] = 0.0;

	// Perform a ray trace using our gathered information
	Handle traceRay = TR_TraceRayFilterEx(bO, bA, MASK_ALL, RayType_Infinite, Callback_TraceEntityFilter);

	// Get entity index from trace results
	int hitEntityIndex = TR_GetEntityIndex(traceRay);

	// Check the index is valid
	if (hitEntityIndex == -1)
		return Plugin_Continue;

	// Construct base entity object from index (index -> ref)
	CBaseEntity hitEntity = CBaseEntity(hitEntityIndex);

	// Check the newly constructed object is valid
	if (!hitEntity.IsValid())
		return Plugin_Continue;

	// Get the class name of the hit entity
	char szClassName[64];
	hitEntity.GetClassname(szClassName, 64);

	// Do our string comparions to check what entity we hit. We adjust EntityType argument according to how we want to spawn our blood effects :)
	if (strcmp(szClassName, "npc_grunt") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_scientist") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_scientist_female") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_security") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_barnacle") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_vortigaunt") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_slave") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_grunt") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_grunt_melee") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_grunt_unarmored") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_grunt_elite") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_human_assassin") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_medic") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_grunt") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_commander") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_human_grenadier") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_headcrab") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_houndeye") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_scientist") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_scientist_torso") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_security") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_grunt") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_grunt_torso") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_zombie_hev") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ZOMBIE_NORMAL);
	else if (strcmp(szClassName, "npc_bullsquid") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_BIG);
	else if (strcmp(szClassName, "npc_sentry_ground") == 0)
		SpawnGoreEffect(traceRay, ENTITY_TURRET);
	else if (strcmp(szClassName, "npc_sentry_ceiling") == 0)
		SpawnGoreEffect(traceRay, ENTITY_TURRET);
	else if (strcmp(szClassName, "npc_sniper") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_apache") == 0)
		SpawnGoreEffect(traceRay, ENTITY_TURRET);
	else if (strcmp(szClassName, "npc_ichthyosaur") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_BIG);
	else if (strcmp(szClassName, "npc_snark") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_crow") == 0)
		SpawnGoreEffect(traceRay, ENTITY_HUMAN_NORMAL);
	else if (strcmp(szClassName, "npc_abrams") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_lav") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_osprey") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_xortEB") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_xort") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_alien_controller") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_xontroller") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_NORMAL);
	else if (strcmp(szClassName, "npc_gargantua") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_BIG);
	else if (strcmp(szClassName, "npc_tentacle") == 0)
		SpawnGoreEffect(traceRay, ENTITY_ALIEN_BIG);
	else if (strcmp(szClassName, "npc_nihilanth") == 0)
		SpawnGoreEffect(traceRay, ENTITY_NIHILANTH);

	// Debug, LOL
	//
	//else if (strcmp(szClassName, "prop_door_rotating") == 0)
	//	SpawnGoreEffect(traceRay, ENTITY_TURRET);
	//
	//

	// We're done with the trace handle, so destroy it
	CloseHandle(traceRay);

	// Return, as we're done here :D
	return Plugin_Continue;
}

/* 
 * Just check we skip past any player entities
 */
public bool Callback_TraceEntityFilter(int entityIndex, int mask)
{
	// Check if entity id is valid
	if (entityIndex == -1)
		return true;

	// Construct base entity object from index (index -> ref)
	CBaseEntity hitEntity = CBaseEntity(entityIndex);

	// Check the newly constructed object is valid
	if (!hitEntity.IsValid())
		return true;

	// Get the class name from our entity object
	char szClassName[64];
	hitEntity.GetClassname(szClassName, 64);

	// Check if the object is of the 'player' class, and return false if so
	if (strcmp(szClassName, "player", false) == 0)
		return false;

	// return true if any other class
	return true;
}

/* 
 * Handle sorting through the gore effects for different entities
 */
void SpawnGoreEffect(Handle traceRay, EntityType type)
{
	// Get the position of where the ray hit
	float position[3];
	TR_GetEndPosition(position, traceRay);

	// Iterate through our different entity types, giving different rgb values based on what blood colour we want
	switch (type)
	{
		case (ENTITY_HUMAN_NORMAL):
		{
			HandleBloodEffects_Human(position);
		}

		case (ENTITY_TURRET):
		{
			HandleBloodEffects_Turret(position);
		}

		case (ENTITY_ZOMBIE_NORMAL):
		{
			HandleBloodEffects_Zombie(position);
		}

		case (ENTITY_ALIEN_NORMAL):
		{
			HandleBloodEffects_Alien(position);
		}

		case (ENTITY_ALIEN_BIG):
		{
			HandleBloodEffects_Alien_Big(position);
		}

		case (ENTITY_NIHILANTH):
		{
			HandleBloodEffects_Nihilanth(position);
		}
	}
}

/* 
 * Particle generator
 */
void CreateBloodStream(float position[3], char[] effectName, float lengthTime = 5.0)
{
	int particle = CreateEntityByName("info_particle_system");

	if (IsValidEdict(particle))
	{
		TeleportEntity(particle, position, NULL_VECTOR, NULL_VECTOR);

		DispatchKeyValue(particle, "targetname", "blood_particle");
		DispatchKeyValue(particle, "effect_name", effectName);

		DispatchSpawn(particle);
		ActivateEntity(particle);

		AcceptEntityInput(particle, "start");
		CreateTimer(lengthTime, Callback_DeleteBloodStream, particle);
	}
}

/* 
 * Particle deletion callback
 */
public Action Callback_DeleteBloodStream(Handle timer, any particle)
{
	if (IsValidEntity(particle))
	{
		char className[64];

		GetEdictClassname(particle, className, sizeof(className));

		if (StrEqual(className, "info_particle_system", false))
			RemoveEdict(particle);
	}

	return Plugin_Continue;
}

/* 
 * Some Utility Functions
 */
bool PerhapsTrueOrFalse() { return GetRandomInt(0, 100) <= 50; }
bool VeryUnlikelyTrue() { return GetRandomInt(0, 100) <= 10; }
bool QuiteLikelyTrue() { return GetRandomInt(0, 100) >= 25; }

/* 
 * For all humans
 */
void HandleBloodEffects_Human(float position[3])
{
	if (PerhapsTrueOrFalse())
	{
		CreateBloodStream(position, "blood_impact_red_01");
		CreateBloodStream(position, "blood_impact_red_01_chunk");
		CreateBloodStream(position, "blood_impact_red_01_droplets");
		CreateBloodStream(position, "blood_impact_red_01_smalldroplets");
	}
	else
	{
		CreateBloodStream(position, "blood_impact_red_02");
		CreateBloodStream(position, "blood_impact_red_02_droplets");
		CreateBloodStream(position, "blood_impact_red_02_mist");
		CreateBloodStream(position, "blood_impact_red_01_smalldroplets");
		CreateBloodStream(position, "blood_impact_red_01_chunk");
	}

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, "blood_trail_red_01", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, "blood_trail_red_02", GetRandomFloat(0.5, 1.5));
		}
	}
}

/* 
 * For all turrets
 */
void HandleBloodEffects_Turret(float position[3])
{
	int pick = GetRandomInt(0, 45);

	if (pick < 15)
	{
		CreateBloodStream(position, "impact_sparks");
	}
	else if (pick >= 15 && pick < 30)
	{
		CreateBloodStream(position, "impact_spark_burst");
	}
	else
	{
		CreateBloodStream(position, "spark_shower", 0.25);
	}
}

/* 
 * For all zombies
 */
void HandleBloodEffects_Zombie(float position[3])
{
	CreateBloodStream(position, "blood_impact_green_01");
	CreateBloodStream(position, "blood_impact_green_01_droplets");
	CreateBloodStream(position, "blood_impact_green_01_smalldroplets");
	
	if (VeryUnlikelyTrue())
	{
		int pick = GetRandomInt(0, 100);

		if (pick < 25)
		{
			CreateBloodStream(position, "blood_zombie_split_spray", GetRandomFloat(0.5, 1.5));
		}
		else if (pick >= 25 && pick < 50)
		{
			CreateBloodStream(position, "blood_zombie_split_spray", GetRandomFloat(0.5, 1.5));
		}
		else if (pick >= 50 && pick < 75)
		{
			CreateBloodStream(position, "blood_zombie_split_spray_tiny", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, "blood_zombie_split_spray_tiny2", GetRandomFloat(0.5, 1.5));
		}
	}
}

/* 
 * For most aliens
 */
void HandleBloodEffects_Alien(float position[3])
{
	CreateBloodStream(position, "blood_impact_green_01");
	CreateBloodStream(position, "blood_impact_green_01_chunk");
	CreateBloodStream(position, "blood_impact_green_01_droplets");
	CreateBloodStream(position, "blood_impact_green_01_smalldroplets");

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, "blood_trail_green_01", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, "blood_trail_green_02", GetRandomFloat(0.5, 1.5));
		}
	}
}

/* 
 * For larger aliens
 */
void HandleBloodEffects_Alien_Big(float position[3])
{
	CreateBloodStream(position, "blood_impact_green_01");
	CreateBloodStream(position, "blood_impact_green_01_droplets");
	CreateBloodStream(position, "blood_impact_green_01_smalldroplets");

	if (QuiteLikelyTrue())
		CreateBloodStream(position, "blood_zombie_split");

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, "blood_trail_green_01", GetRandomFloat(1.5, 2.5));
		}
		else
		{
			CreateBloodStream(position, "blood_trail_green_02", GetRandomFloat(1.5, 2.5));
		}

		if (VeryUnlikelyTrue())
			CreateBloodStream(position, "blood_yellow_spray_01", GetRandomFloat(1.5, 2.5));
	}
}

/* 
 * For big boss man Nihi
 */
void HandleBloodEffects_Nihilanth(float position[3])
{
	CreateBloodStream(position, "blood_impact_yellow_01");
	CreateBloodStream(position, "blood_impact_yellow_01_chunk");
	CreateBloodStream(position, "blood_impact_yellow_01_goop");
	CreateBloodStream(position, "blood_impact_yellow_01_mist");
	CreateBloodStream(position, "blood_impact_yellow_01_smalldroplets");		

	if (PerhapsTrueOrFalse())
		CreateBloodStream(position, "blood_impact_nihilanth");
	else if (PerhapsTrueOrFalse())
		CreateBloodStream(position, "blood_impact_nihilanth_a");

	if (VeryUnlikelyTrue())
	{
		int pick = GetRandomInt(0, 125);

		if (pick < 25)
		{
			CreateBloodStream(position, "blood_trail_green_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick >= 25 && pick < 50)
		{
			CreateBloodStream(position, "blood_trail_green_02", GetRandomFloat(1.5, 2.5));
		}
		else if (pick >= 50 && pick < 75)
		{
			CreateBloodStream(position, "blood_yellow_spray_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick >= 75 && pick < 100)
		{
			CreateBloodStream(position, "blood_yellow_spray_01_b", GetRandomFloat(1.5, 2.5));
		}
		else
		{
			CreateBloodStream(position, "blood_yellow_spray_01_c", GetRandomFloat(1.5, 2.5));
		}
	}
}

/*
I don't know what enemies have synthetic blood? let me know if you do haha

synthetic blood effects
|
|
V

blood_impact_synth_01 
blood_impact_synth_01_arc 
blood_impact_synth_01_arc2 
blood_impact_synth_01_arc3 
blood_impact_synth_01_arc4 
blood_impact_synth_01_arc_parent 
blood_impact_synth_01_arc_parents 
blood_impact_synth_01_armor 
blood_impact_synth_01_droplets 
blood_impact_synth_01_dust 
blood_impact_synth_01_short 
blood_impact_synth_01_spurt 
blood_spurt_synth_01
blood_spurt_synth_01b
blood_drip_synth_01
blood_drip_synth_010
*/