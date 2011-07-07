#include "../include/auto_ptr.h"
#include "../include/dynamic_string.h"

//http://www.ietf.org/rfc/rfc2396.txt
//reserved   = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
//unreserved = alphanum | mark
//mark       = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"

bool IsHex()
{
}

class URI
{
	dynamic_string uri;
	dynamic_string query;
	bool DecodeString(dynamic_string &in_uri);
}

bool URI::DecodeString(dynamic_string &in_uri)
{
	uri = in_uri;
	char* pWrite = uri.GetWritableBuffer();
	const char* pRead = (const char*)in_uri;
	if (!pRead || !*pRead)
		return false;

	do
	{

	}
	while (*++pRead);
}
