## features & fixes (not everything)
- checkpoint system
- map & entity config system
- delayed map start system
- scripted scenes now filter for all players
- re-enables flashlight and object pickup functionality in multiplayer
- fixes sniper's crashing upon its first tick of running
- fixed scientists crash when seeing a non-friendly
- fixed npcs from not being able to shoot with anything but the shotgun
- fixed a crash caused by round restarts

## edt editing guidelines
- match entities by targetname and/or classname, use hammerid as last resort (although it is in many cases unavoidable)
- put spawn points on the floor (in hammer place info_target for getting position, in game use cl_showpos 1). Players must touch spawn items on the first frame of teleport.
- use correct base class for new configs, put repetitive properties in base class
- use "flags" block for modifying spawnflags when you only want to toggle some flags

## todo
- xen portal checkpoint effect
- sniper HasCondition(...) offset should go in gamedata
- first-person camera death code needs to set original viewentity on callback OnMapEnd()
- sprinting animations in a clean way
	- old method consisted of forcing animations
	- bug: throwing nades while sprinting bugs out
- add back old friendly-fire protection for allies from old plugin
	- need some way to reference a deleted entity
		- ff can be griefed by using nades and disconnecting
- npc sleepstates sometimes dont work
- lag compensation for npcs
	- test whether if adding entities to lag compensation table on entity initialization will work
- crossbow/357 sp zoom
	- convar exists for zoom smoothing
- crossbow sp bolts
- bigfish crash
- custom difficulty based on player count
	- weapon_headcrab latching
		- need textures for this (they are on fpsbanana)
	- restore hev suit lines with a detour hook on CBasePlayer::SetSuitUpdate(...)
		- possibly inlined as it is a empty function in normal src mp games
		- should queue sounds if one is playing
		- should reset on death
	- custom marine ai
		- CBaseCombatCharacter::CalcWeaponProficiency(...) hook for accuracy
- trigger_changelevel
	- game code checks for g_pGameRules->IsDeathmatch(), overriding that to false crashes on changelevel, but at least its something (could explore crash dump on linux)
- add regex parsing?
- implement player thirdperson animations when not holding any weapon
	- tried 2 options, both failed
		- cannot do serverside animations, client seems to ignore everything even when m_bClientSideAnimation is false
		- unable to recompile player models due to too many animation files overflow in studiomdl
- find out if the 'admire hands' animation can be reenabled when picking up suit
- hook UpdateEnemyMemory input on npcs, compare if !player or !pvsplayer > call manually (either closest player or all) (UpdateEnemyMemory( pEnemy, pEnemy->GetAbsOrigin(), this ))
- ai_goal_follow (+possibly other ai_goals) with goal of !player - set closest - detour CBaseEntity* CAI_GoalEntity::GetGoalEntity
- detour CBeam::RandomTargetname -> compare for !player -> return closest

## map-issues
- bm_c4a4a
	-point_hurt (cyclone_hurt) has target !player (this picks first one)
- xen maps
	- healing pools have ambient_generic with !player as source entity