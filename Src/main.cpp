#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>

#include <string.h>
#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <tchar.h>

_Check_return_ __inline char * __CRTDECL _strspnp
( 
    _In_z_ const char * _Cpc1, 
    _In_z_ const char * _Cpc2
) 
{ 
    return _Cpc1==NULL ? NULL : ((*(_Cpc1 += strspn(_Cpc1,_Cpc2))!='\0') ? (char*)_Cpc1 : NULL); 
}

#include "settings.h"
#include "mimetypes.h"
#include "responses.h"
#include "auto_ptr.h"
#include "filetree.h"

#include "threadpool_winsock.h"

Settings _settings;
Mimetypes _mimetypes;
char *confFile = "web.conf";

extern void error(char *msg)
{
	puts(msg);

	LPTSTR pMessageBuffer;
	int iError;

	iError = GetLastError( );
	if (iError)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, iError, 0, (LPTSTR) &pMessageBuffer, 0, NULL );
		wprintf(L"System Error: %d: %s\n", iError, pMessageBuffer);
		LocalFree(pMessageBuffer);
	}

	iError = WSAGetLastError( );
	if (iError)
	{
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, iError, 0, (LPTSTR) &pMessageBuffer, 0, NULL );
		wprintf(L"Winsock Error: %d: %s\n", iError, pMessageBuffer);
		LocalFree(pMessageBuffer);
	}

	WSACleanup( );

	exit(1);
}

struct FiberData_Socket : FiberData
{
	SOCKET listensocket;
	int addressfamily;

	FiberData_Socket(LPFIBER_START_ROUTINE lpStartAddress) : FiberData(lpStartAddress) {}
};

void CALLBACK FiberProc(void* lpParameter);
void dostuff(FiberData_Socket* pFiberData, SOCKET sock);

GUID AcceptExGUID = WSAID_ACCEPTEX;
LPFN_ACCEPTEX pAcceptEx = 0;
GUID GetAcceptExSockaddrsGUID = WSAID_GETACCEPTEXSOCKADDRS;
LPFN_GETACCEPTEXSOCKADDRS pGetAcceptExSockaddrs = 0;
GUID DisconnectExGUID = WSAID_DISCONNECTEX;
LPFN_DISCONNECTEX pDisconnectEx = 0;
GUID TransmitFileGUID = WSAID_TRANSMITFILE;
LPFN_TRANSMITFILE pTransmitFile = 0;

#define ACCEPT_BUFFER_LENGTH (0*1024 + (sizeof(sockaddr_storage)+16)*2)

void GetWinsockExtensions(SOCKET socket)
{
	DWORD dwBytes;

	WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&AcceptExGUID, sizeof(GUID),
		(void**)&pAcceptEx, sizeof(void *),
		&dwBytes, 0, 0);

	WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GetAcceptExSockaddrsGUID, sizeof(GUID),
		(void**)&pGetAcceptExSockaddrs, sizeof(void *),
		&dwBytes, 0, 0);

	WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&DisconnectExGUID, sizeof(GUID),
		(void**)&pDisconnectEx, sizeof(void *),
		&dwBytes, 0, 0);

	WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&TransmitFileGUID, sizeof(GUID),
		(void**)&pTransmitFile, sizeof(void *),
		&dwBytes, 0, 0);
}

SOCKET listensock, listensock6;

