#pragma newdecls required
#pragma semicolon 1

methodmap CNPC_Barnacle < CAI_BaseNPC
{
    public static void InitClassdef(const GameData hGameConfig)
    {
        /*
		char szAttachTongueToTarget[] = "CNPC_Barnacle::AttachTongueToTarget";
	    StartPrepSDKCall(SDKCall_Entity);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szAttachTongueToTarget))
			LogMessage("Could not obtain gamedata signature %s", szAttachTongueToTarget);
		else
		{
	        PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_Plain);    // TODO: Probably wrong SDKPass?
			if (!(g_pAttachTongueToTarget = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szAttachTongueToTarget);
		}
        */
    }
	public CNPC_Barnacle(const int iEntIndex = -1)
	{
		return view_as<CNPC_Barnacle>(CAI_BaseNPC(iEntIndex));
	}

    public CRagdollProp GetRagdoll()
    {
        return CRagdollProp(GetEntPropEnt(this.entindex, Prop_Data, "m_hRagdoll"));
    }
    public void SetRagdoll(const CRagdollProp pRagdoll)
    {
        SetEntPropEnt(this.entindex, Prop_Data, "m_hRagdoll", pRagdoll.entindex);
    }
    public void GetLastEnemyPosition(float vec3Buffer[3])
    {
        GetEntPropVector(this.entindex, Prop_Data, "m_vLastEnemyPos", vec3Buffer);
    }
    public void SetLastEnemyPosition(const float vec3Position[3])
    {
        SetEntPropVector(this.entindex, Prop_Data, "m_vLastEnemyPos", vec3Position);
    }
    public bool IsSwallowingPrey()
    {
        return GetEntProp(this.entindex, Prop_Data, "m_bSwallowingPrey") != 0;
    }
    public bool SetSwallowingPrey(const bool bSwallowingPrey)
    {
        SetEntProp(this.entindex, Prop_Data, "m_bSwallowingPrey", bSwallowingPrey);
    }
    public bool IsLiftingPrey()
    {
        return GetEntProp(this.entindex, Prop_Data, "m_bLiftingPrey") != 0;
    }
    public bool SetLiftingPrey(const bool bLiftingPrey)
    {
        SetEntProp(this.entindex, Prop_Data, "m_bLiftingPrey", bLiftingPrey);
    }
}
