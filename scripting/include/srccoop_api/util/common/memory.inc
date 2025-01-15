#pragma newdecls required
#pragma semicolon 1

#if defined __sourcescramble_ext_included

stock MemoryPatch LoadMemPatch(GameData pGameConfig, const char[] szName, bool bEnable = true, bool bRequired = true)
{
	MemoryPatch patch = MemoryPatch.CreateFromConf(pGameConfig, szName);
	if (!patch.Validate())
	{
		if (bRequired)
			SetFailState("Failed to verify mempatch \"%s\"", szName);
		LogError("Failed to verify mempatch \"%s\"", szName);
	}
	else if (bEnable)
	{
		patch.Enable();
	}
	return patch;
}

#endif // __sourcescramble_ext_included

stock any Deref(any addr, NumberType type = NumberType_Int32)
{
	return LoadFromAddress(view_as<Address>(addr), type);
}

// Address arithmetics operators

stock Address operator+(Address oper1, int oper2)
{
	return oper1 + view_as<Address>(oper2);
}

stock Address operator+(int oper1, Address oper2)
{
	return view_as<Address>(oper1) + oper2;
}

stock Address operator-(Address oper1, int oper2)
{
	return oper1 - view_as<Address>(oper2);
}

stock Address operator-(int oper1, Address oper2)
{
	return view_as<Address>(oper1) - oper2;
}

stock bool GetBitFromInt(const int i, const int iBitIndex)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is `0..32`", iBitIndex);
	
	return (i & (1 << iBitIndex)) != 0;
}

stock void SetBitOnInt(int& i, const int iBitIndex, const bool bSetBit)
{
	if (iBitIndex < 0 || iBitIndex > 31)
		ThrowError("Received out of bounds index %d; range is `0..32`", iBitIndex);
	
	if (bSetBit)
	{
		i |= (1 << iBitIndex);
	}
	else
	{
		i &= ~(1 << iBitIndex);
	}
}

stock int NumberTypeToBytes(NumberType eNumberType)
{
	switch (eNumberType)
	{
		case NumberType_Int8: return 1;
		case NumberType_Int16: return 2;
		case NumberType_Int32: return 4;
	}
	ThrowError("Unknown NumberType %d", eNumberType);
	return 0;
}

stock NumberType BytesToNumberType(int iBytes)
{
	switch (iBytes)
	{
		case 1: return NumberType_Int8;
		case 2: return NumberType_Int16;
		case 4: return NumberType_Int32;
	}
	ThrowError("%d bytes not convertible to NumberType", iBytes);
	return NumberType_Int32;
}

// The following are attributed to github.com/nosoop/stocksoup

/**
 * Loads a null-terminated string from the given address.
 * 
 * The function will return an empty string if the address corresponds to a nullptr.  This
 * functionality is present as a workaround for when an SDKCall that expects a char pointer
 * receives a nullptr and attempts to dereference it (see alliedmodders/sourcemod/issues/874).
 * 
 * If it is necessary to differentiate between an empty string and a null pointer, check if addr
 * is null before calling the function, or pass a reference to bIsNullPointer.
 * 
 * @return Number of bytes written.  0 is returned on an empty string or a null pointer.
 */
stock int LoadStringFromAddress(Address addr, char[] buffer, int maxlen, bool &bIsNullPointer = false)
{
	if (!addr)
	{
		bIsNullPointer = true;
		return 0;
	}
	
	int c;
	char ch;
	do
	{
		ch = view_as<int>(Deref(addr + c, NumberType_Int8));
		buffer[c] = ch;
	} while (ch && ++c < maxlen - 1);
	return c;
}

/**
 * Returns an entity index from its address by attempting to read the
 * CBaseEntity::m_RefEHandle member.  This assumes the address of a CBaseEntity is
 * passed in.
 * 
 * @param pEntity		Address of an entity.
 * @return				Entity index, or -1 if not valid.
 */
stock int GetEntityFromAddress(Address pEntity)
{
	if (!pEntity)
		return -1;
	
	static int offs_RefEHandle;
	if (offs_RefEHandle)
		return LoadEntityFromHandleAddress(pEntity + offs_RefEHandle);
	
	// if we don't have it already, attempt to lookup offset based on SDK information
	// CWorld is derived from CBaseEntity so it should have both offsets
	int offs_angRotation = FindDataMapInfo(0, "m_angRotation"),
			offs_vecViewOffset = FindDataMapInfo(0, "m_vecViewOffset");
	
	if (offs_angRotation == -1)
	{
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_angRotation");
	}
	else if (offs_vecViewOffset == -1)
	{
		ThrowError("Could not find offset for ((CBaseEntity) CWorld)::m_vecViewOffset");
	}
	else if ((offs_angRotation + 0x0C) != (offs_vecViewOffset - 0x04))
	{
		char game[32];
		GetGameFolderName(game, sizeof(game));
		ThrowError("Could not confirm offset of CBaseEntity::m_RefEHandle "
				... "(incorrect assumption for game '%s'?)", game);
	}
	
	// offset seems right, cache it for the next call
	offs_RefEHandle = offs_angRotation + 0x0C;
	return GetEntityFromAddress(pEntity);
}
