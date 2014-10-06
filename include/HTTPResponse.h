#pragma once

#include <winsock2.h>
#include "dynamic_array.h"
#include "dynamic_string.h"

class HTTPHeader
{
public:
	dynamic_string Header;
	dynamic_string Value;
	HTTPHeader(dynamic_string _Header=nullptr, dynamic_string _Value=nullptr)
		: Header(_Header), Value(_Value) { };
};

class HTTPResponse
{
public:
	__int16 iStatus;
	dynamic_string cpStatus;
	dynamic_array<HTTPHeader> Headers;

protected:
	HTTPResponse(__int16 _iStatus, dynamic_string _cpStatus);

public:
	virtual void sendto(SOCKET s) const;
	virtual ~HTTPResponse() { };
};

class HTTPResponseHTML : public HTTPResponse
{
protected:
	long iContentLength;
	dynamic_string Content;

public:
	HTTPResponseHTML(__int16 _iStatus, dynamic_string _cpStatus, long _iContentLength, dynamic_string _Content);
	virtual void sendto(SOCKET s) const;
};

class HTTPResponseFile : public HTTPResponse
{
public:
	dynamic_string ContentType;
	dynamic_string ContentSubType;
	int FileHandle;

	HTTPResponseFile(__int16 _iStatus, dynamic_string _cpStatus, int _FileHandle, dynamic_string _ContentType, dynamic_string _ContentSubType);
	HTTPResponseFile(__int16 _iStatus, dynamic_string _cpStatus, dynamic_string FileName, dynamic_string _ContentType, dynamic_string _ContentSubType);
	virtual ~HTTPResponseFile();
	virtual void sendto(SOCKET s) const;
};
