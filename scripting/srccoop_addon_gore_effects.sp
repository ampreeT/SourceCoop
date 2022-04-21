#include <clientprefs>
#include <sdkhooks>
#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1
#pragma tabsize 0

#define SOUND_HITSOUND            "hm_sound.wav"
#define RELATIVE_SOUND_HITSOUND   "sound/hm_sound.wav"
#define HITMARKER_OVERLAY         "hitmarker0"
#define HITMARKER_OVERLAY_VTF     "hitmarker0.vtf"
#define HITMARKER_OVERLAY_VTF_DIR "materials/hitmarker0.vtf"
#define HITMARKER_OVERLAY_VMT     "materials/hitmarker0.vmt"

ConVar cvGoreEffectsEnabled;
Cookie ckGoreEffectsEnabled;
ConVar cvHitMarkerEnabled;
Cookie ckHitMarkerEnabled;
ConVar cvHitSoundEnabled;
Cookie ckHitSoundEnabled;

ConVar cvCurrentDifficultyLevel;

public Plugin myinfo =
{
	name        = "SourceCoop Satisfaction FX",
	author      = "Jimbab",
	description = "Client side visual/sound FX for all weapons, to make the gunplay more satisfying.",
	version     = "1.0.9",
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

enum WeaponType
{
	// Could use inflictor value, but eh
	// This is basically any projectile damage or energy damage, all of which I would consider powerful :D
	WEAPON_POWERFUL = -1,

	WEAPON_CROWBAR = 308,
	WEAPON_CROSSBOW = 312,
	WEAPON_REVOLVER = 515
}

public void OnPluginStart()
{
	cvGoreEffectsEnabled = CreateConVar("sourcecoop_jb_goreeffects_enabled", "1", "Enables Jimbab's Visual Gore FX");
	ckGoreEffectsEnabled = new Cookie("sourcecoop_jb_goreeffects_enabled", "Client Side Visual Hit FX", CookieAccess_Protected);
	cvHitMarkerEnabled = CreateConVar("sourcecoop_jb_hitmarker_enabled", "1", "Enables Jimbab's Visual Hit Marker Effect");
	ckHitMarkerEnabled = new Cookie("sourcecoop_jb_hitmarker_enabled", "Client Side Hit Marker Effect", CookieAccess_Protected);
	cvHitSoundEnabled = CreateConVar("sourcecoop_jb_hitsound_enabled", "1", "Enables Jimbab's Hit Sound Effect");
	ckHitSoundEnabled = new Cookie("sourcecoop_jb_hitsound_enabled", "Client Side Hit Sound Effect", CookieAccess_Protected);

	cvCurrentDifficultyLevel = FindConVar("sourcecoop_difficulty");

	InitSourceCoopAddon();

	if (LibraryExists(SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
}

public void OnConfigsExecuted()
{
	RequestFrame(OnConfigsExecutedPost);
}

public void OnConfigsExecutedPost()
{
	PrecacheSound(SOUND_HITSOUND, true);
	PrecacheModel(HITMARKER_OVERLAY_VMT);

	AddFileToDownloadsTable(RELATIVE_SOUND_HITSOUND);
	AddFileToDownloadsTable(HITMARKER_OVERLAY_VTF_DIR);
	AddFileToDownloadsTable(HITMARKER_OVERLAY_VMT);
}

void OnSourceCoopStarted()
{
	TopMenu hCoopMenu = GetCoopTopMenu();
	TopMenuObject playerCategory = hCoopMenu.FindCategory(COOPMENU_CATEGORY_PLAYER);
	TopMenuObject soundsCategory = hCoopMenu.FindCategory(COOPMENU_CATEGORY_SOUNDS);

	if (playerCategory != INVALID_TOPMENUOBJECT)
	{
		hCoopMenu.AddItem("ToggleJimbabsGoreEffects", GoreEffectsMenuHandler, playerCategory);
		hCoopMenu.AddItem("ToggleJimbabsHitMarkerEffect", HitMarkerEffectMenuHandler, playerCategory);
	}

	if (soundsCategory != INVALID_TOPMENUOBJECT)
		hCoopMenu.AddItem("ToggleJimbabsHitSoundEffect", HitSoundEffectMenuHandler, soundsCategory);
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
				Msg(client, "(JB_FX) Gore Effects Off");
				cvGoreEffectsEnabled.SetBool(false);
			}
			else
			{
				SetCookieBool(ckGoreEffectsEnabled, client, true);
				Msg(client, "(JB_FX) Gore Effects On");
				cvGoreEffectsEnabled.SetBool(true);
			}
		}

		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

public void HitMarkerEffectMenuHandler(TopMenu topMenu, TopMenuAction action, TopMenuObject topObjectId, int client, char[] buffer, int maxLength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxLength, GetCookieBool(ckHitMarkerEnabled, client) ? "Disable Hit Marker Effect" : "Enable Hit Marker Effect");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(client))
		{
			if (GetCookieBool(ckHitMarkerEnabled, client))
			{
				SetCookieBool(ckHitMarkerEnabled, client, false);
				Msg(client, "(JB_FX) Hit Marker Off");
				cvHitMarkerEnabled.SetBool(false);
			}
			else
			{
				SetCookieBool(ckHitMarkerEnabled, client, true);
				Msg(client, "(JB_FX) Hit Marker On");
				cvHitMarkerEnabled.SetBool(true);
			}
		}

		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

public void HitSoundEffectMenuHandler(TopMenu topMenu, TopMenuAction action, TopMenuObject topObjectId, int client, char[] buffer, int maxLength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxLength, GetCookieBool(ckHitSoundEnabled, client) ? "Disable Hit Sound Effect" : "Enable Hit Sound Effect");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		if (AreClientCookiesCached(client))
		{
			if (GetCookieBool(ckHitSoundEnabled, client))
			{
				SetCookieBool(ckHitSoundEnabled, client, false);
				Msg(client, "(JB_FX) Hit Sound Off");
				cvHitSoundEnabled.SetBool(false);
			}
			else
			{
				SetCookieBool(ckHitSoundEnabled, client, true);
				Msg(client, "(JB_FX) Hit Sound On");
				cvHitSoundEnabled.SetBool(true);
			}
		}

		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

public void OnClientCookiesCached(int clientIndex)
{
	if (!IsCookieSet(ckGoreEffectsEnabled, clientIndex))
		SetCookieBool(ckGoreEffectsEnabled, clientIndex, cvGoreEffectsEnabled.BoolValue);

	if (!IsCookieSet(ckHitMarkerEnabled, clientIndex))
		SetCookieBool(ckHitMarkerEnabled, clientIndex, cvHitMarkerEnabled.BoolValue);

	if (!IsCookieSet(ckHitSoundEnabled, clientIndex))
		SetCookieBool(ckHitSoundEnabled, clientIndex, cvHitSoundEnabled.BoolValue);
}

public void OnEntityCreated(int entityIndex, const char[] szClassname)
{
	if (CBaseEntity(entityIndex).IsClassNPC())
		SDKHook(entityIndex, SDKHook_OnTakeDamage, DispatchJbEffects);
}

/*
 * Call upon the effect handlers
 */
public Action DispatchJbEffects(int victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3])
{
	Action result = Plugin_Continue;
	Action storeReturn = Plugin_Continue;

	result = CreateHitMarkerEffect(victim, attacker, inflictor, damage, damagetype, weapon, damageForce, damagePosition);

	if (result != Plugin_Continue)
		storeReturn = result;

	result = CreateHitSoundEffect(victim, attacker, inflictor, damage, damagetype, weapon, damageForce, damagePosition);

	if (result != Plugin_Continue)
		storeReturn = result;

	result = ReCreateGoreEffects(victim, attacker, inflictor, damage, damagetype, weapon, damageForce, damagePosition);

	if (result != Plugin_Continue)
		storeReturn = result;

	return storeReturn;
}

