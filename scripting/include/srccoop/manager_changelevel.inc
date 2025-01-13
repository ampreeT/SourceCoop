#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Changelevel extension of manager
//------------------------------------------------------

enum struct ChangeLevelVoteLump
{
	CChangelevel m_pChangeLevel;
	int votes;
}

methodmap ChangeLevelManager < ArrayList
{
	public ChangeLevelManager()
	{
		return view_as<ChangeLevelManager>(new ArrayList(sizeof(ChangeLevelVoteLump)));
	}

	public void UpdateVotes(int[] iClientVoteIndicies)
	{
		ChangeLevelVoteLump pChangeLvlLump;
		for (int i = 0; i < this.Length; i++)
		{
			this.GetArray(i, pChangeLvlLump);
			pChangeLvlLump.votes = 0;
			for (int j = 1; j <= MaxClients; j++)
			{
				if (iClientVoteIndicies[j] == i)
					pChangeLvlLump.votes++;
			}
			this.SetArray(i, pChangeLvlLump);
		}
	}

	public ChangeLevelManager Sorted()
	{
		ChangeLevelManager pClone = view_as<ChangeLevelManager>(this.Clone());
		pClone.SortCustom(view_as<SortFuncADTArray>(ChangeLvlLumpSortFunc));
		return pClone;
	}

	public void UpdateStatus(int iSecondsLeft)
	{
		char szMapName[MAX_MAPNAME], szDisplay[256];
		ChangeLevelManager pSorted = this.Sorted();
		ChangeLevelVoteLump pChangeLvlLump;
		
		// Purge invalid entities
		for (int i = 0; i < pSorted.Length; i++)
		{
			if (!view_as<CChangelevel>(pSorted.Get(i, ChangeLevelVoteLump::m_pChangeLevel)).IsValid())
			{
				pSorted.Erase(i--);
			}
		}

		if (g_pConvarEndWaitDisplayMode.BoolValue)
		{
			SetHudTextParams(-1.0, 0.05, 1.0, 200, 200, 200, 255, 0, 0.5, 0.1, 0.1);
			
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsClientInGame(client))
				{
					SetGlobalTransTarget(client);
					FormatEx(szDisplay, sizeof(szDisplay), "%t (%d)\n", "changing map", iSecondsLeft);
			
					for (int i = 0; i < pSorted.Length; i++)
					{
						pSorted.GetArray(i, pChangeLvlLump);
						pChangeLvlLump.m_pChangeLevel.GetMapName(szMapName, sizeof(szMapName));
						Format(szDisplay, sizeof(szDisplay), "%s%s (%dx) ", szDisplay, szMapName, pChangeLvlLump.votes);
					}
					
					ShowHudText(client, 2, szDisplay);
				}
			}
		}
		else
		{
			for (int client = 1; client <= MaxClients; client++)
			{
				if (IsClientInGame(client))
				{	
					SetGlobalTransTarget(client);

					Panel panel = new Panel();
					FormatEx(szDisplay, sizeof(szDisplay), "%t.. (%d)", "changing map", iSecondsLeft);
					panel.SetTitle(szDisplay);
					
					for (int i = 0; i < pSorted.Length; i++)
					{
						pSorted.GetArray(i, pChangeLvlLump);
						pChangeLvlLump.m_pChangeLevel.GetMapName(szMapName, sizeof(szMapName));
						Format(szDisplay, sizeof(szDisplay), "%s (%d %t)", szMapName, pChangeLvlLump.votes, pChangeLvlLump.votes == 1? "player" : "players");
						panel.DrawItem(szDisplay);
					}

					panel.Send(client, ChangeLevelStatusPanelCallback, 1);
					delete panel;
				}
			}
		}
		
		delete pSorted;
	}

	public bool SetPlayerVote(CChangelevel pChangelevel, CBasePlayer pPlayer, int[] iClientVoteIndicies)
	{
		int iPlayer = pPlayer.entindex;
		ChangeLevelVoteLump pChangeLvlLump;
		for (int i = 0; i < this.Length; i++)
		{
			if (this.GetArray(i, pChangeLvlLump) && pChangeLvlLump.m_pChangeLevel == pChangelevel)
			{
				if (iClientVoteIndicies[iPlayer] != i)
				{
					iClientVoteIndicies[iPlayer] = i;
					this.UpdateVotes(iClientVoteIndicies);
					return true;
				}
				return false;
			}
		}
		pChangeLvlLump.m_pChangeLevel = pChangelevel;
		iClientVoteIndicies[iPlayer] = this.PushArray(pChangeLvlLump);
		this.UpdateVotes(iClientVoteIndicies);
		return true;
	}
	
	public CChangelevel GetWinner()
	{
		ChangeLevelManager pSorted = this.Sorted();
		ChangeLevelVoteLump pChangeLvlLump;
		pSorted.GetArray(0, pChangeLvlLump);
		delete pSorted;
		return pChangeLvlLump.m_pChangeLevel;
	}
}

public int ChangeLvlLumpSortFunc(int index1, int index2, ArrayList array, Handle hndl)
{
	ChangeLevelVoteLump item1, item2; array.GetArray(index1, item1); array.GetArray(index2, item2);
	if (item1.votes < item2.votes)
		return 1;
	if (item2.votes < item1.votes)
		return -1;
	return 0;
}

public int ChangeLevelStatusPanelCallback(Menu menu, MenuAction action, int param1, int param2) {return -1;}
