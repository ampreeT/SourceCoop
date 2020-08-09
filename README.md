# About
This project aims to make Black Mesa campaign levels and other custom maps fully playable in multiplayer.  
The plugin enables features that are ported from the singleplayer version of the game and enhances the gameplay for co-op.

Map entities can be directly modified as they load on the server, which removes the need for players download any additional files.

Both linux and windows dedicated servers are supported.

[> Getting started <](https://github.com/ampreeT/SourceCoop/wiki/Getting-started)

## Features & fixes
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
- optional thirdperson plugin
- optional scoreboard and chat killfeed plugin
- **_many others that will be documented once the project is completed_**

# ToDo
### must-fix
- target picker on bm_c2a5i (env_mortar_controller's vgui launch button doesn't work)
- first-person camera death code needs to set original viewentity on callback OnMapEnd()
- custom difficuly based on player count
- npcs need to pusht player away, otherwise they walk into player, make him stuck/stop rendering
	- investigate https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/mp/src/game/server/ai_basenpc.cpp#L3822
- random linux server crashes related to npcs (pAnim, CalcAnimation, etc)
	- debug SelectSchedule...
- point_viewcontrol needs to respect the "Interruptable by player" spawnflag for all clients

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
- make point_hurt with target of !player affect all players
- npc_xenturret - fix beam end position

### feature
- add a hardcore mode where map resets when everyone dies, disable respawning, put dead players on spectate
- xen portal checkpoint effect
- allow configuring more features
	- create bitflags of enabled features, accessible with cvar, togglable with commands
- show changelevel bounds with tempents

### map-issues
- bm_c1a3c
	- getting possessed by mortars
- bm_c3a1b
	- tank downstairs can bug and never break, preventing progress
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
