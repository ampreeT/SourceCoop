#pragma newdecls required
#pragma semicolon 1

methodmap CBaseTrigger < CBaseToggle
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(CBaseToggle(iEntIndex));
	}

	public bool IsEnabled()
	{
		return !GetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled");
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.GetEntIndex(), Prop_Data, "m_bDisabled", !bEnabled);
	}
	public bool CanTouchClients()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 0);		// SF_TRIGGER_ALLOW_CLIENTS 0x01
	}
	public void SetCanTouchClients(const bool bTouchClients)
	{
		// Players can fire this trigger
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 0, bTouchClients);	// SF_TRIGGER_ALLOW_CLIENTS 0x01
	}
	public bool CanTouchNPCS()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 1);		// SF_TRIGGER_ALLOW_NPCS 0x02
	}
	public void SetCanTouchNPCS(const bool bTouchNPCs)
	{
		// NPCS can fire this trigger
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 1, bTouchNPCs);	// SF_TRIGGER_ALLOW_NPCS 0x02
	}
	public bool CanTouchPushables()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 2);			// SF_TRIGGER_ALLOW_PUSHABLES 0x04
	}
	public void SetCanTouchPushables(const bool bTouchPushables)
	{
		// Pushables can fire this trigger
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 2, bTouchPushables);	// SF_TRIGGER_ALLOW_PUSHABLES 0x04
	}
	public bool CanTouchPhysics()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 3);		// SF_TRIGGER_ALLOW_PHYSICS 0x08
	}
	public void SetCanTouchPhysics(const bool bTouchPhysics)
	{
		// Physics objects can fire this trigger
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 3, bTouchPhysics);	// SF_TRIGGER_ALLOW_PHYSICS 0x08
	}
	public bool CanTouchPlayerAllyNPCS()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 4);				// SF_TRIGGER_ONLY_PLAYER_ALLY_NPCS 0x10
	}
	public void SetCanTouchPlayerAllyNPCS(const bool bTouchPlayerAllyNPCs)
	{
		// *if* NPCs can fire this trigger, this flag means only player allies do so
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 4, bTouchPlayerAllyNPCs);	// SF_TRIGGER_ONLY_PLAYER_ALLY_NPCS 0x10
	}
	public bool CanTouchClientsOnlyInVehicles()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 5);						// SF_TRIGGER_ONLY_CLIENTS_IN_VEHICLES 0x20
	}
	public void SetCanTouchClientsOnlyInVehicles(const bool bTouchClientsOnlyInVehicles)
	{
		// *if* Players can fire this trigger, this flag means only players inside vehicles can
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 5, bTouchClientsOnlyInVehicles);	// SF_TRIGGER_ONLY_CLIENTS_IN_VEHICLES 0x20
	}
	public bool CanTouchAll()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 6);	// SF_TRIGGER_ALLOW_ALL 0x40
	}
	public void SetCanTouchAll(const bool bTouchAll)
	{
		// Everything can fire this trigger EXCEPT DEBRIS!
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 6, bTouchAll);	// SF_TRIGGER_ALLOW_ALL 0x40
	}
	//public bool CanBePushedOnlyOnce()
	//{
	//    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 7);	// SF_TRIG_PUSH_ONCE 0x80
	//}
	//public void SetPushOnce(const bool bAllow)
	//{
	//	// trigger_push removes itself after firing once
	//    SetSpawnFlagOnEntity(this.GetEntIndex(), 7, bAllow);	// SF_TRIG_PUSH_ONCE 0x80
	//}
	//public bool IsPushAffectPlayerOnLadder()
	//{
	//    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 8);	// SF_TRIG_PUSH_AFFECT_PLAYER_ON_LADDER 0x100
	//}
	//public void SetPushAffectPlayerOnLadder(const bool bAllow)
	//{
	//	// if pushed object is player on a ladder, then this disengages them from the ladder (HL2only)
	//    SetSpawnFlagOnEntity(this.GetEntIndex(), 8, bAllow);	// SF_TRIG_PUSH_AFFECT_PLAYER_ON_LADDER 0x100
	//}
	public bool CanTouchClientsOnlyOutOfVehicles()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 9);							// SF_TRIGGER_ONLY_CLIENTS_OUT_OF_VEHICLES 0x200
	}
	public void SetCanTouchClientsOnlyOutOfVehicles(const bool bTouchClientsOnlyOutOfVehicles)
	{
		// *if* Players can fire this trigger, this flag means only players outside vehicles can
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 9, bTouchClientsOnlyOutOfVehicles);	// SF_TRIGGER_ONLY_CLIENTS_OUT_OF_VEHICLES 0x200
	}
	public bool CanTouchDebris()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 10);		// SF_TRIG_TOUCH_DEBRIS 0x400
	}
	public void SetCanTouchDebris(const bool bTouchDebris)
	{
		// Will touch physics debris objects
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 10, bTouchDebris);	// SF_TRIG_TOUCH_DEBRIS 0x400
	}
	public bool CanTouchNPCSOnlyInVehicles()
	{
	    return IsSpawnFlagSetOnEntity(this.GetEntIndex(), 11);					// SF_TRIGGER_ONLY_NPCS_IN_VEHICLES 0x800
	}
	public void SetCanTouchNPCSOnlyInVehicles(const bool bTouchNPCsOnlyInVehicles)
	{
		// *if* NPCs can fire this trigger, only NPCs in vehicles do so (respects player ally flag too)
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 11, bTouchNPCsOnlyInVehicles);	// SF_TRIGGER_ONLY_NPCS_IN_VEHICLES 0x800
	}
	public bool CanTouchBots()
	{
	    return !IsSpawnFlagSetOnEntity(this.GetEntIndex(), 12);	// SF_TRIGGER_DISALLOW_BOTS 0x1000
	}
	public void SetCanTouchBots(const bool bTouchBots)
	{
		// Bots are not allowed to fire this trigger
	    SetSpawnFlagOnEntity(this.GetEntIndex(), 12, !bTouchBots);	// SF_TRIGGER_DISALLOW_BOTS 0x1000
	}
	// void CBaseTrigger::InitTrigger() from SDK
	public void InitTrigger()
	{
		this.SetSolidType(this.GetParent().IsValid() ? SOLID_VPHYSICS : SOLID_BSP);
		
		int iSolidFlags = this.GetSolidFlags();	
		if (this.IsEnabled())
		{
			iSolidFlags |= FSOLID_TRIGGER;
		}
		else
		{
			iSolidFlags &= ~FSOLID_TRIGGER;
		}

		if (this.CanTouchDebris())
		{
			iSolidFlags |= FSOLID_TRIGGER_TOUCH_DEBRIS;
		}

		this.SetSolidFlags(iSolidFlags | FSOLID_NOT_SOLID);
		this.SetMoveType(MOVETYPE_NONE);
		static char szBuffer[PLATFORM_MAX_PATH];
		this.GetModelName(szBuffer, sizeof(szBuffer));
		this.SetModel(szBuffer); // initialize the model from name
		this.AddEffects(EF_NODRAW);
	}
}
