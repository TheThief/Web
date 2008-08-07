#pragma once

#include <assert.h>
#include <new.h>

template <class T>
class auto_ptr
{
protected:
	T*ptr;
public:
	auto_ptr(T*rhs=NULL) : ptr(rhs) { }
	~auto_ptr() { delete ptr; }

	operator T*() { return ptr; }
	T* operator ->() { return ptr; }
	T operator *() { return *ptr; }
	T* operator =(T*rhs) { ptr=rhs; return ptr; }
};

template <class T>
class auto_ptr_array
{
protected:
	T*ptr;
public:
	auto_ptr_array(T*rhs=NULL) : ptr(rhs) { }
	~auto_ptr_array() { delete [] ptr; }

	operator T*() { return ptr; }
	T* operator ->() { return ptr; }
	T operator *() { return *ptr; }
	T* operator =(T*rhs) { ptr=rhs; return ptr; }
};

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
	refcounted_base_ptr() : ptr(NULL) { }
	refcounted_base_ptr(const refcounted_base_ptr& rhs) : ptr(rhs.ptr)
	{
		AddRef();
	}
	refcounted_base_ptr(refcounted_base* const & _ptr) : ptr(_ptr)
	{
		AddRef();
	}
	refcounted_base_ptr(void*null) : ptr(NULL)
	{
		assert(null == NULL);
	}
	refcounted_base_ptr(int null) : ptr(NULL)
	{
		assert(null == NULL);
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

	operator bool() const
	{
		return ptr != NULL;
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

template <class T>
class dynamic_array
{
protected:
	refcounted_memory_ptr ptr;
	size_t iNum;
	size_t iMax;
public:
	dynamic_array() : ptr(NULL), iNum(0), iMax(0)
	{
	}
	dynamic_array(const dynamic_array& rhs) : ptr(rhs.ptr), iNum(rhs.iNum), iMax(rhs.iMax)
	{
	}
	~dynamic_array()
	{
		for (size_t i=0; i<iNum; i++)
		{
			(*this)[i].~T();
		}
		ptr = NULL;
	}

	T& operator [](size_t i) const
	{
		assert(i>=0 && i<iNum);
		return ((T*)ptr)[i];
	}

	size_t Num() const
	{
		return iNum;
	}

	size_t AddItem(T& Item)
	{
		if (iNum >= iMax)
		{
			ExpandMaxSize();
		}
		size_t i = iNum;
		iNum++;
		new(&(((T*)ptr)[i])) T(Item);
		return i;
	}

	void ExpandMaxSize(int Amount = 10)
	{
		SetMaxSize(iMax + Amount);
	}

	void SetMaxSize(size_t MaxSize)
	{
		iMax = MaxSize;
		refcounted_memory_ptr ptrNew = new(iMax*sizeof(T)) refcounted_memory();
		if (ptr)
		{
			if (iNum>0)
			{
				memcpy((T*)ptrNew, (T*)ptr, iNum*sizeof(T));
			}
		}
		ptr = ptrNew;
	}

	void SetSize(size_t Size)
	{
		if (iNum > Size)
		{
			for (size_t i=Size; i<iNum; i++)
			{
				(*this)[i].~T();
			}
		}

		if (iMax < Size)
		{
			SetMaxSize(Size);
		}

		if (iNum < Size)
		{
			for (size_t i=iNum; i<Size; i++)
			{
				new(&(((T*)ptr)[i])) T();
			}
		}

		iNum = Size;
	}

	void clonebuffer()
	{
		if (ptr && ptr.GetRefCount() > 1)
		{
			if (iNum <= 0)
			{
				ptr = NULL;
			}
			else
			{
				refcounted_memory_ptr ptrNew = new(iNum*sizeof(T)) refcounted_memory();
				if (ptr)
				{
					memcpy((T*)ptrNew, (T*)ptr, iNum*sizeof(T));
				}
				ptr = ptrNew;
			}
		}
	}
};
