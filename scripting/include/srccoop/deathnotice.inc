#pragma newdecls required
#pragma semicolon 1

int g_pDamageFlagList[] =
{
	DMG_PLASMA,
	DMG_SLOWBURN,
	DMG_RADIATION,
	DMG_POISON,
	DMG_NERVEGAS,
	DMG_ENERGYBEAM,
	DMG_SONIC,
	DMG_SHOCK,
	DMG_BLAST,
	DMG_BURN,
	DMG_CRUSH,
};

char g_pDamageStringList[sizeof(g_pDamageFlagList)][] =
{
	"Plasma",
	"Fire",
	"Radiation",
	"Poison",
	"Nerve Gas",
	"Laser Beam",
	"Sonic Shockwave",
	"Electric Shock",
	"Explosion",
	"Fire",
	"Physics Prop"
};

enum struct DnTimerData
{
	// Weapon display name.
	char m_szWeapon[48];
	// Attacker.
	CBasePlayer m_pTarget1;
	// Victim.
	CBasePlayer m_pTarget2;
}

enum struct DnEntry
{
	// Weapon display name.
	char m_szWeapon[48];
	// Attacker Name.
	char m_szTarget1Name[MAX_NAME_LENGTH];
	// Victim Name.
	char m_szTarget2Name[MAX_NAME_LENGTH];
	// Attacker.
	CBasePlayer m_pTarget1;
	// Victim.
	CBasePlayer m_pTarget2;
	// Was this a suicide?
	bool m_bSuicide;
}

enum struct DnData
{
	// Array of `DnEntry`.
	ArrayList m_sList;
	// Bot 1 used for displays when `sc_killfeed` is set to `2`.
	CBasePlayer m_pBot1;
	// Bot 2 used for displays when `sc_killfeed` and `sc_killfeed_entity_kills` is set to `2`.
	CBasePlayer m_pBot2;
	// Timer used for displays.
	Handle m_hTimer;
	// Timer display data.
	DnTimerData m_sTimerData;
	// Controls where if the "player_death" event should broadcast.
	bool m_bBroadcastEnabled;
	// Are we currently renaming a deathnotice bot?
	bool m_bChangingBotName;
	// `sc_killfeed`
	ConVar m_piNoticeLevel;
	// `sc_killfeed_player_kills`
	ConVar m_piNoticePlayerKills;
	// `sc_killfeed_entity_kills`
	ConVar m_piNoticeEntityKills;
	// `sc_killfeed_suicides`
	ConVar m_piNoticeSuicides;
}

DnData g_sDnData;

