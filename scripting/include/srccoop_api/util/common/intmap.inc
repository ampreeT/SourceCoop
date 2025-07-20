#pragma newdecls required
#pragma semicolon 1

// Note: There's a native implementation locked away in SM 1.13
// https://github.com/alliedmodders/sourcemod/pull/2018

methodmap IntMapSnapshot < Handle
{
	// Returns the number of keys in the map snapshot.
	property int Length
	{
		public get()
		{
			return view_as<StringMapSnapshot>(this).Length;
		}
	}
	
	// Retrieves the key of a given index in a map snapshot.
	// 
	// @param index      Key index (starting from 0).
	// @return           The key.
	// @error            Index out of range.
	public int GetKey(int index)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		view_as<StringMapSnapshot>(this).GetKey(index, buffer, sizeof(buffer));
		return StringToInt(buffer);
	}
}

methodmap IntMap < Handle
{
	// The IntMap must be freed via delete or CloseHandle().
	public IntMap()
	{
		return view_as<IntMap>(new StringMap());
	}
	
	// Sets a value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param value      Value to store at this key.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetValue(int key, any value, bool replace = true)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).SetValue(buffer, value, replace);
	}
	
	// Sets an array value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param array      Array to store.
	// @param num_items  Number of items in the array.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetArray(int key, const any[] array, int num_items, bool replace = true)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).SetArray(buffer, array, num_items, replace);
	}
	
	// Sets a string value in a Map, either inserting a new entry or replacing an old one.
	//
	// @param key        The key.
	// @param value      String to store.
	// @param replace    If false, operation will fail if the key is already set.
	// @return           True on success, false on failure.
	public bool SetString(int key, const char[] value, bool replace = true)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).SetString(buffer, value, replace);
	}
	
	// Retrieves a value in a Map.
	//
	// @param key        The key.
	// @param value      Variable to store value.
	// @return           True on success.  False if the key is not set, or the key is set 
	//                   as an array or string (not a value).
	public bool GetValue(int key, any& value)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).GetValue(buffer, value);
	}
	
	// Retrieves an array in a Map.
	//
	// @param key        The key.
	// @param array      Buffer to store array.
	// @param max_size   Maximum size of array buffer.
	// @param size       Optional parameter to store the number of elements written to the buffer.
	// @return           True on success.  False if the key is not set, or the key is set 
	//                   as a value or string (not an array).
	public bool GetArray(int key, any[] array, int max_size, int& size = 0)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).GetArray(buffer, array, max_size, size);
	}
	
	// Retrieves a string in a Map.
	//
	// @param key        The key.
	// @param value      Buffer to store value.
	// @param max_size   Maximum size of string buffer.
	// @param size       Optional parameter to store the number of bytes written to the buffer.
	// @return           True on success.  False if the key is not set, or the key is set 
	//                   as a value or array (not a string).
	public bool GetString(int key, char[] value, int max_size, int& size = 0)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).GetString(buffer, value, max_size, size);
	}
	
	// Removes a key entry from a Map.
	//
	// @param key        The key.
	// @return           True on success, false if the value was never set.
	public bool Remove(int key)
	{
		static char buffer[MAX_CHARS_INT(INT_MIN)];
		IntToString(key, buffer, sizeof(buffer));
		return view_as<StringMap>(this).Remove(buffer);
	}
	
	// Clears all entries from a Map.
	public void Clear()
	{
		view_as<StringMap>(this).Clear();
	}
	
	// Create a snapshot of the map's keys. See IntMapSnapshot.
	public IntMapSnapshot Snapshot()
	{
		return view_as<IntMapSnapshot>(view_as<StringMap>(this).Snapshot());
	}
	
	// Retrieves the number of elements in a map.
	property int Size
	{
		public get()
		{
			return view_as<StringMap>(this).Size;
		}
	}
}
