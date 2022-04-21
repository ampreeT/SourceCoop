#include <clientprefs>
#include <sdkhooks>
#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1
#pragma tabsize 0

public Plugin myinfo =
{
	name        = "SourceCoop damage effects fix",
	author      = "Jimbab",
	description = "Client side damage effects fix for all weapons.",
	version     = "1.1.0",
	url         = "https://github.com/ampreeT/SourceCoop"
};

public void OnPluginStart()
{
	InitSourceCoopAddon();
}

public void OnEntityCreated(int entityIndex, const char[] szClassname)
{
	if (CBaseEntity(entityIndex).IsClassNPC())
		SDKHook(entityIndex, SDKHook_OnTakeDamage, DispatchEffects);
}

public Action DispatchEffects(int victim, int& attacker, int& inflictor, float& damage, int& damagetype, int& weapon, float damageForce[3], float damagePosition[3])
{
	// Construct our entity objects based on their entity ID's
	CBaseEntity playerAttacker = CBaseEntity(attacker);
	CBaseEntity npcVictim = CBaseEntity(victim);

	// Check victim and attacker are both npc and real player, respectively
	if (!playerAttacker.IsClassPlayer() || !npcVictim.IsClassNPC())
		return Plugin_Continue;

	// Recreate the effects
	CreateDamageEffect(damagePosition, npcVictim);

	return Plugin_Continue;
}

/*
 * Do our prop checks to check what type of entity we hit.
 */
void CreateDamageEffect(float position[3], CBaseEntity parent)
{
	int bloodColour = GetEntProp(parent.GetEntIndex(), Prop_Data, "m_bloodColor");

	switch (bloodColour)
	{
		case (-1):    // Turrets, Mechanical, etc.
		{
			HandleEffects_Metal(position, parent);
		}

		case (0):    // Human NPC's
		{
			HandleEffects_RedBlood(position, parent);
		}

		case (1):    // Alien NPC's (yellow blood)
		{
			HandleEffects_YellowBlood(position, parent);
		}

		case (2):    // Alien NPC's (green blood)
		{
			HandleEffects_GreenBlood(position, parent);
		}

		case (3):    // NPC's with synthetic blood
		{
			HandleEffects_SynthBlood(position, parent);
		}
	}
}

/*
 * Particle generator
 */
void CreateParticleEffect(float position[3], CBaseEntity parent, char[] effectName, float lengthTime = 5.0)
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
		CreateTimer(lengthTime, Callback_DeleteParticleSystem, particle);
	}
}

/*
 * Particle system deletion callback
 */
public Action Callback_DeleteParticleSystem(Handle timer, CBaseEntity particle)
{
	if (particle.IsValid())
	{
		char className[64];
		particle.GetClassname(className, sizeof(className));

		if (StrEqual(className, "info_particle_system", false))
			RemoveEdict(particle.GetEntIndex());
	}

	return Plugin_Continue;
}

/*
 * For all mechanical npc's
 */
void HandleEffects_Metal(float position[3], CBaseEntity parent)
{
	CreateParticleEffect(position, parent, "impact_sparks");
	CreateParticleEffect(position, parent, "impact_spark_burst");
}

/*
 * For all humans/soldiers
 */
void HandleEffects_RedBlood(float position[3], CBaseEntity parent)
{
	if (GetRandomInt(1, 2) == 1)
	{
		CreateParticleEffect(position, parent, "blood_impact_red_01");
		CreateParticleEffect(position, parent, "blood_impact_red_01_chunk");
		CreateParticleEffect(position, parent, "blood_impact_red_01_droplets");
		CreateParticleEffect(position, parent, "blood_impact_red_01_smalldroplets");
	}
	else
	{
		CreateParticleEffect(position, parent, "blood_impact_red_02");
		CreateParticleEffect(position, parent, "blood_impact_red_02_droplets");
		CreateParticleEffect(position, parent, "blood_impact_red_02_mist");
	}
}

/*
 * For all yellow blooded npc's
 */
void HandleEffects_YellowBlood(float position[3], CBaseEntity parent)
{
	CreateParticleEffect(position, parent, "blood_impact_yellow_01");
	CreateParticleEffect(position, parent, "blood_impact_yellow_01_chunk");
	CreateParticleEffect(position, parent, "blood_impact_yellow_01_goop");
	CreateParticleEffect(position, parent, "blood_impact_yellow_01_mist");
	CreateParticleEffect(position, parent, "blood_impact_yellow_01_smalldroplets");
}

/*
 * For all green blooded npc's
 */
void HandleEffects_GreenBlood(float position[3], CBaseEntity parent)
{
	CreateParticleEffect(position, parent, "blood_impact_green_01");
	CreateParticleEffect(position, parent, "blood_impact_green_01_chunk");
	CreateParticleEffect(position, parent, "blood_impact_green_01_droplets");
	CreateParticleEffect(position, parent, "blood_impact_green_01_smalldroplets");

	if (GetRandomInt(1, 4) == 1)
		CreateParticleEffect(position, parent, "blood_trail_green_01", 0.25);
	else if (GetRandomInt(1, 4) == 1)
		CreateParticleEffect(position, parent, "blood_trail_green_02", 0.25);
}

/*
 * For all npc's with synthetic blood (synths)
 */
void HandleEffects_SynthBlood(float position[3], CBaseEntity parent)
{
	int armourValue = GetEntProp(parent.GetEntIndex(), Prop_Data, "m_ArmorValue");

	CreateParticleEffect(position, parent, "blood_impact_synth_01");
	CreateParticleEffect(position, parent, "blood_impact_synth_01_droplets");
	CreateParticleEffect(position, parent, "blood_impact_synth_01_spurt");

	if (armourValue > 0)
		CreateParticleEffect(position, parent, "blood_impact_synth_01_armor");
}