#pragma newdecls required
#pragma semicolon 1

methodmap Task_t
{
	public static Task_t FromAddress(const Address pAddress)
	{
		return view_as<Task_t>(pAddress);
	}

	public Address GetAddress()
	{
		return view_as<Address>(this);
	}
	public sharedtasks_e GetTask()
	{
		return view_as<sharedtasks_e>(LoadFromAddress(this.GetAddress() + 0x0, NumberType_Int32));
	}
	public void SetTask(const sharedtasks_e iTask)
	{
		StoreToAddress(this.GetAddress()  + 0x0, iTask, NumberType_Int32);
	}
	public float GetTaskData()
	{
		return LoadFromAddress(this.GetAddress()  + 0x4, NumberType_Int32); 
	}
	public void SetTaskData(const float flTaskData)
	{
		StoreToAddress(this.GetAddress()  + 0x4, flTaskData, NumberType_Int32); 
	}
}
