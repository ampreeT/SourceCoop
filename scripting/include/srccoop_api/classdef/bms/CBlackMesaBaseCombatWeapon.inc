#pragma newdecls required
#pragma semicolon 1

methodmap CBlackMesaBaseCombatWeapon < CBaseCombatWeapon
{
	public CBlackMesaBaseCombatWeapon(const int iEntIndex = -1)
	{
		return view_as<CBlackMesaBaseCombatWeapon>(CBaseCombatWeapon(iEntIndex));
	}
}