public Action CreateHitMarkerEffect(int victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3])
{
	// Check hit sound effect is actually enabled
	if (cvHitMarkerEnabled.BoolValue == false)
		return Plugin_Continue;

	// Construct our entity objects based on their entity ID's
	CBaseCombatCharacter playerAttacker = CBaseCombatCharacter(attacker);
	CBaseCombatCharacter npcVictim = CBaseCombatCharacter(victim);

	// Check victim and attacker are both npc and real player, respectively
	if (!playerAttacker.IsClassPlayer() || !npcVictim.IsClassNPC())
		return Plugin_Continue;

	// Draw hitmarker
	ClientCommand(attacker, "r_screenoverlay \"%s\"", HITMARKER_OVERLAY);

	// Create timed callback for removing hitmarker
	CreateTimer(0.06, Callback_RemoveOverlay, attacker);

	// Make sure observers are also seeing effects
	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || !IsClientObserver(i))
			continue;

		int observerMode = GetEntProp(i, Prop_Send, "m_iObserverMode");

		if (observerMode != 4 && observerMode != 5)
			continue;

		if (GetEntPropEnt(i, Prop_Send, "m_hObserverTarget") != attacker)
			continue;

		ClientCommand(i, "r_screenoverlay \"%s\"", HITMARKER_OVERLAY);
		CreateTimer(0.06, Callback_RemoveOverlay, i);
	}

	return Plugin_Continue;
}

