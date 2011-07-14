#include <string.h>
#include <string>

#include "virtual_file.h"

#include "dynamic_string.h"
#include "auto_ptr.h"
#include "responses.h"
#include "HTTPResponse.h"
#include "mimetypes.h"

const char* pDefaultFile = "index.html";
extern Mimetypes _mimetypes;

const HTTPResponse* VirtualFolder::GetFromPath(dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
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
				return ppSubObjects[i]->GetFromPath(host, pFullPath, pPartialPath+iNameLen, s);
			}
		}
	}

	return &status404;
}

#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <errno.h>
extern void error(char *msg);

const HTTPResponse* PhysicalFolder::GetFromPath(dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	if (pPartialPath[0]!='/')
	{
		// Needs changing to a "301 Permenantly Moved"
		return &status404;
	}

	const HTTPResponse* pResponse = VirtualFolder::GetFromPath(host, pFullPath, pPartialPath, s);
	if (pResponse == &status404)
	{
		dynamic_string partialfile(pPartialPath, strcspn(pPartialPath,"?"));
		dynamic_string fullfile(pFullPath, strcspn(pFullPath,"?"));

		int FileHandle;
		dynamic_string FileName = FilePath;
		FileName += partialfile;

		// _stat?
		int attrib = GetFileAttributesA(FileName);
		if (attrib == INVALID_FILE_ATTRIBUTES)
		{
			int iError = GetLastError();
			if (iError == ERROR_FILE_NOT_FOUND || iError == ERROR_PATH_NOT_FOUND)
			{
				return &status404;
			}
			else if (iError == ERROR_ACCESS_DENIED)
			{
				return &status403;
			}
			else
			{
				error("Error retrieving file attributes");
			}
		}

		if (attrib & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FileName[FileName.Len() - 1] != '/')
			{
				HTTPResponse* pResponse = new HTTPResponseHTML(status301);
				pResponse->Headers.AddItem(HTTPHeader("Location", dynamic_string("http://") + host + fullfile + "/"));
				return pResponse;
			}
			else
			{
				FileName += pDefaultFile;
			}
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

const HTTPResponse* VirtualFile::GetFromPath(dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	size_t iNameLen = strcspn(pPartialPath,"?");

	if (iNameLen==0)
	{
		return new HTTPResponseFile(200, "OK", FilePath, ContentType, ContentSubType);
	}

	return &status404;
}
