#pragma newdecls required
#pragma semicolon 1

methodmap CDynamicProp < CBreakableProp
{
	public CDynamicProp(const int iEntIndex = -1)
	{
		return view_as<CDynamicProp>(CBreakableProp(iEntIndex));
	}
	public static CDynamicProp Create()
	{
		return CDynamicProp(CreateEntityByName("prop_dynamic"));
	}
	public static CDynamicProp CreateAndSpawn(const char[] szModelName)
	{
		CDynamicProp pDynamicProp = CDynamicProp.Create();
		if (pDynamicProp != NULL_CBASEENTITY)
		{
			pDynamicProp.KeyValue("model", szModelName);
			pDynamicProp.Spawn();
			pDynamicProp.Activate();
		}
		return pDynamicProp;
	}
	
	public void SetAnimation(const char[] szSequenceName)
	{
		this.AcceptInputStr("SetAnimation", szSequenceName);
	}
}
