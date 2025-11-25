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

__If you are a server operator who is looking to host your own cooperative server__, then follow any installation method below and forward the server ports:

### 🔨 Server Installation Methods

<details>
<summary><b>Option 1: Installation by SourceCoop Launcher</b></summary>
<br>

The SourceCoop Launcher provides an automated way to set up your cooperative server. It simplifies the installation process by handling the necessary files and configurations.

- Download the latest release of the [SCLauncher](https://github.com/Alienmario/SCLauncher/releases/).
- Run the executable to proceed with the automated server setup.
- Once the launcher completes its process, ensure you have [forwarded the server ports](#port-forwarding) to allow players to join.

</details>

<details>
<summary><b>Option 2: Script Installation</b></summary>
<br>

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
    Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process; ./srccoop-bms-windows-install.ps1
    ```
    ##### Linux Bash Terminal
    ```bash
    chmod +x "./srccoop-bms-linux-install.sh"; ./srccoop-bms-linux-install.sh
    ```

- After the installation process is complete, the server can be started by running the following commands. [Make sure that the server ports are forwarded so players to be able to join the server!](#port-forwarding)
    ##### Windows PowerShell Terminal
    ```powershell
    cd "Black Mesa Dedicated Server"; ./srcds_coop.bat
    ```
    ##### Linux Bash Terminal
    ```bash
    cd "Black Mesa Dedicated Server"; ./srcds_coop.sh
    ```

</details>

<details>
<summary><b>Option 3: Manual Installation</b></summary>
<br>

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
- Install [SourceMod](https://www.sourcemod.net/downloads.php?branch=stable) (latest tested build ➤ __7163__) onto the server.
- Install [the latest SourceCoop release](https://github.com/ampreeT/SourceCoop/releases) onto the server.
- [Forward the server ports](#port-forwarding).

</details>

A step-by-step guide for Black Mesa is also available on <a href="https://steamcommunity.com/sharedfiles/filedetails/?id=2200247356"><img src="images/icon-steam-small.png" height="16px"> Steam</a>.

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
- [Stadium](https://steamcommunity.com/sharedfiles/filedetails/?id=3164506630)
- [Further Data](https://steamcommunity.com/sharedfiles/filedetails/?id=2316239201)
- [Meltdown](https://steamcommunity.com/sharedfiles/filedetails/?id=2639914442)
- [Quarantine](https://steamcommunity.com/sharedfiles/filedetails/?id=798988742)
- [Escapade](https://steamcommunity.com/sharedfiles/filedetails/?id=873700194)
- [Stalwart XT](https://steamcommunity.com/sharedfiles/filedetails/?id=1850064669)
- [Uncharted Territory](https://steamcommunity.com/sharedfiles/filedetails/?id=1205460043)
- [Superbus Via Inscientiae](https://steamcommunity.com/sharedfiles/filedetails/?id=1086124981)
- [Pipeline](https://steamcommunity.com/sharedfiles/filedetails/?id=2775454271)
- [Cold Storage](https://steamcommunity.com/sharedfiles/filedetails/?id=1377042249)
- [Going Around](https://steamcommunity.com/sharedfiles/filedetails/?id=2344380359)
- [Surface Contamination](https://steamcommunity.com/sharedfiles/filedetails/?id=2904429204)
- [Access Point](https://steamcommunity.com/sharedfiles/filedetails/?id=2161275016)

SourceCoop __allows single-player map configurations__ without decompiling and redistributing; __learn more about creating your own__ on the [EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format).

If you have already created native cooperative support for your map by including a EDT file, please make a pull request onto the `README.md` with your Steam Workshop item link so we can showcase it off!

<a id="configuration"></a>
## ⚙️ Configuration

<details>
<summary><b>Commands</b></summary>
<br>

All players that are connected to the server are able to execute the following commands:

* **`sm_coopmenu`**: Displays the coop menu
* **`sizeup`**: Displays the coop menu
* **`sm_thirdperson`**: Type `!thirdperson` to go into thirdperson mode
* **`sm_firstperson`**: Type `!firstperson` to exit thirdperson mode
* **`stuck`**: Unstuck command
* **`unstuck`**: Unstuck command
* **`sm_skipintro`**: Starts a skip intro vote
* **`sm_restartmap`**: Starts a restart map vote
* **`sm_changemap`**: Shows a menu for changing maps
* **`sm_survival`**: Starts a survival vote

</details>

<details>
<summary><b>Admin Commands</b></summary>
<br>

The server console and [admins configured within SourceMod](https://wiki.alliedmods.net/Adding_Admins_(SourceMod)) are able to execute the following commands:

* **`sc_save`**: Exports last saved player equipment state to a file
* **`sc_load`**: Imports saved data from file and attempts to equip each player
* **`sc_clear`**: Clear persisted equipment and equip players with the map defaults
* **`sourcecoop_dump`**: Command for dumping map entities to a file
* **`sc_dump`**: Command for dumping map entities to a file
* **`sc_mkconfigs`**: Creates default edt configs for all maps in the maps directory which are missing one
* **`sc_revive`**: Force respawn player
* **`sc_reload_maps`**: Reloads all entries in the votemap menu from storage

</details>

<details>
<summary><b>ConVars</b></summary>
<br>

The server console and [admins configured within SourceMod](https://wiki.alliedmods.net/Adding_Admins_(SourceMod)) are able to execute the following ConVars:

* **`sourcecoop_version`**: The version of the SourceCoop mod.
* **`sourcecoop_respawntime`**: `2.0` - Sets player respawn time in seconds.
* **`sourcecoop_start_wait_period`**: `15.0` - The max number of seconds to wait since the first player spawned in to start the map.
* **`sourcecoop_start_wait_mode`**: `2` - 0 = The timer is not skipped (exceptions are maps without an intro_type or delayed outputs set). 1 = The timer is skipped when all players enter the game. 2 = The timer is skipped when player count matches the previous map's player count.
* **`sourcecoop_end_wait_period`**: `60.0` - The max number of seconds to wait since the first player triggered a changelevel. The timer speed increases each time a new player finishes the level.
* **`sourcecoop_end_wait_factor`**: `1.0` - Controls how much the number of finished players increases the changelevel timer speed. `1.0` means full, `0` means none (timer will run full length).
* **`sourcecoop_homemap`**: The map to return to after finishing a campaign/map.
* **`sourcecoop_end_wait_display_mode`**: `1` - Sets which method to show countdown. `0` is panel, `1` is hud text.
* **`sourcecoop_validate_steamids`**: `0` - Validate players' Steam IDs? Increases security at the cost of some functionality breakage when Steam goes down.
* **`sourcecoop_default_config`**: Default edt file, relative to game folder. This file is copied when starting a map with missing config as `<sourcecoop_default_config_dest>/<mapname>.edt`.
* **`sourcecoop_default_config_dest`**: `"maps"` - Destination folder for `sourcecoop_default_config`, relative to the game folder. Should be one of the edt scan paths!
* **`sc_killfeed`**: `2` - Controls the display of the kill feed (`0`: disabled, `1`: chat, `2`: hud). If set to `2`, then the plugin will spawn in fake clients to display on the kill feed.
* **`sc_killfeed_player_kills`**: `2` - Controls display of player kills on the kill feed (`0`: hide, `1`: players, `2`: entities).
* **`sc_killfeed_entity_kills`**: `2` - Controls display of entity kills on the kill feed (`0`: hide, `1`: players, `2`: entities).
* **`sc_killfeed_suicides`**: `2` - Controls display of suicides on the kill feed (`0`: hide, `1`: players, `2`: entities).
* **`sourcecoop_survival_mode`**: `0` - Sets survival mode. `0` = off. `1` will respawn players if all are dead, `2` will restart the map.
* **`sourcecoop_survival_respawn`**: `1` - Whether to respawn dead players at checkpoints.
* **`sourcecoop_survival_spawn_timeout`**: `-1` - Number of seconds after the map starts (after initial timer) to allow spawning in, or `-1` for no time limit.
* **`sourcecoop_difficulty`**: `0` - Sets the difficulty - from `0` (base difficulty) and up.
* **`sourcecoop_difficulty_auto`**: `2` - Sets automatic difficulty mode. `-1` disables. `0` balances difficulty between min and max convars. Values above 0 set the difficulty increment per player, ignoring the min and max cvars.
* **`sourcecoop_difficulty_auto_min`**: `1` - When automatic difficulty mode is set to `0`, this is the difficulty at `1` player.
* **`sourcecoop_difficulty_auto_max`**: `20` - When automatic difficulty mode is set to `0`, this is the difficulty at max players.
* **`sourcecoop_difficulty_announce`**: `1` - Toggles announcing changes in difficulty.
* **`sourcecoop_difficulty_ignoredmgto`**: List of classnames where player->npc damage is exempt from difficulty scaling. Separated by semicolon.
* **`sourcecoop_difficulty_ignoredmgfrom`**: List of classnames where npc->player damage is exempt from difficulty scaling. Separated by semicolon.
* **`sourcecoop_earbleed_default`**: `0` - Sets the default setting of the earbleed player preference.
* **`sourcecoop_fpd_fade_ms`**: `1500` - Duration in milliseconds to fade the first-person death screen to black. `0` to disable.
* **`sourcecoop_fpd_player_toggle`**: `1` - Enable players to choose death camera option regardless of server/map settings.
* **`sourcecoop_logo_material`**: The material used for the landing screen.
* **`sourcecoop_revive_time`**: `4.0` - Sets time that you have to hold `E` to revive.
* **`sourcecoop_revive_score`**: `1` - Sets score to give for reviving a player.
* **`sourcecoop_revive_messages`**: `0` - Shows messages such as `"You have started reviving x."`
* **`sourcecoop_revive_ragdoll_effects_timer`**: `4.0` - Delay for applying ragdoll highlighting effects. `-1` to disable all ragdoll effects.
* **`sourcecoop_revive_ragdoll_particle`**: `1` - Whether to spawn a particle inside player ragdolls to improve their visibility.
* **`sourcecoop_revive_ragdoll_blink`**: `1` - Whether to blink player ragdolls to improve their visibility.
* **`sourcecoop_revive_in_classic_mode`**: `1` - Whether to allow reviving in non-survival mode.
* **`sourcecoop_killfeed_default`**: `0` - Sets the default setting of the killfeed player preference.
* **`sourcecoop_thirdperson_enabled`**: `1` - Is thirdperson enabled?
* **`sourcecoop_next_stuck`**: `60.0` - Prevents using stuck for this many seconds after using.
* **`sourcecoop_voting_autoreload`**: `1` - Sets whether to reload all votemap menu entries on mapchange, which can prolong map loading times.
* **`sourcecoop_voting_skipintro`**: `1` - Allow skip intro voting?
* **`sourcecoop_voting_restartmap`**: `1` - Allow restart map voting?
* **`sourcecoop_voting_changemap`**: `1` - Allow change map voting?
* **`sourcecoop_voting_survival`**: `2` - Allow survival mode voting? Use one of the values from `sourcecoop_survival_mode` to select the mode to vote for.
* **`sourcecoop_workshop_message`**: `"Missing map! Subscribe to SourceCoop workshop collection + restart game"` - The message to display to players missing workshop maps. Supported placeholders: `{BSPNAME}`.

</details>

<details>
<summary><b>Toggleable Features</b></summary>
<br>

ConVar: `sc_ft <FEATURE> <0 or 1>`

> #### ⚠️ Gameplay Impact
>
> It is recommended to leave these features at the default values as these are configured per map within EDT configurations. __Modifying feature values could negatively impact the gameplay experience__.

* **`FIRSTPERSON_DEATHCAM`**: Enables the first-person death camera.
* **`HEV_SOUNDS`**: Enables HEV sounds.
* **`INSTANCE_ITEMS`**: Instances pickup items and weapons for each player. Instanced items disappear once picked up and 'respawn' along with the player.
* **`INSTANCE_ITEMS_NORESET`**: If enabled, picked up items will not 'respawn' after death.
* **`KEEP_EQUIPMENT`**: Makes players spawn with previously picked up equipment (suit, weapons). Global for all players.
* **`DISABLE_CANISTER_DROPS`**: Disables item drops when players die in multiplayer.
* **`NOBLOCK`**: Prevents player-on-player collisions. (This feature requires `mp_teamplay 1` to fix smoothness issues.)
* **`SHOW_WELCOME_MESSAGE`**: Shows players a greeting message with basic plugin info.
* **`AUTODETECT_MAP_END`**: Detects commonly used commands for ending singleplayer maps from `point_clientcommand` and `point_servercommand` entities and changes the map. At first, this feature checks `sourcecoop_homemap` is set (see below), then checks if `nextmap` is set. If none are set, the map is not changed. Recommended to keep enabled.
* **`CHANGELEVEL_FX`**: Show visual effects (spawn particles) at level change locations.
* **`TRANSFER_PLAYER_STATE`**: Enables player persistence through level changes. Currently, players will carry over their health, armor and equipment for the first spawn point (checkpoint) in the map. Afterwards, the default map equipment is used.
* **`SP_WEAPONS`**: Sets whether to use the singleplayer variants of weapons.

</details>

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
