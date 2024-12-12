#pragma newdecls required
#pragma semicolon 1

methodmap CVEngineServer
{
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