public Action Callback_RemoveOverlay(Handle timer, any clientIndex)
{
	if (IsClientInGame(clientIndex))
		ClientCommand(clientIndex, "r_screenoverlay \"\"");

	return Plugin_Continue;
}

public Action CreateHitSoundEffect(int victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3])
{
	// Check hit sound effect is actually enabled
	if (cvHitSoundEnabled.BoolValue == false)
		return Plugin_Continue;

	// Construct our entity objects based on their entity ID's
	CBaseEntity playerAttacker = CBaseEntity(attacker);
	CBaseEntity npcVictim = CBaseEntity(victim);

	// Check victim and attacker are both npc and real player, respectively
	if (!playerAttacker.IsClassPlayer() || !npcVictim.IsClassNPC())
		return Plugin_Continue;

	// If all is good, emit the sound
	EmitSoundToClient(playerAttacker.GetEntIndex(), SOUND_HITSOUND, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_GUNFIRE);

	// Make sure observers are also hearing effects
	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || !IsClientObserver(i))
			continue;

		int observerMode = GetEntProp(i, Prop_Send, "m_iObserverMode");

		if (observerMode != 4 && observerMode != 5)
			continue;

		if (GetEntPropEnt(i, Prop_Send, "m_hObserverTarget") != attacker)
			continue;

		EmitSoundToClient(i, SOUND_HITSOUND, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_GUNFIRE);
	}

	return Plugin_Continue;
}

public Action ReCreateGoreEffects(int victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3])
{
	// Check gore effects is actually enabled
	if (cvGoreEffectsEnabled.BoolValue == false)
		return Plugin_Continue;

	// Cons(truct our entity objects based on their entity ID's
	CBaseEntity playerAttacker = CBaseEntity(attacker);
	CBaseEntity npcVictim = CBaseEntity(victim);

	// Check victim and attacker are both npc and real player, respectively
	if (!playerAttacker.IsClassPlayer() || !npcVictim.IsClassNPC())
		return Plugin_Continue;

	// Calculate pseudo headshot potential
	float originalDamage = damage * (cvCurrentDifficultyLevel.FloatValue * 0.1 + 1);

	// Handle more powerful weapons
	if (weapon == view_as<int>(WEAPON_POWERFUL) || weapon == view_as<int>(WEAPON_CROWBAR) || weapon == view_as<int>(WEAPON_CROSSBOW) || weapon == view_as<int>(WEAPON_REVOLVER))
	{
		// Cool headshot fx
		if (originalDamage > 65)
			CreateDamageEffectFromClass(damagePosition, npcVictim, 2.0, true);
		else
			CreateDamageEffectFromClass(damagePosition, npcVictim, 2.0);

		// Return, so that we don't create a second set of FX
		return Plugin_Continue;
	}

	// Handle normal weapons
	if (originalDamage > 65)
		CreateDamageEffectFromClass(damagePosition, npcVictim, _, true);
	else
		CreateDamageEffectFromClass(damagePosition, npcVictim);

	return Plugin_Continue;
}

/*
 * Do our prop checks to check what type of entity we hit. We adjust EntityType argument and scale, according to how we want to spawn our effects :)
 */
