#pragma newdecls required
#pragma semicolon 1

//===================================================================================================================
// Hud Element hiding flags
enum HideHudFlags_t
{
    HIDEHUD_WEAPONSELECTION = (1 << 0),     // Hide ammo count & weapon selection
    HIDEHUD_FLASHLIGHT = (1 << 1),
    HIDEHUD_ALL = (1 << 2),
    HIDEHUD_HEALTH = (1 << 3),              // Hide health & armor / suit battery
    HIDEHUD_PLAYERDEAD = (1 << 4),          // Hide when local player's dead
    HIDEHUD_NEEDSUIT = (1 << 5),            // Hide when the local player doesn't have the HEV suit
    HIDEHUD_MISCSTATUS = (1 << 6),          // Hide miscellaneous status elements (trains, pickup history, death notices, etc)
    HIDEHUD_CHAT = (1 << 7),                // Hide all communication elements (saytext, voice icon, etc)
    HIDEHUD_CROSSHAIR = (1 << 8),           // Hide crosshairs
    HIDEHUD_VEHICLE_CROSSHAIR = (1 << 9),	// Hide vehicle crosshair
    HIDEHUD_INVEHICLE = (1 << 10),
    HIDEHUD_BONUS_PROGRESS = (1 << 11),     // Hide bonus progress display (for bonus map challenges)
}