methodmap DnManager
{
	public static void Initialize()
	{
		g_sDnData.m_sList = new ArrayList(sizeof(DnEntry));
		// The killfeed is by default only enabled for BM, pending full integration with other games.
		#if defined SRCCOOP_BLACKMESA
		g_sDnData.m_piNoticeLevel = CreateConVar("sc_killfeed", "2", "Controls the display of the kill feed (0: disabled, 1: chat, 2: hud). If this Convar is set to `2`, then the plugin will spawn in fake clients to display on the kill feed.", _, true, 0.0, true, 2.0);
		#else
		g_sDnData.m_piNoticeLevel = CreateConVar("sc_killfeed", "0", "Controls the display of the kill feed (0: disabled, 1: chat, 2: hud). If this Convar is set to `2`, then the plugin will spawn in fake clients to display on the kill feed.", _, true, 0.0, true, 2.0);
		#endif
		g_sDnData.m_piNoticePlayerKills = CreateConVar("sc_killfeed_player_kills", "2", "Controls display of player kills on the kill feed (0: hide, 1: players, 2: entities).", _, true, 0.0, true, 2.0);
		g_sDnData.m_piNoticeEntityKills = CreateConVar("sc_killfeed_entity_kills", "2", "Controls display of entity kills on the kill feed (0: hide, 1: players, 2: entities).", _, true, 0.0, true, 2.0);
		g_sDnData.m_piNoticeSuicides = CreateConVar("sc_killfeed_suicides", "2", "Controls display of suicides on the kill feed (0: hide, 1: players, 2: entities).", _, true, 0.0, true, 2.0);

		HookConVarChange(g_sDnData.m_piNoticeLevel, DeathNotice_OnConVarChangeScKillfeed);
	}
	// Clears all data and kicks the bots.
	//
	public static void Clear()
	{
		DnTimerData sTimerData;
		g_sDnData.m_sList.Clear();
		g_sDnData.m_sTimerData = sTimerData;
		
		if (g_sDnData.m_pBot1.IsValid())
		{
			g_sDnData.m_pBot1.Kick();
		}

		if (g_sDnData.m_pBot2.IsValid())
		{
			g_sDnData.m_pBot2.Kick();
		}
	}
	// Obtains a bot.
	//
	public static CBasePlayer GetBot1()
	{
		if (g_sDnData.m_piNoticeLevel.IntValue == 2)
		{
			if (GetRealClientCount(true, false, false) > 0)
			{
				if (!g_sDnData.m_pBot1.IsValid())
				{
					g_sDnData.m_pBot1 = CBasePlayer(CreateFakeClient("ScBot01"));
					g_sDnData.m_pBot1.SetTeam(TEAM_SPECTATOR);
					g_sDnData.m_pBot1.m_fFlags |= (FL_FAKECLIENT | FL_NOTARGET);
				}
				return g_sDnData.m_pBot1;
			}
		}
		return NULL_CBASEENTITY;
	}
	// Obtains a bot.
	//
	public static CBasePlayer GetBot2()
	{
		if (g_sDnData.m_piNoticeLevel.IntValue == 2)
		{
			if (GetRealClientCount(true, false, false) > 0)
			{
				if (!g_sDnData.m_pBot2.IsValid())
				{
					g_sDnData.m_pBot2 = CBasePlayer(CreateFakeClient("ScBot02"));
					g_sDnData.m_pBot2.SetTeam(TEAM_SPECTATOR);
					g_sDnData.m_pBot2.m_fFlags |= (FL_FAKECLIENT | FL_NOTARGET);
				}
				return g_sDnData.m_pBot2;
			}
		}
		return NULL_CBASEENTITY;
	}
	// Hook callback to hide player name changes for bots.
	// Bots have to change their names in order to display a death display.
	//
	public static Action TextMsg(BfRead bf)
    {
		return (g_sDnData.m_bChangingBotName) ? Plugin_Stop : Plugin_Continue;
	}
	// Hook callback to hide the player death displays unless this plugin's features are enabled.
	//
	public static Action PlayerDeath(Event hEvent)
    {
		if (!CoopManager.IsCoopModeEnabled())
			return Plugin_Continue;

		if (g_sDnData.m_piNoticeLevel.IntValue == 0)
			return Plugin_Continue;

		return g_sDnData.m_bBroadcastEnabled ? Plugin_Continue : Plugin_Stop;
	}
	// Checks whether if a entity should be allowed to display as a notification.
	// This is reserved for entities that would not make sense being displayed.
	//
	public static bool IsEntityAllowed(CBaseEntity pEntity)
	{
		char szClassname[MAX_CLASSNAME];
		pEntity.GetClassname(szClassname, sizeof(szClassname));
		return strcmp(szClassname, "npc_bullseye", false) != 0;
	}
	// Handles changing the death notice bot names
	//
	public static bool HandleDisplayName(CBasePlayer pPlayer, const char[] szName)
	{
		// Changes the player name if it is a bot handled by the manager.
		// This is used for the display of a entity on death notices.
		if (pPlayer.IsValid())
		{
			if (pPlayer == g_sDnData.m_pBot1 || pPlayer == g_sDnData.m_pBot2)
			{
				g_sDnData.m_bChangingBotName = true;
				pPlayer.SetName(szName);
				g_sDnData.m_bChangingBotName = false;
			}
		}
	}
	// Formats damage flags for a display event.
	//
	public static void GetDisplayNameFromDamageForEvent(const int iDamageFlags, const char[] szClassname, char[] szBuffer, const int iMaxLength)
	{
		for (int i = 0; i < sizeof(g_pDamageFlagList); ++i)
		{
			if (iDamageFlags & g_pDamageFlagList[i])
			{
				strcopy(szBuffer, iMaxLength, g_pDamageStringList[i]);
				break;
			}
		}
		strcopy(szBuffer, iMaxLength, szClassname);
	}
	// Formats the NPC string for a display event.
	//
	public static void GetDisplayNameForEvent(CBaseEntity pEntity, char[] szBuffer, const int iMaxLength)
	{
		char szClassname[MAX_CLASSNAME];
		pEntity.GetClassname(szClassname, sizeof(szClassname));

		// TODO:
		// Handle damage flags for `func_movelinear`/`func_tracktrain`. 
		if (strcmp(szClassname, "point_hurt", false) == 0)
		{
			int iDamageFlags = view_as<CPointHurt>(pEntity).GetDamageType();
			DnManager.GetDisplayNameFromDamageForEvent(iDamageFlags, szClassname, szBuffer, iMaxLength);
		}
		else if (strcmp(szClassname, "trigger_hurt", false) == 0)
		{
			int iDamageFlags = view_as<CTriggerHurt>(pEntity).GetDamageType();
			DnManager.GetDisplayNameFromDamageForEvent(iDamageFlags, szClassname, szBuffer, iMaxLength);
		}
		else
		{
			for (int i = 0; i < sizeof(g_pDnDisplayList); ++i)
			{
				if (strcmp(szClassname, g_pDnDisplayList[i][0], false) == 0)
				{
					strcopy(szBuffer, iMaxLength, g_pDnDisplayList[i][1]);
					return;
				}
			}

			strcopy(szBuffer, iMaxLength, szClassname);
		}
	}
	// Formats the weapon string for a display event.
	//
	public static void GetWeaponNameForEvent(CBaseEntity pInflictor, char[] szBuffer, const int iMaxLength)
	{
		if (pInflictor.IsValid())
		{
			char szClassname[MAX_CLASSNAME];
			pInflictor.GetClassname(szClassname, sizeof(szClassname));

			for (int i = 0; i < sizeof(g_pDnWeaponIconList); ++i)
			{
				if (strcmp(szClassname, g_pDnWeaponIconList[i][0], false) == 0)
				{
					strcopy(szBuffer, iMaxLength, g_pDnWeaponIconList[i][1]);
					return;
				}
			}
		}
		strcopy(szBuffer, iMaxLength, "");
	}
	// Player killed Player or Entity
	//
	public static bool WriteEntryPlayerKill(DnEntry sEntry, CBasePlayer pAttacker, CBaseEntity pVictim)
	{
		sEntry.m_pTarget1 = pAttacker;
		sEntry.m_pTarget1.GetName(sEntry.m_szTarget1Name, sizeof(sEntry.m_szTarget1Name));

		if (pVictim.IsPlayer())
		{
			if (pAttacker == pVictim)
			{
				// Check if player suicides is enabled.
				if (g_sDnData.m_piNoticeSuicides.IntValue < 1)
					return false;

				sEntry.m_pTarget2 = sEntry.m_pTarget1;
				sEntry.m_szTarget2Name = sEntry.m_szTarget1Name;
				sEntry.m_bSuicide = true;
			}
			else
			{
				// Check if player killing players is enabled.
				if (g_sDnData.m_piNoticePlayerKills.IntValue == 0)
					return false;
				
				// The victim is a player so it can be used directly.
				sEntry.m_pTarget2 = view_as<CBasePlayer>(pVictim);
				sEntry.m_pTarget2.GetName(sEntry.m_szTarget2Name, sizeof(sEntry.m_szTarget2Name));
				sEntry.m_bSuicide = false;
			}
		}
		else
		{
			// Check if player killing entities is enabled.
			if (g_sDnData.m_piNoticePlayerKills.IntValue != 2)
				return false;

			// The victim is a entity so a bot will have to be used.
			sEntry.m_pTarget2 = DnManager.GetBot1();
			if (g_sDnData.m_piNoticeLevel.IntValue == 2 && !sEntry.m_pTarget2.IsValid())
				return false;
				
			DnManager.GetDisplayNameForEvent(pVictim, sEntry.m_szTarget2Name, sizeof(sEntry.m_szTarget2Name));
			sEntry.m_bSuicide = false;
		}
		return true;
	}
	// Entity killed Entity or Player
	//
	public static bool WriteEntryEntityKills(DnEntry sEntry, CBaseEntity pAttacker, CBaseEntity pVictim)
	{
		// TODO:
		// Should this be moved further down? This bot will always be created if `sc_killfeed` is set to `2`.
		sEntry.m_pTarget1 = DnManager.GetBot1();
		// pTarget1 will be NULL unless the notice level is set to 2.
		if (g_sDnData.m_piNoticeLevel.IntValue == 2 && !sEntry.m_pTarget1.IsValid())
			return false;
		DnManager.GetDisplayNameForEvent(pAttacker, sEntry.m_szTarget1Name, sizeof(sEntry.m_szTarget1Name));

		if (pVictim.IsPlayer())
		{
			// Check if feature is enabled.
			if (g_sDnData.m_piNoticeEntityKills.IntValue == 0)
				return false;

			// The victim is a player so it can be used directly.
			sEntry.m_pTarget2 = view_as<CBasePlayer>(pVictim);
			sEntry.m_pTarget2.GetName(sEntry.m_szTarget2Name, sizeof(sEntry.m_szTarget2Name));
			sEntry.m_bSuicide = false;
		}
		else
		{
			if (pAttacker == pVictim)
			{
				// Check if entity suicides is enabled.
				if (g_sDnData.m_piNoticeSuicides.IntValue < 2)
					return false;
				
				sEntry.m_pTarget2 = sEntry.m_pTarget1;
				sEntry.m_szTarget2Name = sEntry.m_szTarget1Name;
				sEntry.m_bSuicide = true;
			}
			else
			{
				// Check if entities killing entities is enabled.
				if (g_sDnData.m_piNoticeEntityKills.IntValue != 2)
					return false;

				// The victim is a entity so a bot will have to be used.
				sEntry.m_pTarget2 = DnManager.GetBot2();
				if (g_sDnData.m_piNoticeLevel.IntValue == 2 && !sEntry.m_pTarget2.IsValid())
					return false;
				
				DnManager.GetDisplayNameForEvent(pVictim, sEntry.m_szTarget2Name, sizeof(sEntry.m_szTarget2Name));
				sEntry.m_bSuicide = false;
			}
		}
		return true;
	}
	// Hook callback that detects and handles player and entity kills.
	//
	public static void EntityKilled(Event hEvent)
	{
		if (!CoopManager.IsCoopModeEnabled())
    	    return;

		CBaseEntity pAttacker = CBaseEntity(hEvent.GetInt("entindex_attacker"));
		CBaseEntity pVictim = CBaseEntity(hEvent.GetInt("entindex_killed"));
		CBaseEntity pInflictor = CBaseEntity(hEvent.GetInt("entindex_inflictor"));

		if (!pAttacker.IsValid() || !pVictim.IsValid() || !DnManager.IsEntityAllowed(pVictim))
			return;
		
		DnEntry sEntry;
		if (pAttacker.IsPlayer())
		{
			if (!DnManager.WriteEntryPlayerKill(sEntry, view_as<CBasePlayer>(pAttacker), pVictim))
				return;
		}
		else
		{
			if (!DnManager.WriteEntryEntityKills(sEntry, pAttacker, pVictim))
				return;
		}
		DnManager.GetWeaponNameForEvent(pInflictor, sEntry.m_szWeapon, sizeof(sEntry.m_szWeapon));
		DnManager.PushDeathNotice(sEntry);
	}
	// Pushes a death notice entry onto the display.
	//
	public static void PushDeathNotice(DnEntry sEntry)
	{
		if (g_sDnData.m_piNoticeLevel.IntValue == 2)
		{
			// Delay is used to allow time for the game to update the player name.
			// If the death notification displays before the player name is
			// updated, then the old name will be displayed which is unintended behavior.
			float flDelay = 0.35; 

			// Pushes a entry a new queue if a queue does not exist.
			// If a queue does exist, pushes a entry to the queue. 
			g_sDnData.m_sList.PushArray(sEntry, sizeof(sEntry));
			if (!g_sDnData.m_hTimer)
			{
				g_sDnData.m_hTimer = CreateTimer(flDelay, DeathNotice_Timer, _, TIMER_REPEAT);
			}
		}
		else if (g_sDnData.m_piNoticeLevel.IntValue == 1)
		{
			if (sEntry.m_bSuicide)
			{
				// TODO:
				// Translations.
				MsgAllNoSrv("%t", "entity suicided", sEntry.m_szTarget1Name);
			}
			else
			{
				if (strlen(sEntry.m_szWeapon) != 0)
				{
					// TODO:
					// Translations.
					MsgAllNoSrv("%t", "entity killed entity with weapon", sEntry.m_szTarget1Name, sEntry.m_szTarget2Name, sEntry.m_szWeapon);
				}
				else
				{
					// TODO:
					// Translations.
					MsgAllNoSrv("%t", "entity killed entity", sEntry.m_szTarget1Name, sEntry.m_szTarget2Name);
				}
			}
		}
	}
	// Sends a "player_death" event to the client without being blocked.
	//
	public static void DrawNoticeIngame(CBasePlayer pAttacker, CBasePlayer pVictim, const char[] szWeapon)
	{
		// The "player_death" event is hooked so the game's normal
		// death notifications do not appear. The broadcasting
		// of the event is temporarily enabled so that this function
		// can send it.

		// Enable broadcasting temporarily for a single "player_death" event to send.
		g_sDnData.m_bBroadcastEnabled = true;

		// this displays a death notice ingame
		Event hEvent = CreateEvent("player_death");
		hEvent.SetString("weapon", szWeapon);
		hEvent.SetInt("attacker", pAttacker.GetUserId());
		hEvent.SetInt("userid", pVictim.GetUserId());
		hEvent.Fire();

		// Disable broadcast to disallow the "player_death" event from being sent.
		g_sDnData.m_bBroadcastEnabled = false;
	}
}

