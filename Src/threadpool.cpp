#define _WIN32_WINNT 0x0500
#include <windows.h>

#include "threadpool.h"

DWORD MasterFiberTlsIndex;
HANDLE IOCompletionPort;

DWORD WINAPI WorkerThreadProc(void* lpParameter);

FiberData::FiberData(LPFIBER_START_ROUTINE lpStartAddress)
{
	ResetOverlapped();
	FiberHandle = CreateFiberEx(4096, 16384, 0, lpStartAddress, this);
	InitializeCriticalSectionAndSpinCount(&RunningLock, 4000);
	bIsFinished = false;
}

FiberData::~FiberData()
{
	DeleteCriticalSection(&RunningLock);
	DeleteFiber(FiberHandle);
}

void FiberData::ResetOverlapped()
{
	memset((OVERLAPPED*)this, 0, sizeof(OVERLAPPED));
}

void InitThreadPool()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	MasterFiberTlsIndex = TlsAlloc();
	IOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, sysinfo.dwNumberOfProcessors);
	for (int i = 0; i < (int)sysinfo.dwNumberOfProcessors; i++)
	{
		CreateThread(NULL, 4096, &WorkerThreadProc, NULL, 0, NULL);
	}
}

void BindHandle(HANDLE Handle)
{
	CreateIoCompletionPort(Handle, IOCompletionPort, 0, 0);
}

void QueueFiber(FiberData* pFiberData)
{
	PostQueuedCompletionStatus(IOCompletionPort, 0, 0, pFiberData);
}

void DestroyThreadPool()
{
	CloseHandle(IOCompletionPort);
}

DWORD WINAPI WorkerThreadProc(void* lpParameter)
{
	void* pMasterFiber = ConvertThreadToFiber(NULL);
	TlsSetValue(MasterFiberTlsIndex, pMasterFiber);

	while(true)
	{
		DWORD NumberOfBytes;
		ULONG_PTR CompletionKey;
		LPOVERLAPPED lpOverlapped;
		BOOL IOSuccess = GetQueuedCompletionStatus(IOCompletionPort, &NumberOfBytes, &CompletionKey, &lpOverlapped, INFINITE);
		FiberData* pFiberData = (FiberData*)lpOverlapped;
		EnterCriticalSection(&pFiberData->RunningLock);
		SwitchToFiber(pFiberData->FiberHandle);
		LeaveCriticalSection(&pFiberData->RunningLock);
		if (pFiberData->bIsFinished)
		{
			delete pFiberData;
		}
	}

	return 0;
}
