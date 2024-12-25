#pragma newdecls required
#pragma semicolon 1

static Handle g_pGetClusterForOrigin;
static Handle g_pGetPVSForCluster;
static Handle g_pCheckOriginInPVS;

methodmap CVEngineServer
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		char szGetClusterForOrigin[] = "CVEngineServer::GetClusterForOrigin";
		StartPrepSDKCall(SDKCall_Engine);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetClusterForOrigin))
			LogMessage("Could not obtain gamedata offset %s", szGetClusterForOrigin);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetClusterForOrigin = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetClusterForOrigin);
		}

		char szGetPVSForCluster[] = "CVEngineServer::GetPVSForCluster";
		StartPrepSDKCall(SDKCall_Engine);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szGetPVSForCluster))
			LogMessage("Could not obtain gamedata offset %s", szGetPVSForCluster);
		else
		{
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pGetPVSForCluster = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szGetPVSForCluster);
		}

		char szCheckOriginInPVS[] = "CVEngineServer::CheckOriginInPVS";
		StartPrepSDKCall(SDKCall_Engine);
		if (!PrepSDKCall_SetFromConf(hGameConfig, SDKConf_Virtual, szCheckOriginInPVS))
			LogMessage("Could not obtain gamedata offset %s", szCheckOriginInPVS);
		else
		{
			PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
			PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
			if (!(g_pCheckOriginInPVS = EndPrepSDKCall())) SetFailState("Could not prep SDK call %s", szCheckOriginInPVS);
		}
	}

	public static int GetClusterForOrigin(const float vec3Origin[3])
	{
		return SDKCall(g_pGetClusterForOrigin, vec3Origin);
	}
	public static MemoryBlock GetPVSForCluster(const int iCluster)
	{
		MemoryBlock pPVS = new MemoryBlock(MAX_MAP_CLUSTERS / 8);
		SDKCall(g_pGetPVSForCluster, iCluster, pPVS.Size, pPVS.Address);
		return pPVS;
	}
	public static bool CheckOriginInPVS(const float vec3Origin[3], const MemoryBlock pCheckPVS)
	{
		return SDKCall(g_pCheckOriginInPVS, vec3Origin, pCheckPVS.Address, pCheckPVS.Size) != 0;
	}
}
