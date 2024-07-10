<img src="https://steamuserimages-a.akamaihd.net/ugc/2455105898789623149/F6C61075AE3C71306CEFAA0A8F9E7870B710B481/" width="235" alt="SourceCoop" align="right">

[![](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml/badge.svg)](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml)
[![](https://img.shields.io/github/v/release/ampreeT/SourceCoop?style=flat&label=Release&labelColor=%232C3137&color=%23EB551B)](https://github.com/ampreeT/SourceCoop/releases/latest)
[![](https://img.shields.io/discord/973591793117564988.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/Fh77rxQaEB)

SourceCoop is a cooperative mod for Source Engine games that enables single-player campaigns to be played together.
It currently supports <a href="https://store.steampowered.com/app/362890/Black_Mesa/"><img src="images/icon-bms-small.png" height="16px"> Black Mesa</a> and <a href="https://store.steampowered.com/app/320/HalfLife_2_Deathmatch/"><img src="images/icon-hl2dm-small.png" height="16px"> Half-Life 2: Deathmatch</a>.

- Supports campaigns.
- Restores single-player functionality.
- Handles player and equipment persistence between maps.
- Easy to use optional addons.
- Easy for mappers to convert new and existing maps from single-player into cooperative.

| Table of Contents |
|:--:|
| [🖥️ Setup Guide](#setup-guide) - [⚙️ Configuration](#configuration) - [🌎 Campaign Support](#campaign-support) - [Contributing](#contributing) - [📸 Credits](#credits) |
| [Features & Configuration](https://github.com/ampreeT/SourceCoop/wiki/Features-&-Configuration) - [Server Running Tips](https://github.com/ampreeT/SourceCoop/wiki/Server-running-tips) - [Public Servers](https://github.com/ampreeT/SourceCoop/wiki/Public-Servers) |
| [📝 Developing](https://github.com/ampreeT/SourceCoop/wiki/Developing) - [🗃️ EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format) - [📘 Authoring Maps](https://github.com/ampreeT/SourceCoop/wiki/Authoring-maps-for-SourceCoop) |

## Setup Guide

__If you are someone who is looking to play on a server__, then you are already set up and ready to play! Cooperative servers can be found in the server browser just like any other server.

> #### 🌐 Player Downloads
>
> Upon joining a server, players will be able to automatically download most necessary files. For custom workshop maps in Black Mesa, players will have to manually subscribe to the Steam Workshop item before starting their game. [An official Steam Workshop collection containing all supported SourceCoop maps can be found here](https://steamcommunity.com/sharedfiles/filedetails/?id=2375865650).

__If you are a server operator who is looking to host your own cooperative server__, then follow a installation method below and forward the necessary ports:

### 🔨 Manual Installation

- Install a Source Engine Dedicated Server using <a href="https://developer.valvesoftware.com/wiki/SteamCMD"><img src="images/icon-steam-small.png" height="16px"> SteamCMD</a>.
    - <a href="https://steamdb.info/app/346680/"><img src="images/icon-bms-small.png" height="16px"> Black Mesa Dedicated Server</a> (AppID ➤ __346680__)
    - <a href="https://steamdb.info/app/232370/"><img src="images/icon-hl2dm-small.png" height="16px"> Half-Life 2: Deathmatch Dedicated Server</a> (AppID ➤ __232370__)
    ##### SteamCMD Terminal
    ```powershell
    login "anonymous"
    app_update <APPID>
    quit
    ```
- Install [Metamod:Source](https://www.sourcemm.net/downloads.php?branch=stable) (latest tested build ➤ __1155__) onto the server.
- Install [SourceMod](https://www.sourcemod.net/downloads.php?branch=stable) (latest tested build ➤ __6968__) onto the server.
- Install [the latest SourceCoop release](https://github.com/ampreeT/SourceCoop/releases) onto the server.

A visual step-by-step guide for Black Mesa is also available on <a href="https://steamcommunity.com/sharedfiles/filedetails/?id=2200247356"><img src="https://static-00.iconduck.com/assets.00/steam-icon-256x256-r6dygp6h.png" height="16px"> Steam</a>.

### 📜 Script Installation 

The script installation will automatically go through the process of installing the server prerequisites, files and plugins that are required for running a cooperative server.

- Download the corresponding installation script for your system.
    - [🪟 Windows PowerShell](scripts/srccoop-bms-windows-install.ps1)
    - [🐧 Linux Bash]()

- Run the following commands in a terminal to execute the script:
    > #### 📂 New Directories
    >
    > On script execution, the following directories will be created within the terminal's current directory:
    >
    > - Black Mesa Dedicated Server
    > - SteamCMD
    ##### Windows PowerShell (Admin Access)
    ```
    Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
    ./srccoop-bms-windows-install.ps1
    ```
    ##### Linux Bash (Sudo Access)
    ```
    sudo ./srccoop-bms-linux-install.ps1
    ```

### 🛜 Port Forwarding

##### Server Inbound Rules

| Port  | Forward Type | Description                                                                 |
|-------|--------------|-----------------------------------------------------------------------------|
| 27015 | TCP/UDP      | Game transmission, pings and RCON - Can be changed using `-port` on startup |
| 27020 | UDP          | SourceTV transmission - Can be changed using `+tv_port` on startup          |
| 27005 | UDP          | Client Port - Can be changed using `-clientport` on startup                 |
| 26900 | UDP          | Steam Port, outgoing - Can be changed using `-sport` on startup             |

## ⚙️ Configuration

### Commands

### ConVars

| Name                                | Default       | Description                                                                                                                                                                                                                                                                                            | Addon      |
|-------------------------------------|---------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------|
| sourcecoop_homemap                  |               | The map to return to after finishing a campaign/map. Example Value: `"bm_c1a0a"`                                                                                                                                                                                                                       |            |
| sourcecoop_respawntime              | `2.0`         | Sets the player respawn time in seconds. Minimum Value: `0.1`                                                                                                                                                                                                                                          |            |
| sourcecoop_survival_mode            | `0`           | Controls whether if survival mode is enabled. Values: `0` = off, `1` = respawn players if everyone is dead, `2` = restart the map if everyone is dead                                                                                                                                                  |            |
| sourcecoop_survival_respawn         | `1`           | Controls whether to respawn dead players at checkpoints when the ConVar `sourcecoop_survival_mode` is set to `1`. Values: `0` = No respawn, `1` = Respawn                                                                                                                                              |            |
| sourcecoop_survival_spawn_timeout   | `-1`          | Number of seconds after the map starts (after initial timer) to allow spawning in. Set value to `-1` for no time limit. To allow players to load into the server, this value should be set to a high enough value.                                                                                     |            |
| sourcecoop_team                     | `"scientist"` | Controls the cooperative team when the ConVar `mp_teamplay` is set to `1`. If this is set to a empty or invalid value, then team management is not enforced. Values: `"marines"`, `"scientist"`, team index                                                                                            |            |
| sourcecoop_disable_teamselect       | `1`           | Controls whether to spawn players instantly by skipping the team select screen. Values: `0` = Team select, `1` = No team select                                                                                                                                                                        |            |
| sourcecoop_start_wait_period        | `15.0`        | The max number of seconds to wait since the first player spawned in to start the map. The timer is skipped when all players enter the game.                                                                                                                                                            |            |
| sourcecoop_end_wait_period          | `60.0`        | The max number of seconds to wait since the first player triggered a changelevel. The timer speed increases each time a new player finishes the level.                                                                                                                                                 |            |
| sourcecoop_end_wait_factor          | `1.0`         | Controls how much the number of finished players increases the changelevel timer speed. Values: `0.0` (full) to `1.0` (none, timer will run full length)                                                                                                                                               |            |
| sourcecoop_end_wait_display_mode    | `1`           | Sets which method to show changelevel countdown. Values: `0` = Panel, `1` = HUD Text                                                                                                                                                                                                                   |            |
| sourcecoop_validate_steamids        | `0`           | Validate players steam id's? Increases security at the cost of some functionality breakage when Steam goes down. At the time of writing this includes survival mode and equipment persistence.                                                                                                         |            |
| sourcecoop_debug                    | `0`           | Sets debug log locations. This is available only if the plugin has been compiled with debugging enabled.                                                                                                                                                                                               |            |
| sourcecoop_debug_parts              | `6`           | Selects additional information included in debug logs. This is available only if the plugin has been compiled with debugging enabled.                                                                                                                                                                  |            |
| sourcecoop_voting_autoreload        | `1`           | Sets whether to reload all votemap menu entries on map change. This can prolong map loading times.                                                                                                                                                                                                     | Voting     |
| sourcecoop_voting_skipintro         | `1`           | Allows players to vote to skip the intro sequence.                                                                                                                                                                                                                                                     | Voting     |
| sourcecoop_voting_restartmap        | `1`           | Enables voting to restart the current map.                                                                                                                                                                                                                                                             | Voting     |
| sourcecoop_voting_changemap         | `1`           | Permits players to vote for changing the map.                                                                                                                                                                                                                                                          | Voting     |
| sourcecoop_voting_survival          | `2`           | Allows voting for survival mode. Use values from `sourcecoop_survival_mode` to select the specific mode for voting.                                                                                                                                                                                    | Voting     |
| sourcecoop_ks_default               |               | Sets the default state for killsounds. Players can individually toggle killsounds in the coop menu and their preferences will be saved in cookies.                                                                                                                                                     | Killsounds |
| sourcecoop_next_stuck               | `60.0`        | Prevents using the `stuck` command for this many seconds after each use.                                                                                                                                                                                                                               | Unstuck    |
| sourcecoop_earbleed_default         | `0`           | Sets the default player preference for the ear-bleed effect caused by explosions in cooperative mode. Preferences are saved in cookies.                                                                                                                                                                | Earbleed   |
| sourcecoop_killfeed_default         | `1`           | Awards scoreboard score to players for killing NPCs.                                                                                                                                                                                                                                                   | Scoring    |
| sourcecoop_thirdperson_enabled      |               | Enables whether if players are allowed to use thirdperson via chat command `!thirdperson`, console command `thirdperson` or the coop menu. Be aware that this requires enabling cheats on the client to work. Using thirdperson will break some weapon effects due to client-side prediction glitches. |            |
| sourcecoop_difficulty               |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_auto          |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_auto_min      |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_auto_max      |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_announce      |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_ignoredmgto   |               |                                                                                                                                                                                                                                                                                                        |            |
| sourcecoop_difficulty_ignoredmgfrom |               |                                                                                                                                                                                                                                                                                                        |            |

#### Toggleable Features

ConVar: `sc_ft <FEATURE> <0 or 1>`

> #### ⚠️ Gameplay Impact
>
> It is recommended to leave these features at the default values as these are configured per map within EDT configurations. __Modifying feature values could negatively impact the gameplay experience__.

| Feature                     | Description                                                                                                                    |
|-----------------------------|--------------------------------------------------------------------------------------------------------------------------------|
| FIRSTPERSON_DEATHCAM        | Enables the first-person death camera.                                                                                          |
| HEV_SOUNDS                  | Enables HEV sounds.                                                                                                         |
| INSTANCE_ITEMS              | Instances pickup items and weapons for each player. Instanced items disappear once picked up and 'respawn' along with the player.        |
| INSTANCE_ITEMS_NORESET      | If enabled, items will not 'respawn' picked up items after death.                                                                       |
| KEEP_EQUIPMENT              | Makes players spawn with previously picked up equipment (suit, weapons). Global for all players.                               |
| STRIP_DEFAULT_EQUIPMENT     | Removes default multiplayer equipment.                                                                                         |
| STRIP_DEFAULT_EQUIPMENT_KEEPSUIT |                                                                                                                                  |
| DISABLE_CANISTER_DROPS      | Disables item drops when players die in multiplayer.                                                                             |
| NO_TELEFRAGGING             | Prevents teleporting props and players from slaying other players.                                                             |
| NOBLOCK                     | Prevents player-on-player collisions. (This feature requires `mp_teamplay 1` to fix smoothness issues.)                                                        |
| SHOW_WELCOME_MESSAGE        | Shows players a greeting message with basic plugin info.                                                                       |
| AUTODETECT_MAP_END          | Detects commonly used commands for ending singleplayer maps from `point_clientcommand` and `point_servercommand` entities and changes the map. At first, this feature checks `sourcecoop_homemap` is set (see below), then checks if `nextmap` is set. If none are set, the map is not changed. Recommended to keep enabled. |
| CHANGELEVEL_FX              | Show visual effects (spawn particles) at level change locations.                                                               |
| TRANSFER_PLAYER_STATE       | Enables player persistence through level changes. Currently, players will carry over their health, armor and equipment for the first spawn point (checkpoint) in the map. Afterwards, the default map equipment is used. |

## 🌎 Campaign Support

<h3><img src="images/icon-bms-small.png" height="20px"> Black Mesa - <a href="https://steamcommunity.com/sharedfiles/filedetails/?id=2375865650">Steam Workshop Collection</a></h3>

- [Main Campaign](https://store.steampowered.com/app/362890/Black_Mesa/)
- [Stojkeholm](https://steamcommunity.com/sharedfiles/filedetails/?id=2320533262)
- [Emergency 17](https://steamcommunity.com/sharedfiles/filedetails/?id=934371395)

SourceCoop __allows single-player map configurations__ without decompiling and redistributing; __learn more about creating your own__ on the [EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format).

## Contributing

__If you are looking to help with the development of the project__, we are always looking for more help! Heres some ways you can help:

- Reporting inconsistencies and bugs
- Debugging and tracking down issues
- Adding features
- Adding new configurations for campaigns and maps

__If you are interested in helping us__, contact us on [Discord](https://discord.gg/Fh77rxQaEB) or create a pull request.

## 📸 Credits

### 🙏 Contributors

- __ampreeT__ :: [Steam](https://steamcommunity.com/id/ampreeT) | [GitHub](https://github.com/ampreeT) :: programming, reverse engineering, map editing
- __kasull__ :: [Steam](https://steamcommunity.com/id/kasull/) | [GitHub](https://github.com/kasullian) :: programming, reverse engineering, trailer production
- __Alienmario__ :: [Steam](https://steamcommunity.com/id/4oM0/) | [GitHub](https://github.com/Alienmario) :: programming, reverse engineering, map editing
- __Balimbanana__ :: [Steam](https://steamcommunity.com/id/Balimbanana/) | [GitHub](https://github.com/Balimbanana) :: programming, reverse engineering
- __Rock__ :: [Steam](https://steamcommunity.com/id/Rock48/) | [GitHub](https://github.com/Rock48) :: programming, map editing
- __Krozis Kane__ :: [Steam](https://steamcommunity.com/id/Krozis_Kane/) | [GitHub](https://github.com/KrozisKane) :: map editing
- __ReservedRegister__ :: [GitHub](https://github.com/ReservedRegister) :: reverse engineering
- __raicovx__ :: [Github](https://github.com/raicovx) :: equipment persistence
- __Jimmy-Baby__ :: [Github](https://github.com/Jimmy-Baby) :: damage effects addon
- __Removiekeen__ :: [Steam](https://steamcommunity.com/profiles/76561198804614641/) :: logo design
- __yarik2720__ :: [Github](https://github.com/yarik2720) :: russian translation, ci/cd

### 🗄️ External Libraries

- [SourceMod](https://github.com/alliedmodders/sourcemod)
- [SourceScramble](https://github.com/nosoop/SMExt-SourceScramble)
- [stocksoup](https://github.com/nosoop/stocksoup)
- [sm-logdebug](https://github.com/Alienmario/sm-logdebug)
- [smlib](https://github.com/bcserv/smlib/tree/transitional_syntax)
