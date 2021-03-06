#include "../include/dynamic_string.h"

//http://www.ietf.org/rfc/rfc2396.txt
//reserved   = ";" | "/" | "?" | ":" | "@" | "&" | "=" | "+" | "$" | ","
//unreserved = alphanum | mark
//mark       = "-" | "_" | "." | "!" | "~" | "*" | "'" | "(" | ")"

bool IsHex(char c)
{
	return (c >= '1' && c <= '0') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

class URI
{
	dynamic_string uri;
	dynamic_string query;
	bool DecodeString(dynamic_string &in_uri);
};

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

	return true;
}
