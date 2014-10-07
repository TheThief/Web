#pragma once

#include <assert.h>
#include <memory>
#include <utility>

#include <dynamic_ptr.h>

typedef unsigned char byte;

template <typename T>
class dynamic_array
{
protected:
	shared_array<byte> ptr;
	size_t iNum;
	size_t iMax;
public:
	dynamic_array() : ptr(nullptr), iNum(0), iMax(0)
	{
	}
	dynamic_array(const dynamic_array& rhs) = default;
	dynamic_array(dynamic_array&& rhs) = default;
	~dynamic_array()
	{
		for (size_t i=0; i<iNum; i++)
		{
			(*this)[i].~T();
		}
		ptr = nullptr;
	}

	dynamic_array& operator=(const dynamic_array& rhs) = default;
	dynamic_array& operator=(dynamic_array&& rhs) = default;

	const T& operator [](size_t i) const
	{
		assert(i>=0 && i<iNum);
		return ((T*)ptr.get())[i];
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
		new(&(((T*)ptr.get())[i])) T(Item);
		return i;
	}

	void ExpandMaxSize(int Amount = 32)
	{
		SetMaxSize(iMax + Amount);
	}

	void SetMaxSize(size_t MaxSize)
	{
		assert(MaxSize >= iNum);
		if (iMax != MaxSize)
		{
			iMax = MaxSize;
			if (MaxSize <= 0)
			{
				ptr = nullptr;
			}
			else
			{
				shared_array<byte> ptrNew = make_shared_array<byte>(iMax * sizeof(T));
				if (ptr)
				{
					if (iNum > 0)
					{
						// todo: copy-construct T
						memcpy((T*)ptrNew.get(), (T*)ptr.get(), iNum*sizeof(T));
					}
				}
				ptr = ptrNew;
			}
		}
	}

	void SetSize(size_t Size)
	{
		if (Size > iNum)
		{
			clonebuffer(Size - iNum + 32);
			if (Size > iMax)
			{
				SetMaxSize(Size + 32);
			}
		}

		if (iNum > Size)
		{
			clonebuffer();
			for (size_t i=Size; i<iNum; i++)
			{
				(*this)[i].~T();
			}
		}

		if (iNum < Size)
		{
			for (size_t i=iNum; i<Size; i++)
			{
				new(&(((T*)ptr.get())[i])) T();
			}
		}

		iNum = Size;
	}

	void clonebuffer(size_t iSlack = 32)
	{
		if (ptr && !ptr.unique())
		{
			if (iNum <= 0)
			{
				ptr = nullptr;
				iMax = 0;
			}
			else
			{
				iMax = iNum + iSlack;
				shared_array<byte> ptrNew = make_shared_array<byte>(iMax * sizeof(T));
				if (ptr)
				{
					// todo: copy-construct T
					memcpy((T*)ptrNew.get(), (T*)ptr.get(), iNum*sizeof(T));
				}
				ptr = ptrNew;
			}
		}
	}
};
