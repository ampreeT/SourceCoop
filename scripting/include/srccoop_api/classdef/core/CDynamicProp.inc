#pragma newdecls required
#pragma semicolon 1

methodmap CDynamicProp < CBreakableProp
{
	public CDynamicProp(const int iEntIndex = -1)
	{
		return view_as<CDynamicProp>(CBreakableProp(iEntIndex));
	}
	public static CDynamicProp Create(const char[] szModelName)
	{
		CDynamicProp pDynamicProp = CDynamicProp(CreateEntityByName("prop_dynamic"));
		if (pDynamicProp.IsValid())
		{
			int iModelIndex = PrecacheModel(szModelName);
			if (iModelIndex)
			{
				pDynamicProp.SetModel(szModelName);
				pDynamicProp.SetModelIndex(iModelIndex);
				pDynamicProp.Activate();
				pDynamicProp.Spawn();
			}
		}
		return pDynamicProp;
	}
	public void SetAnimation(const char[] szSequenceName)
	{
		this.AcceptInputStr("SetAnimation", szSequenceName);
	}
}
