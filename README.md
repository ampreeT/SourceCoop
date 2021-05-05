# SourceCoop

SourceCoop is a cooperative plugin for [Black Mesa](https://store.steampowered.com/app/362890/Black_Mesa/ "Black Mesa") that __allows multiple players to play together on official and custom campaigns__. The plugin fixes various crashes, adjusts maps for cooperative play, re-enables single-player functionality, and __only needs to be installed on the server__.

## Installation Guide
__If you are someone who is looking to play on a server__, you are already completely set up and ready to play! Cooperative servers can be found on the server browser just like any other server.

__If you are a server operator who is looking to host your own cooperative server__, then follow the instructions below.
- Install the latest release version of [Metamod:Source](https://www.sourcemm.net/downloads.php?branch=stable)
	- latest tested working build is __1143__
- Install a development version of [SourceMod](https://www.sourcemod.net/downloads.php?branch=dev)
	- latest tested working build is __6645__
- Install the latest release version of [DHooks with Detours support](https://github.com/peace-maker/DHooks2/releases)
	- latest tested working build is __dhooks-2.2.0-detours15-sm110__
- Install the latest release of SourceCoop from the [releases page](https://github.com/ampreeT/SourceCoop/releases)

A step-by-step guide is also available on [Steam](https://steamcommunity.com/sharedfiles/filedetails/?id=2200247356).

## Server configuration

### Recommendations
- Setting convar __mp_teamplay__ to __1__ will provide lag-free noblock
- Setting convar __sv_always_run__ to __1__ will avoid glitchy sprinting weapon animations

### Plugin Convars
- __sourcecoop_enabled__ - Enables cooperative functionality on maps.
- __sourcecoop_team__ - Sets which team to use in TDM mode. Valid values are "marines" or "scientist". Any other values will assume the server will handle it (not recommended).
- __sourcecoop_respawntime__ - Set player respawn time in seconds. (Cannot be used to extend respawn time).
- __sourcecoop_start_wait_period__ - The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.
-  __sourcecoop_end_wait_period__ - The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.
-  __sourcecoop_end_wait_factor__ - Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).
-  __sourcecoop_homemap__ - The map to return to after finishing a campaign/map.
-  __sourcecoop_debug__ - Sets debug logging options

__Additional configuration can be found on the [wiki page](https://github.com/ampreeT/SourceCoop/wiki/Features-&-Configuration).__

## Other Information
__If you are looking to help with the development of the project__, we are always looking for more help! Heres some ways you can help:
- Debugging and tracking down crashes
- Adding configs for new campaigns or levels
- Simply playing on a server to help us test

If you are interested in helping us, create a pull request above or contact __ampreeT__ or __Alienmario__ below.
## Credits
- __ampreeT__ | programming, reverse engineering, map editing
	- [Steam](https://steamcommunity.com/id/ampreeT) | [GitHub](https://github.com/ampreeT)
- __kasull__ | programming, reverse engineering, trailer production
	- [Steam](https://steamcommunity.com/id/kasull/) | [GitHub](https://github.com/kasullian)
- __Alienmario__ | programming, reverse engineering, map editing
	- [Steam](https://steamcommunity.com/id/4oM0/) | [GitHub](https://github.com/Alienmario)
- __Rock__ | miscellaneous plugin features, map editing
	- [Steam](https://steamcommunity.com/id/Rock48/) | [GitHub](https://github.com/Rock48)
- __Krozis Kane__ | map editing
	- [Steam](https://steamcommunity.com/id/Krozis_Kane/) | [GitHub](https://github.com/KrozisKane)
### Playtesters
- [Lear](https://steamcommunity.com/id/SKGNick)
- [bddu](https://steamcommunity.com/id/bddu/)
- [Foo-Fighter](https://steamcommunity.com/id/GumpForest/)
- [Diego003](https://steamcommunity.com/id/Diego63212/)
- [Googolplex23](https://steamcommunity.com/id/pandlfisher/)
- [WindedCone](https://steamcommunity.com/id/AceOak57/)
- [Antonio115](https://steamcommunity.com/profiles/76561198880559068/)
