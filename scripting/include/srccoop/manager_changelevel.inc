#if defined _srccoop_manager_changelevel_included
 #endinput
#endif
#define _srccoop_manager_changelevel_included

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
			for(int j = 1; j <= MaxClients; j++)
			{
				if(iClientVoteIndicies[j] == i) pChangeLvlLump.votes++;
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
		
		if (g_pConvarEndWaitDisplayMode.BoolValue)
		{
			Format(szDisplay, sizeof(szDisplay), "Changing map (%d)\n", iSecondsLeft);
			
			for (int i = 0; i < pSorted.Length; i++)
			{
				pSorted.GetArray(i, pChangeLvlLump);
				if(pChangeLvlLump.m_pChangeLevel.IsValid())
				{
					pChangeLvlLump.m_pChangeLevel.GetMapName(szMapName, sizeof(szMapName));
					Format(szDisplay, sizeof(szDisplay), "%s%s (%d players) ", szDisplay, szMapName, pChangeLvlLump.votes);
				}
			}
			
			SetHudTextParams(-1.0, 0.05, 1.0, 200, 200, 200, 255, 0, 0.5, 0.1, 0.1);
			
			for (int i = 1; i <= MaxClients; i++)
			{
				if (IsClientInGame(i))
					ShowHudText(i, 2, szDisplay);
			}
		}
		else
		{
			Panel panel = new Panel();
			
			Format(szDisplay, sizeof(szDisplay), "Changing map.. (%d)", iSecondsLeft);
			panel.SetTitle(szDisplay);
			
			for (int i = 0; i < pSorted.Length; i++)
			{
				pSorted.GetArray(i, pChangeLvlLump);
				if(pChangeLvlLump.m_pChangeLevel.IsValid())
				{
					pChangeLvlLump.m_pChangeLevel.GetMapName(szMapName, sizeof(szMapName));
					Format(szDisplay, sizeof(szDisplay), "%s (%d players)", szMapName, pChangeLvlLump.votes);
					panel.DrawItem(szDisplay);
				}
			}
			
			for(int i = 1; i <= MaxClients; i++)
			{
				if(IsClientInGame(i))
					panel.Send(i, ChangeLevelStatusPanelCallback, 1);
			}
			
			delete panel;
		}
		
		delete pSorted;
	}

	public bool SetPlayerVote(CChangelevel pChangelevel, CBasePlayer pPlayer, int[] iClientVoteIndicies)
	{
		int iPlayer = pPlayer.GetEntIndex();
		ChangeLevelVoteLump pChangeLvlLump;
		for (int i = 0; i < this.Length; i++)
		{
			if(this.GetArray(i, pChangeLvlLump) && pChangeLvlLump.m_pChangeLevel == pChangelevel)
			{
				if(iClientVoteIndicies[iPlayer] != i)
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
	if(item1.votes < item2.votes)
		return 1;
	if(item2.votes < item1.votes)
		return -1;
	return 0;
}

public int ChangeLevelStatusPanelCallback(Menu menu, MenuAction action, int param1, int param2) {return -1;}
