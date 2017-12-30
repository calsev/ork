/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"


namespace ork {


/*
The C++ standard does not offer a smart pointer with value sematics.
For one, C++ does not offer a standard way to copy an object to prevent slicing.

Nonetheless, pimpl is so common that we implement a unique ownership, copyable pointer.
The pointer therefore has value semantics, and if such a thing is ever standardized it may be called value_ptr.
We implement a simple wrapper around unique_ptr that delegates copying and deleting to a templated functor.

No default templates are offered.
If pimpl is being used the functor interfaces cannot be defined in a header.

The functor must implement both copy and delete interfaces
std::unique_ptr<T, D> D(const T&);
void D(T*);
Note that the copy interface must handle null objects by returning empty pointer

No casting interface is offered, although if the copier supports it...
No auto_ptr conversion is offered, because who uses auto_ptr?
No relational operators are offered, because these are a source of more bugs than functionality.
*/


template<typename T>
struct default_deleter {
	void operator()(const T*t) {
		delete t;
	}
	T operator()(const T&t) {
		return T(t);
	}
};


//This allows smart interfaces over singleton implementations
template<typename T>
struct singleton_deleter {
	void operator()(const T*t) {
		//Do Nothing
	}
	T operator()(const T&t) {
		return t;
	}
};


template<class T, class D>
class value_ptr {
public:
	using element_type = T;
	using pointer = T*;//This is a little simplistic
	using copier_type = D;
	using deleter_type = D;
private:
	std::unique_ptr<T, D>_ptr;
public:
	constexpr value_ptr() noexcept : _ptr() {}
	constexpr value_ptr(nullptr_t) noexcept : _ptr(nullptr) {}
	explicit value_ptr(pointer p) noexcept : _ptr(p) {}
	value_ptr(pointer p, typename std::conditional<std::is_reference<D>::value, D, const D&> del) noexcept : _ptr(p, del) {}
	value_ptr(pointer p, typename std::remove_reference<D>::type&& del) noexcept : _ptr(p, std::move(del)) {}

	value_ptr(value_ptr&& x) noexcept : _ptr(std::move(x._ptr)) {}
	template<class D2>
	value_ptr(value_ptr<T, D2>&& x) noexcept : _ptr(std::move(x._ptr)) {}

	//Allow conversion from unique_ptr
	value_ptr(std::unique_ptr<T, D>&& x) noexcept : _ptr(std::move(x)) {}
	template<class D2>
	value_ptr(std::unique_ptr<T, D2>&& x) noexcept : _ptr(std::move(x)) {}

	//The big payoff
	value_ptr(const value_ptr&x) : _ptr(x.get_deleter()(*x._ptr)) {}
	template<class D2>
	value_ptr(const value_ptr<T, D2>& x) : _ptr(x.get_deleter()(*x._ptr)) {}

	//The big payoff + conversion
	value_ptr(const std::unique_ptr<T, D>& x) : _ptr(x.get_deleter()(*x._ptr)) {}
	template<class D2>
	value_ptr(const std::unique_ptr<T, D2>& x) : _ptr(x.get_deleter()(*x._ptr)) {}

	//Destructor is implicitly defined
public:
	value_ptr& operator=(nullptr_t) noexcept {
		_ptr = nullptr;
		return *this;
	}
	
	value_ptr& operator=(value_ptr&& x) noexcept {
		_ptr = std::move(x._ptr);
		return *this;
	}
	template<class D2>
	value_ptr& operator=(value_ptr<T, D2>&& x) noexcept {
		_ptr = std::move(x._ptr);
		return *this;
	}

	//Allow conversion from unique_ptr
	value_ptr& operator=(std::unique_ptr<T, D>&& x) noexcept {
		_ptr = std::move(x);
		return *this;
	}
	template<class D2>
	value_ptr& operator=(std::unique_ptr<T, D2>&& x) noexcept {
		_ptr = std::move(x);
		return *this;
	}

	//The big payoff
	value_ptr& operator=(const value_ptr& x) {
		_ptr = x.get_deleter()(*x);
		return *this;
	}
	template<class D2>
	value_ptr& operator=(const value_ptr<T, D2>& x) {
		_ptr = x.get_deleter()(*x);
		return *this;
	}

	//The big payoff + conversion
	value_ptr& operator=(const std::unique_ptr<T, D>& x) {
		_ptr = x.get_deleter()(*x);
		return *this;
	}
	template<class D2>
	value_ptr& operator=(const std::unique_ptr<T, D2>& x) {
		_ptr = x.get_deleter()(*x);
		return *this;
	}
public:
	typename std::add_lvalue_reference<element_type>::type operator*() const {
		return _ptr.operator*();
	}
	pointer operator->() const noexcept {
		return _ptr.operator->();
	}
	pointer get() const noexcept {
		return _ptr.get();
	}
	deleter_type& get_deleter() noexcept {
		return _ptr.get_deleter();
	}
	const deleter_type& get_deleter() const noexcept {
		return _ptr.get_deleter();
	}
	explicit operator bool() const noexcept {
		return _ptr.operator bool();
	}
	pointer release() noexcept {
		return _ptr.release();
	}
	//The reset interface is different than std, but I prefer overloads to default params
	void reset() noexcept {
		_ptr.reset();
	}
	void reset(pointer p) noexcept {
		_ptr.reset(p);
	}
	void swap(value_ptr& x) noexcept {
		_ptr.swap(x._ptr);
	}
};


}//namespace ork


namespace std {

template <class T, class D>
void swap(ork::value_ptr<T, D>& x, ork::value_ptr<T, D>& y) noexcept {
	x.swap(y);
}
	
}//namespace std
