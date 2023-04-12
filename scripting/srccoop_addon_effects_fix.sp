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
	version     = SRCCOOP_VERSION,
	url         = SRCCOOP_URL
};

ConVar cvarAlienBlood;
ConVar cvarHumanBlood;

public void OnPluginStart()
{
	InitSourceCoopAddon();
	cvarAlienBlood = FindConVar("violence_ablood");
	cvarHumanBlood = FindConVar("violence_hblood");
}

public void OnEntityCreated(int entityIndex, const char[] szClassname)
{
	CBaseEntity entity = CBaseEntity(entityIndex);

	if (entity.IsClassNPC())
	{
		SDKHook(entityIndex, SDKHook_OnTakeDamagePost, DispatchEffects);
	}
}

public void DispatchEffects(int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom)
{
	// Construct our entity objects based on their entity ID's
	CBaseEntity playerAttacker = CBaseEntity(attacker);
	CBaseEntity npcVictim = CBaseEntity(victim);

	if (playerAttacker.IsClassPlayer())
		CreateDamageEffect(damagePosition, npcVictim);
}

/*
 * Do our prop checks to check what type of entity we hit.
 */
void CreateDamageEffect(const float position[3], CBaseEntity parent)
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
			if (cvarHumanBlood.BoolValue)
				HandleEffects_RedBlood(position, parent);
		}

		case (1):    // Alien NPC's (yellow blood)
		{
			if (cvarAlienBlood.BoolValue)
				HandleEffects_YellowBlood(position, parent);
		}

		case (2):    // Alien NPC's (green blood)
		{
			if (cvarAlienBlood.BoolValue)
				HandleEffects_GreenBlood(position, parent);
		}

		case (3):    // NPC's with synthetic blood
		{
			if (cvarAlienBlood.BoolValue)
				HandleEffects_SynthBlood(position, parent);
		}
	}
}

/*
 * Particle generator
 */
void CreateParticleSystem(const float position[3], CBaseEntity parent, char[] effectName, float lengthTime = 5.0)
{
	CParticleSystem particle = CParticleSystem.Create(effectName);

	if (particle.IsValid())
	{
		particle.Teleport(position);
		particle.SetParent(parent);

		particle.Spawn();
		particle.Activate();
		
		particle.KillAfterTime(lengthTime);
	}
}

/*
 * For all mechanical npc's
 */
void HandleEffects_Metal(const float position[3], CBaseEntity parent)
{
	CreateParticleSystem(position, parent, "impact_sparks");
	CreateParticleSystem(position, parent, "impact_spark_burst");
}

/*
 * For all humans/soldiers
 */
void HandleEffects_RedBlood(const float position[3], CBaseEntity parent)
{
	if (GetRandomInt(1, 2) == 1)
	{
		CreateParticleSystem(position, parent, "blood_impact_red_01");
		CreateParticleSystem(position, parent, "blood_impact_red_01_chunk");
		CreateParticleSystem(position, parent, "blood_impact_red_01_droplets");
		CreateParticleSystem(position, parent, "blood_impact_red_01_smalldroplets");
	}
	else
	{
		CreateParticleSystem(position, parent, "blood_impact_red_02");
		CreateParticleSystem(position, parent, "blood_impact_red_02_droplets");
		CreateParticleSystem(position, parent, "blood_impact_red_02_mist");
	}
}

/*
 * For all yellow blooded npc's
 */
void HandleEffects_YellowBlood(const float position[3], CBaseEntity parent)
{
	CreateParticleSystem(position, parent, "blood_impact_yellow_01");
	CreateParticleSystem(position, parent, "blood_impact_yellow_01_chunk");
	CreateParticleSystem(position, parent, "blood_impact_yellow_01_goop");
	CreateParticleSystem(position, parent, "blood_impact_yellow_01_mist");
	CreateParticleSystem(position, parent, "blood_impact_yellow_01_smalldroplets");
}

/*
 * For all green blooded npc's
 */
void HandleEffects_GreenBlood(const float position[3], CBaseEntity parent)
{
	CreateParticleSystem(position, parent, "blood_impact_green_01");
	CreateParticleSystem(position, parent, "blood_impact_green_01_chunk");
	CreateParticleSystem(position, parent, "blood_impact_green_01_droplets");
	CreateParticleSystem(position, parent, "blood_impact_green_01_smalldroplets");

	if (GetRandomInt(1, 4) == 1)
		CreateParticleSystem(position, parent, "blood_trail_green_01", 0.25);
	else if (GetRandomInt(1, 4) == 1)
		CreateParticleSystem(position, parent, "blood_trail_green_02", 0.25);
}

/*
 * For all npc's with synthetic blood (synths)
 */
void HandleEffects_SynthBlood(const float position[3], CBaseEntity parent)
{
	CreateParticleSystem(position, parent, "blood_impact_synth_01");
	CreateParticleSystem(position, parent, "blood_impact_synth_01_droplets");
	CreateParticleSystem(position, parent, "blood_impact_synth_01_spurt");
}