void CreateDamageEffectFromClass(float position[3], CBaseEntity parent, float scale = 1.0, bool pseudoHeadshot = false)
{
	int bloodColour = GetEntProp(parent.GetEntIndex(), Prop_Data, "m_bloodColor");

	char szClassName[64];
	parent.GetClassname(szClassName, sizeof szClassName);

	switch (bloodColour)
	{
		case (-1):    // Turrets, Mechanical, etc.
		{
			CreateDamageEffect(position, parent, ENTITY_TURRET, scale, pseudoHeadshot);
		}

		case (0):    // Human NPC's
		{
			CreateDamageEffect(position, parent, ENTITY_HUMAN_NORMAL, scale, pseudoHeadshot);
		}

		case (1):    // Alien NPC's (yellow blood)
		{
			if (strcmp(szClassName, "npc_bullsquid") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_gargantua") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_tentacle") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_nihilanth") == 0)
				CreateDamageEffect(position, parent, ENTITY_NIHILANTH, scale, pseudoHeadshot);
			else
				CreateDamageEffect(position, parent, ENTITY_ALIEN_NORMAL, scale, pseudoHeadshot);
		}

		case (2):    // Alien NPC's (green blood)
		{
			if (strcmp(szClassName, "npc_bullsquid") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_gargantua") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_tentacle") == 0)
				CreateDamageEffect(position, parent, ENTITY_ALIEN_BIG, scale, pseudoHeadshot);
			else if (strcmp(szClassName, "npc_nihilanth") == 0)
				CreateDamageEffect(position, parent, ENTITY_NIHILANTH, scale, pseudoHeadshot);
			else
				CreateDamageEffect(position, parent, ENTITY_ALIEN_NORMAL, scale, pseudoHeadshot);
		}

		case (3):    // NPC's with synthetic blood
		{
		}
	}
}

/*
 * Handle sorting through the gore effects for different entities
 */
void CreateDamageEffect(float position[3], CBaseEntity parent, EntityType type, float scale, bool pseudoHeadshot)
{
	// Iterate through our different entity types, calling their respective effect handlers
	switch (type)
	{
		case (ENTITY_HUMAN_NORMAL):
		{
			HandleBloodEffects_Human(position, parent, scale, pseudoHeadshot);
		}

		case (ENTITY_TURRET):
		{
			HandleBloodEffects_Turret(position, parent);
		}

		case (ENTITY_ZOMBIE_NORMAL):
		{
			HandleBloodEffects_Zombie(position, parent, scale, pseudoHeadshot);
		}

		case (ENTITY_ALIEN_NORMAL):
		{
			HandleBloodEffects_Alien(position, parent, scale, pseudoHeadshot);
		}

		case (ENTITY_ALIEN_BIG):
		{
			HandleBloodEffects_Alien_Big(position, parent, scale, pseudoHeadshot);
		}

		case (ENTITY_NIHILANTH):
		{
			HandleBloodEffects_Nihilanth(position, parent, scale);
		}
	}
}

/*
 * Particle generator
 */
void CreateBloodStream(float position[3], CBaseEntity parent, char[] effectName, float lengthTime = 5.0)
{
	CBaseEntity particle = CBaseEntity(CreateEntityByName("info_particle_system"));

	if (particle.IsValid())
	{
		particle.Teleport(position);

		particle.SetKeyValueStr("targetname", "blood_particle");
		particle.SetKeyValueStr("effect_name", effectName);
		particle.SetParent(parent);

		particle.Spawn();
		particle.Activate();

		particle.AcceptInputStr("start");
		CreateTimer(lengthTime, Callback_DeleteParticleSystem, particle.GetEntIndex());
	}
}

/*
 * Particle system deletion callback
 */
public Action Callback_DeleteParticleSystem(Handle timer, any particle)
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
bool PerhapsTrueOrFalse()
{
	return GetRandomInt(0, 100) <= 50;
}
bool VeryUnlikelyTrue()
{
	return GetRandomInt(0, 100) <= 10;
}
bool QuiteLikelyTrue()
{
	return GetRandomInt(0, 100) >= 25;
}

/*
 * For all humans
 */
