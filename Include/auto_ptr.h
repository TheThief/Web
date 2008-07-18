#pragma once

#include <assert.h>

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
	//T* operator ->() { return ptr; }
	//T operator *() { return *ptr; }
	T& operator =(T rhs) { object=rhs; return object; }
};

class refcounted_memory : public refcounted_base
{
protected:
	size_t iBytes;

	//refcounted_memory(size_t _iBytes) : refcounted_base(), iBytes(_iBytes) { }

public:
	//refcounted_object() : refcounted_base(), object() { }
	//refcounted_object() : refcounted_base(), object() { }
	//refcounted_object(T rhs) : refcounted_base(), object(rhs) { }

	//operator T&() { return object; }
	//T* operator ->() { return ptr; }
	//T operator *() { return *ptr; }
	//T& operator =(T rhs) { object=rhs; return object; }
	void* operator new(size_t thissize, size_t iBytes)
	{
		refcounted_memory* pNew = (refcounted_memory*)::new byte[thissize+iBytes];
		pNew->iBytes = iBytes;
		return pNew;
	}
	void operator delete(void* pDel, size_t thissize)
	{
		::delete(pDel);
	}

	template <class T>
	operator T*()
	{
		return (T*)((byte*)this+sizeof(*this));
	}
};

class refcounted_base_ptr
{
protected:
	refcounted_base* ptr;

public:
	refcounted_base_ptr() : ptr(NULL) { }
	refcounted_base_ptr(const refcounted_base_ptr& rhs) : ptr(rhs.ptr) { }

	virtual ~refcounted_base_ptr()
	{
		if (ptr)
		{
			ptr->RemoveRef();
		}
	}

	refcounted_base_ptr& operator =(const refcounted_base_ptr& rhs)
	{
		if (rhs.ptr)
		{
			rhs.ptr->AddRef();
		}
		if (ptr)
		{
			ptr->RemoveRef();
		}
		ptr = rhs.ptr;
		return *this;
	}
};

template <class T>
class refcounted_object_ptr : public refcounted_base_ptr
{
public:
	refcounted_object_ptr(const refcounted_object_ptr<T>& rhs) : ((refcounted_object<T>*)ptr)(rhs.ptr)
	{
		if (ptr)
		{
			((refcounted_object<T>*)ptr)->AddRef();
		}
	}

	operator T*() const { return &((refcounted_object<T>*)ptr)->object; }
	T* operator ->() const { return &((refcounted_object<T>*)ptr)->object; }
	T operator *() const { return ((refcounted_object<T>*)ptr)->object; }

	refcounted_object_ptr<T>& operator =(const refcounted_object_ptr<T>& rhs)
	{
		return refcounted_base_ptr::operator =(rhs)
	}
};

template <class T>
class dynamic_array
{
protected:
	//T*ptr;
	refcounted_memory*ptr;
	int iNum;
	int iMax;
public:
	dynamic_array() : ptr(NULL), iNum(0), iMax(0)
	{
	}
	~dynamic_array()
	{
		if (ptr)
			//delete [] ptr;
			ptr->RemoveRef();
	}

	T& operator [](int i) const
	{
		assert(i>=0 && i<iNum);
		return ((T*)*ptr)[i];
	}

	int Num() const
	{
		return iNum;
	}

	int AddItem(T& Item)
	{
		if (iNum>=iMax)
		{
			Expand();
		}
		int i = iNum;
		iNum++;
		((T*)*ptr)[i] = T(Item);
		return i;
	}

	void Expand(int Amount = 10)
	{
		iMax += Amount;
		//T* ptrNew = new T[iNewMax];
		refcounted_memory* ptrNew = new(iMax*sizeof(T)) refcounted_memory;
		ptrNew->AddRef();
		if (ptr)
		{
			if (iNum>0)
			{
				memcpy(((T*)*ptrNew), ((T*)*ptr), iNum*sizeof(T));
			}
			ptr->RemoveRef();
			delete ptr;
		}
		ptr = ptrNew;
	}

	//operator T*() { return ptr; }
	//T* operator ->() { return ptr; }
	//T operator *() { return *ptr; }
	//T* operator =(T*rhs) { ptr=rhs; return ptr; }
};
