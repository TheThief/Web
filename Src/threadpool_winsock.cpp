//#include <winsock2.h>
#include "threadpool_winsock.h"

//TODO: Fix hack
extern LPFN_ACCEPTEX pAcceptEx;
extern LPFN_DISCONNECTEX pDisconnectEx;
extern LPFN_TRANSMITFILE pTransmitFile;

BOOL Fiber_AcceptEx(SOCKET sListenSocket, SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived)
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();

	if (!pAcceptEx(sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, pFiberData))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return FALSE;
	}

	Fiber_YieldForIO();

	DWORD dwFlags = 0;
	if (!WSAGetOverlappedResult(sAcceptSocket, pFiberData, lpdwBytesReceived, FALSE, &dwFlags))
		return FALSE;

	return TRUE;
}

BOOL Fiber_DisconnectEx(SOCKET s, DWORD dwFlags)
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();

	if (!pDisconnectEx(s, pFiberData, dwFlags, 0))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return FALSE;
	}

	Fiber_YieldForIO();

	DWORD dwBytes = 0;
	if (!WSAGetOverlappedResult(s, pFiberData, &dwBytes, FALSE, &dwFlags))
		return FALSE;

	return TRUE;
}


BOOL Fiber_Recv(SOCKET s, PVOID lpOutputBuffer, DWORD dwReceiveDataLength, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags)
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();

	WSABUF buffer;
	buffer.buf = (CHAR*)lpOutputBuffer;
	buffer.len = dwReceiveDataLength;
	if (WSARecv(s, &buffer, 1, lpNumberOfBytesRecvd, lpFlags, pFiberData, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return FALSE;
	}

	Fiber_YieldForIO();

	if (!WSAGetOverlappedResult(s, pFiberData, lpNumberOfBytesRecvd, FALSE, lpFlags))
		return FALSE;

	return TRUE;
}

BOOL Fiber_Send(SOCKET s, PVOID lpInputBuffer, DWORD dwSendDataLength, LPDWORD lpNumberOfBytesSent, DWORD dwFlags)
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();

	WSABUF buffer;
	buffer.buf = (CHAR*)lpInputBuffer;
	buffer.len = dwSendDataLength;
	if (WSASend(s, &buffer, 1, lpNumberOfBytesSent, dwFlags, pFiberData, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return FALSE;
	}

	Fiber_YieldForIO();

	if (!WSAGetOverlappedResult(s, pFiberData, lpNumberOfBytesSent, FALSE, &dwFlags))
		return FALSE;

	return TRUE;
}

BOOL Fiber_TransmitFile(SOCKET hSocket, HANDLE hFile, DWORD nNumberOfBytesToWrite, DWORD nNumberOfBytesPerSend, LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers)
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();

	if (!pTransmitFile(hSocket, hFile, nNumberOfBytesToWrite, nNumberOfBytesPerSend, pFiberData, lpTransmitBuffers, 0))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return FALSE;
	}

	Fiber_YieldForIO();

	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	if (!WSAGetOverlappedResult(hSocket, pFiberData, &dwBytes, FALSE, &dwFlags))
		return FALSE;

	return TRUE;
}
