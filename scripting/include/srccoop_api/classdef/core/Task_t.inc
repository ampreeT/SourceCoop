#pragma newdecls required
#pragma semicolon 1

methodmap Task_t
{
	public Task_t(const Address pAddress)
	{
		return view_as<Task_t>(pAddress);
	}

	public sharedtasks_e GetTask()
	{
		return view_as<sharedtasks_e>(LoadFromAddress(view_as<Address>(this) + 0x0, NumberType_Int32));
	}
	public void SetTask(const sharedtasks_e iTask)
	{
		StoreToAddress(view_as<Address>(this) + 0x0, iTask, NumberType_Int32);
	}
	public float GetTaskData()
	{
		return LoadFromAddress(view_as<Address>(this) + 0x4, NumberType_Int32); 
	}
	public void SetTaskData(const float flTaskData)
	{
		StoreToAddress(view_as<Address>(this) + 0x4, flTaskData, NumberType_Int32); 
	}
}
