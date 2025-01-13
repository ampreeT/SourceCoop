#pragma newdecls required
#pragma semicolon 1

#include <topmenus>

#include <srccoop_api>
#include <srccoop/public>
#include <srccoop/features>

public SharedPlugin __pl_srccoop_addon = 
{
    name = SRCCOOP_LIBRARY,
    file = "srccoop.smx",
#if defined REQUIRE_PLUGIN
    required = 1,
#else
    required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public void __pl_srccoop_addon_SetNTVOptional()
{
    MarkNativeAsOptional("SC_GetCoopTopMenu");
    MarkNativeAsOptional("SC_IsCurrentMapCoop");
    MarkNativeAsOptional("SC_Respawn");
    MarkNativeAsOptional("SC_GetSurvivalMode");
    MarkNativeAsOptional("SC_TeleportToCurrentCheckpoint");
    MarkNativeAsOptional("SC_IsCoopFeatureEnabled");
    MarkNativeAsOptional("SC_FixWeaponPrediction");
}
#endif

stock GameData InitSourceCoopAddon(bool bCloseGameData = true)
{
	GameData pGameConfig = LoadGameConfigFile(SRCCOOP_GAMEDATA_NAME);
	if (pGameConfig == null)
		SetFailState("Couldn't load game config: \"%s\"", SRCCOOP_GAMEDATA_NAME);
	
	InitClassdef(pGameConfig);
	if (bCloseGameData)
	{
		CloseHandle(pGameConfig);
        return null;
	}
	return pGameConfig;
}

/**
 * Global callback after map config is loaded in OnMapStart.
 *
 * @param kv          The configuration file imported into keyvalues. You must call kv.Rewind() first to ensure consistency.
 * @param location    From where the config was loaded.
 */
forward void SC_OnCoopMapConfigLoaded(KeyValues kv, CoopConfigLocation location);

/**
 * Global callback for when a coop map actually starts,
 * that is potentially after the player wait timer completes.
 */
forward void SC_OnCoopMapStart();

/**
 * Provides an opportunity for intercepting map changes that are handled by the plugin.
 * Possible sources of origin that are hooked by the plugin:
 * game_end, trigger_changelevel, point_servercommand, point_clientcommand in SP conversions.
 * 
 * @param szNextMap     The map that would be changed to if not intercepted.
 * @param szReason      The reason that would be passed to ForceChangeLevel if not intercepted. Defined in public.inc
 *                      Should be preserved, unless there is a specific reason not to.
 * @param isFinal       True if a final map of a series just completed. False for all other reasons.
 * @return              Plugin_Continue to proceed with the mapchange, other values to block it.
 */
forward Action SC_OnCoopMapEnd(const char[] szNextMap, const char[] szReason, bool isFinal);

/**
 * Called when the core plugin creates server-side player ragoll on death.
 * The timing of which can vary by game.
 */
forward void SC_OnPlayerRagdollCreated(CBasePlayer pPlayer, CBaseAnimating pRagdoll);

/**
 * Retrieves the handle to the coop TopMenu.
 *
 * @return              Handle to the TopMenu
 */
native TopMenu SC_GetCoopTopMenu();

/**
 * Returns whether current map is a coop map.
 *
 */
native bool SC_IsCurrentMapCoop();

/**
 * Respawns a dead player. Clears death state in survival.
 * 
 * @param options	The spawn options enum struct from public.inc
 * @return    		true if player was dead and respawned
 * @error     		Invalid CBasePlayer
 */
native bool SC_Respawn(CBasePlayer pPlayer, any options[sizeof(SpawnOptions)]);

/**
 * Gets the survival mode.
 * 
 * @return    Current survival mode (enum)
 */
native SurvivalMode SC_GetSurvivalMode();

/**
 * Teleports a player to active checkpoint.
 * 
 * @error     Invalid CBasePlayer
 * @return    true for success, false if there is no checkpoint
 */
native bool SC_TeleportToCurrentCheckpoint(CBasePlayer pPlayer);

/**
 * Checks if a SourceCoop feature is enabled.
 */
native bool SC_IsCoopFeatureEnabled(SourceCoopFeature iFeatureBits);

/**
 * Workaround for weapon prediction errors after the player has returned from third-person.
 * Uses equipment pack & restore mechanism.
 * @error     Invalid or dead CBasePlayer
 * @return    false if player's equipment has already been stored and is waiting for restore
 */
native bool SC_FixWeaponPrediction(CBasePlayer pPlayer);