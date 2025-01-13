#include <sourcemod>
#include <sdkhooks>

#include <srccoop_addon>

#pragma newdecls required
#pragma semicolon 1

public Plugin myinfo =
{
	name = "SourceCoop Difficulty",
	author = "Alienmario",
	description = "NPC difficulty scaling",
	version = SRCCOOP_VERSION,
	url = SRCCOOP_URL
};

#define SCALE 0.1

ConVar g_pConvarDifficulty;
ConVar g_pConvarDifficultyAuto;
ConVar g_pConvarDifficultyAutoMin;
ConVar g_pConvarDifficultyAutoMax;
ConVar g_pConvarDifficultyAnnounce;
ConVar g_pConvarDifficultyIgnoreDamageTo;
ConVar g_pConvarDifficultyIgnoreDamageFrom;

StringMap g_pMapIgnoredTo;
StringMap g_pMapIgnoredFrom;

int g_iDifficulty;
bool g_bEnabled;

public void OnPluginStart()
{
	LoadTranslations("srccoop_difficulty.phrases");
	InitSourceCoopAddon();

	g_pConvarDifficulty = CreateConVar("sourcecoop_difficulty", "0", "Sets the difficulty - from 0 (base difficulty) and up.", _, true, 0.0);
	g_pConvarDifficultyAuto = CreateConVar("sourcecoop_difficulty_auto", "1", "Sets automatic difficulty mode. -1 disables. 0 balances difficulty between min and max convars. Values above 0 set the difficulty increment per player, ignoring the min and max cvars.", _, true, -1.0);
	g_pConvarDifficultyAutoMin = CreateConVar("sourcecoop_difficulty_auto_min", "0", "When automatic difficulty mode is set to 0, this is the difficulty at 1 player.", _, true, 0.0);
	g_pConvarDifficultyAutoMax = CreateConVar("sourcecoop_difficulty_auto_max", "20", "When automatic difficulty mode is set to 0, this is the difficulty at max players.", _, true, 0.0);
	g_pConvarDifficultyAnnounce = CreateConVar("sourcecoop_difficulty_announce", "1", "Toggles announcing changes in difficulty.", _, true, 0.0, true, 1.0);
	g_pConvarDifficultyIgnoreDamageTo = CreateConVar("sourcecoop_difficulty_ignoredmgto", "npc_headcrab;npc_barnacle;npc_puffballfungus", "List of classnames where player->npc damage is exempt from difficulty scaling. Separated by semicolon.");
	g_pConvarDifficultyIgnoreDamageFrom = CreateConVar("sourcecoop_difficulty_ignoredmgfrom", "npc_puffballfungus", "List of classnames where npc->player damage is exempt from difficulty scaling. Separated by semicolon.");
	
	g_pConvarDifficulty.AddChangeHook(OnDifficultyChanged);
	g_pConvarDifficultyIgnoreDamageTo.AddChangeHook(OnIgnoreDmgToChanged);
	g_pConvarDifficultyIgnoreDamageFrom.AddChangeHook(OnIgnoreDmgFromChanged);
	g_pConvarDifficultyAuto.AddChangeHook(OnAutoMinMaxChanged);
	g_pConvarDifficultyAutoMin.AddChangeHook(OnAutoMinMaxChanged);
	g_pConvarDifficultyAutoMax.AddChangeHook(OnAutoMinMaxChanged);
	
	g_pMapIgnoredTo = new StringMap();
	g_pMapIgnoredFrom = new StringMap();
	RefreshIgnoreMap(g_pMapIgnoredTo, g_pConvarDifficultyIgnoreDamageTo);
	RefreshIgnoreMap(g_pMapIgnoredFrom, g_pConvarDifficultyIgnoreDamageFrom);
	
	HookEvent("player_team", Event_PlayerChangeTeam, EventHookMode_PostNoCopy);
}

public void OnMapStart()
{
	g_bEnabled = SC_IsCurrentMapCoop();
}

void OnDifficultyChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	g_iDifficulty = convar.IntValue;
	if (g_bEnabled && g_pConvarDifficultyAnnounce.BoolValue)
	{
		MsgAll("%t", "difficulty changed", g_iDifficulty);
	}
}

public void OnClientSayCommand_Post(int client, const char[] command, const char[] sArgs)
{
	if (StrEqual(sArgs, "difficulty", false))
	{
		MsgAll("%t", "current difficulty", g_iDifficulty);
	}
}

void OnIgnoreDmgToChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	RefreshIgnoreMap(g_pMapIgnoredTo, convar);
}

void OnIgnoreDmgFromChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	RefreshIgnoreMap(g_pMapIgnoredFrom, convar);
}

void OnAutoMinMaxChanged(ConVar convar, const char[] oldValue, const char[] newValue)
{
	UpdateAutoDifficulty();
}

public void OnClientPutInServer(int client)
{
	SDKHook(client, SDKHook_OnTakeDamage, Hook_OnPlayerTakeDamage);
}

public void OnClientDisconnect_Post(int client)
{
	UpdateAutoDifficulty();
}

public void Event_PlayerChangeTeam(Event hEvent, const char[] szName, bool bDontBroadcast)
{
	RequestFrame(Frame_PlayerChangeTeamPost);
}

public void Frame_PlayerChangeTeamPost()
{
	UpdateAutoDifficulty();
}

void UpdateAutoDifficulty()
{
	if (g_pConvarDifficultyAuto.FloatValue == 0.0)
	{
		int min = g_pConvarDifficultyAutoMin.IntValue;
		int max = g_pConvarDifficultyAutoMax.IntValue;
		
		if (min > max)
		{
			MsgSrv("%t", "unable to set");
			return;
		}
		
		float ratio = (GetRealClientCount(true, false, true) - 1) / float(MaxClients - 1);
		g_pConvarDifficulty.IntValue = RoundToFloor((max - min) * ratio + min);
	}
	else if (g_pConvarDifficultyAuto.FloatValue > 0.0)
	{
		g_pConvarDifficulty.IntValue = RoundFloat(g_pConvarDifficultyAuto.FloatValue * (GetRealClientCount(true, false, true) - 1));
	}
}

public void OnEntityCreated(int iEntIndex, const char[] szClassname)
{
	CBaseEntity pEntity = CBaseEntity(iEntIndex);
	if (pEntity.IsNPC())
	{
		SDKHook(iEntIndex, SDKHook_OnTakeDamage, Hook_OnNpcTakeDamage);
		
		if (StrEqual(szClassname, "npc_apache"))
		{
			HookSingleEntityOutput(iEntIndex, "OnWishToDie", Hook_ApacheAboutToDie, true);
		}
	}
}

public Action Hook_OnPlayerTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon, float damageForce[3], float damagePosition[3])
{
	if (g_iDifficulty && g_bEnabled)
	{
		CBaseEntity pAttacker = CBaseEntity(attacker);
		
		if (pAttacker.IsNPC() && PassIgnoreMap(g_pMapIgnoredFrom, attacker))
		{
			damage += g_iDifficulty * SCALE * damage;
			return Plugin_Changed;
		}
	}
	return Plugin_Continue;
}

public Action Hook_OnNpcTakeDamage(int victim, int &attacker, int &inflictor, float &damage, int &damagetype, int &weapon, float damageForce[3], float damagePosition[3])
{
	if (g_iDifficulty && g_bEnabled)
	{
		CBaseEntity pAttacker = CBaseEntity(attacker);
		
		if (pAttacker.IsPlayer() && PassIgnoreMap(g_pMapIgnoredTo, victim))
		{
			damage = damage / (g_iDifficulty * SCALE + 1);
			return Plugin_Changed;
		}
	}
	return Plugin_Continue;
}

public void Hook_ApacheAboutToDie(const char[] output, int caller, int activator, float delay)
{
	// The apache needs to receive full rpg damage for the final hit
	SDKUnhook(caller, SDKHook_OnTakeDamage, Hook_OnNpcTakeDamage);
}

bool PassIgnoreMap(StringMap map, int iEntIndex)
{
	char szClassname[MAX_CLASSNAME]; int v;
	GetEntityClassname(iEntIndex, szClassname, sizeof(szClassname));
	return !map.GetValue(szClassname, v);
}

void RefreshIgnoreMap(StringMap map, ConVar convar)
{
	char szIgnores[2048];
	convar.GetString(szIgnores, sizeof(szIgnores));
	map.Clear();
	
	int start;
	int len = strlen(szIgnores);
	for (int i = 0; i < len; i++)
	{
		if (szIgnores[i] == ';')
		{
			szIgnores[i] = '\0';
			map.SetValue(szIgnores[start], 1);
			start = i + 1;
		}
	}
	if (szIgnores[start] != '\0')
	{
		map.SetValue(szIgnores[start], 1);
	}
}