void HandleBloodEffects_Human(float position[3], CBaseEntity parent, float scale, bool pseudoHeadshot)
{
	if (PerhapsTrueOrFalse())
	{
		CreateBloodStream(position, parent, "blood_impact_red_01");
		CreateBloodStream(position, parent, "blood_impact_red_01_chunk");
		CreateBloodStream(position, parent, "blood_impact_red_01_droplets");
		CreateBloodStream(position, parent, "blood_impact_red_01_smalldroplets");
	}
	else
	{
		CreateBloodStream(position, parent, "blood_impact_red_02");
		CreateBloodStream(position, parent, "blood_impact_red_02_droplets");
		CreateBloodStream(position, parent, "blood_impact_red_02_mist");
		CreateBloodStream(position, parent, "blood_impact_red_01_smalldroplets");
		CreateBloodStream(position, parent, "blood_impact_red_01_chunk");
	}

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, parent, "blood_trail_red_01", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, parent, "blood_trail_red_02", GetRandomFloat(0.5, 1.5));
		}
	}

	if (scale == 2.0 && PerhapsTrueOrFalse())
	{
		int pick = GetRandomInt(0, 4);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_human_b", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_human_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_human_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_human_e", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_human_f", GetRandomFloat(0.5, 1.5));
		}
	}

	if (pseudoHeadshot)
		CreateBloodStream(position, parent, "gib_human_spurt_old", GetRandomFloat(0.5, 1.0));
}

/*
 * For all turrets
 */
void HandleBloodEffects_Turret(float position[3], CBaseEntity parent)
{
	int pick = GetRandomInt(0, 45);

	if (pick < 15)
	{
		CreateBloodStream(position, parent, "impact_sparks");
	}
	else if (pick >= 15 && pick < 30)
	{
		CreateBloodStream(position, parent, "impact_spark_burst");
	}
	else
	{
		CreateBloodStream(position, parent, "spark_shower", 0.25);
	}
}

/*
 * For all zombies
 */
void HandleBloodEffects_Zombie(float position[3], CBaseEntity parent, float scale, bool pseudoHeadshot)
{
	CreateBloodStream(position, parent, "blood_impact_green_01");
	CreateBloodStream(position, parent, "blood_impact_green_01_droplets");
	CreateBloodStream(position, parent, "blood_impact_green_01_smalldroplets");

	if (VeryUnlikelyTrue())
	{
		int pick = GetRandomInt(0, 100);

		if (pick >= 50)
		{
			CreateBloodStream(position, parent, "blood_zombie_split_spray_tiny", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, parent, "blood_zombie_split_spray_tiny2", GetRandomFloat(0.5, 1.5));
		}
	}

	if (scale == 2.0 && PerhapsTrueOrFalse())
	{
		int pick = GetRandomInt(0, 4);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_zombie_split_spray", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_human_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_human_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_human_e", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_human_f", GetRandomFloat(0.5, 1.5));
		}
	}

	if (pseudoHeadshot)
		CreateBloodStream(position, parent, "gib_human_spurt_old", GetRandomFloat(0.5, 1.0));
}

/*
 * For most aliens
 */
void HandleBloodEffects_Alien(float position[3], CBaseEntity parent, float scale, bool pseudoHeadshot)
{
	CreateBloodStream(position, parent, "blood_impact_green_01");
	CreateBloodStream(position, parent, "blood_impact_green_01_chunk");
	CreateBloodStream(position, parent, "blood_impact_green_01_droplets");
	CreateBloodStream(position, parent, "blood_impact_green_01_smalldroplets");

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, parent, "blood_trail_green_01", GetRandomFloat(0.5, 1.5));
		}
		else
		{
			CreateBloodStream(position, parent, "blood_trail_green_02", GetRandomFloat(0.5, 1.5));
		}
	}

	if (scale == 2.0 && PerhapsTrueOrFalse())
	{
		int pick = GetRandomInt(0, 4);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_alien_e", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_alien_b", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_alien_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_alien_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_alien_e", GetRandomFloat(0.5, 1.5));
		}
	}

	if (pseudoHeadshot)
		CreateBloodStream(position, parent, "gib_alien_spurt_old", GetRandomFloat(0.5, 1.0));
}

/*
 * For larger aliens
 */
