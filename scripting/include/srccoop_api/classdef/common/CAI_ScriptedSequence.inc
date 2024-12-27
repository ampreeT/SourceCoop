#pragma newdecls required
#pragma semicolon 1

methodmap CAI_ScriptedSequence < CBaseEntity
{
	public CAI_ScriptedSequence(const int iEntIndex = -1)
	{
		return view_as<CAI_ScriptedSequence>(CBaseEntity(iEntIndex));
	}
	public static CAI_ScriptedSequence Create()
	{
		return CAI_ScriptedSequence(CreateEntityByName("scripted_sequence"));
	}

	public CBaseEntity GetTargetEnt()
	{
		return CBaseEntity(GetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt"));
	}
	public void SetTargetEnt(const CBaseEntity pTarget)
	{
		SetEntPropEnt(this.entindex, Prop_Data, "m_hTargetEnt", pTarget.entindex);
	}
}
