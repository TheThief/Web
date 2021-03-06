#include <string.h>
#include <string>

#include "virtual_file.h"

#include "dynamic_string.h"
#include "responses.h"
#include "HTTPResponse.h"
#include "mimetypes.h"

const char* pDefaultFile = "index.html";
extern Mimetypes _mimetypes;

dynamic_ptr<HTTPResponse> VirtualFolder::GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	if (pPartialPath[0] != '/')
	{
		// Needs changing to a "301 Permenantly Moved"
		return status404;
	}

	pPartialPath++;
	size_t iNameLen = strcspn(pPartialPath, "?");

	if (iNameLen == 0)
	{
		pPartialPath = pDefaultFile;
		iNameLen = strcspn(pPartialPath, "?");
	}

	for (int i = 0; i < iNumSubObjects; i++)
	{
		if (strlen(ppSubObjects[i]->Name) <= iNameLen)
		{
			if (strncmp(pPartialPath, ppSubObjects[i]->Name, iNameLen) == 0)
			{
				return ppSubObjects[i]->GetFromPath(method, host, pFullPath, pPartialPath + iNameLen, s);
			}
		}
	}

	return status404;
}

#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <errno.h>
extern void error(char *msg);

dynamic_ptr<HTTPResponse> PhysicalFolder::GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	if (pPartialPath[0] != '/')
	{
		// Needs changing to a "301 Permenantly Moved"
		return status404;
	}

	dynamic_ptr<HTTPResponse> Response = VirtualFolder::GetFromPath(method, host, pFullPath, pPartialPath, s);
	if (Response->iStatus == 404)
	{
		dynamic_string partialfile(pPartialPath, strcspn(pPartialPath, "?"));
		dynamic_string fullfile(pFullPath, strcspn(pFullPath, "?"));

		int FileHandle;
		dynamic_string FileName = FilePath;
		FileName += partialfile;

		// _stat?
		int attrib = GetFileAttributesA((LPCSTR)FileName);
		if (attrib == INVALID_FILE_ATTRIBUTES)
		{
			int iError = GetLastError();
			if (iError == ERROR_FILE_NOT_FOUND || iError == ERROR_PATH_NOT_FOUND)
			{
				return status404;
			}
			else if (iError == ERROR_ACCESS_DENIED)
			{
				return status403;
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
				std::shared_ptr<HTTPResponse> Response301 = std::make_shared<HTTPResponseHTML>(status301);
				Response301->Headers.AddItem(HTTPHeader("Location", dynamic_string("http://") + host + fullfile + "/"));
				return std::move(Response301);
			}
			else
			{
				FileName += pDefaultFile;
			}
		}

		errno_t err = _sopen_s(&FileHandle, (const char*)FileName, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL, _SH_DENYWR, 0);
		if (err == EACCES)
		{
			return status403;
		}
		if (err == 0)
		{
			dynamic_string fileext;
			fileext.SetWritableBufferLen(32);
			_splitpath_s((const char*)FileName, nullptr, 0, nullptr, 0, nullptr, 0, fileext.GetWritableBuffer(), fileext.MaxSize());
			fileext.Normalize();
			return std::make_shared<HTTPResponseFile>(200, "OK", FileHandle, _mimetypes.getType((const char*)fileext), _mimetypes.getSubType((const char*)fileext));
		}
		else if (err == ENOENT)
		{
			return status404;
		}
		else
		{
			return status500;
		}
	}
	else
	{
		return Response;
	}
}

dynamic_ptr<HTTPResponse> VirtualFile::GetFromPath(dynamic_string method, dynamic_string host, const char* pFullPath, const char* pPartialPath, SOCKET s) const
{
	size_t iNameLen = strcspn(pPartialPath, "?");

	if (iNameLen == 0)
	{
		return std::make_shared<HTTPResponseFile>(200, "OK", FilePath, ContentType, ContentSubType);
	}

	return status404;
}
