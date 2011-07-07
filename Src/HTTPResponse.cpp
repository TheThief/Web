#include <winsock2.h>

#include <stdio.h>

#include "../include/HTTPResponse.h"
#include "../include/auto_ptr.h"
#include "../include/Settings.h"

extern Settings _settings;

#define SEND_BUFFER_LENGTH (1*1024)

HTTPResponse::HTTPResponse(__int16 _iStatus, dynamic_string _cpStatus)
	: iStatus(_iStatus), cpStatus(_cpStatus)
{
	Headers.AddItem(HTTPHeader("Cache-Control", "public, max-age=3600"));
	Headers.AddItem(HTTPHeader("Connection", "close"));
}

void HTTPResponse::sendto(SOCKET s) const
{
	int n = 0;
	auto_ptr_array<char> send_buffer = new char[SEND_BUFFER_LENGTH];
	n += sprintf_s(send_buffer, SEND_BUFFER_LENGTH,
		"HTTP/1.1 %d %s\r\n", iStatus, (const char*)cpStatus);
#if !_DEBUG
	if (_settings.bDebugLog)
	{
		printf("%x: %s\n", s, send_buffer);
	}
#endif

	for (size_t i=0; i<Headers.Num(); i++)
	{
		n += sprintf_s(send_buffer+n, SEND_BUFFER_LENGTH-n,
			"%s: %s\r\n", (const char*)Headers[i].Header, (const char*)Headers[i].Value);
	}

#if _DEBUG
	if (_settings.bDebugLog)
	{
		printf("%x:\n%s\n", s, send_buffer);
	}
#endif

	n += sprintf_s(send_buffer+n, SEND_BUFFER_LENGTH-n, "\r\n");

	send(s, send_buffer, n, 0);
}

HTTPResponseHTML::HTTPResponseHTML(__int16 _iStatus, dynamic_string _cpStatus, long _iContentLength, dynamic_string _Content)
	: HTTPResponse(_iStatus, _cpStatus), iContentLength(_iContentLength), Content(_Content)
{
	dynamic_string sContentTypeHeader = dynamic_string::printf("text/html; charset=%s", _settings.defaultCharset.c_str());
	Headers.AddItem(HTTPHeader("Content-Type", sContentTypeHeader));
	dynamic_string sContentLength = dynamic_string::printf("%d", iContentLength);
	Headers.AddItem(HTTPHeader("Content-Length", sContentLength));
};

void HTTPResponseHTML::sendto(SOCKET s) const
{
	HTTPResponse::sendto(s);
	send(s, Content, iContentLength, 0);

#if _DEBUG
	if (_settings.bDebugLog)
	{
		puts(Content);
	}
#endif
}

#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <errno.h>

HTTPResponseFile::HTTPResponseFile(__int16 _iStatus, dynamic_string _cpStatus, dynamic_string FileName, dynamic_string _ContentType, dynamic_string _ContentSubType)
	: HTTPResponse(_iStatus,_cpStatus), ContentType(_ContentType), ContentSubType(_ContentSubType)
{
	_sopen_s(&FileHandle,FileName,_O_BINARY|_O_RDONLY|_O_SEQUENTIAL,_SH_DENYWR,0);

	dynamic_string sContentTypeHeader = dynamic_string::printf("%s/%s", (const char*)ContentType, (const char*)ContentSubType);
	if (ContentType == "Text")
	{
		sContentTypeHeader += dynamic_string::printf("; charset=%s", _settings.defaultCharset.c_str());
	}
	Headers.AddItem(HTTPHeader("Content-Type", sContentTypeHeader));

	long iContentLength = _filelength(FileHandle);
	dynamic_string sContentLength = dynamic_string::printf("%d", iContentLength);
	Headers.AddItem(HTTPHeader("Content-Length", sContentLength));
}

HTTPResponseFile::HTTPResponseFile(__int16 _iStatus, dynamic_string _cpStatus, int _FileHandle, dynamic_string _ContentType, dynamic_string _ContentSubType)
	: HTTPResponse(_iStatus,_cpStatus), FileHandle(_FileHandle), ContentType(_ContentType), ContentSubType(_ContentSubType)
{
	dynamic_string sContentTypeHeader = dynamic_string::printf("%s/%s", (const char*)ContentType, (const char*)ContentSubType);
	if (ContentType == "Text")
	{
		sContentTypeHeader += dynamic_string::printf("; charset=%s", _settings.defaultCharset.c_str());
	}
	Headers.AddItem(HTTPHeader("Content-Type", sContentTypeHeader));

	long iContentLength = _filelength(FileHandle);
	dynamic_string sContentLength = dynamic_string::printf("%d", iContentLength);
	Headers.AddItem(HTTPHeader("Content-Length", sContentLength));
}

HTTPResponseFile::~HTTPResponseFile()
{
	_close(FileHandle);
}

void HTTPResponseFile::sendto(SOCKET s) const
{
	HTTPResponse::sendto(s);

#if _DEBUG
	bool bText;
	if (_settings.bDebugLog)
	{
		bText = ( memcmp(ContentType,"text",5) == 0 );
		if (!bText)
		{
			puts("--Non-text-data--");
		}
	}
#endif
	int n = 0;
	auto_ptr_array<char> send_buffer = new char[SEND_BUFFER_LENGTH];
	while((n =_read(FileHandle, send_buffer, SEND_BUFFER_LENGTH)) > 0)
	{
		n = send(s, send_buffer, n, 0);
#if _DEBUG
		if (_settings.bDebugLog)
		{
			if (bText)
			{
				_write(stdout->_file, send_buffer, n);
			}
		}
#endif
	}
#if _DEBUG
	if (_settings.bDebugLog)
	{
		puts("");
		puts("");
	}
#endif
}
