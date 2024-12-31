#pragma newdecls required
#pragma semicolon 1

enum CBaseTrigger_SpawnFlags
{
	SF_TRIGGER_ALLOW_CLIENTS				= 0x01,		// Players can fire this trigger
	SF_TRIGGER_ALLOW_NPCS					= 0x02,		// NPCS can fire this trigger
	SF_TRIGGER_ALLOW_PUSHABLES				= 0x04,		// Pushables can fire this trigger
	SF_TRIGGER_ALLOW_PHYSICS				= 0x08,		// Physics objects can fire this trigger
	SF_TRIGGER_ONLY_PLAYER_ALLY_NPCS		= 0x10,		// *if* NPCs can fire this trigger, this flag means only player allies do so
	SF_TRIGGER_ONLY_CLIENTS_IN_VEHICLES		= 0x20,		// *if* Players can fire this trigger, this flag means only players inside vehicles can 
	SF_TRIGGER_ALLOW_ALL					= 0x40,		// Everything can fire this trigger EXCEPT DEBRIS!
	SF_TRIGGER_ONLY_CLIENTS_OUT_OF_VEHICLES	= 0x200,	// *if* Players can fire this trigger, this flag means only players outside vehicles can 
	SF_TRIG_PUSH_ONCE						= 0x80,		// trigger_push removes itself after firing once
	SF_TRIG_PUSH_AFFECT_PLAYER_ON_LADDER	= 0x100,	// if pushed object is player on a ladder, then this disengages them from the ladder (HL2only)
	SF_TRIG_TOUCH_DEBRIS 					= 0x400,	// Will touch physics debris objects
	SF_TRIGGER_ONLY_NPCS_IN_VEHICLES		= 0x800,	// *if* NPCs can fire this trigger, only NPCs in vehicles do so (respects player ally flag too)
	SF_TRIGGER_DISALLOW_BOTS                = 0x1000,   // Bots are not allowed to fire this trigger
}

methodmap CBaseTrigger < CBaseToggle
{
	public CBaseTrigger(const int iEntIndex = -1)
	{
		return view_as<CBaseTrigger>(CBaseToggle(iEntIndex));
	}

	property CBaseTrigger_SpawnFlags m_spawnflags
	{
		public get()
		{
			return view_as<CBaseTrigger_SpawnFlags>(view_as<CBaseEntity>(this).m_spawnflags);
		}
		public set(const CBaseTrigger_SpawnFlags eSpawnFlags)
		{
			view_as<CBaseEntity>(this).m_spawnflags = view_as<int>(eSpawnFlags);
		}
	}
	public bool IsEnabled()
	{
		return !GetEntProp(this.entindex, Prop_Data, "m_bDisabled");
	}
	public void SetEnabled(const bool bEnabled)
	{
		SetEntProp(this.entindex, Prop_Data, "m_bDisabled", !bEnabled);
	}
	// void CBaseTrigger::InitTrigger() from SDK
	public void InitTrigger()
	{
		this.SetSolidType(this.GetParent() != NULL_CBASEENTITY ? SOLID_VPHYSICS : SOLID_BSP);
		
		if (this.IsEnabled())
		{
			this.m_usSolidFlags |= FSOLID_TRIGGER;
		}
		else
		{
			this.m_usSolidFlags &= ~FSOLID_TRIGGER;
		}

		if (this.m_spawnflags & SF_TRIG_TOUCH_DEBRIS)
		{
			this.m_usSolidFlags |= FSOLID_TRIGGER_TOUCH_DEBRIS;
		}

		this.m_usSolidFlags |= FSOLID_NOT_SOLID;
		this.SetMoveType(MOVETYPE_NONE);
		static char szBuffer[PLATFORM_MAX_PATH];
		this.GetModelName(szBuffer, sizeof(szBuffer));
		this.SetModel(szBuffer); // initialize the model from name
		this.m_fEffects |= EF_NODRAW;
	}
}
