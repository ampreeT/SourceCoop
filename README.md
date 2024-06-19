# SourceCoop

[![CI](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml/badge.svg)](https://github.com/ampreeT/SourceCoop/actions/workflows/plugin.yml)
[![GitHub Release](https://img.shields.io/github/v/release/ampreeT/SourceCoop?style=flat&label=Release&labelColor=%232C3137&color=%23EB551B)](https://github.com/ampreeT/SourceCoop/releases/latest)
[![Discord](https://img.shields.io/discord/973591793117564988.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/Fh77rxQaEB)

| [Installation Guide](#installation-guide) - [Campaign Support](#campaign-support) - [Contributing](#contributing) - [Credits](#credits)
:--: |
| [Features & Configuration](https://github.com/ampreeT/SourceCoop/wiki/Features-&-Configuration) - [Server Running Tips](https://github.com/ampreeT/SourceCoop/wiki/Server-running-tips) - [Public Servers](https://github.com/ampreeT/SourceCoop/wiki/Public-Servers) |
|  [Developing](https://github.com/ampreeT/SourceCoop/wiki/Developing) - [EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format) - [Authoring Maps](https://github.com/ampreeT/SourceCoop/wiki/Authoring-maps-for-SourceCoop) |

SourceCoop is a cooperative mod for Source Engine games enabling singleplayer campaigns to be played together. It currently supports [Black Mesa](https://store.steampowered.com/app/362890/Black_Mesa/) and [Half-Life 2: Deathmatch](https://store.steampowered.com/app/362890/Black_Mesa/).

- Supports campaigns.
- Restores singleplayer functionality.
- Handles world, player and equipment persistence between maps.
- Easy to use optional addons.
- Easy to convert maps from singleplayer into cooperative.

## Installation Guide

__If you are someone who is looking to play on a server__, then you are already set up and ready to play! Cooperative servers can be found in the server browser just like any other server.

__If you are a server operator who is looking to host your own cooperative server__, then follow the instructions below:

- Install [Metamod:Source](https://www.sourcemm.net/downloads.php?branch=stable) (latest tested build ➤ __1148__)
- Install [SourceMod](https://www.sourcemod.net/downloads.php?branch=stable) (latest tested build ➤ __6960__)
- Install [the latest release of SourceCoop](https://github.com/ampreeT/SourceCoop/releases).

A step-by-step guide for Black Mesa is also available on [Steam](https://steamcommunity.com/sharedfiles/filedetails/?id=2200247356).

## Campaign Support

### Black Mesa

- [Main Campaign](https://store.steampowered.com/app/362890/Black_Mesa/)
- [Stojkeholm](https://steamcommunity.com/sharedfiles/filedetails/?id=2320533262)
- [Emergency 17](https://steamcommunity.com/sharedfiles/filedetails/?id=934371395)

### Custom Configurations

SourceCoop is __designed to support configurations of singleplayer maps__ without going through the process of decompiling and redistributing map files. Configurations can be created for maps that SourceCoop already does not handle. __If you are interested in creating your own configuration__, read more on [EDT Map Script Format](https://github.com/ampreeT/SourceCoop/wiki/EDT---Map-script-format).

## Contributing

__If you are looking to help with the development of the project__, we are always looking for more help! Heres some ways you can help:

- Debugging and tracking down issues
- Adding features
- Adding new configurations for campaigns and maps

__If you are interested in helping us__, contact us on [Discord](https://discord.gg/Fh77rxQaEB) or create a pull request.

## Dependencies

- [SourceMod](https://github.com/alliedmodders/sourcemod)
- [SourceScramble](https://github.com/nosoop/SMExt-SourceScramble)
- [stocksoup](https://github.com/nosoop/stocksoup)
- [sm-logdebug](https://github.com/Alienmario/sm-logdebug)
- [smlib](https://github.com/bcserv/smlib/tree/transitional_syntax)

# Credits

- __ampreeT__ :: [Steam](https://steamcommunity.com/id/ampreeT) | [GitHub](https://github.com/ampreeT) :: programming, reverse engineering, map editing
- __kasull__ :: [Steam](https://steamcommunity.com/id/kasull/) | [GitHub](https://github.com/kasullian) :: programming, reverse engineering, trailer production
- __Alienmario__ :: [Steam](https://steamcommunity.com/id/4oM0/) | [GitHub](https://github.com/Alienmario) :: programming, reverse engineering, map editing
- __Balimbanana__ :: [Steam](https://steamcommunity.com/id/Balimbanana/) | [GitHub](https://github.com/Balimbanana) :: programming, reverse engineering
- __Rock__ :: [Steam](https://steamcommunity.com/id/Rock48/) | [GitHub](https://github.com/Rock48) :: programming, map editing
- __Krozis Kane__ :: [Steam](https://steamcommunity.com/id/Krozis_Kane/) | [GitHub](https://github.com/KrozisKane) :: map editing
- __ReservedRegister__ :: [GitHub](https://github.com/ReservedRegister) :: reverse engineering
- __raicovx__ :: [Github](https://github.com/raicovx) :: Equipment Persistence
- __Jimmy-Baby__ :: [Github](https://github.com/Jimmy-Baby) :: Damage Effects Addon
- __Removiekeen__ :: [Steam](https://steamcommunity.com/profiles/76561198804614641/) :: Logo Design
- __yarik2720__ :: [Github](https://github.com/yarik2720) :: CI, Russian Translation