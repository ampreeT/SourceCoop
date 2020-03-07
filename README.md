## features & fixes (not everything)
- checkpoint system
- scripted scenes now filter for all players
- re-enables flashlight functionality in multiplayer
- fixes sniper's crashing upon its first tick of running
- fixed scientists crash when seeing a non-friendly
- fixed npcs from not being able to shoot with anything but the shotgun
- fixed a crash caused by round restarts

## todo
- xen portal checkpoint effect
- sniper HasCondition(...) offset should go in gamedata
- first-person camera death code needs to set original viewentity on callback OnMapEnd()
- strict type-error checking for checkpoint & map filesystem
- sprinting animations in a clean way
	- old method consisted of forcing animations
- add back old friendly-fire protection for allies from old plugin
	- need some way to reference a deleted entity
		- ff can be griefed by using nades and disconnecting
- npc sleepstates sometimes dont work
- lag compensation for npcs
- crossbow/357 sp zoom
	- convar exists for zoom smoothing
- crossbow sp bolts
- bigfish crash
- crash caused by zombies/vortigaunts attacking a player-held object
- custom difficulty based on player count
	- weapon_headcrab latching
		- need textures for this (they are on fpsbanana)
	- restore hev suit lines with a detour hook on CBasePlayer::SetSuitUpdate(...)
		- possibly inlined as it is a empty function in normal src mp games
	- custom marine ai
		- CBaseCombatCharacter::CalcWeaponProficiency(...) hook for accuracy

## map-issues
- bm_c0a0a
	- need soundtrack on join
	- remove fade and teleport
- bm_c0a0b
	- sound rotation bug with tram