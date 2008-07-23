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

	int Len() const
	{
		return Num();
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

	static dynamic_string printf(const char* _Format, ...)
	{
	    va_list args;
	    va_start( args, _Format );

		int n = _vscprintf(_Format, args) + 1; // + 1 for '\0'

		dynamic_string newstring;
		newstring.Expand(n);
		newstring.iNum = n;
		vsprintf_s(newstring.ptr, n, _Format, args);

		return newstring;
	}
};
