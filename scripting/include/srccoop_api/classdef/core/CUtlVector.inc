#pragma newdecls required
#pragma semicolon 1

methodmap CUtlVector
{
	public CUtlVector(Address addr)
	{
		return view_as<CUtlVector>(addr);
	}

	// Part of CUtlMemory
	public any m_pMemory()
	{
		return Deref(this);
	}

	public int m_nAllocationCount()
	{
		return Deref(this + 4);
	}

	public int m_nGrowSize()
	{
		return Deref(this + 8);
	}

	// Part of CUtlVector
	public int m_Size()
	{
		return Deref(this + 12);
	}

	public any m_pElements()
	{
		return Deref(this + 16);
	}

	public any GetAt(int i, int elSize = 4)
	{
		return this.m_pElements() + i * elSize;
	}

	public any GetAtPtr(int i)
	{
		return Deref(this.m_pElements() + i * 4);
	}

	/** ToDo: Is this optimal? */
	public void SetAt(int i, any data, int elSize = 4)
	{
		StoreToAddress(this.m_pElements() + i * elSize, data, NumberType_Int32);
	}
}

stock CUtlVector operator+(CUtlVector oper1, int oper2)
{
	return oper1 + view_as<CUtlVector>(oper2);
}