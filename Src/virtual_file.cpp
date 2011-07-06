#include <string.h>
#include <string>

#include "../include/virtual_file.h"

#include "../include/dynamic_string.h"
#include "../include/auto_ptr.h"
#include "../include/responses.h"
#include "../include/HTTPResponse.h"
#include "../include/mimetypes.h"

const char* pDefaultFile = "index.html";
extern Mimetypes _mimetypes;

const HTTPResponse* VirtualFolder::GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	if (pPartialPath[0]!='/')
	{
		// Needs changing to a "301 Permenantly Moved"
		return &status404;
	}

	pPartialPath++;
	size_t iNameLen = strcspn(pPartialPath,"?");

	if (iNameLen==0)
	{
		pPartialPath = pDefaultFile;
		iNameLen = strcspn(pPartialPath,"?");
	}

	for(int i=0; i<iNumSubObjects; i++)
	{
		if (strlen(ppSubObjects[i]->Name)<=iNameLen)
		{
			if (strncmp(pPartialPath, ppSubObjects[i]->Name, iNameLen)==0)
			{
				return ppSubObjects[i]->GetFromPath(pFullPath, pPartialPath+iNameLen, s);
			}
		}
	}

	return &status404;
}

#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <errno.h>

const HTTPResponse* PhysicalFolder::GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	if (pPartialPath[0]!='/')
	{
		// Needs changing to a "301 Permenantly Moved"
		return &status404;
	}

	const HTTPResponse* pResponse = VirtualFolder::GetFromPath(pFullPath, pPartialPath, s);
	if (pResponse == &status404)
	{
		pPartialPath++;
		size_t iNameLen = strcspn(pPartialPath,"?");

		if (iNameLen==0)
		{
			pPartialPath = pDefaultFile;
			iNameLen = strcspn(pPartialPath,"?");
		}

		int FileHandle;
		dynamic_string FileName = FilePath;
		FileName += "/";
		FileName += pPartialPath;

		// _stat?
		int attrib = GetFileAttributesA(FileName);
		if (attrib != INVALID_FILE_ATTRIBUTES && attrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			// Should have 301 if not already / terminated
			FileName += "/";
			FileName += pDefaultFile;
		}

		errno_t err = _sopen_s(&FileHandle, FileName, _O_BINARY|_O_RDONLY|_O_SEQUENTIAL, _SH_DENYWR, 0);
		if (err == EACCES)
		{
			return &status403;
		}
		if (err == 0)
		{
			dynamic_string fileext;
			fileext.SetWritableBufferLen(32);
			_splitpath_s(FileName, NULL, 0, NULL, 0, NULL, 0, fileext.GetWritableBuffer(), fileext.MaxSize());
			fileext.Normalize();
			return new HTTPResponseFile(200, "OK", FileHandle, _mimetypes.getType((const char*)fileext), _mimetypes.getSubType((const char*)fileext));
		}
		else if (err == ENOENT)
		{
			return &status404;
		}
		else
		{
			return &status500;
		}
	}
	else
	{
		return pResponse;
	}
}

const HTTPResponse* VirtualFile::GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	size_t iNameLen = strcspn(pPartialPath,"/?");

	if (iNameLen==0)
	{
		return new HTTPResponseFile(200, "OK", FilePath, ContentType, ContentSubType);
	}

	return &status404;
}
