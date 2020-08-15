# SourceCoop

SourceCoop is a cooperative plugin for [Black Mesa](https://store.steampowered.com/app/362890/Black_Mesa/ "Black Mesa") that __allows multiple players to play together on official and custom campaigns__. The plugin fixes various crashes, adjusts maps for cooperative play, re-enables single-player functionality, and __only needs to be installed on the server__. SourceCoop is currently in development and currently supports all earthbound levels from the official campaign.

## Installation Guide
__If you are someone who is looking to play on a server__, you are already completely set up and ready to play! Cooperative servers can be found on the server browser just like any other server.

__If you are a server operator who is looking to host your own cooperative server__, then follow the instructions below.
- Install the latest development version of [Metamod Source](https://www.sourcemm.net/downloads.php?branch=dev)
- Install the latest development version of [SourceMod](https://www.sourcemod.net/downloads.php?branch=dev)
- Install the latest release version of [DHooks with Detours support](https://github.com/peace-maker/DHooks2/releases)
- Download and copy the latest release of SourceCoop and drop it into your sourcemod directory
### Server Settings Recommendations
- Setting convar __mp_teamplay__ to __1__ will provide lag-free noblock

## Server Convars
- __sourcecoop_enabled__ - Enables cooperative functionality on maps.
- __sourcecoop_show_welcome_msg__ - Enables plugin greeting players with a message. Valid values are true or false.
- __sourcecoop_team__ - Sets which team to use in TDM mode. Valid values are "marines" or "scientist". Any other values will assume the server will handle it (not recommended).
- __sourcecoop_respawntime__ - Set player respawn time in seconds. (Cannot be used to extend respawn time).
- __sourcecoop_start_wait_period__ - The max number of seconds to wait since first player spawned in to start the map. The timer is skipped when all players enter the game.
-  __sourcecoop_end_wait_period__ - The max number of seconds to wait since first player triggered a changelevel. The timer speed increases each time a new player finishes the level.
-  __sourcecoop_end_wait_factor__ - Controls how much the number of finished players increases the changelevel timer speed. 1.0 means full, 0 means none (timer will run full length).
-  __sourcecoop_debug__ - Sets debug logging options
## Other Information
__If you are looking to help with the development of the project__, we are always looking for more help! Heres some ways you can help:
- Debugging and tracking down crashes
- Helping us make the last few official maps compatible with the plugin
- Adding new campaigns or levels to the plugin
- Simply playing on a server to help us test

If you are interested in helping us, create a pull request above or contact __ampreeT__ or __AlienMario__ below.
## Credits
- __ampreeT__ | programming, reverse engineering, map editing
	- [Steam](https://steamcommunity.com/id/ampreeT) | [GitHub](https://github.com/ampreeT)
- __kasull__ | programming, reverse engineering, trailer production
	- [Steam](https://steamcommunity.com/id/kasull/) | [GitHub](https://github.com/kasullian)
- __Alienmario__ | programming, reverse engineering, map editing
	- [Steam](https://steamcommunity.com/id/4oM0/) | [GitHub](https://github.com/Alienmario)
- __Rock__ | miscellaneous plugin features, map editing
	- [Steam](https://steamcommunity.com/id/Rock48/) | [GitHub](https://github.com/Rock48)
### Playtesters
- [Lear](https://steamcommunity.com/id/SKGNick)
- [bddu](https://steamcommunity.com/id/bddu/)
- [Foo-Fighter](https://steamcommunity.com/id/GumpForest/)
- [Diego003](https://steamcommunity.com/id/Diego63212/)
- [Googolplex23](https://steamcommunity.com/id/pandlfisher/)
- [WindedCone](https://steamcommunity.com/id/AceOak57/)
- [Antonio115](https://steamcommunity.com/profiles/76561198880559068/)
