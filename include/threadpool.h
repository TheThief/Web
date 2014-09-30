#pragma once

extern DWORD MasterFiberTlsIndex;
extern HANDLE IOCompletionPort;

struct FiberData;

struct FiberData : OVERLAPPED
{
	void* FiberHandle;
	CRITICAL_SECTION RunningLock;
	bool bIsFinished;

	FiberData(LPFIBER_START_ROUTINE lpStartAddress);
	~FiberData();

	void ResetOverlapped();
};

void InitThreadPool();
void BindHandle(HANDLE Handle);
void QueueFiber(FiberData* pFiberData);
void DestroyThreadPool();

inline void Fiber_YieldForIO()
{
	void* pMasterFiber = TlsGetValue(MasterFiberTlsIndex);
	SwitchToFiber(pMasterFiber);
}

inline void Fiber_Yield()
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->ResetOverlapped();
	QueueFiber(pFiberData);
	void* pMasterFiber = TlsGetValue(MasterFiberTlsIndex);
	SwitchToFiber(pMasterFiber);
}

inline void Fiber_Finish()
{
	FiberData* pFiberData = (FiberData*)GetFiberData();
	pFiberData->bIsFinished = true;
	void* pMasterFiber = TlsGetValue(MasterFiberTlsIndex);
	SwitchToFiber(pMasterFiber);
}