int main(int argc, char *argv[])
{
	InitThreadPool();

	if (argc > 1) confFile = argv[1];
	errno_t err = 0;
	if ( (err = _access_s(confFile, 0)) == 0 ){
		_settings.load(confFile);
	}else{
		_settings.save(confFile);
	}
	_mimetypes.load(_settings.mimeTypesFile);

	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
		error("ERROR initializing Winsock");
	if ( wsaData.wVersion != MAKEWORD( 2, 2 ) )
		error("ERROR initializing Winsock: Incompatible version");

	struct sockaddr_in serv_addr;
	struct sockaddr_in6 serv_addr6;

	// IPv4
	listensock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listensock == INVALID_SOCKET)
	{
		puts("Couldn't open IPv4 socket");
	}
	else
	{
		ZeroMemory(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(_settings.port);
		if (bind(listensock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
			error("ERROR on binding");
		if (listen(listensock,5) < 0)
			error("ERROR on listening");
	}

	// IPv6
	listensock6 = WSASocket(AF_INET6, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listensock6 == INVALID_SOCKET)
	{
		puts("Couldn't open IPv6 socket");
	}
	else
	{
		ZeroMemory(&serv_addr6, sizeof(serv_addr6));
		serv_addr6.sin6_family = AF_INET6;
		serv_addr6.sin6_addr = in6addr_any;
		serv_addr6.sin6_port = htons(_settings.port);
		if (bind(listensock6, (struct sockaddr *) &serv_addr6, sizeof(serv_addr6)) < 0)
			error("ERROR on binding");
		if (listen(listensock6,5) < 0)
			error("ERROR on listening");
	}

	if (listensock == INVALID_SOCKET && listensock6 == INVALID_SOCKET)
		error("Couldn't open any sockets");

	if (listensock != INVALID_SOCKET)
	{
		BindHandle((HANDLE)listensock);

		GetWinsockExtensions(listensock);

		for (int i = 0; i < _settings.maxConnections; i++)
		{
			FiberData_Socket* pFiberData = new FiberData_Socket(&FiberProc);
			pFiberData->listensocket = listensock;
			pFiberData->addressfamily = AF_INET;
			QueueFiber(pFiberData);
		}
	}
	if (listensock6 != INVALID_SOCKET)
	{
		BindHandle((HANDLE)listensock6);

		if (listensock == INVALID_SOCKET)
		{
			GetWinsockExtensions(listensock6);
		}

		for (int i = 0; i < _settings.maxConnections; i++)
		{
			FiberData_Socket* pFiberData = new FiberData_Socket(&FiberProc);
			pFiberData->listensocket = listensock6;
			pFiberData->addressfamily = AF_INET6;
			QueueFiber(pFiberData);
		}
	}

	Sleep(INFINITE);

	return 0;
}

void CALLBACK FiberProc(void* lpParameter)
{
	FiberData_Socket* pFiberData = (FiberData_Socket*)lpParameter;

	SOCKET socket = WSASocket(pFiberData->addressfamily, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	BindHandle((HANDLE)socket);

	while (true)
	{
		DWORD dwBytes;
		byte acceptbuffer[ACCEPT_BUFFER_LENGTH];
		if (!Fiber_AcceptEx(pFiberData->listensocket, socket, &acceptbuffer, 0, sizeof(sockaddr_storage)+16, sizeof(sockaddr_storage)+16, &dwBytes))
			error("AcceptEx Failed");

		if (_settings.bDebugLog)
		{
			sockaddr* pcli_addr;
			sockaddr* psrv_addr;
			int cli_addr_len, srv_addr_len;
			pGetAcceptExSockaddrs(&acceptbuffer, 0, sizeof(sockaddr_storage)+16, sizeof(sockaddr_storage)+16, &psrv_addr, &srv_addr_len, &pcli_addr, &cli_addr_len);
			if (pcli_addr->sa_family == AF_INET)
			{
				const struct sockaddr_in& cli_addr4 = (sockaddr_in&)*pcli_addr;
				printf("%x: IPv4 connection from %s\n", socket, inet_ntoa(cli_addr4.sin_addr));
			}
			else if (pcli_addr->sa_family == AF_INET6)
			{
				const struct sockaddr_in6& cli_addr6 = (sockaddr_in6&)*pcli_addr;
				printf("%x: IPv6 connection from [%x:%x:%x:%x:%x:%x:%x:%x]\n", socket,
					ntohs(cli_addr6.sin6_addr.u.Word[0]), ntohs(cli_addr6.sin6_addr.u.Word[1]),
					ntohs(cli_addr6.sin6_addr.u.Word[2]), ntohs(cli_addr6.sin6_addr.u.Word[3]),
					ntohs(cli_addr6.sin6_addr.u.Word[4]), ntohs(cli_addr6.sin6_addr.u.Word[5]),
					ntohs(cli_addr6.sin6_addr.u.Word[6]), ntohs(cli_addr6.sin6_addr.u.Word[7]));
			}
			else
				error("Bad socket in accept");
		}

		dostuff(pFiberData, socket);

		if (_settings.bDebugLog)
			printf("%x: Disconnecting...\n", socket);
		if (!Fiber_DisconnectEx(socket, TF_REUSE_SOCKET))
		{
			//int iError = WSAGetLastError();
			//if (iError != WSAENOTCONN && iError != WSAECONNRESET)
			error("DisconnectEx Failed");
		}
		if (_settings.bDebugLog)
			printf("%x: Disconnected\n", socket);
	}

	if (_settings.bDebugLog)
		printf("Fiber - Done\n");
	Fiber_Finish();
}

#define HEADER_BUFFER_LENGTH (1*1024)
#define   SEND_BUFFER_LENGTH (1*1024)
/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff(FiberData_Socket* pFiberData, SOCKET sock)
{
	DWORD headerbytes = 0;
	auto_ptr_array<char> buffer = new char[HEADER_BUFFER_LENGTH];
	char* line_start = buffer;
	char* line_end = buffer;
	while(1)
	{
		DWORD dwBytes = 0;
		DWORD dwFlags = 0;
		line_end = buffer + dwBytes;
		BOOL result = Fiber_Recv(sock, line_end, HEADER_BUFFER_LENGTH - dwBytes - 1, &dwBytes, &dwFlags);
		if ( !result )
			error("Recv failed");
		if (dwBytes <= 0)
			return; // Connection closed
		headerbytes += dwBytes;
		buffer[headerbytes]='\0';
		line_end = strpbrk(line_end,"\r\n");
		if ( line_end )
			break;
	}

	if (_settings.bDebugLog)
	{
		printf("%x:\n%s", sock, buffer);
	}

	if (headerbytes >= 1023)
	{
		status500.sendto(sock);
		return;
	}

	char* splitpoint[3] = {NULL,NULL,NULL};
	splitpoint[0] = strpbrk(line_start," \r\n");
	if ( splitpoint[0] )
		splitpoint[1] = strpbrk(splitpoint[0]+1," \r\n");
	if ( splitpoint[1] )
		splitpoint[2] = strpbrk(splitpoint[1]+1," \r\n");
	if ( !(splitpoint[0] && splitpoint[1] && splitpoint[2] && *splitpoint[0] == ' ' && *splitpoint[1] == ' ' && *splitpoint[2] != ' ') )
	{
		status400.sendto(sock);
		return;
	}
	dynamic_string method(line_start, splitpoint[0] - line_start);
	dynamic_string URL(splitpoint[0]+1, splitpoint[1] - (splitpoint[0]+1));
	dynamic_string HTTPVersion(splitpoint[1]+1, splitpoint[2] - (splitpoint[1]+1));

	if (method != "GET" && method != "HEAD")
	{
		status501.sendto(sock);
		return;
	}

	if (HTTPVersion != "HTTP/1.1")
	{
		status505.sendto(sock);
		return;
	}

	if (URL.Len() <= 0 || URL[0] != '/')
	{
		status400.sendto(sock);
		return;
	}

	dynamic_array<HTTPHeader> headers;
	while(1)
	{
		line_start = line_end + 1;
		if (*line_end == '\r' && *line_start == '\n')
		{
			line_start++;
		}
		line_end = line_start;
		line_end = strpbrk(line_end,"\r\n");
		while ( !line_end )
		{
			line_end = buffer + headerbytes;
			DWORD dwBytes = 0;
			DWORD dwFlags = 0;
			line_end = buffer + dwBytes;
			BOOL result = Fiber_Recv(sock, line_end, HEADER_BUFFER_LENGTH - dwBytes - 1, &dwBytes, &dwFlags);
			if ( !result )
				error("Recv failed");
			if (dwBytes <= 0)
				return; // Connection closed
			headerbytes += dwBytes;
			buffer[headerbytes]='\0';
			line_end = strpbrk(line_end,"\r\n");
			if ( line_end )
				break;
		}
		if (line_end == line_start)
		{
			// blank line == end of headers / start of body
			break;
		}

		// parse headers
		char* header_split = strpbrk(line_start, ": \r\n");
		char* header_value = _strspnp(header_split + 1, " \t");
		headers.AddItem(HTTPHeader(dynamic_string(line_start, header_split - line_start), dynamic_string(header_value, line_end - header_value)));
	}

	dynamic_string host;
	dynamic_string fullhost;
	for (int i = 0; i < (int)headers.Num(); i++)
	{
		if (headers[i].Header == "Host")
		{
			fullhost = headers[i].Value;
			host = dynamic_string(headers[i].Value, strcspn(headers[i].Value, ":"));
			goto foundhost;
		}
	}
		status400nohost.sendto(sock);
		return;
foundhost:
	for (int i = 0; i < (int)_settings.hostnames.Num(); i++)
	{
		if (host == _settings.hostnames[i])
		{
			goto validhost;
		}
	}
		status400badhost.sendto(sock);
		return;
validhost:

	// This is just a little bit hacky...
	if ( strstr( URL, "/../" ) )
	{
		status400.sendto(sock);
		return;
	}
	const HTTPResponse* pResponse = _settings.getVirtualFolder()->GetFromPath(fullhost, URL, URL, sock);
	pResponse->sendto(sock);

	// this is a hack...
	if (pResponse->iStatus == 200 || pResponse->iStatus == 301)
	{
		delete pResponse;
	}

//	if (n <= 0) break;// error("ERROR writing to socket");
}
