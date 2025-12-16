#pragma newdecls required
#pragma semicolon 1

methodmap CEnvScreenOverlay  < CBaseEntity
{
	public CEnvScreenOverlay(const int iEntIndex = -1)
	{
		return view_as<CEnvScreenOverlay>(CBaseEntity(iEntIndex));
	}
	
	//Create new env_screenoverlay entity.
	public static CEnvScreenOverlay Create()
	{
		return CEnvScreenOverlay(CreateEntityByName("env_screenoverlay"));
	}

	//Get overlay material texture that will be used when screenoverlay is active. Integer paramater is value from 0 to 9.
	public bool GetOverlayName(int iOverlayNumber, char[] szBuffer, const int iMaxLength)
	{
		if(iOverlayNumber < 0)
			iOverlayNumber = 0;

		if(iOverlayNumber > 9)
			iOverlayNumber = 9;

		return GetEntPropString(this.entindex, Prop_Send, "m_iszOverlayNames", szBuffer, iMaxLength, iOverlayNumber)  != 0;
	}

	//Set overlay material texture that will be used when screenoverlay is active. Integer paramater is value from 0 to 9.
	public void SetOverlayName(int iOverlayNumber, char[] szBuffer)
	{
		if(iOverlayNumber < 0)
			iOverlayNumber = 0;

		if(iOverlayNumber > 9)
			iOverlayNumber = 9;

		SetEntPropString(this.entindex, Prop_Send, "m_iszOverlayNames", szBuffer, iOverlayNumber);
	}

	//Get overlay times, from 0 to 9.
	public float GetOverlayTimes(int iOverlayNumber)
	{
		if(iOverlayNumber < 0)
			iOverlayNumber = 0;

		if(iOverlayNumber > 9)
			iOverlayNumber = 9;

		return GetEntPropFloat(this.entindex, Prop_Send, "m_flOverlayTimes", iOverlayNumber);
	}

	//Set overlay times, from 0 to 9.
	public void SetOverlayTimes(int iOverlayNumber, const float flDuration)
	{
		if(iOverlayNumber < 0)
			iOverlayNumber = 0;

		if(iOverlayNumber > 9)
			iOverlayNumber = 9;

		SetEntPropFloat(this.entindex, Prop_Send, "m_flOverlayTimes", flDuration, iOverlayNumber);
	}

	//Set next overlay start time for certain player.
	public void SetNextOverlayStartTime(const float flNextOverlayTime, const CBasePlayer pPlayer)
	{
		g_flNextOverlayTime[pPlayer.entindex] = flNextOverlayTime;
	}

	//Get next overlay start time for certain player.
	public float GetNextOverlayStartTime(const CBasePlayer pPlayer)
	{
		return g_flNextOverlayTime[pPlayer.entindex];
	}

	//Get desired overlay.
	public int GetDesiredOverlay()
	{
		return GetEntProp(this.entindex, Prop_Data, "m_iDesiredOverlay");
	}

	//Set desired overlay.
	public void SetDesiredOverlay(int iDesiredOverlay)
	{
		if(iDesiredOverlay < 0)
			iDesiredOverlay = 0;
		
		if(iDesiredOverlay > 9)
			iDesiredOverlay = 9;

		SetEntProp(this.entindex, Prop_Data, "m_iDesiredOverlay", iDesiredOverlay);
	}

	//Get next overlay will be played.
	public int GetNextOverlay(const CBasePlayer pPlayer)
	{
		return g_iNextScreenOverlayIndex[pPlayer.entindex];
	}

	//Set next overlay will be played.
	public void SetNextOverlay(int iNextOverlay, const CBasePlayer pPlayer)
	{
		if(iNextOverlay < 0)
			iNextOverlay = 0;
		
		if(iNextOverlay > 9)
			iNextOverlay = 9;

		g_iNextScreenOverlayIndex[pPlayer.entindex] = iNextOverlay;
	}

	//Returns false if this screenoverlay marked as disabled or true if this screenoverlay marked as enabled.
	public bool GetActiveState()
	{
		//HACK! Use UNUSED field to store active state
		if(GetEntProp(this.entindex, Prop_Data, "m_iHealth") != 1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	//True to mark this screenoverlay as enabled, false to mark this screenoverlay as disabled.
	public void SetActiveState(const bool bIsActive)
	{
		//HACK! Use UNUSED field to store active state
		if(bIsActive)
		{
			SetEntProp(this.entindex, Prop_Data, "m_iHealth", 1);
		}
		else
		{
			SetEntProp(this.entindex, Prop_Data, "m_iHealth", 0);
		}
	}

	public int GetActiveStoredOverlay(const CBasePlayer pPlayer)
	{
		return g_iActiveScreenOverlayEntity[pPlayer.entindex];
	}

	public void SetActiveStoredOverlay(const int iEntIndex, const CBasePlayer pPlayer)
	{
		g_iActiveScreenOverlayEntity[pPlayer.entindex] = iEntIndex; //store active screenoverlay for this player
	}

	//Fire StartOverlays input.
	public void StartOverlays(const CBasePlayer pPlayer)
	{
		AcceptEntityInput(this.entindex, "StartOverlays", pPlayer.entindex);
	}

	//Fire StopOverlays input.
	public void StopOverlays(const CBasePlayer pPlayer)
	{
		AcceptEntityInput(this.entindex, "StopOverlays", pPlayer.entindex);
	}

	//Fire StartOverlaysForEveryone input.
	public void StartOverlaysForEveryone(const CBasePlayer pPlayer)
	{
		AcceptEntityInput(this.entindex, "StartOverlaysForEveryone", pPlayer.entindex);
	}

	//Fire StopOverlaysForEveryone input.
	public void StopOverlaysForEveryone(const CBasePlayer pPlayer)
	{
		AcceptEntityInput(this.entindex, "StopOverlaysForEveryone", pPlayer.entindex);
	}

	//Fire SwitchOverlay input, defined by number from 1 to 10.
	public void SwitchOverlay(const char[] szOverlayNumber)
	{
		SetVariantString(szOverlayNumber);
		AcceptEntityInput(this.entindex, "SwitchOverlay");
	}
}