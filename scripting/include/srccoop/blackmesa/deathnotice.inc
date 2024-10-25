#if defined _srccoop_deathnotice_game_included
 #endinput
#endif
#define _srccoop_deathnotice_game_included

#pragma newdecls required
#pragma semicolon 1

char g_pDnDisplayList[][2][] = 
{
	// NPCS.
	{ "npc_abrams", "HECU Abrams" },
	{ "npc_alien_controller", "Alien Controller" },
	{ "npc_alien_grunt", "Alien Grunt" },
	{ "npc_alien_grunt_elite", "Elite Alien Grunt" },
	{ "npc_alien_grunt_melee", "Alien Grunt" },
	{ "npc_alien_slave", "Alien Slave" },
	{ "npc_alien_slave_dummy", "Alien Slave" },
	{ "npc_apache", "HECU Apache" },
	{ "npc_barnacle", "Barnacle" },
	{ "npc_beneathticle", "Underwater Barnacle" },
	{ "npc_boid", "Boid" },
	{ "npc_boid_flock", "Boid Flock" },
	{ "npc_bullsquid", "Bullsquid" },
	{ "npc_bullsquid_melee", "Bullsquid" },
	{ "npc_crow", "Crow" },
	{ "npc_eli", "Dr. Eli" },
	{ "npc_furniture", "Furniture" },
	{ "npc_gargantua", "Gargantua" },
	{ "npc_Gonarch", "Gonarch" },
	{ "npc_headcrab", "Headcrab" },
	{ "npc_headcrab_baby", "Baby Headcrab" },
	{ "npc_headcrab_black", "Poison Headcrab" },
	{ "npc_headcrab_fast", "Fast Headcrab" },
	{ "npc_headcrab_poison", "Poison Headcrab" },
	{ "npc_hecu_marine", "HECU Soldier" },
	{ "npc_houndeye", "Houndeye" },
	{ "npc_houndeye_knockback", "Knockback Houndeye" },
	{ "npc_houndeye_suicide", "Explosive Houndeye" },
	{ "npc_human_assassin", "Assassin" },
	{ "npc_human_commander", "HECU Commander" },
	{ "npc_human_grenadier", "HECU Soldier" },
	{ "npc_human_grunt", "HECU Soldier" },
	{ "npc_human_medic", "HECU Soldier" },
	{ "npc_human_scientist", "Scientist" },
	{ "npc_human_scientist_eli", "Dr. Eli" },
	{ "npc_human_scientist_female", "Scientist" },
	{ "npc_human_scientist_kleiner", "Dr. Kleiner" },
	{ "npc_human_security", "Security Guard" },
	{ "npc_ichthyosaur", "Ichthyosaur" },
	{ "npc_kleiner", "Dr. Kliener" },
	{ "npc_lav", "HECU LAV" },
	{ "npc_leech", "Leech" },
	{ "npc_nihilanth", "Nihilanth" },
	{ "npc_osprey", "Osprey" },
	{ "npc_pigeon", "Pigeon" },
	{ "npc_plantlight", "Plantlight" },
	{ "npc_plantlight_stalker", "Plantlight" },
	{ "npc_protozoan", "Protozoan" },
	{ "npc_puffballfungus", "Puffball Fungus" },
	{ "npc_seagull", "Seagull" },
	{ "npc_sentry_ceiling", "Turret" },
	{ "npc_sentry_ground", "Turret" },
	{ "npc_snark", "Snark" },
	{ "npc_sniper", "HECU Sniper" },
	{ "npc_tentacle", "Alien Tentacle" },
	{ "npc_vortigaunt", "Alien Slave" },
	{ "npc_xen_grunt", "Alien Grunt" },
	{ "npc_xentacle", "Alien Tentacle" },
	{ "npc_xentree", "Alien Tree" },
	{ "npc_xenturret", "Alien Turret" },
	{ "npc_xontroller", "Alien Xontroller" },
	{ "npc_xort", "Alien Slave" },
	{ "npc_xortEB", "Alien Slave" },
	{ "npc_zombie_grunt", "Zombie" },
	{ "npc_zombie_grunt_torso", "Zombie" },
	{ "npc_zombie_hev", "Zombie" },
	{ "npc_zombie_scientist", "Zombie" },
	{ "npc_zombie_scientist_torso", "Zombie" },
	{ "npc_zombie_security", "Zombie" },
	
	// Entities.
	{ "env_beam", "Laser Beam" },
	{ "env_explosion", "Explosion" },
	{ "env_fire", "Fire" },
	{ "env_laser", "Laser Beam" },
	{ "entityflame", "Fire" },
	{ "func_movelinear", "Dynamic Prop" },
	{ "prop_physics", "Physics Prop" },
	{ "prop_xen_int_barrel", "Physics Prop" },
	{ "prop_barrel_cactus", "Cactus" },
	{ "prop_barrel_cactus_semilarge", "Cactus" },
	{ "func_minefield", "Landmine" },

	// Weapon Projectiles.
	{ "grenade_bolt", "Crossbow Bolt" },
	{ "grenade_frag", "Frag Grenade" },
	{ "grenade_hornet", "Hornet" },
	{ "grenade_mp5_contact", "MP5 Grenade" },
	{ "grenade_rpg", "RPG" },
	{ "grenade_apache_rpg", "Apache" },
	{ "grenade_satchel", "Satchel" },
	{ "grenade_tripmine", "Tripmine" },
	{ "npc_snark", "Snark" },
	
	// Entity Projectiles.
	{ "grenade_mortar", "Mortar Shell" },
	{ "grenade_mortar_small", "Mortar Shell" },
	{ "grenade_mortar_large", "Mortar Shell" },
};

char g_pDnWeaponIconList[][2][] =
{
	// Weapons.
	{ "weapon_357", "357" },
	{ "weapon_assassin_glock", "assassin_glock" },
	{ "weapon_crossbow", "crossbow" },
	{ "weapon_crowbar", "crowbar" },
	{ "weapon_frag", "frag" },
	{ "weapon_glock", "glock" },
	{ "weapon_gluon", "gluon" },
	{ "weapon_headcrab", "headcrab" },
	{ "weapon_hivehand", "hivehand" },
	{ "weapon_mp5", "mp5" },
	{ "weapon_rpg", "rpg" },
	{ "weapon_satchel", "satchel" },
	{ "weapon_shotgun", "shotgun" },
	{ "weapon_snark", "snark" },
	{ "weapon_tau", "tau" },
	{ "weapon_tripmine", "tripmine" },

	// Weapon Projectiles.
	{ "grenade_bolt", "crossbow" },
	{ "grenade_frag", "frag" },
	{ "grenade_hornet", "hivehand" },
	{ "grenade_mp5_contact", "mp5" },
	{ "grenade_rpg", "rpg" },
	{ "grenade_apache_rpg", "rpg" },
	{ "grenade_satchel", "satchel" },
	{ "grenade_tripmine", "tripmine" },
	{ "npc_snark", "snark" },
};

// TODO:
// Check the following:
// 
// env_gunfire
// env_mortar_launcher
// env_sporeexplosion
// func_50cal
// func_door
// func_tow
// func_tow_mp

// grenade_nuke

// grenade_spit
// grenade_tank_shell
// grenade_tow


// proto_sniper