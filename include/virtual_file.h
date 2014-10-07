#pragma once

#include <winsock2.h>
#include "dynamic_string.h"
#include "HTTPResponse.h"

class VirtualObject
{
public:
	const char* Name;

	virtual const HTTPResponse* GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const = 0;

	VirtualObject(const char* _Name)
		: Name(_Name)
	{
	}
};

class VirtualFolder : public VirtualObject
{
public:
	int iNumSubObjects;
	VirtualObject** ppSubObjects;

	virtual const HTTPResponse* GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const;

	VirtualFolder(const char* _Name, int _iNumSubObjects, VirtualObject** _ppSubObjects)
		: VirtualObject(_Name), iNumSubObjects(_iNumSubObjects), ppSubObjects(_ppSubObjects)
	{
	}
};

class PhysicalFolder : public VirtualFolder
{
public:
	dynamic_string FilePath;

	virtual const HTTPResponse* GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const;
	PhysicalFolder(const char* _Name, dynamic_string _FilePath, int _iNumSubObjects, VirtualObject** _ppSubObjects)
		: VirtualFolder(_Name, _iNumSubObjects, _ppSubObjects), FilePath(_FilePath)
	{
	}
};

class VirtualFile : public VirtualObject
{
public:
	const char* FilePath;

	const char* ContentType;
	const char* ContentSubType;

	virtual const HTTPResponse* GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const;

	VirtualFile(const char* _Name, const char* _FilePath, const char* _ContentType, const char* _ContentSubType)
		: VirtualObject(_Name), FilePath(_FilePath), ContentType(_ContentType), ContentSubType(_ContentSubType)
	{
	}
};
