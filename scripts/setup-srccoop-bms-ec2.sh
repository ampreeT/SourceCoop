#!/bin/bash

# `setup-srccoop-bms-ec2.sh` can be used as user data to deploy to EC2 instances.
#
# Steps:
#
# - Operating System: Amazon Linux 2 AMI
# - Machine: t3a.micro
# - Security Group: TODO
# - Storage: 35 GB, Root Volume: gp3
# - Insert this file as user data
# - Launch instance
# - Connect to the server using `screen -r srcds`.
#
# TODO:
#
# - Enable logging on SRCDS
# - Enable logging on SourceMod

HOME_USER_INSTALL="ec2-user"

# Exit early on error.
set -x -e

sudo yum update -y
# Used for running 32-bit SteamCMD.
sudo yum install glibc.i686 -y
# Used for running the dedicated server.
sudo yum install libstdc++.so.6 -y
# Used for running the dedicated server. 
sudo yum install ncurses-compat-libs.i686 -y

su "$HOME_USER_INSTALL" -c bash << 'EOF'
# Login into user account.
function download_extract() {
    wget "$1" -O "contents.temp"
    if [[ "$1" == *.tar.gz ]]; then
        tar zxf "contents.temp"
    elif [[ "$1" == *.zip ]]; then
        unzip "contents.temp"
    fi
    rm "contents.temp"
}

# Exit early on error.
set -x -e

# Create `SteamCMD` directory.
cd ~
mkdir -p "SteamCMD"
cd "SteamCMD"

# Download SteamCMD.
download_extract "http://media.steampowered.com/client/steamcmd_linux.tar.gz"

# Install Black Mesa Dedicated Server.
echo -e "login anonymous\napp_update 346680\nquit" | ~/SteamCMD/steamcmd.sh

# Remove textures to save ~9 GB.
# Materials are needed on the server but textures are not needed.
cd "../Steam/steamapps/common/Black Mesa Dedicated Server"
rm bms/bms_textures*
rm hl2/hl2_textures*

# Install Metamod Source, SourceMod, Accelerator and SourceCoop.
cd "bms"
download_extract "https://mms.alliedmods.net/mmsdrop/1.11/mmsource-1.11.0-git1155-linux.tar.gz"
download_extract "https://sm.alliedmods.net/smdrop/1.11/sourcemod-1.11.0-git6964-linux.tar.gz"
download_extract "https://builds.limetech.io/files/accelerator-2.5.0-git138-cd575aa-linux.zip"
download_extract $(wget -qO- "https://api.github.com/repos/ampreeT/SourceCoop/releases/latest" | grep "browser_download_url" | cut -d '"' -f 4)

# Create `mapcycle.txt`.
rm mapcycle.txt
echo "bm_c0a0a" >> "mapcycle.txt"   # Black Mesa: Chapter 1
echo "bm_c1a0a" >> "mapcycle.txt"   # Black Mesa: Chapter 2
echo "bm_c1a1a" >> "mapcycle.txt"   # Black Mesa: Chapter 3
echo "bm_c1a2a" >> "mapcycle.txt"   # Black Mesa: Chapter 4
echo "bm_c1a3a" >> "mapcycle.txt"   # Black Mesa: Chapter 5
echo "bm_c1a4a" >> "mapcycle.txt"   # Black Mesa: Chapter 6
echo "bm_c2a1a" >> "mapcycle.txt"   # Black Mesa: Chapter 7
echo "bm_c2a1a" >> "mapcycle.txt"   # Black Mesa: Chapter 8
echo "bm_c2a2a" >> "mapcycle.txt"   # Black Mesa: Chapter 9
echo "bm_c2a3a" >> "mapcycle.txt"   # Black Mesa: Chapter 10
echo "bm_c2a4a" >> "mapcycle.txt"   # Black Mesa: Chapter 11
echo "bm_c2a4e" >> "mapcycle.txt"   # Black Mesa: Chapter 12
echo "bm_c2a5a" >> "mapcycle.txt"   # Black Mesa: Chapter 13
echo "bm_c3a1a" >> "mapcycle.txt"   # Black Mesa: Chapter 14
echo "bm_c3a2a" >> "mapcycle.txt"   # Black Mesa: Chapter 15
echo "bm_c4a1a" >> "mapcycle.txt"   # Black Mesa: Chapter 16
echo "bm_c4a2a" >> "mapcycle.txt"   # Black Mesa: Chapter 17
echo "bm_c4a3a" >> "mapcycle.txt"   # Black Mesa: Chapter 18
#echo "fd01"     >> "mapcycle.txt"   # Further Data

cd "cfg"

# Empty `autoexec.cfg` as the default settings is not used on the server.
rm "autoexec.cfg"
echo "" >> "autoexec.cfg"

# Create `server.cfg`.
rm "server.cfg"
echo "// SourceCoop settings" >> "server.cfg"
echo "hostname \"Black Mesa: Deathmatch\"" >> "server.cfg"  # make this adjustable
echo "mp_timelimit 0      // Prevents map switch from round timers." >> "server.cfg"
echo "mp_fraglimit 0      // Prevents the match from ending when a player has a high enough score." >> "server.cfg"
echo "mp_teamplay 1       // Enables the scientist team." >> "server.cfg"
echo "mp_friendlyfire 0   // Disables friendly fire." >> "server.cfg"
echo "mp_forcerespawn 1" >> "server.cfg"
echo "" >> "server.cfg"
echo "// Add your settings below" >> "server.cfg"
# sv_password ""                      // Sets a server password for locking the server.
# rcon_password ""                    // Sets a RCON password for accessing adminstrative features.

# Add your maps, plugins and configuration here.

# Adds a job to launch the `srcds_run` everytime the instance starts.
# `srcds_run` handles server restarts if the server goes down.
(crontab -l; echo "@reboot screen -dmS srcds ~/Steam/steamapps/common/Black\ Mesa\ Dedicated\ Server/srcds_run -console -game bms -port 27015 -insecure +sv_lan 0 +maxplayers 18 +map bm_c1a0a") | crontab -
EOF

# Reboot the instance to start the Black Mesa Dedicated Server.
sudo reboot

echo "hostname \"Black Mesa: Deathmatch\"" >> "server.cfg"  # make this adjustable
echo "mp_timelimit 0      // Prevents map switch from round timers." >> "server.cfg"