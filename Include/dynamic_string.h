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
	dynamic_string(const char* _conststring, size_t _len) : dynamic_array(), conststring(NULL)
	{
		SetSize(_len + 1);
		memcpy((char*)ptr, _conststring, (_len) * sizeof(char));
		((char*)ptr)[_len] = '\0';
	}
#ifdef _STRING_
	dynamic_string(const std::string& rhs) : dynamic_array(), conststring(NULL)
	{
		SetSize(rhs.size()+1);
		rhs._Copy_s(ptr, Size(), rhs.size()+1);
	}
#endif

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

	bool operator ==(const dynamic_string& rhs)
	{
		return *this == (const char *)rhs;
	}

	bool operator ==(const char* rhs)
	{
		const char* slhs = *this;
		const char* srhs = rhs;
		// both empty
		if (!slhs && !srhs)
		{
			return true;
		}
		// one set and the other not
		if (!slhs || !srhs)
		{
			return false;
		}
		return (_stricmp(slhs, srhs) == 0);
	}

	bool operator !=(const dynamic_string& rhs)
	{
		return *this != (const char *)rhs;
	}

	bool operator !=(const char* rhs)
	{
		const char* slhs = *this;
		const char* srhs = rhs;
		// both empty
		if (!slhs && !srhs)
		{
			return false;
		}
		// one set and the other not
		if (!slhs || !srhs)
		{
			return true;
		}
		return (_stricmp(slhs, srhs) != 0);
	}

	// Including null terminator, in bytes
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

	// Excluding null terminator, in characters
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

	// Including null terminator, in bytes
	size_t MaxSize() const
	{
		return iMax;
	}

	// Excluding null terminator, in characters
	size_t MaxLen() const
	{
		if (iMax > 0)
		{
			return iMax - 1; // discount null terminator
		}
		else
		{
			return 0;
		}
	}

	void SetWritableBufferLen(size_t iNewMaxLen)
	{
		size_t iNewMaxSize = iNewMaxLen + 1; // + 1 for null terminator
		clonebuffer(iNewMaxSize);
		SetMaxSize(iNewMaxSize);
		SetSize(iNewMaxSize);
		memset(ptr, 0, iNewMaxSize);
	}

	void Normalize()
	{
		if (!conststring && ptr)
		{
			iNum = strlen((char*)ptr) + 1;
			assert(iNum <= iMax);
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

	const char& operator [](int i) const
	{
		assert(i >= 0 && (size_t)i < Len() + 1); // allow accessing null terminator
		return ((const char*)*this)[i];
	}

	char* GetWritableBuffer()
	{
		assert(MaxLen() > 0);
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

	void ToLower()
	{
		clonebuffer();
		if (ptr)
		{
			_strlwr_s(ptr, Size());
		}
	}

	void ToUpper()
	{
		clonebuffer();
		if (ptr)
		{
			_strupr_s(ptr, Size());
		}
	}
};
