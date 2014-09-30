#pragma once

#include <winsock2.h>
#include <Mswsock.h>

#include "threadpool.h"

BOOL Fiber_AcceptEx(SOCKET sListenSocket, SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived);
BOOL Fiber_DisconnectEx(SOCKET s, DWORD dwFlags);
BOOL Fiber_Recv(SOCKET s, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags);
BOOL Fiber_Send(SOCKET s, PVOID lpInputBuffer, DWORD dwSendDataLength, LPDWORD lpNumberOfBytesSent, DWORD dwFlags);
BOOL Fiber_TransmitFile(SOCKET hSocket, HANDLE hFile, DWORD nNumberOfBytesToWrite, DWORD nNumberOfBytesPerSend, LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers);
