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

	size_t Len() const
	{
		if (conststring)
		{
			return strlen(conststring);
		}
		else
		{
			return Num();
		}
	}

	operator const char*() const
	{
		if (conststring)
		{
			return conststring;
		}
		else
		{
			return ptr;
		}
	}

	operator char*()
	{
		clonebuffer();
		return ptr;
	}

	static dynamic_string printf(const char* _Format, ...)
	{
		va_list args;
		va_start( args, _Format );

		int n = _vscprintf(_Format, args) + 1; // + 1 for '\0'

		dynamic_string newstring;
		newstring.SetSize(n);
		vsprintf_s(newstring.ptr, n, _Format, args);

		return newstring;
	}

	void clonebuffer()
	{
		if (conststring)
		{
			size_t iLen = Len();
			if (iLen > 0)
			{
				SetSize(iLen + 1);
				memcpy((char*)ptr, conststring, iNum*sizeof(char));
			}
			conststring = NULL;
		}
		else
		{
			dynamic_array::clonebuffer();
		}
	}
};
