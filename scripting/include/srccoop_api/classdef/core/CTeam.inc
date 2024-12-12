#pragma newdecls required
#pragma semicolon 1

methodmap CTeam < CBaseEntity
{
	public CTeam(const int iEntIndex = -1)
	{
		return view_as<CTeam>(CBaseEntity(iEntIndex));
	}
	public static CTeam Get(const int iTeamNum)
	{
		int iEntIndex = -1;
		while ((iEntIndex = FindEntityByClassname(iEntIndex, "team_manager")) != -1)
		{
			CTeam pTeam = CTeam(iEntIndex);
			if (iTeamNum == pTeam.GetTeam())
			{
				return pTeam;
			}
		}
		return NULL_CBASEENTITY;
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetTeam()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Data, "m_iTeamNum");
	}
	public int GetScore()
	{
		return GetEntProp(this.GetEntIndex(), Prop_Send, "m_iScore");
	}
	public void SetScore(const int iScore)
	{
		SetEntProp(this.GetEntIndex(), Prop_Send, "m_iScore", iScore);
	}
}
