#pragma once

#include <winsock2.h>

class VirtualObject
{
public:
	const char* Name;

	virtual void GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s) = 0;

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

	virtual void GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s);

	VirtualFolder(const char* _Name, int _iNumSubObjects, VirtualObject** _ppSubObjects)
		: VirtualObject(_Name), iNumSubObjects(_iNumSubObjects), ppSubObjects(_ppSubObjects)
	{
	}
};

class VirtualFile : public VirtualObject
{
public:
	const char* FilePath;

	const char* ContentType;
	const char* ContentSubType;

	virtual void GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s);

	VirtualFile(const char* _Name, const char* _FilePath, const char* _ContentType, const char* _ContentSubType)
		: VirtualObject(_Name), FilePath(_FilePath), ContentType(_ContentType), ContentSubType(_ContentSubType)
	{
	}
};
