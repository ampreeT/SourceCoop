#pragma newdecls required
#pragma semicolon 1

methodmap CPlayerResource < CBaseEntity
{
	public CPlayerResource(const int iEntIndex = -1)
	{
		return view_as<CPlayerResource>(CBaseEntity(iEntIndex));
	}
	public static CPlayerResource Get()
	{
		return CPlayerResource(GetPlayerResourceEntity());
	}

	public bool IsValid()
	{
		return view_as<CBaseEntity>(this).IsValid() && this.GetEntIndex() > 0;
	}
	public int GetPing(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iPing", _, pPlayer.entindex);
	}
	public void SetPing(CBasePlayer pPlayer, int iPing)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iPing", iPing, _, pPlayer.entindex);
	}
	public int GetScore(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iScore", _, pPlayer.entindex);
	}
	public void SetScore(CBasePlayer pPlayer, int iScore)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iScore", iScore, _, pPlayer.entindex);
	}
	public int GetDeaths(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iDeaths", _, pPlayer.entindex);
	}
	public void SetDeaths(CBasePlayer pPlayer, int iDeaths)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iDeaths", iDeaths, _, pPlayer.entindex);
	}
	public bool IsConnected(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bConnected", _, pPlayer.entindex));
	}
	public void SetConnected(CBasePlayer pPlayer, bool bConnected)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bConnected", bConnected, _, pPlayer.entindex);
	}
	public int GetTeam(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iTeam", _, pPlayer.entindex);
	}
	public void SetTeam(CBasePlayer pPlayer, int iTeam)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iTeam", iTeam, _, pPlayer.entindex);
	}
	public bool IsAlive(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bAlive", _, pPlayer.entindex));
	}
	public void SetAlive(CBasePlayer pPlayer, bool bAlive)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bAlive", bAlive, _, pPlayer.entindex);
	}
	public int GetHealth(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iHealth", _, pPlayer.entindex);
	}
	public void SetHealth(CBasePlayer pPlayer, int iHealth)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iHealth", iHealth, _, pPlayer.entindex);
	}

	#if defined SRCCOOP_BLACKMESA
	public int GetAccountId(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iAccountID", _, pPlayer.entindex);
	}
	public void SetAccountId(CBasePlayer pPlayer, int iAccountID)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iAccountID", iAccountID, _, pPlayer.entindex);
	}
	public bool IsValidPlayer(CBasePlayer pPlayer)
	{
		return view_as<bool>(GetEntProp(this.entindex, Prop_Send, "m_bValid", _, pPlayer.entindex));
	}
	public void SetValidPlayer(CBasePlayer pPlayer, bool bValid)
	{
		SetEntProp(this.entindex, Prop_Send, "m_bValid", bValid, _, pPlayer.entindex);
	}
	public int GetFrags(CBasePlayer pPlayer)
	{
		return GetEntProp(this.entindex, Prop_Send, "m_iFrags", _, pPlayer.entindex);
	}
	public void SetFrags(CBasePlayer pPlayer, int iFrags)
	{
		SetEntProp(this.entindex, Prop_Send, "m_iFrags", iFrags, _, pPlayer.entindex);
	}
	#endif // SRCCOOP_BLACKMESA
}
