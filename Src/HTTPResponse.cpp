#include <winsock2.h>

#include <stdio.h>

#include "../include/HTTPResponse.h"
#include "../include/auto_ptr.h"

#define SEND_BUFFER_LENGTH (1*1024)

void HTTPResponse::sendto(SOCKET s) const
{
	int n = 0;
	auto_ptr_array<char> send_buffer = new char[SEND_BUFFER_LENGTH];
	n += sprintf_s(send_buffer, SEND_BUFFER_LENGTH,
		"HTTP/1.1 %d %s\r\n", iStatus, cpStatus);
#if !_DEBUG
	puts(send_buffer);
#endif

	for (size_t i=0; i<Headers.Num(); i++)
	{
		n += sprintf_s(send_buffer+n, SEND_BUFFER_LENGTH-n,
			"%s: %s\r\n", (const char*)Headers[i].Header, (const char*)Headers[i].Value);
	}

	n += sprintf_s(send_buffer+n, SEND_BUFFER_LENGTH-n, "\r\n");

	send(s, send_buffer, n, 0);

#if _DEBUG
	puts(send_buffer);
#endif
}

HTTPResponseHTML::HTTPResponseHTML(__int16 _iStatus, char* _cpStatus, long _iContentLength, const char* _Content)
	: HTTPResponse(_iStatus, _cpStatus), iContentLength(_iContentLength), Content(_Content)
{
	Headers.AddItem(HTTPHeader("Content-Type", "text/html"));
	dynamic_string sContentLength = dynamic_string::printf("%d", iContentLength);
	Headers.AddItem(HTTPHeader("Content-Length", sContentLength));
	Headers.AddItem(HTTPHeader("Cache-Control", "public, max-age=3600"));
};

void HTTPResponseHTML::sendto(SOCKET s) const
{
	HTTPResponse::sendto(s);
	send(s, Content, iContentLength, 0);

#if _DEBUG
	puts(Content);
#endif
}

#include <fcntl.h>
#include <share.h>
#include <io.h>

HTTPResponseFile::HTTPResponseFile(__int16 _iStatus, const char* _cpStatus, const char* FileName) : HTTPResponse(_iStatus,_cpStatus)
{
	_sopen_s(&FileHandle,FileName,_O_BINARY|_O_RDONLY|_O_SEQUENTIAL,_SH_DENYWR,0);

	//Headers.AddItem(HTTPHeader("Content-Type", "text/html"));

	//long iContentLength = _filelength(FileHandle);
	//auto_ptr_array<char> sContentLength = new char[32];
	//sprintf_s(sContentLength, 32, "%d", iContentLength);
	//Headers.AddItem(HTTPHeader("Content-Length", sContentLength));
}

HTTPResponseFile::~HTTPResponseFile()
{
	_close(FileHandle);
}

void HTTPResponseFile::sendto(SOCKET s) const
{
	long iContentLength = _filelength(FileHandle);
	dynamic_string sResponse;
	sResponse += dynamic_string::printf("HTTP/1.1 %d %s\r\n", iStatus, cpStatus);
#if !_DEBUG
	puts(sResponse);
#endif

	sResponse += dynamic_string::printf("Content-Type: %s/%s\r\n", ContentType, ContentSubType);
	sResponse += dynamic_string::printf("Content-Length: %d\r\n", iContentLength);
	sResponse += dynamic_string("Cache-Control: public, max-age=3600\r\n");
	sResponse += dynamic_string("\r\n");

	send(s, sResponse, sResponse.Len(), 0);
#if _DEBUG
	puts(sResponse);
#endif

#if _DEBUG
	bool bText = ( memcmp(ContentType,"text",5) == 0 );
	if (!bText)
	{
		puts("--Non-text-data--");
		puts("");
	}
#endif
	int n = 0;
	auto_ptr_array<char> send_buffer = new char[SEND_BUFFER_LENGTH];
	while((n =_read(FileHandle, send_buffer, SEND_BUFFER_LENGTH)) > 0)
	{
		n = send(s, send_buffer, n, 0);
#if _DEBUG
		if (bText)
		{
			n = _write(stdout->_file, send_buffer, n);
		}
#endif
	}
#if _DEBUG
	n = puts("");
#endif
}
