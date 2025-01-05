#pragma newdecls required
#pragma semicolon 1

static int g_iCSoundSize = -1;

methodmap CSoundEnt < CPointEntity
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		g_iCSoundSize = hGameConfig.GetOffset("sizeof(CSound)");
		if (g_iCSoundSize < 0)
			SetFailState("Could not obtain CSound struct size");
	}
	public CSoundEnt(const int iEntIndex = -1)
	{
		return view_as<CSoundEnt>(CPointEntity(iEntIndex));
	}
	public static CSoundEnt Get()
	{
		return CSoundEnt(FindEntityByClassname(-1, "soundent"));
	}

	// Gets the index of the first sound in the active sound list.
	public void ActiveList(CSound pSound)
	{
		pSound.m_pSoundEnt = this;
		pSound.m_iSoundIndex = GetEntProp(this.entindex, Prop_Data, "m_iActiveSound");
	}
}

enum struct CSound
{
	CSoundEnt m_pSoundEnt;
	int m_iSoundIndex;

	CSoundEnt GetSoundEnt()
	{
		return this.m_pSoundEnt;
	}
	int GetSoundIndex()
	{
		return this.m_iSoundIndex;
	}
	bool IsValid()
	{
		return this.GetSoundEnt().IsValid() && this.m_iSoundIndex >= 0 && this.m_iSoundIndex < MAX_WORLD_SOUNDS_MP;
	}
	Address GetAddress()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		CSoundEnt pSoundEnt = this.GetSoundEnt();
		int iOffsetPoolStart = FindDataMapInfo(pSoundEnt.entindex, "m_SoundPool");
		if (iOffsetPoolStart == -1)
			ThrowError("Unable to retrieve offset %s", "m_SoundPool");

		return pSoundEnt.address + iOffsetPoolStart + this.GetSoundIndex() * g_iCSoundSize;
	}
	Sound_t GetType()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iType"), NumberType_Int32);
	}
	int GetVolume()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iVolume"), NumberType_Int32);
	}
	CBaseEntity GetOwner()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());
		
		return CBaseEntity(LoadEntityFromHandleAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_hOwner")));
	}
	void Next()
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());

		this.m_iSoundIndex = LoadFromAddress(GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_iNext"), NumberType_Int16);
	}
	void GetOrigin(float vecBuffer[3])
	{
		if (!this.IsValid())
			ThrowError("Attempted to use invalid CSound index %i.", this.GetSoundIndex());

		Address pAddress = GetSoundPoolPropAddress(this.GetSoundEnt(), this.GetSoundIndex(), "m_vecOrigin");
		vecBuffer[0] = LoadFromAddress(pAddress + 0x0, NumberType_Int32);
		vecBuffer[1] = LoadFromAddress(pAddress + 0x4, NumberType_Int32);
		vecBuffer[2] = LoadFromAddress(pAddress + 0x8, NumberType_Int32);
	}
	void GetSoundReactOrigin(float vecBuffer[3])
	{
		Sound_t eType = this.GetType();
		if (eType == SOUND_BULLET_IMPACT || eType == SOUND_PHYSICS_DANGER)
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner != NULL_CBASEENTITY)
			{
				pOwner.GetAbsOrigin(vecBuffer);
			}
			else
			{
				this.GetOrigin(vecBuffer);
			}
			return;
		}

		if (eType & SOUND_CONTEXT_REACT_TO_SOURCE)
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner != NULL_CBASEENTITY)
			{
				pOwner.GetAbsOrigin(vecBuffer);
				return;
			}
		}

		if ((eType & SOUND_DANGER) && (eType & SOUND_CONTEXT_FROM_SNIPER))
		{
			CBaseEntity pOwner = this.GetOwner();
			if (pOwner != NULL_CBASEENTITY)
			{
				pOwner.GetAbsOrigin(vecBuffer);
				return;
			}
		}

		this.GetOrigin(vecBuffer);
	}
}

// This function grabs the member offset within a CSound element from the base of the sound entity.
//
static Address GetSoundPoolPropAddress(CSoundEnt pSoundEnt, const int iSoundIndex, const char[] szProperty)
{
	// Obtain the start offset `CSoundEnt::m_SoundPool`
	int iOffsetPoolStart = FindDataMapInfo(pSoundEnt.entindex, "m_SoundPool");
	if (iOffsetPoolStart == -1)
		ThrowError("Unable to retrieve offset %s", "m_SoundPool");

	// obtain the offset within `CSound`
	int iOffsetFromStartProperty = FindDataMapInfo(pSoundEnt.entindex, szProperty);
	if (iOffsetFromStartProperty == -1)
		ThrowError("Unable to retrieve offset %s", szProperty);
	
	int iOffset = iOffsetFromStartProperty - iOffsetPoolStart;
	if (iOffset < 0)
		ThrowError("Internal Error: Computed offset %s is negative (%i); %s offset: %i, SoundPool offset: %i", szProperty, iOffset, szProperty, iOffsetFromStartProperty, iOffsetPoolStart);
	
	// this + offsetof(m_SoundPool[iSoundIndex]) + offsetof(property)
	return pSoundEnt.address + iOffsetPoolStart + iSoundIndex * g_iCSoundSize + iOffset;
}
