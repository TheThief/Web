#pragma once

#include <assert.h>
#include <memory>
#include <utility>

#if __cpp_lib_shared_ptr_arrays // I'm guessing here
template<typename T>
using shared_array = std::shared_ptr<T[]>;
template<typename T>
using make_shared_array = std::make_shared<T[]>;
#elif __cpp_lib_experimental_shared_ptr_arrays
#include <experimental/memory>
template<typename T>
using shared_array = std::shared_ptr<T[]>;
template<typename T>
using make_shared_array = std::make_shared<T[]>;
#else
template<typename T>
class shared_array : public std::shared_ptr<T>
{
public:
	using std::shared_ptr<T>::shared_ptr;
	using std::shared_ptr<T>::operator =;

	shared_array<T>& operator=(nullptr_t)
	{
		(std::shared_ptr<T>&)*this = nullptr;

		return *this;
	}

	element_type& operator[](ptrdiff_t i) const noexcept
	{
		return get()[i];
	}
};

template<typename T>
shared_array<T>
	make_shared_array(size_t count)
{
	return shared_array<T>(new T[count], std::default_delete<T[]>());
}
#endif

template <typename T>
class dynamic_ptr
{
protected:
	std::shared_ptr<T> ptr;
	const T* rawptr;
public:
	dynamic_ptr() : ptr(nullptr), rawptr(nullptr) {}
	dynamic_ptr(std::shared_ptr<T> rhs) : ptr(std::move(rhs)), rawptr(ptr.get()) {}
	dynamic_ptr(const T& rhs) : ptr(nullptr), rawptr(&rhs) {}
	dynamic_ptr& operator=(std::shared_ptr<T> rhs)
	{
		ptr = std::move(rhs);
		rawptr = ptr.get();
	}
	dynamic_ptr& operator=(const T& rhs)
	{
		ptr = nullptr;
		rawptr = &rhs;
	}

	dynamic_ptr(const dynamic_ptr& rhs) = default;
	dynamic_ptr(dynamic_ptr&& rhs) = default;
	~dynamic_ptr() = default;
	dynamic_ptr& operator=(const dynamic_ptr& rhs) = default;
	dynamic_ptr& operator=(dynamic_ptr&& rhs) = default;

	T* clone()
	{
		if (rawptr && !(ptr && ptr.unique()))
		{
			ptr = std::make_shared<T>(*rawptr);
			rawptr = ptr.get();
		}
		return rawptr;
	}

	const T& operator*() const
	{
		return *rawptr;
	}

	const T* operator->() const
	{
		return rawptr;
	}
};

template <typename T>
class dynamic_ptr<T[]>
{
protected:
	shared_array<T> ptr;
	T* rawptr;
	//size_t size;
public:
	dynamic_ptr() : ptr(nullptr), rawptr(nullptr)
	{
	}
	dynamic_ptr(const dynamic_ptr& rhs) = default;
	dynamic_ptr(dynamic_ptr&& rhs) = default;
	~dynamic_ptr() = default;
	dynamic_ptr& operator=(const dynamic_ptr& rhs) = default;
	dynamic_ptr& operator=(dynamic_ptr&& rhs) = default;

	//T* clone()
	//{
	//	if (rawptr && !(ptr && ptr.unique()))
	//	{
	//		ptr = make_shared<T>(*rawptr);
	//		rawptr = ptr.get();
	//	}
	//	return rawptr;
	//}

	const T& operator[](ptrdiff_t i) const
	{
		return rawptr[i];
	}
};
