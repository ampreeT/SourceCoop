#pragma newdecls required
#pragma semicolon 1

static int g_iOffset_CParamsManger_bIsMultiplayer = -1;

methodmap CParamsManager
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		g_iOffset_CParamsManger_bIsMultiplayer = GetGamedataInt(hGameConfig, "CParamsManager::m_bIsMultiplayer");
		if (g_iOffset_CParamsManger_bIsMultiplayer < 0)
			SetFailState("Could not obtain member variable offset `CParamsManager::m_bIsMultiplayer`");
	}
	public CParamsManager(const Address pAddress)
	{
		return view_as<CParamsManager>(pAddress);
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public bool IsMultiplayer()
	{
		return LoadFromAddress(this.GetAddress() + g_iOffset_CParamsManger_bIsMultiplayer, NumberType_Int32);
	}
	public void SetMultiplayer(const bool bIsMultiplayer)
	{
		StoreToAddress(this.GetAddress() + g_iOffset_CParamsManger_bIsMultiplayer, bIsMultiplayer, NumberType_Int32);
	}
}
