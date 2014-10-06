#pragma once

#error auto_ptr is deprecated, use shared_ptr

#include <assert.h>
#include <new.h>

typedef unsigned char byte;

class refcounted_base
{
protected:
	int iRefcount;

public:
	refcounted_base() : iRefcount(0) { }
	virtual ~refcounted_base()
	{
		assert(iRefcount==0);
	}

	void AddRef()
	{
		iRefcount++;
	}
	void RemoveRef()
	{
		iRefcount--;
		assert(iRefcount>=0);
		if (iRefcount<=0)
		{
			delete this;
		}
	}

	int GetRefCount() const
	{
		return iRefcount;
	}
};

template <class T>
class refcounted_object : public refcounted_base
{
protected:
	T object;

public:
	refcounted_object() : refcounted_base(), object() { }
	refcounted_object(T rhs) : refcounted_base(), object(rhs) { }

	operator T&() { return object; }
	T& operator =(T rhs) { object=rhs; return object; }
};

class refcounted_memory : public refcounted_base
{
protected:
#if _DEBUG
	size_t iBytes;
	byte* pData;
#endif

public:
	void* operator new(size_t thissize, size_t iBytes)
	{
		refcounted_memory* pNew = (refcounted_memory*)::new byte[thissize+iBytes];
#if _DEBUG
		pNew->iBytes = iBytes;
		pNew->pData = (byte*)pNew + sizeof(refcounted_memory);
#endif
		return pNew;
	}
	void operator delete(void* pDel, size_t thissize)
	{
		::delete(pDel);
	}

	operator byte*()
	{
		return (byte*)this + sizeof(refcounted_memory);
	}
};

class refcounted_base_ptr
{
protected:
	refcounted_base* ptr;

	void AddRef() const
	{
		if (ptr)
		{
			ptr->AddRef();
		}
	}
	void RemoveRef() const
	{
		if (ptr)
		{
			ptr->RemoveRef();
		}
	}

public:
	refcounted_base_ptr() : ptr(nullptr) { }
	refcounted_base_ptr(const refcounted_base_ptr& rhs) : ptr(rhs.ptr)
	{
		AddRef();
	}
	refcounted_base_ptr(refcounted_base* const & _ptr) : ptr(_ptr)
	{
		AddRef();
	}
	refcounted_base_ptr(void*null) : ptr(nullptr)
	{
		assert(null == nullptr);
	}
	refcounted_base_ptr(int null) : ptr(nullptr)
	{
		assert(null == nullptr);
	}

	virtual ~refcounted_base_ptr()
	{
		if (ptr)
		{
			ptr->RemoveRef();
		}
	}

	refcounted_base_ptr& operator =(const refcounted_base_ptr& rhs)
	{
		// AddRef first to avoid RemoveRef possibly deleting rhs
		rhs.AddRef();
		RemoveRef();
		ptr = rhs.ptr;
		return *this;
	}

	bool operator ==(const refcounted_base_ptr& rhs)
	{
		return ptr == rhs.ptr;
	}

	operator bool() const
	{
		return ptr != nullptr;
	}

	int GetRefCount() const
	{
		if (ptr)
		{
			return ptr->GetRefCount();
		}
		else
		{
			return 0;
		}
	}
};

template <class T>
class refcounted_object_ptr : public refcounted_base_ptr
{
public:
	refcounted_object_ptr() : refcounted_base_ptr() { }
	refcounted_object_ptr(const refcounted_object_ptr<T>& rhs) : refcounted_base_ptr(rhs) { }
	refcounted_object_ptr(refcounted_object<T>* const & _ptr) : refcounted_base_ptr(_ptr) { }
	refcounted_object_ptr(void*null) : refcounted_base_ptr(null) { }
	refcounted_object_ptr(int null) : refcounted_base_ptr(null) { }

	operator T*() const { return &((refcounted_object<T>*)ptr)->object; }
	T* operator ->() const { return &((refcounted_object<T>*)ptr)->object; }
	T operator *() const { return ((refcounted_object<T>*)ptr)->object; }

	refcounted_object_ptr<T>& operator =(const refcounted_object_ptr<T>& rhs)
	{
		return refcounted_base_ptr::operator =(rhs)
	}
};

class refcounted_memory_ptr : public refcounted_base_ptr
{
public:
	refcounted_memory_ptr() : refcounted_base_ptr() { }
	refcounted_memory_ptr(const refcounted_memory_ptr& rhs) : refcounted_base_ptr(rhs) { }
	refcounted_memory_ptr(refcounted_memory* const & _ptr) : refcounted_base_ptr(_ptr) { }
	refcounted_memory_ptr(void*null) : refcounted_base_ptr(null) { }
	refcounted_memory_ptr(int null) : refcounted_base_ptr(null) { }

	template <class T>
	operator T*() const { return (T*)((byte*)(*(refcounted_memory*)ptr)); }

	refcounted_memory_ptr& operator =(const refcounted_memory_ptr& rhs)
	{
		return (refcounted_memory_ptr&)refcounted_base_ptr::operator =(rhs);
	}
};
