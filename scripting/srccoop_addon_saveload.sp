#include <clientprefs>
#include <sdkhooks>
#include <sourcemod>
#include <srccoop_api>

#pragma newdecls required
#pragma semicolon 1
#pragma tabsize 0

char saveFilePath[PLATFORM_MAX_PATH];

public Plugin myinfo =
{
	name        = "SourceCoop Saving and Loading",
	author      = "Jimbab",
	description = "Server Sided level state saving.",
	version     = "1.0.0",
	url         = "https://github.com/ampreeT/SourceCoop"
};

public void OnPluginStart()
{
	InitSourceCoopAddon();

	if (LibraryExists(SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
    
    CreateDirectory("addons/sourcemod/data/srccoop", 3);
    BuildPath(Path_SM, saveFilePath, sizeof saveFilePath, "data/srccoop/savefile.sav");
}

public void OnLibraryAdded(const char[] name)
{
	if (StrEqual(name, SRCCOOP_LIBRARY))
		OnSourceCoopStarted();
}

void OnSourceCoopStarted()
{
	TopMenu hCoopMenu = GetCoopTopMenu();
	TopMenuObject menuCategory = hCoopMenu.FindCategory(COOPMENU_CATEGORY_OTHER);	

	if (menuCategory != INVALID_TOPMENUOBJECT)
	{
		hCoopMenu.AddItem("SaveButton", SaveMenuHandler, menuCategory);
		hCoopMenu.AddItem("LoadButton", LoadMenuHandler, menuCategory);
	}
}

public void SaveMenuHandler(TopMenu topMenu, TopMenuAction action, TopMenuObject topObjectId, int client, char[] buffer, int maxLength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxLength, "Save Current Level");
	}
	else if (action == TopMenuAction_SelectOption)
	{
        Save(client);
		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

public void LoadMenuHandler(TopMenu topMenu, TopMenuAction action, TopMenuObject topObjectId, int client, char[] buffer, int maxLength)
{
	if (action == TopMenuAction_DisplayOption)
	{
		Format(buffer, maxLength, "Load Saved Level");
	}
	else if (action == TopMenuAction_SelectOption)
	{
		Load(client);
		topMenu.Display(client, TopMenuPosition_LastCategory);
	}
}

void Save(int client)
{
    // Handle to our save file
    Handle saveFile;

    // Check if file exists and, whether we must create it or not
    if (!FileExists(saveFilePath))
        // Create a new file and return handle
        saveFile = OpenFile(saveFilePath, "wt");
    else
        // Open an existing save file and return handle
        saveFile = OpenFile(saveFilePath, "rt");

    // Check the process worked correctly
    if (saveFile == INVALID_HANDLE)
    {
        Msg(client, "Failed to save progress.");
        return;
    }

    // Get the current map string
    char saveContents[64];
    GetCurrentMap(saveContents, sizeof saveContents);

    // Write it to the file
    if (!WriteFileLine(saveFile, saveContents))
    {
        Msg(client, "Failed to save progress.");
        CloseHandle(saveFile);

        return;
    }

    Msg(client, "Save progress successfully, for level: %s.", saveContents);

    // We're done
    CloseHandle(saveFile);
}

void Load(int client)
{
    // Check if file exists and, whether we actually have something to load
    if (!FileExists(saveFilePath))
    {
        Msg(client, "No save file exists.");
        return;
    }

    // Open our file in read mode
    Handle saveFile = OpenFile(saveFilePath, "rt");

    // Check the process worked correctly
    if (saveFile == INVALID_HANDLE)
        return;

    // Buffer for save info
    char saveInfo[64];

    // Read map string
    if (!ReadFileLine(saveFile, saveInfo, 64))
        return;

    // We're done with the file
    CloseHandle(saveFile);

    // Load the level
    ServerCommand("changelevel %s", saveInfo);
}