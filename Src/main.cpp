#define _WIN32_WINNT 0x0500
#include <winsock2.h>
#include <ws2tcpip.h>

#include <string.h>
#include <fcntl.h>
#include <share.h>
#include <io.h>

#include "../include/responses.h"
#include "../include/auto_ptr.h"
#include "../include/filetree.h"

const int port = 10000;


DWORD WINAPI ThreadProc(LPVOID lpParameter);
void dostuff(SOCKET);

void error(char *msg)
{
	WSACleanup( );
//	perror(msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
		error("ERROR initializing Winsock");

	if ( wsaData.wVersion != MAKEWORD( 2, 2 ) )
	{
		error("ERROR initializing Winsock: Incompatible version");
	}

	SOCKET sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	   error("ERROR opening socket");
	ZeroMemory(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		//HANDLE hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)newsockfd, 0, NULL);
		QueueUserWorkItem(ThreadProc, (LPVOID)newsockfd, 0);
		//if (hThread == NULL)
		//{
		//	error("ERROR spawning new thread");
		//}
		//CloseHandle(hThread);
	}
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	SOCKET newsockfd = SOCKET(lpParameter);
	dostuff(newsockfd);
	closesocket(newsockfd);

	return 0;
}

#define HEADER_BUFFER_LENGTH (1*1024)
#define   SEND_BUFFER_LENGTH (1*1024)
/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (SOCKET sock)
{
	int n=0, i=0;
	auto_ptr_array<char> buffer = new char[HEADER_BUFFER_LENGTH];
	char* line_end=buffer;
	while(1)
	{
		i = recv(sock, buffer+n, HEADER_BUFFER_LENGTH-1-n, 0);
		if ( i<=0 )
			break;
		n+=i;
		buffer[n]='\0';
		line_end = strpbrk(line_end,"\r\n");
		if ( !line_end )
			line_end = buffer + n;
		else
			break;
	}
	if (n <= 0)
		return;
	puts(buffer);

	if (n>=1023)
	{
		status500.sendto(sock);
		return;
	}

	char* splitpoint[3] = {NULL,NULL,NULL};
	splitpoint[0] = strpbrk(buffer," \r\n");
	if ( splitpoint[0] != NULL )
		splitpoint[1] = strpbrk(splitpoint[0]+1," \r\n");
	if ( splitpoint[1] != NULL )
		splitpoint[2] = strpbrk(splitpoint[1]+1," \r\n");
	if ( !(splitpoint[0] && *splitpoint[0] == ' ' && splitpoint[1] && *splitpoint[1] == ' ' && splitpoint[2] && *splitpoint[2] != ' ') )
	{
		status400.sendto(sock);
		return;
	}

	if (splitpoint[0]-buffer != 3 || strncmp(buffer,"GET",splitpoint[0]-buffer)!=0)
	{
		status501.sendto(sock);
		return;
	}

	if (splitpoint[2]-(splitpoint[1]+1) != 8 || strncmp(splitpoint[1]+1,"HTTP/1.1",splitpoint[2]-(splitpoint[1]+1))!=0)
	{
		status505.sendto(sock);
		return;
	}

	if (splitpoint[1]-(splitpoint[0]+1) >= 1 && strncmp(splitpoint[0]+1,"/",1)==0)
	{
		*splitpoint[1] = '\0';
		webroot.GetFromPath(splitpoint[0]+1, splitpoint[0]+1, sock);
	}
	else
	{
		status400.sendto(sock);
	}
//	if (n <= 0) break;// error("ERROR writing to socket");
}
