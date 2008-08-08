#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "auto_ptr.h"

class dynamic_string : protected dynamic_array<char>
{
protected:
	const char* conststring;

public:
	dynamic_string() : dynamic_array(), conststring(NULL) { }
	dynamic_string(const dynamic_string& rhs) : dynamic_array(rhs), conststring(rhs.conststring) { }
	dynamic_string(const char* _conststring) : dynamic_array(), conststring(_conststring) { }

	dynamic_string& operator +=(const dynamic_string& rhs)
	{
		size_t irLen = rhs.Len();
		if (irLen > 0)
		{
			size_t iLen = Len();
			size_t iNewSize = iLen + irLen + 1; // + 1 for null terminator
			clonebuffer(irLen + 32);
			SetSize(iNewSize);
			memcpy((char*)ptr + iLen, (const char*)rhs, (irLen + 1)*sizeof(char));
		}

		return *this;
	}

	// Including null terminator
	size_t Size() const
	{
		if (conststring)
		{
			return strlen(conststring) + 1; // + 1 for null terminator
		}
		else
		{
			return Num();
		}
	}

	// Excluding null terminator
	size_t Len() const
	{
		if (conststring)
		{
			return strlen(conststring);
		}
		else
		{
			size_t iNum = Num();
			if (iNum > 0)
			{
				return Num() - 1; // discount null terminator
			}
			else
			{
				return 0;
			}
		}
	}

	operator const char*() const
	{
		if (conststring)
		{
			return conststring;
		}
		else if (ptr)
		{
			return ptr;
		}
		else
		{
			return "";
		}
	}

	operator char*()
	{
		assert(Len() > 0);
		clonebuffer();
		return ptr;
	}

	static dynamic_string printf(const char* _Format, ...)
	{
		va_list args;
		va_start( args, _Format );

		size_t iNewSize = _vscprintf(_Format, args) + 1; // + 1 for null terminator

		dynamic_string newstring;
		newstring.SetSize(iNewSize);
		vsprintf_s(newstring.ptr, iNewSize, _Format, args);

		return newstring;
	}

	void clonebuffer(size_t iSlack = 32)
	{
		size_t iLen = Len();
		if ( iLen <= 0 )
		{
			SetSize(0);
			SetMaxSize(0);
			conststring = NULL;
		}
		else
		{
			if (conststring)
			{
				size_t iNewSize = iLen + 1; // + 1 for null terminator
				size_t iNewMaxSize = iNewSize + iSlack;
				SetMaxSize(iNewMaxSize);
				SetSize(iNewSize);
				memcpy((char*)ptr, conststring, iNum*sizeof(char));
				conststring = NULL;
			}
			else
			{
				dynamic_array::clonebuffer(iSlack);
			}
		}
	}
};
