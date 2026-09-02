#pragma newdecls required
#pragma semicolon 1

static int g_iOffset_m_szGameDir = -1;

methodmap SVC_ServerInfo
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		g_iOffset_m_szGameDir = GetGamedataInt(hGameConfig, "SVC_ServerInfo::m_szGameDir");
		if (g_iOffset_m_szGameDir < 0)
			SetFailState("Could not obtain member variable offset `SVC_ServerInfo::m_szGameDir`");
	}
	public static SVC_ServerInfo FromAddress(const Address pAddress)
	{
		return view_as<SVC_ServerInfo>(pAddress);
	}
	public Address GetAddress()
	{
		return view_as<Address>(this);
	}

	public bool GetGameDir(char[] szBuffer, const int iMaxLength)
	{
		return LoadStringFromAddress(LoadFromAddress(this.GetAddress() + g_iOffset_m_szGameDir, NumberType_Int32), szBuffer, iMaxLength) != 0;
	}
	public void SetGameDirFromPtr(const Address pAddress)
	{
		StoreToAddress(this.GetAddress() + g_iOffset_m_szGameDir, pAddress, NumberType_Int32);
	}
}

/// bms linux FillServerInfo 2026-02-23
//  | Offset | Type | Member | Assigned by |
//  |---:|---|---|---|
//  | 0x00 | void * | vtable ptr (inherited polymorphic base) | implicit |
//  | 0x04 | bool | CNetMessage::m_bReliable | not touched here |
//  | 0x08 | INetChannel * | CNetMessage::m_NetChannel | not touched here |
//  | 0x0C | IServerMessageHandler * | SVC_ServerInfo::m_pMessageHandler | not touched here |
//  | 0x10 | int | m_nProtocol | = 0x18 |
//  | 0x14 | int | m_nServerCount | = GetSpawnCount() |
//  | 0x18 | bool | m_bIsDedicated | = IsDedicated() |
//  | 0x19 | bool | m_bIsHLTV | = IsHLTV() |
//  | 0x1A | bool | m_bIsReplay | = IsReplay() |
//  | 0x1B | char | m_cOS | 'L'/'W', then tolower |
//  | 0x1C | CRC32_t (uint32) | m_nMapCRC | not written in this function |
//  | 0x20 | MD5Value_t (unsigned char bits[16]) | m_nMapMD5 | copied from this+0xF1 |
//  | 0x30 | int | m_nMaxClients | = GetMaxClients() |
//  | 0x34 | int | m_nMaxClasses | = serverclasses |
//  | 0x38 | int | m_nPlayerSlot | not written here |
//  | 0x3C | float | m_fTickInterval | = GetTickInterval() |
//  | 0x40 | const char * | m_szGameDir | points to static gamedir |
//  | 0x44 | const char * | m_szMapName | = GetMapName() |
//  | 0x48 | const char * | m_szSkyName | = this + 0x91 |
//  | 0x4C | const char * | m_szHostName | = GetName() |
