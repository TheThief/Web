#pragma once

#include <winsock2.h>
#include "auto_ptr.h"

class HTTPHeader
{
public:
	const char* Header;
	const char* Value;
	HTTPHeader(const char* _Header=NULL, const char* _Value=NULL)
		: Header(_Header), Value(_Value) { };
};

class HTTPResponse
{
public:
	__int16 iStatus;
	const char* cpStatus;
	dynamic_array<HTTPHeader> Headers;

protected:
	HTTPResponse(__int16 _iStatus, const char* _cpStatus) : iStatus(_iStatus), cpStatus(_cpStatus) {};
	void sendto(SOCKET s) const;
};

class HTTPResponseHTML : public HTTPResponse
{
protected:
	long iContentLength;
	const char* Content;

public:
	HTTPResponseHTML(__int16 _iStatus, char* _cpStatus, long _iContentLength, const char* _Content);
	void sendto(SOCKET s) const;
};

class HTTPResponseFile : public HTTPResponse
{
public:
	const char* ContentType;
	const char* ContentSubType;
	int FileHandle;

	HTTPResponseFile(__int16 _iStatus, const char* _cpStatus, const char* FileName);
	~HTTPResponseFile();
	void sendto(SOCKET s) const;
};
