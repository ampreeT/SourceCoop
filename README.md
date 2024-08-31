<img src="images/logo-sc.png" width="235" alt="SourceCoop" align="right">

[![](https://img.shields.io/github/v/release/ampreeT/SourceCoop?style=flat&label=Release&labelColor=%232C3137&color=%23EB551B)](https://github.com/ampreeT/SourceCoop/releases/latest)
[![](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml/badge.svg)](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml)
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
| [🖥️ Setup Guide](#setup-guide) - [🌎 Campaign Support](#campaign-support) - [⚙️ Configuration](#configuration) - [🌱 Contributing](#contributing) - [📸 Credits](#credits) |
| [📡 Server Running Tips](https://github.com/ampreeT/SourceCoop/wiki/Server-running-tips) - [🔗 Public Servers](https://github.com/ampreeT/SourceCoop/wiki/Public-Servers) |
| [📝 Developing](https://github.com/ampreeT/SourceCoop/wiki/Developing) - [🗃️ EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format) - [📘 Authoring Maps](https://github.com/ampreeT/SourceCoop/wiki/Authoring-maps-for-SourceCoop) |

<a id="setup-guide"></a>
## 🖥️ Setup Guide

__If you are someone who is looking to play on a server__, then you are already set up and ready to play! Cooperative servers can be found in the server browser just like any other server.

> #### 🌐 Player Downloads
>
> Upon joining a server, players will be able to automatically download most necessary files. For custom workshop maps in Black Mesa, players will have to manually subscribe to the Steam Workshop item before starting their game. [An official Steam Workshop collection containing all supported SourceCoop maps can be found here](https://steamcommunity.com/sharedfiles/filedetails/?id=2375865650).

__If you are a server operator who is looking to host your own cooperative server__, then follow a installation method below and forward the necessary ports:

### 📜 Script Installation 

The script installation will automatically go through the process of installing the server files and plugins that are required for running a cooperative server.

> #### 🐧Linux Distributions
> 
> The Linux installation script has been tested with the following distributions:
> 
> - Ubuntu
> - Debian
> 
> __If the installation script does not support the Linux distribution that you are using__, then feel free to modify the script and create a pull request!

- Download the corresponding installation script for your system.
    - <img src="images/icon-bms-small.png" height="16px"> Black Mesa
        - [🪟 Windows PowerShell](scripts/srccoop-bms-windows-install.ps1)
        - [🐧 Linux Bash](scripts/srccoop-bms-linux-install.sh)

- Run the following commands in a terminal to start the installation:
    > #### 📂 New Directories
    >
    > On script execution, the following directories will be created within the terminal's current directory:
    >
    > - Black Mesa Dedicated Server
    > - SteamCMD
    > - Steam
    ##### Windows PowerShell Terminal
    ```powershell
    Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
    ./srccoop-bms-windows-install.ps1
    ```
    ##### Linux Bash Terminal
    ```bash
    chmod +x "./srccoop-bms-linux-install.sh"
    ./srccoop-bms-linux-install.sh
    ```

- After the installation process is complete, the server can be started by running the following commands. [Make sure that the necessary ports are forwarded so players to be able to join the server!](#port-forwarding)
    ##### Windows PowerShell Terminal
    ```powershell
    cd "Black Mesa Dedicated Server"
    ./srcds_coop.bat
    ```
    ##### Linux Bash Terminal
    ```bash
    cd "Black Mesa Dedicated Server"
    ./srcds_coop.sh
    ```

### 🔨 Manual Installation

- Install a Source Engine Dedicated Server using <a href="https://developer.valvesoftware.com/wiki/SteamCMD"><img src="images/icon-steam-small.png" height="16px"> SteamCMD</a>.
    - <a href="https://steamdb.info/app/346680/"><img src="images/icon-bms-small.png" height="16px"> Black Mesa Dedicated Server</a> (AppID ➤ __346680__)
    - <a href="https://steamdb.info/app/232370/"><img src="images/icon-hl2dm-small.png" height="16px"> Half-Life 2: Deathmatch Dedicated Server</a> (AppID ➤ __232370__)
    ##### SteamCMD Terminal (Black Mesa)
    ```powershell
    login "anonymous"
    app_update 346680
    quit
    ```
    ##### SteamCMD Terminal (Half-Life 2: Deathmatch)
    ```powershell
    login "anonymous"
    app_update 232370
    quit
    ```
- Install [Metamod:Source](https://www.sourcemm.net/downloads.php?branch=stable) (latest tested build ➤ __1155__) onto the server.
- Install [SourceMod](https://www.sourcemod.net/downloads.php?branch=stable) (latest tested build ➤ __6968__) onto the server.
- Install [the latest SourceCoop release](https://github.com/ampreeT/SourceCoop/releases) onto the server.
- [Forward the necessary ports.](#port-forwarding)

A visual step-by-step guide for Black Mesa is also available on <a href="https://steamcommunity.com/sharedfiles/filedetails/?id=2200247356"><img src="images/icon-steam-small.png" height="16px"> Steam</a>.

<a id="port-forwarding"></a>
### 🛜 Port Forwarding

In order for players to able to join the server, you will need to only forward the default game transmission TCP/UDP port `27015`. All other ports are optional.

##### Server Inbound Rules

| Port  | Forward Type | Description                                                                 |
|-------|--------------|-----------------------------------------------------------------------------|
| `27015` | TCP/UDP      | Game transmission, pings and RCON - Can be changed using `-port` on startup |
| `27020` | UDP          | SourceTV transmission - Can be changed using `+tv_port` on startup          |
| `27005` | UDP          | Client Port - Can be changed using `-clientport` on startup                 |
| `26900` | UDP          | Steam Port, outgoing - Can be changed using `-sport` on startup             |

<a id="campaign-support"></a>
## 🌎 Campaign Support

<h3><img src="images/icon-bms-small.png" height="20px"> Black Mesa - <a href="https://steamcommunity.com/sharedfiles/filedetails/?id=2375865650">Steam Workshop Collection</a></h3>

- [Main Campaign](https://store.steampowered.com/app/362890/Black_Mesa/)
- [Stojkeholm](https://steamcommunity.com/sharedfiles/filedetails/?id=2320533262)
- [Emergency 17](https://steamcommunity.com/sharedfiles/filedetails/?id=934371395)

SourceCoop __allows single-player map configurations__ without decompiling and redistributing; __learn more about creating your own__ on the [EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format).

If you have already created native cooperative support for your map by including a EDT file, please make a pull request onto the `README.md` with your Steam Workshop item link so we can showcase it off!

<a id="configuration"></a>
## ⚙️ Configuration

### Commands

| **Command**               | **Description**                                                   | **Addon**        |
|---------------------------|-------------------------------------------------------------------|------------------|
| `sm_coopmenu`             | Displays the coop menu                                            | Base             |
| `sizeup`                  | Displays the coop menu                                            | Base             |
| `sm_thirdperson`          | Type `!thirdperson` to go into thirdperson mode                     | Thirdperson      |
| `sm_firstperson`          | Type `!firstperson` to exit thirdperson mode                        | Thirdperson      |
| `stuck`                   | Unstuck command                                                   | Unstuck          |
| `unstuck`                 | Unstuck command                                                   | Unstuck          |
| `sm_skipintro`            | Starts a skip intro vote                                          | Voting           |
| `sm_restartmap`           | Starts a restart map vote                                         | Voting           |
| `sm_changemap`            | Shows a menu for changing maps                                    | Voting           |
| `sm_survival`             | Starts a survival vote                                            | Voting           |

### Admin Commands

| **Command**               | **Description**                                                                                    | **Addon**        |
|---------------------------|----------------------------------------------------------------------------------------------------|------------------|
| `sc_save`                 | Exports last saved player equipment state to a file.                                               | Base             |
| `sc_load`                 | Imports saved data from file and attempts to equip each player.                                    | Base             |
| `sc_clear`                | Clear persisted equipment and equip players with the map defaults.                                 | Base             |
| `sourcecoop_dump`         | Command for dumping map entities to a file.                                                        | Base             |
| `sc_dump`                 | Command for dumping map entities to a file.                                                        | Base             |
| `sc_mkconfigs`            | Creates default edt configs for all maps in the maps directory which are missing one.              | Base             |
| `sc_revive`               | Force respawn player.                                                                              | Revive           |
| `sc_reload_maps`          | Reloads all entries in the votemap menu from storage.                                              | Voting           |

### ConVars
| **Name**                               | **Default** | **Description**                                                                                                                                                   | **Addon**              |
|----------------------------------------|-------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------------------|
| `sourcecoop_version`                      |    | The version of the SourceCoop mod.                                                                                                                                 | Base                   |
| `sourcecoop_respawntime`                  | `2.0`               | Sets player respawn time in seconds.                                                                                                                               | Base                   |
| `sourcecoop_start_wait_period`            | `15.0`              | The max number of seconds to wait since the first player spawned in to start the map. The timer is skipped when all players enter the game.                         | Base                   |
| `sourcecoop_end_wait_period`              | `60.0`              | The max number of seconds to wait since the first player triggered a changelevel. The timer speed increases each time a new player finishes the level.             | Base                   |
| `sourcecoop_end_wait_factor`              | `1.0`               | Controls how much the number of finished players increases the changelevel timer speed. `1.0` means full, `0` means none (timer will run full length).                 | Base                   |
| `sourcecoop_homemap`                      |                   | The map to return to after finishing a campaign/map.                                                                                                               | Base                   |
| `sourcecoop_end_wait_display_mode`        | `1`                 | Sets which method to show countdown. `0` is panel, `1` is hud text.                                                                                                    | Base                   |
| `sourcecoop_validate_steamids`            | `0`                 | Validate players' Steam IDs? Increases security at the cost of some functionality breakage when Steam goes down.                                                   | Base                   |
| `sourcecoop_default_config`               |                   | Default edt file, relative to game folder. This file is copied when starting a map with missing config as `<sourcecoop_default_config_dest>/<mapname>.edt`.         | Base                   |
| `sourcecoop_default_config_dest`          | `"maps"`              | Destination folder for `sourcecoop_default_config`, relative to the game folder. Should be one of the edt scan paths!                                              | Base                   |
| `sc_killfeed`                             | `2`                 | Controls the display of the kill feed (`0`: disabled, `1`: chat, `2`: hud). If set to `2`, then the plugin will spawn in fake clients to display on the kill feed.       | Base                   |
| `sc_killfeed_player_kills`                | `2`                 | Controls display of player kills on the kill feed (`0`: hide, `1`: players, `2`: entities).                                                                              | Base                   |
| `sc_killfeed_entity_kills`                | `2`                 | Controls display of entity kills on the kill feed (`0`: hide, `1`: players, `2`: entities).                                                                              | Base                   |
| `sc_killfeed_suicides`                    | `2`                 | Controls display of suicides on the kill feed (`0`: hide, `1`: players, `2`: entities).                                                                                  | Base                   |
| `sourcecoop_survival_mode`                | `0`                 | Sets survival mode. `0` = off. `1` will respawn players if all are dead, `2` will restart the map.                                                                       | Base                   |
| `sourcecoop_survival_respawn`             | `1`                 | Whether to respawn dead players at checkpoints.                                                                                                                    | Base                   |
| `sourcecoop_survival_spawn_timeout`       | `-1`                | Number of seconds after the map starts (after initial timer) to allow spawning in, or `-1` for no time limit.                                                        | Base                   |
| `sourcecoop_difficulty`                   | `0`                 | Sets the difficulty - from `0` (base difficulty) and up.                                                                                                             | Difficulty             |
| `sourcecoop_difficulty_auto`              | `2`                 | Sets automatic difficulty mode. `-1` disables. `0` balances difficulty between min and max convars. Values above 0 set the difficulty increment per player, ignoring the min and max cvars. | Difficulty             |
| `sourcecoop_difficulty_auto_min`          | `1`                 | When automatic difficulty mode is set to `0`, this is the difficulty at `1` player.                                                                                    | Difficulty             |
| `sourcecoop_difficulty_auto_max`          | `20`                | When automatic difficulty mode is set to `0`, this is the difficulty at max players.                                                                                 | Difficulty             |
| `sourcecoop_difficulty_announce`          | `1`                 | Toggles announcing changes in difficulty.                                                                                                                          | Difficulty             |
| `sourcecoop_difficulty_ignoredmgto`       |  | List of classnames where player->npc damage is exempt from difficulty scaling. Separated by semicolon.                                                             | Difficulty             |
| `sourcecoop_difficulty_ignoredmgfrom`     |  | List of classnames where npc->player damage is exempt from difficulty scaling. Separated by semicolon.                                                             | Difficulty             |
| `sourcecoop_earbleed_default`             | `0`                 | Sets the default setting of the earbleed player preference.                                                                                                        | Earbleed               |
| `sourcecoop_fpd_fade_ms`                  | `1500`              | Duration in milliseconds to fade the first-person death screen to black. `0` to disable.                                                                             | First Person Death     |
| `sourcecoop_fpd_player_toggle`            | `1`                 | Enable players to choose death camera option regardless of server/map settings.                                                                                    | First Person Death     |
| `sourcecoop_logo_material`                |  | The material used for the landing screen.                                                                                                                        | Landing Screen         |
| `sourcecoop_revive_time`                  | `4.0`               | Sets time that you have to hold `E` to revive.                                                                                                                        | Revive                 |
| `sourcecoop_revive_score`                 | `1`                 | Sets score to give for reviving a player.                                                                                                                           | Revive                 |
| `sourcecoop_revive_messages`              | `0`                 | Shows messages such as `"You have started reviving x."`                                                                                                               | Revive                 |
| `sourcecoop_revive_ragdoll_effects_timer` | `4.0`               | Delay for applying ragdoll highlighting effects. `-1` to disable all ragdoll effects.                                                                                 | Revive                 |
| `sourcecoop_revive_ragdoll_particle`      | `1`                 | Whether to spawn a particle inside player ragdolls to improve their visibility.                                                                                     | Revive                 |
| `sourcecoop_revive_ragdoll_blink`         | `1`                 | Whether to blink player ragdolls to improve their visibility.                                                                                                       | Revive                 |
| `sourcecoop_revive_in_classic_mode`       | `1`                 | Whether to allow reviving in non-survival mode.                                                                                                                     | Revive                 |
| `sourcecoop_killfeed_default`             | `0`                 | Sets the default setting of the killfeed player preference.                                                                                                         | Scoring                |
| `sourcecoop_thirdperson_enabled`          | `1`                 | Is thirdperson enabled?                                                                                                                                             | Thirdperson            |
| `sourcecoop_next_stuck`                   | `60.0`              | Prevents using stuck for this many seconds after using.                                                                                                             | Unstuck                |
| `sourcecoop_voting_autoreload`            | `1`                 | Sets whether to reload all votemap menu entries on mapchange, which can prolong map loading times.                                                                  | Voting                 |
| `sourcecoop_voting_skipintro`             | `1`                 | Allow skip intro voting?                                                                                                                                           | Voting                 |
| `sourcecoop_voting_restartmap`            | `1`                 | Allow restart map voting?                                                                                                                                          | Voting                 |
| `sourcecoop_voting_changemap`             | `1`                 | Allow change map voting?                                                                                                                                           | Voting                 |
| `sourcecoop_voting_survival`              | `2`                 | Allow survival mode voting? Use one of the values from `sourcecoop_survival_mode` to select the mode to vote for.                                                   | Voting                 |
| `sourcecoop_workshop_message`             | `"Missing map! Subscribe to SourceCoop workshop collection + restart game"` | The message to display to players missing workshop maps. Supported placeholders: `{BSPNAME}`.                  | Workshop               |

#### Toggleable Features

ConVar: `sc_ft <FEATURE> <0 or 1>`

> #### ⚠️ Gameplay Impact
>
> It is recommended to leave these features at the default values as these are configured per map within EDT configurations. __Modifying feature values could negatively impact the gameplay experience__.

| **Feature**                     | **Description**                                                                                                                    |
|-----------------------------|--------------------------------------------------------------------------------------------------------------------------------|
| `FIRSTPERSON_DEATHCAM`        | Enables the first-person death camera.                                                                                          |
| `HEV_SOUNDS`                  | Enables HEV sounds.                                                                                                         |
| `INSTANCE_ITEMS`              | Instances pickup items and weapons for each player. Instanced items disappear once picked up and 'respawn' along with the player.        |
| `INSTANCE_ITEMS_NORESET`      | If enabled, items will not 'respawn' picked up items after death.                                                                       |
| `KEEP_EQUIPMENT`              | Makes players spawn with previously picked up equipment (suit, weapons). Global for all players.                               |
| `STRIP_DEFAULT_EQUIPMENT`     | Removes default multiplayer equipment.                                                                                         |
| `STRIP_DEFAULT_EQUIPMENT_KEEPSUIT` |                                                                                                                                  |
| `DISABLE_CANISTER_DROPS`      | Disables item drops when players die in multiplayer.                                                                             |
| `NO_TELEFRAGGING`             | Prevents teleporting props and players from slaying other players.                                                             |
| `NOBLOCK`                     | Prevents player-on-player collisions. (This feature requires `mp_teamplay 1` to fix smoothness issues.)                                                        |
| `SHOW_WELCOME_MESSAGE`        | Shows players a greeting message with basic plugin info.                                                                       |
| `AUTODETECT_MAP_END`          | Detects commonly used commands for ending singleplayer maps from `point_clientcommand` and `point_servercommand` entities and changes the map. At first, this feature checks `sourcecoop_homemap` is set (see below), then checks if `nextmap` is set. If none are set, the map is not changed. Recommended to keep enabled. |
| `CHANGELEVEL_FX`              | Show visual effects (spawn particles) at level change locations.                                                               |
| `TRANSFER_PLAYER_STATE`       | Enables player persistence through level changes. Currently, players will carry over their health, armor and equipment for the first spawn point (checkpoint) in the map. Afterwards, the default map equipment is used. |

<a id="contributing"></a>
## 🌱 Contributing

__If you are looking to help with the development of the project__, we are always looking for more help! Heres some ways you can help:

- Reporting inconsistencies and bugs
- Debugging and tracking down issues
- Adding features
- Adding new configurations for campaigns and maps

__If you are interested in helping us__, contact us on [Discord](https://discord.gg/Fh77rxQaEB) or create a pull request.

<a id="credits"></a>
## 📸 Credits

### 🙏 Contributors

- __ampreeT__ :: [Steam](https://steamcommunity.com/id/ampreeT) | [GitHub](https://github.com/ampreeT) :: programming, reverse engineering, map editing
- __kasull__ :: [Steam](https://steamcommunity.com/id/kasull/) | [GitHub](https://github.com/kasullian) :: programming, reverse engineering
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
