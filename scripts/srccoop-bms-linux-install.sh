#!/bin/bash -e

# Install packages for SteamCMD and Black Mesa Dedicated Server.
sudo dpkg --add-architecture i386
sudo apt-get update -y
sudo apt-get install unzip -y
sudo apt-get install wget -y
# Required for running SteamCMD.
sudo apt-get install lib32gcc-s1 -y
# Required for running Black Mesa Dedicated Server.
sudo apt-get install lib32stdc++6 -y
sudo apt-get install libncurses5 libncurses5:i386 -y

# Install SteamCMD.
mkdir -p "./SteamCMD"
wget "http://media.steampowered.com/client/steamcmd_linux.tar.gz" -O ".tmp.tar.gz"
tar -xf ".tmp.tar.gz" -C "./SteamCMD"
rm ".tmp.tar.gz"

# Install Black Mesa Dedicated Server.
# TODO: Check error code of process and early exit on failure.
mkdir -p "./Black Mesa Dedicated Server"
echo -e "force_install_dir \"../Black Mesa Dedicated Server\"\nlogin anonymous\napp_update 346680\nquit" | ./SteamCMD/steamcmd.sh

# Install the latest version of Metamod Source.
wget $(wget -qO- "https://www.sourcemm.net/downloads.php" | grep "<a class='quick-download download-link'" | grep -m1 "linux.tar.gz" | sed -n 's/.*href='\''//; s/'\''.*//p') -O ".tmp.tar.gz"
tar -xf ".tmp.tar.gz" -C "./Black Mesa Dedicated Server/bms"
rm ".tmp.tar.gz"

# Install the latest version of Metamod Source.
wget $(wget -qO- "https://www.sourcemod.net/downloads.php" | grep "<a class='quick-download download-link'" | grep -m1 "linux.tar.gz" | sed -n 's/.*href='\''//; s/'\''.*//p') -O ".tmp.tar.gz"
tar -xf ".tmp.tar.gz" -C "./Black Mesa Dedicated Server/bms"
rm ".tmp.tar.gz"

# Install the latest version of Accelerator.
wget "https://builds.limetech.io/"$(wget -qO- "https://builds.limetech.io/?p=accelerator" | grep -m1 "linux.zip" | cut -d '"' -f2) -O ".tmp.zip"
unzip ".tmp.zip" -d "./Black Mesa Dedicated Server/bms"
rm ".tmp.zip"

# Install the latest release of SourceCoop.
wget $(wget -qO- "https://api.github.com/repos/ampreeT/SourceCoop/releases/latest" | grep "browser_download_url" | grep -m1 "bms.zip" | cut -d '"' -f 4) -O ".tmp.zip"
unzip ".tmp.zip" -d "./Black Mesa Dedicated Server/bms"
rm ".tmp.zip"

## OPTIONAL: Remove textures to save ~9 GB.
## On the server, materials are needed but textures are not.
## If the server ever needs to be updated, these files will be redownloaded again.
#rm ./Black\ Mesa\ Dedicated\ Server/bms/bms_textures*
#rm ./Black\ Mesa\ Dedicated\ Server/hl2/hl2_textures*

# Create a script to run and automatically restart the server.
srcds_coop_bash=$(cat << EOF
#!/bin/bash

./srcds_run -console -insecure -game bms +maxplayers 32 +map bm_c0a0a
EOF
)
echo "$srcds_coop_bash" > "./Black Mesa Dedicated Server/srcds_coop.sh"
chmod +x "./Black Mesa Dedicated Server/srcds_coop.sh"

# Create a `mapcycle.txt` consisting of the starting chapter maps.
map_cycle_txt=$(cat << EOF
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
EOF
)
echo "$map_cycle_txt" > "./Black Mesa Dedicated Server/bms/mapcycle.txt"

# Create a `server.cfg` with ideal default settings.
server_cfg=$(cat << EOF
// SourceCoop settings.
mp_timelimit 0      // Prevents map switch from round timers.
mp_fraglimit 0      // Prevents the match from ending when a player has a high enough score.
mp_teamplay 1       // Enables the scientist team.
mp_friendlyfire 0   // Disables friendly fire.
mp_forcerespawn 1   // Forces the player to respawn.

// Add your settings below.
hostname "Black Mesa: Cooperative"  // The name of the server.
sv_password ""                      // Sets a server password for locking the server.
rcon_password ""                    // Sets a RCON password for accessing adminstrative features. This is not recommended and SourceMod should be used instead.
EOF
)
echo "$server_cfg" > "./Black Mesa Dedicated Server/bms/cfg/server.cfg"

# Empty the default `autoexec.cfg` as it does nothing for the server.
> "./Black Mesa Dedicated Server/bms/cfg/autoexec.cfg"

# Create an empty `userconfig.cfg` to suppress console warnings.
> "./Black Mesa Dedicated Server/bms/cfg/userconfig.cfg"