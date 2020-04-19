# About
This project aims to make Black Mesa campaign levels and other custom maps fully playable in multiplayer.  
We do this by fixing and enabling features from the singleplayer version of the game. Maps are directly modified as they load on the server, so there's no need for players to redownload them.
Both linux and windows dedicated servers are supported.

## Features & fixes (not everything)
- checkpoint system
- map config & entity edit system
- delayed map start system
- scripted scenes now filter for all players
- re-enables flashlight and object pickup functionality in multiplayer
- fixes sniper's crashing upon its first tick of running
- fixed scientists crash when seeing a non-friendly
- fixed npcs from not being able to shoot with anything but the shotgun
- fixed a crash caused by round restarts

## How to install
- Install latest build of metamod and sourcemod. Developement builds -not stable- are required for both at the moment (you may have to delete metamod's 64bit files to get it to work).
- Install DHooks with detour support extension.
- Copy the folders from this repository into sourcemod and compile the plugin.

## Edt editing guidelines
- match entities by targetname and/or classname, use hammerid as last resort (although it is in many cases unavoidable)
- put spawn points on the floor (in hammer place info_target for getting position, in game use cl_showpos 1). Players must touch spawn items on the first frame of teleport.
- use correct base class for new configs, put repetitive properties in base class
- use "flags" block for modifying spawnflags when you only want to toggle some flags

## ToDo
### must-fix
- first-person camera death code needs to set original viewentity on callback OnMapEnd()
- bigfish crash
- hook UpdateEnemyMemory input on npcs, compare if !player or !pvsplayer > call manually (either closest player or all) (UpdateEnemyMemory( pEnemy, pEnemy->GetAbsOrigin(), this ))
- custom difficulty based on player count
- npcs can walk into player, make him stuck, then stop rendering

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
- trigger_changelevel
	- game code checks for g_pGameRules->IsDeathmatch(), overriding that to false crashes on changelevel, but at least its something (could explore crash dump on linux)
- implement player thirdperson animations when not holding any weapon
	- tried 2 options, both failed
		- cannot do serverside animations, client seems to ignore everything even when m_bClientSideAnimation is false
		- unable to recompile player models due to too many animation files overflow in studiomdl
- find out if the 'admire hands' animation can be reenabled when picking up suit - guessing this has to do with sp/mp viewmodels
- detour CBeam::RandomTargetname -> compare for !player -> return closest
- no breakmodel gibs appear (MP issue)
- checkpoint teleport should attempt to find free space if blocked
- suit sound queue does not clear on death or spawn

### feature
- add a hardcore mode where map resets when everyone dies, disable respawning, put dead players on spectate
- add some sort of agree-to-change voting / player counter / timer before switching to next map
- xen portal checkpoint effect
- allow configuring enabled features from edt configs

## Map issues
- bm_c4a4a
	- point_hurt (cyclone_hurt) has target !player (this picks first one)
- xen maps
	- healing pools have ambient_generic with !player as source entity
	
## Authors
- [ampreeT](https://steamcommunity.com/id/ampreeT/)
- [Alienmario](https://steamcommunity.com/id/4oM0/)