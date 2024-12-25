#pragma newdecls required
#pragma semicolon 1

methodmap CMultiplayRules
{
	public static bool IsTeamplay()
	{
		switch (GetEngineVersion())
		{
			case Engine_BlackMesa:
				return GetTeamCount() >= 4;
			case Engine_HL2DM:
				return GameRules_GetProp("m_bTeamPlayEnabled") != 0;
		}
	}
}
