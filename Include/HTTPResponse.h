#pragma once

#include <winsock2.h>
#include "dynamic_string.h"

class HTTPHeader
{
public:
	dynamic_string Header;
	dynamic_string Value;
	HTTPHeader(dynamic_string _Header=NULL, dynamic_string _Value=NULL)
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