void HandleBloodEffects_Alien_Big(float position[3], CBaseEntity parent, float scale, bool pseudoHeadshot)
{
	CreateBloodStream(position, parent, "blood_impact_green_01");
	CreateBloodStream(position, parent, "blood_impact_green_01_droplets");
	CreateBloodStream(position, parent, "blood_impact_green_01_smalldroplets");

	if (QuiteLikelyTrue())
		CreateBloodStream(position, parent, "blood_alien_g");

	if (VeryUnlikelyTrue())
	{
		if (PerhapsTrueOrFalse())
		{
			CreateBloodStream(position, parent, "blood_trail_green_01", GetRandomFloat(1.5, 2.5));
		}
		else
		{
			CreateBloodStream(position, parent, "blood_trail_green_02", GetRandomFloat(1.5, 2.5));
		}

		if (VeryUnlikelyTrue())
			CreateBloodStream(position, parent, "blood_yellow_spray_01", GetRandomFloat(1.5, 2.5));
	}

	if (scale == 2.0 && PerhapsTrueOrFalse())
	{
		int pick = GetRandomInt(0, 4);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_alien_f", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_alien_b", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_alien_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_alien_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_alien_e", GetRandomFloat(0.5, 1.5));
		}
	}

	if (pseudoHeadshot)
		CreateBloodStream(position, parent, "gib_alien_spurt_old", GetRandomFloat(0.5, 1.0));
}

/*
 * For big boss man Nihi
 */
void HandleBloodEffects_Nihilanth(float position[3], CBaseEntity parent, float scale)
{
	CreateBloodStream(position, parent, "blood_impact_yellow_01");
	CreateBloodStream(position, parent, "blood_impact_yellow_01_chunk");
	CreateBloodStream(position, parent, "blood_impact_yellow_01_goop");
	CreateBloodStream(position, parent, "blood_impact_yellow_01_mist");
	CreateBloodStream(position, parent, "blood_impact_yellow_01_smalldroplets");

	if (PerhapsTrueOrFalse())
		CreateBloodStream(position, parent, "blood_impact_nihilanth");
	else if (PerhapsTrueOrFalse())
		CreateBloodStream(position, parent, "blood_impact_nihilanth_a");

	if (VeryUnlikelyTrue() && scale == 1.0)
	{
		int pick = GetRandomInt(0, 9);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_alien_f", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_alien_b", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_alien_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_alien_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_alien_e", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 5)
		{
			CreateBloodStream(position, parent, "blood_trail_green_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 6)
		{
			CreateBloodStream(position, parent, "blood_trail_green_02", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 7)
		{
			CreateBloodStream(position, parent, "blood_yellow_spray_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 8)
		{
			CreateBloodStream(position, parent, "blood_yellow_spray_01_b", GetRandomFloat(1.5, 2.5));
		}
		else
		{
			CreateBloodStream(position, parent, "gib_alien_spurt", GetRandomFloat(1.5, 2.5));
		}
	}
	else if (QuiteLikelyTrue() && scale == 2.0)
	{
		int pick = GetRandomInt(0, 9);

		if (pick == 0)
		{
			CreateBloodStream(position, parent, "blood_alien_f", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 1)
		{
			CreateBloodStream(position, parent, "blood_alien_b", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 2)
		{
			CreateBloodStream(position, parent, "blood_alien_c", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 3)
		{
			CreateBloodStream(position, parent, "blood_alien_d", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 4)
		{
			CreateBloodStream(position, parent, "blood_alien_e", GetRandomFloat(0.5, 1.5));
		}
		else if (pick == 5)
		{
			CreateBloodStream(position, parent, "blood_trail_green_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 6)
		{
			CreateBloodStream(position, parent, "blood_trail_green_02", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 7)
		{
			CreateBloodStream(position, parent, "blood_yellow_spray_01", GetRandomFloat(1.5, 2.5));
		}
		else if (pick == 8)
		{
			CreateBloodStream(position, parent, "blood_yellow_spray_01_b", GetRandomFloat(1.5, 2.5));
		}
		else
		{
			CreateBloodStream(position, parent, "gib_alien_spurt", GetRandomFloat(1.5, 2.5));
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