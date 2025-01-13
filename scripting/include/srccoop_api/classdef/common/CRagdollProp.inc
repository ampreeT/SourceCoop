#pragma newdecls required
#pragma semicolon 1

static Handle g_pCreateServerRagdoll;

methodmap CRagdollProp < CBaseAnimating 
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szCreateServerRagdoll[] = "CreateServerRagdoll";
		StartPrepSDKCall(SDKCall_Static);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Signature, szCreateServerRagdoll))
			LogMessage("Could not obtain gamedata signature %s", szCreateServerRagdoll);
		else
		{
			PrepSDKCall_SetReturnInfo(SDKType_CBaseEntity, SDKPass_Pointer);
			PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer); // CBaseAnimating *pAnimating
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // int forceBone
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // const CTakeDamageInfo &info
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain); // int collisionGroup
			PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain); // bool bUseLRURetirement
			if (!(g_pCreateServerRagdoll = EndPrepSDKCall()))
				SetFailState("Could not prep SDK call %s", szCreateServerRagdoll);
		}
	}
	public CRagdollProp(const int iEntIndex = -1)
	{
		return view_as<CRagdollProp>(CBaseAnimating(iEntIndex));
	}
}

stock CRagdollProp CreateServerRagdoll(const CBaseAnimating pAnimating, const int iForceBone, const CTakeDamageInfo pInfo, const Collision_Group_t eCollisionGroup, const bool bUseLRURetirement = false)
{
	return CRagdollProp
	(
		SDKCall(g_pCreateServerRagdoll, pAnimating.entindex, iForceBone, pInfo, eCollisionGroup, bUseLRURetirement)
	);
}