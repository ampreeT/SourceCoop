# Download and extract SteamCMD.
New-Item -Name "SteamCMD" -ItemType Directory
Invoke-WebRequest -Uri "https://steamcdn-a.akamaihd.net/client/installer/steamcmd.zip" -OutFile ".tmp.zip"
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./SteamCMD"
Remove-Item -Path ".tmp.zip" -Force

# Install Black Mesa Dedicated Server.
New-Item -Name "Black Mesa Dedicated Server" -ItemType Directory
Start-Process -FilePath "./SteamCMD/steamcmd.exe" -ArgumentList '+force_install_dir "../Black Mesa Dedicated Server"', "+login anonymous", "+app_update 346680", "+quit" -Wait -NoNewWindow

# Install the latest version of Metamod Source.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri (((Invoke-WebRequest -Uri "https://www.sourcemm.net/downloads.php").Links | Where-Object { $_.href -like "*-windows.zip" }).href | Select-Object -First 1)
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms"
Remove-Item -Path ".tmp.zip" -Force

# Install the latest version of SourceMod.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri (((Invoke-WebRequest -Uri "https://www.sourcemod.net/downloads.php").Links | Where-Object { $_.href -like "*-windows.zip" }).href | Select-Object -First 1)
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms"
Remove-Item -Path ".tmp.zip" -Force

# Install the latest version of Accelerator.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri ("https://builds.limetech.io/" + ((Invoke-WebRequest -Uri "https://builds.limetech.io/?p=accelerator").Links | Where-Object { $_.href -like "*-windows.zip" } | Select-Object -First 1).href)
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms"
Remove-Item -Path ".tmp.zip" -Force

# Install the latest release of SourceCoop.
Invoke-WebRequest -OutFile ".tmp.zip" -Uri ((Invoke-WebRequest "https://api.github.com/repos/ampreeT/SourceCoop/releases/latest" | ConvertFrom-Json).assets | Where-Object { $_.name -like "*-bms.zip" } | Select-Object -First 1).browser_download_url
Expand-Archive -LiteralPath ".tmp.zip" -DestinationPath "./Black Mesa Dedicated Server/bms"
Remove-Item -Path ".tmp.zip" -Force

# OPTIONAL: Remove textures to save ~9 GB.
# Materials are needed on the server but textures are not needed.
# If the server ever needs to be updated, these files will be redownloaded again.
Remove-Item -Path "./Black Mesa Dedicated Server/bms/bms_textures*" -Force
Remove-Item -Path "./Black Mesa Dedicated Server/hl2/hl2_textures*" -Force