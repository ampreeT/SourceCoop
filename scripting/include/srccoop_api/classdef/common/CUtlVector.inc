#pragma newdecls required
#pragma semicolon 1

// Reference: https://github.com/Scags/SM-Memory/blob/main/pawn/sourcemod/scripting/include/smmem/vec.inc

methodmap CUtlVector
{
	public CUtlVector(Address addr)
	{
		return view_as<CUtlVector>(addr);
	}

	// Part of CUtlMemory
	property any m_pMemory
	{
		public get() {return Deref(this);}
	}
	property int m_nAllocationCount
	{
		public get() {return Deref(this + 4);}
	}
	property int m_nGrowSize
	{
		public get() {return Deref(this + 8);}
	}
	// Part of CUtlVector
	property int m_Size
	{
		public get() {return Deref(this + 12);}
	}
	property any m_pElements
	{
		public get() {return Deref(this + 16);}
	}

	public int Count()
	{
		return this.m_Size;
	}

	/**
	 * Returns data at the specified index.
	 * @param i index
	 * @param iEntrySize The size of each entry in bytes.
	 */
	public any Get(int i, int iEntrySize = 4)
	{
		return this.m_pElements + i * iEntrySize;
	}

	/**
	 * Gets dereferenced value of a pointer at the specified index.
	 * Only usable for lists of pointers.
	 * @param i index
	 */
	public any GetPtr(int i)
	{
		return Deref(this.m_pElements + i * 4);
	}

	/**
	 * Sets data at the specified index.
	 * @param i           index
	 * @param data        value to set
	 * @param iEntrySize  The size of each entry in bytes. Allowed values are 1, 2, 4.
	 */
	public void Set(int i, any data, int iEntrySize = 4)
	{
		StoreToAddress(this.m_pElements + i * iEntrySize, data, BytesToNumberType(iEntrySize));
	}
}

stock CUtlVector operator+(CUtlVector oper1, int oper2)
{
	return oper1 + view_as<CUtlVector>(oper2);
}
