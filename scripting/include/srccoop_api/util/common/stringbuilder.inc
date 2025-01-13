#pragma newdecls required
#pragma semicolon 1

/**
 * A dynamically growing string.
 * Calling DataPack methods on StringBuilder instances is not considered safe.
 */
methodmap StringBuilder < DataPack
{
	public StringBuilder()
	{
		return view_as<StringBuilder>(new DataPack()).Reset();
	}

	property int BufferSize
	{
		public get()
		{
			DataPackPos save = this.Position;

			view_as<DataPack>(this).Reset(false);
			int size = this.ReadCell() + 1;

			this.Position = save;
			return size;
		}
	}

	/**
	 * You are responsible for passing a BufferSize or greater sized str.
	 */
	public StringBuilder ToString(char[] str)
	{
		view_as<DataPack>(this).Reset(false);
		int maxlen = this.ReadCell() + 1;

		int written;
		while (this.IsReadable())
		{
			int len = this.ReadCell();
			this.ReadString(str[written], maxlen);
			written += len;
		}
		
		return this;
	}

	public bool ToFile(File pFile, bool term = false)
	{
		char[] szOut = new char[this.BufferSize];
		this.ToString(szOut);
		return pFile.WriteString(szOut, term);
	}

	public StringBuilder Append(const char[] str)
	{
		DataPackPos save = this.Position;

		int len = strlen(str);

		view_as<DataPack>(this).Reset(false);
		int total = this.ReadCell() + len;
		view_as<DataPack>(this).Reset(false);
		this.WriteCell(total, false);

		this.Position = save;

		this.WriteCell(len, true);
		this.WriteString(str, true);
		return this;
	}

	/**
	 * Alternative String append variant.
	 * Because of optimization of the limit, the string needs to be temporarily written to,
	 * so const is not possible here.
	 */
	public StringBuilder AppendEx(char[] str, int limit = cellmax)
	{
		DataPackPos save = this.Position;

		int len = strlen(str);
		char bkpchr = EOS;
		
		if (limit < len)
		{
			bkpchr = str[limit];
			str[limit] = EOS;
			len = limit;
		}

		view_as<DataPack>(this).Reset(false);
		int total = this.ReadCell() + len;
		view_as<DataPack>(this).Reset(false);
		this.WriteCell(total, false);

		this.Position = save;

		this.WriteCell(len, true);
		this.WriteString(str, true);

		if (bkpchr != EOS)
		{
			str[limit] = bkpchr;
		}

		return this;
	}

	public StringBuilder AppendInt(int value)
	{
		char szValue[12]; IntToString(value, szValue, sizeof(szValue));
		return this.Append(szValue);
	}

	public StringBuilder AppendFloat(float value, char[] fmt = "%.6f")
	{
		char szValue[32]; FormatEx(szValue, sizeof(szValue), fmt, value);
		return this.Append(szValue);
	}

	public StringBuilder AppendBuilder(StringBuilder sb)
	{
		char[] szBuffer = new char[sb.BufferSize];
		sb.ToString(szBuffer);
		return this.Append(szBuffer);
	}

	/**
	 * @return false on error, otherwise true
	 */
	public bool AppendFile(File pFile, int iBufferSize = 1024)
	{
		char[] szBuffer = new char[iBufferSize];
		for (;;)
		{
			int c = pFile.ReadString(szBuffer, iBufferSize);
			if (c == -1)
				return false;
			if (c == 0)
				break;
			
			this.Append(szBuffer);
		}
		return true;
	}

	/**
	 * Appends whole line into the builder, including line endings.
	 * If the implementation here confuses you, see https://github.com/alliedmodders/sourcemod/issues/1500
	 * 
	 * @return false on error, otherwise true
	 */
	public bool AppendFileLine(File pFile)
	{
		static char bf[100];
		do
		{
			if (!pFile.ReadLine(bf, sizeof(bf)))
			{
				if (pFile.EndOfFile())
					break;
				return false;
			}
			
			this.Append(bf);
		} while (StrContains(bf, "\n") == -1);

		return true;
	}

	public StringBuilder Reset()
	{
		view_as<DataPack>(this).Reset(true);
		this.WriteCell(0);
		return this;
	}
}