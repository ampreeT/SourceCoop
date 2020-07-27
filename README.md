# About
This project aims to make Black Mesa campaign levels and other custom maps fully playable in multiplayer.  
The plugin enables features that are ported from the singleplayer version of the game and enhances the gameplay for co-op.

Map entities can be directly modified as they load on the server, which removes the need for players download any additional files.

Both linux and windows dedicated servers are supported.

## Features & fixes (not everything)
- checkpoint system
- map config & entity edit system
- delayed map start, delayed mapchange system
- noblock and friendly fire protection
- scripted scenes now filter for all players
- re-enables flashlight and object pickup functionality in multiplayer
- fixes sniper's crashing upon its first tick of running
- fixed scientists crash when seeing a non-friendly
- fixed npcs from not being able to shoot with anything but the shotgun
- fixed a crash caused by round restarts

## How to install
- Install latest dev build of [metamod](https://www.sourcemm.net/downloads.php?branch=dev). (Tested with build 1130) - On linux you may have to delete metamod's 64bit files.
- Install latest dev build of [sourcemod](https://www.sourcemod.net/downloads.php?branch=dev). (Tested with build 6562)
- Install latest [DHooks with detour support](https://github.com/peace-maker/DHooks2/releases) extension.
- Copy the folders from this repository into sourcemod.

At this point you can jump in and play supported maps in coop.
Playing with mp_teamplay 1 is recommended as this allows lag-free noblock.

## Adding coop support
To enable coop mode on desired map, an edt file for it needs to be created first.
EDT files are searched at specific locations in this order:
1. sourcemod/data/srccoop/mapname.edt
2. maps/mapname.edt (This location can also be loaded from inside the bsp, therefore enabling embedded configs)

## EDT format
To be explained.. You may take a look at included edt's for examples.

### Tips
- Match entities by targetname and/or classname, use hammerid as last resort (although it is in many cases unavoidable)
- Put spawn points on the floor (in hammer place info_target for getting position, in game use cl_showpos 1). Players must touch spawn items on the first frame of first teleport, if there are any to pick up.
- Use correct base class for new configs, put repetitive properties in base class
- Use "flags" block for modifying spawnflags when you only want to toggle some flags

## Customizing
The following convars are available:
- sm_coop_enabled - Sets if coop is enabled on coop maps
- sm_coop_team - Sets which team to use in TDM mode. Valid values are [marines] or [scientist]. Setting anything else will not manage teams.
- sm_coop_respawntime - Sets player respawn time in seconds. (This can only be used for making respawn times quicker, not longer)
- sm_coop_start_wait_period - The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.
- sm_coop_end_wait_period - The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.
- sm_coop_end_wait_factor - Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).
- sm_coop_debug - Sets debug logging options

# ToDo
### must-fix
- first-person camera death code needs to set original viewentity on callback OnMapEnd()
- hook UpdateEnemyMemory input on npcs, compare if !player or !pvsplayer > call manually (either closest player or all) (UpdateEnemyMemory( pEnemy, pEnemy->GetAbsOrigin(), this ))
- custom difficulty based on player count
- npcs can walk into player, make him stuck, then stop rendering
- npc_xenturret AI causes crash on map load (it seems to be fine after loading)

### non-critical
- sniper HasCondition(...) offset should go in gamedata
- add back old friendly-fire protection for allies from old plugin
	- need some way to reference a deleted entity
		- ff can be griefed by using nades and disconnecting
- npc sleepstates sometimes dont work
- lag compensation for npcs
	- test whether if adding entities to lag compensation table on entity initialization will work
- crossbow/357 sp zoom
	- convar exists for zoom smoothing
- crossbow sp bolts
- weapon_headcrab latching
	- need textures for this (they are on fpsbanana)
- custom marine ai
	- CBaseCombatCharacter::CalcWeaponProficiency(...) hook for accuracy
- implement player thirdperson animations when not holding any weapon
	- tried 2 options, both failed
		- cannot do serverside animations, client seems to ignore everything even when m_bClientSideAnimation is false
		- unable to recompile player models due to too many animation files overflow in studiomdl
- find out if the 'admire hands' animation can be reenabled when picking up suit - guessing this has to do with sp/mp viewmodels
- detour CBeam::RandomTargetname -> compare for !player -> return closest
- checkpoint teleport should attempt to find free space if blocked
- suit sound queue does not clear on respawn
- barnacle grab range is larger than in sp?
- tentacles can only 'hear' one player walking
- npc voice volume seems way too low
- add instancing for wall chargers
- trip mines lose the bright blue beam color when mp_teamplay=1
- (BM MP issue) other players' flashlight stays in place when they get out of range. One possible resolution would be to change transmit options or hook settransmit if it is an entity.

### feature
- add a hardcore mode where map resets when everyone dies, disable respawning, put dead players on spectate
- xen portal checkpoint effect
- allow configuring more features from edt configs

### map-issues
- c1a1a
	- Eli sometimes mysteriously dies and breaks the game
- c2a3a
	- during final cutscene hud is turned off, players cannot use chat or press esc
- bm_c4a4a
	- point_hurt (cyclone_hurt) has target !player (this picks first one)
- xen maps
	- healing pools have ambient_generic with !player as source entity
	
# Authors
- [ampreeT](https://steamcommunity.com/id/ampreeT/)
- [Alienmario](https://steamcommunity.com/id/4oM0/)
- [Rock](https://steamcommunity.com/id/Rock48/)

## Dedicated Playtesters
- [Diego003](https://steamcommunity.com/id/Diego63212/)
- [Googolplex23](https://steamcommunity.com/id/pandlfisher/)
- [WindedCone](https://steamcommunity.com/id/AceOak57/)
- [Antonio115](https://steamcommunity.com/profiles/76561198880559068/)
