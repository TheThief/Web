#include <string.h>

#include "../include/virtual_file.h"

#include "../include/auto_ptr.h"
#include "../include/responses.h"
#include "../include/HTTPResponse.h"

const char* defaultfile = "index.html";

void VirtualFolder::GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s)
{
	if (pPartialPath[0]!='/')
	{
		// Needs changing to a "301 Permenantly Moved"
		status404.sendto(s);
		return;
	}

	pPartialPath++;
	size_t iNameLen = strcspn(pPartialPath,"/?");

	if (iNameLen==0)
	{
		pPartialPath = defaultfile;
		iNameLen = strcspn(pPartialPath,"/?");
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

	status404.sendto(s);
	return;
}

void VirtualFile::GetFromPath(const char* pFullPath, const char* pPartialPath, SOCKET s)
{
	size_t iNameLen = strcspn(pPartialPath,"/?");

	if (iNameLen==0)
	{
		HTTPResponseFile response(200, "OK", FilePath);
		response.ContentType = ContentType;
		response.ContentSubType = ContentSubType;
		response.sendto(s);
		return;
	}

	status404.sendto(s);
	return;
}