public void DeathNotice_OnConVarChangeScKillfeed(ConVar piConVar, char[] szOldValue, char[] szNewValue)
{
	if (piConVar.IntValue != 2)
	{
		DnManager.Clear();
	}
}

// Draws a notice with a delay between each display. 
// The delay is used to allow time for the game to update the player name.
// If the death notification displays before the player name is
// updated, then the old name will be displayed which is unintended behavior.
//
public Action DeathNotice_Timer(Handle hTimer)
{
	// If the timer just started, this won't call immediately.
	if (g_sDnData.m_sTimerData.m_pTarget1.IsValid() && g_sDnData.m_sTimerData.m_pTarget2.IsValid())
	{
		DnManager.DrawNoticeIngame(g_sDnData.m_sTimerData.m_pTarget1, g_sDnData.m_sTimerData.m_pTarget2, g_sDnData.m_sTimerData.m_szWeapon);
	}

	// Consume a entry.
	if (g_sDnData.m_sList.Length > 0)
	{
		// Get entry.
		DnEntry sEntry;
		g_sDnData.m_sList.GetArray(0, sEntry, sizeof(sEntry));
		
		// Assign the current entry to the timer.
		g_sDnData.m_sTimerData.m_szWeapon = sEntry.m_szWeapon;
		g_sDnData.m_sTimerData.m_pTarget1 = sEntry.m_pTarget1;
		g_sDnData.m_sTimerData.m_pTarget2 = sEntry.m_pTarget2;

		// Change the player's name if it is death notice bot.
		DnManager.HandleDisplayName(g_sDnData.m_sTimerData.m_pTarget1, sEntry.m_szTarget1Name);
		if (g_sDnData.m_sTimerData.m_pTarget1 != g_sDnData.m_sTimerData.m_pTarget2)
		{
			// This is formatted to avoid a naming conflict with the first bot.
			char szTarget2Name[MAX_NAME_LENGTH];
			Format(szTarget2Name, sizeof(szTarget2Name), "%s ", sEntry.m_szTarget2Name);
			DnManager.HandleDisplayName(g_sDnData.m_sTimerData.m_pTarget2, szTarget2Name);
		}

		// Remove the current entry.
		g_sDnData.m_sList.Erase(0);

		return Plugin_Continue;
	}
	else
	{
		// Destroy timer.
		DnTimerData sTimerData;
		g_sDnData.m_hTimer = INVALID_HANDLE;
		g_sDnData.m_sTimerData = sTimerData;
		return Plugin_Stop;
	}
}

//public Action Event_LogicAchievementOnFired(const char[] szOutput, int iCaller, int iActivator, float flDelay)
//{
//	//PrintToChatAll(SRCCOOP_CHAT_TAG..."Server has earned the achievement \x03%s", "#BMS_BROWN_MOTION_NAME");
//
//	if (CoopManager.IsCoopModeEnabled())
//	{
//		char szAchievement[96];
//		CLogicAchievement(iCaller).GetAchievementEventID(szAchievement, sizeof(szAchievement));
//		if (strncmp(szAchievement, ACHIEVEMENT_PREFIX, strlen(ACHIEVEMENT_PREFIX)) == 0)
//		{
//			// broadcast achievement ui element to every player
//			Event hAchievementEvent = CreateEvent("achievement_event");
//			hAchievementEvent.SetString("achievement_name", szAchievement[strlen(ACHIEVEMENT_PREFIX)]);
//			hAchievementEvent.SetInt("cur_val", 1);
//			hAchievementEvent.SetInt("max_val", 1);
//			hAchievementEvent.Fire();
//		}
//	}
//
//	return Plugin_Continue;
//}