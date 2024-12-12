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
				return view_as<bool>(GameRules_GetProp("m_bTeamPlayEnabled"));
		}
	}
}
