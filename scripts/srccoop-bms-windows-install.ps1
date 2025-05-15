$ErrorActionPreference = "Stop"

# Install SteamCMD.
New-Item -Name "SteamCMD" -ItemType Directory -Force
Invoke-WebRequest -Uri "https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip" -OutFile ".tmp.zip"
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./SteamCMD" -Force
Remove-Item -Path ".tmp.zip" -Force

# Install Black Mesa Dedicated Server.
# TODO: Check error code of process and early exit on failure.
New-Item -Name "Black Mesa Dedicated Server" -ItemType Directory -Force
Start-Process -FilePath "./SteamCMD/steamcmd.exe" -ArgumentList '+force_install_dir "../Black Mesa Dedicated Server"', "+login anonymous", "+app_update 346680 validate", "+quit" -Wait -NoNewWindow

# Install the latest version of Metamod Source.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri (((Invoke-WebRequest -Uri "https://www.sourcemm.net/downloads.php").Links | Where-Object { $_.href -like "*-windows.zip" }).href | Select-Object -First 1)
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms" -Force
Remove-Item -Path ".tmp.zip" -Force

# Commented out temporarily due to needing SourceMod version 7163.
# The newer SourceMod versions have a regression within their detour hooking utilities.
## Install the latest version of SourceMod.
#Invoke-WebRequest -OutFile ".tmp.zip" -Uri (((Invoke-WebRequest -Uri "https://www.sourcemod.net/downloads.php").Links | Where-Object { $_.href -like "*-windows.zip" }).href | Select-Object -First 1)
#Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms"
#Remove-Item -Path ".tmp.zip" -Force

# Install SourceMod version 7163.
# The newer SourceMod versions have a regression within their detour hooking utilities.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri "https://sm.alliedmods.net/smdrop/1.12/sourcemod-1.12.0-git7163-windows.zip"
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms" -Force
Remove-Item -Path ".tmp.zip" -Force

# Install the latest version of Accelerator.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri ("https://builds.limetech.io/" + ((Invoke-WebRequest -Uri "https://builds.limetech.io/?p=accelerator").Links | Where-Object { $_.href -like "*-windows.zip" } | Select-Object -First 1).href)
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms" -Force
Remove-Item -Path ".tmp.zip" -Force

# Install the latest release of SourceCoop.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri ((Invoke-WebRequest "https://api.github.com/repos/ampreeT/SourceCoop/releases/latest" | ConvertFrom-Json).assets | Where-Object { $_.name -like "*-bms.zip" } | Select-Object -First 1).browser_download_url
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms" -Force
Remove-Item -Path ".tmp.zip" -Force

## OPTIONAL: Remove textures to save ~9 GB.
## On the server, materials are needed but textures are not.
## If the server ever needs to be updated, these files will be redownloaded again.
#Remove-Item -Path "./Black Mesa Dedicated Server/bms/bms_textures*" -Force
#Remove-Item -Path "./Black Mesa Dedicated Server/hl2/hl2_textures*" -Force

# Commented out due to stdout being malformed.
## Install Alien Swarm: Reactive Drop Dedicated Server.
## The default `srcds.exe` will always spawn a new console while the `srcds_console.exe` that is shipped with Alien Swarm: Reactive Drop does not.
## https://github.com/Facepunch/garrysmod-issues/issues/3771#issuecomment-1507467323
#New-Item -Name "Alien Swarm Reactive Drop" -ItemType Directory
#Start-Process -FilePath "./SteamCMD/steamcmd.exe" -ArgumentList '+force_install_dir "../Alien Swarm Reactive Drop"', "+login anonymous", "+app_update 582400", "+quit" -Wait -NoNewWindow
#Copy-Item "./Alien Swarm Reactive Drop/srcds_console.exe" -Destination "./Black Mesa Dedicated Server" -Force
#Remove-Item -Path "./Alien Swarm Reactive Drop" -Force -Recurse

# Create a script to run and automatically restart the server.
$srcds_coop_bat = @'
@echo off
echo [%time%] Starting Black Mesa Dedicated Server..

:loop
start /WAIT /B srcds.exe -nomessagebox -console -game bms -ip 0.0.0.0 +maxplayers 32 +mp_teamplay 1 +map bm_c0a0a
echo [%time%] Restarting Black Mesa Dedicated Server..
timeout 5 > nul
goto loop
'@
Set-Content -Path "./Black Mesa Dedicated Server/srcds_coop.bat" -Value $srcds_coop_bat

# Create a `mapcycle.txt` consisting of the starting chapter maps.
$map_cycle_txt = @'
bm_c0a0a
bm_c1a0a
bm_c1a1a
bm_c1a2a
bm_c1a3a
bm_c1a4a
bm_c2a1a
bm_c2a1a
bm_c2a2a
bm_c2a3a
bm_c2a4a
bm_c2a4e
bm_c2a5a
bm_c3a1a
bm_c3a2a
bm_c4a1a
bm_c4a2a
bm_c4a3a
'@
Set-Content -Path "./Black Mesa Dedicated Server/bms/mapcycle.txt" -Value $map_cycle_txt

# Create a `server.cfg` with ideal default settings.
$server_cfg = @'
// SourceCoop settings.
mp_fraglimit 0      // Prevents the match from ending when a player has a high enough score.
mp_teamplay 1       // Enables the scientist team.
mp_friendlyfire 0   // Disables friendly fire.
mp_forcerespawn 1   // Forces the player to respawn.

// Add your settings below.
hostname "Black Mesa: Cooperative"  // The name of the server.
sv_password ""                      // Sets a server password for locking the server.
rcon_password ""                    // Sets a RCON password for accessing adminstrative features. This is not recommended and SourceMod should be used instead.
'@
Set-Content -Path "./Black Mesa Dedicated Server/bms/cfg/server.cfg" -Value $server_cfg

# Empty the default `autoexec.cfg` as it does nothing for the server.
Set-Content -Path "./Black Mesa Dedicated Server/bms/cfg/autoexec.cfg" -Value ""

# Create an empty `userconfig.cfg` to suppress console warnings.
Set-Content -Path "./Black Mesa Dedicated Server/bms/cfg/userconfig.cfg" -Value ""