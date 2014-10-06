#pragma once

#include <assert.h>
#include <memory>
#include <utility>

typedef unsigned char byte;

#if __cpp_lib_shared_ptr_arrays // I'm guessing here
using std::shared_ptr;
using std::make_shared;
#elif __cpp_lib_experimental_shared_ptr_arrays
#include <experimental/memory>
using std::shared_ptr;
using std::make_shared;
#else
template<typename T>
class shared_ptr : public std::shared_ptr<T>
{
public:
	using std::shared_ptr<T>::shared_ptr;
	using std::shared_ptr<T>::operator =;
};

template<typename T>
class shared_ptr<T[]> : public std::shared_ptr<T>
{
public:
	using std::shared_ptr<T>::shared_ptr;
	using std::shared_ptr<T>::operator =;

	shared_ptr<T[]>& operator=(nullptr_t)
	{
		(std::shared_ptr<T>&)*this = nullptr;

		return *this;
	}

	element_type& operator[](ptrdiff_t i) const noexcept
	{
		return get()[i];
	}
};

template<typename T, typename... Types>
std::enable_if_t<!std::is_array<T>::value, shared_ptr<T>>
	make_shared(Types&&... Args)
{
	return std::make_shared<T>(std::forward<Types>(Args)...);
}

template<typename T>
std::enable_if_t<std::is_array<T>::value, shared_ptr<T>>
	make_shared(size_t count)
{
	return shared_ptr<T>(new std::remove_extent<T>::type[count], std::default_delete<T>());
}
#endif

template <class T>
class dynamic_array
{
protected:
	shared_ptr<byte[]> ptr;
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
				shared_ptr<byte[]> ptrNew = make_shared<byte[]>(iMax * sizeof(T));
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
				shared_ptr<byte[]> ptrNew = make_shared<byte[]>(iMax * sizeof(T));
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
