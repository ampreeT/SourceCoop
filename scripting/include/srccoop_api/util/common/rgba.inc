#pragma newdecls required
#pragma semicolon 1

methodmap RGBA
{
	public RGBA(const int r, const int g, const int b, const int a)
	{
		return view_as<RGBA>((r & 0xFF) << 0 | (g & 0xFF) << 8 | (b & 0xFF) << 16 | (a & 0xFF) << 24);
	}
	property int r
	{
		public get()
		{
			return (view_as<int>(this) >> 0) & 0xFF;
		}
	}
	property int g
	{
		public get()
		{
			return (view_as<int>(this) >> 8) & 0xFF;
		}
	}
	property int b
	{
		public get()
		{
			return (view_as<int>(this) >> 16) & 0xFF;
		}
	}
	property int a
	{
		public get()
		{
			return (view_as<int>(this) >> 24) & 0xFF;
		}
	}
	
	public int[] ToArray()
	{
		int arr[4];
		arr[0] = this.r;
		arr[1] = this.g;
		arr[2] = this.b;
		arr[3] = this.a;
		return arr;
	}
}
