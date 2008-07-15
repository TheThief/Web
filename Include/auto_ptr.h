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

template <class T>
class refcounted_object
{
protected:
	int iRefcount;
	T object;

public:
	refcounted_object() : iRefcount(0), object() { }
	refcounted_object(T rhs) : iRefcount(0), object(rhs) { }
	~refcounted_object()
	{
		assert(iRefcount==0);
	}

	operator T&() { return object; }
	//T* operator ->() { return ptr; }
	//T operator *() { return *ptr; }
	T& operator =(T rhs) { object=rhs; return object; }

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
class refcounted_ptr
{
protected:
	refcounted_object<T>* ptr;

public:
	refcounted_ptr() : ptr(NULL) { }
	refcounted_ptr(T* rhs) : ptr(rhs)
	{
		if (ptr)
		{
			ptr->AddRef();
		}
	}
	refcounted_ptr(const refcounted_ptr& rhs) : refcounted_ptr(rhs.ptr) { }

	~refcounted_ptr()
	{
		if (ptr)
		{
			ptr->RemoveRef();
		}
	}

	operator T*() const { return &ptr->object; }
	T* operator ->() const { return &ptr->object; }
	T operator *() const { return ptr->object; }

	T* operator =(T* rhs)
	{
		if (rhs)
		{
			rhs->AddRef();
		}
		if (ptr)
		{
			ptr->RemoveRef();
		}
		ptr = rhs;
		return ptr;
	}
	refcounted_ptr& operator =(const refcounted_ptr& rhs)
	{
		operator =(rhs.ptr);
		return *this;
	}
};

template <class T>
class dynamic_array
{
protected:
	T*ptr;
	int iNum;
	int iMax;
public:
	dynamic_array() : ptr(NULL), iNum(0), iMax(0)
	{
	}
	~dynamic_array()
	{
		if (ptr)
			delete [] ptr;
	}

	T& operator [](int i) const
	{
		assert(i>=0 && i<iNum);
		return ptr[i];
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
		ptr[i] = Item;
		return i;
	}

	void Expand(int Amount = 10)
	{
		int iNewMax = iMax + Amount;
		T* ptrNew = new T[iNewMax];
		if (ptr)
		{
			if (iNum>0)
			{
				memcpy(ptrNew, ptr, iNum*sizeof(T));
			}
			delete[] ptr;
		}
		ptr = ptrNew;
	}

	//operator T*() { return ptr; }
	//T* operator ->() { return ptr; }
	//T operator *() { return *ptr; }
	//T* operator =(T*rhs) { ptr=rhs; return ptr; }
};
