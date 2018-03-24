/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/type_traits.hpp"


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
    ORK_INLINE void operator()(const T* t)
    {
        delete t;
    }
    ORK_INLINE T* operator()(const T& t)
    {
        return new T(t);
    }
};


// This allows smart interfaces over singleton implementations
template<typename T>
struct singleton_deleter {
    void operator()(const T*)
    {
        // Do Nothing
    }
    T* operator()(const T& t)
    {
        return new T(t);
    }
};


#define ORK_PTR_TYPES(PTR) \
    /* Allow cv, but not ref*/ \
    using element_type = typename std::remove_reference<T>::type; \
    using pointer = typename std::add_pointer<element_type>::type; \
    /* Must be a base value type*/ \
    using copier_type = typename ORK_VAL(D); \
    using deleter_type = copier_type; \
    /* For access*/ \
    template<typename T2, typename D2> \
    friend class PTR;


#define ORK_MOVE_PTR(PTR) \
    ORK_INLINE PTR(PTR&& ptr) ORK_NO_EXCEPT : _ptr(std::move(ptr._ptr)) {} \
    template<typename T2, typename D2> \
    ORK_INLINE PTR(PTR<T2, D2>&& ptr) ORK_NO_EXCEPT : _ptr(std::move(ptr._ptr)) \
    {} \
    /*Allow conversion from unique_ptr*/ \
    template<typename T2, typename D2> \
    ORK_INLINE PTR(std::unique_ptr<T2, D2>&& ptr) ORK_NO_EXCEPT \
        : _ptr(ptr.release()) \
    {} \
\
    ORK_INLINE PTR& operator=(PTR&& ptr) ORK_NO_EXCEPT \
    { \
        _ptr = std::move(ptr._ptr); \
        return *this; \
    } \
    template<typename T2, typename D2> \
    ORK_INLINE PTR& operator=(PTR<T2, D2>&& ptr) ORK_NO_EXCEPT \
    { \
        _ptr = std::move(ptr._ptr); \
        return *this; \
    } \
    /* Allow conversion from unique_ptr*/ \
    template<typename T2, typename D2> \
    ORK_INLINE PTR& operator=(std::unique_ptr<T2, D2>&& ptr) ORK_NO_EXCEPT \
    { \
        _ptr = std::move(ptr); \
        return *this; \
    }


#define ORK_GET_PTR(PTR) \
    ORK_INLINE element_type& operator*() const ORK_NO_EXCEPT \
    { \
        return _ptr.operator*(); \
    } \
    ORK_INLINE pointer operator->() const ORK_NO_EXCEPT \
    { \
        return _ptr.operator->(); \
    } \
    ORK_INLINE pointer get() const ORK_NO_EXCEPT \
    { \
        return _ptr.get(); \
    } \
    ORK_INLINE explicit operator bool() const ORK_NO_EXCEPT \
    { \
        return _ptr.operator bool(); \
    } \
    template<typename T2, typename D2> \
    ORK_INLINE void swap(PTR<T2, D2>& ptr) ORK_NO_EXCEPT \
    { \
        _ptr.swap(ptr._ptr); \
    }


template<class T, class D>
class value_ptr {
public:
    ORK_PTR_TYPES(value_ptr)

private:
    std::unique_ptr<element_type, deleter_type> _ptr;

public:
    ORK_CONSTEXPR ORK_INLINE value_ptr() ORK_NO_EXCEPT : _ptr() {}
    ORK_CONSTEXPR ORK_INLINE explicit value_ptr(nullptr_t) ORK_NO_EXCEPT : _ptr(nullptr)
    {}
    ORK_INLINE explicit value_ptr(pointer p) ORK_NO_EXCEPT : _ptr(p) {}
    ORK_INLINE value_ptr(pointer p, const deleter_type& del) ORK_NO_EXCEPT
        : _ptr(p, del)
    {}
    ORK_INLINE value_ptr(pointer p, deleter_type&& del) ORK_NO_EXCEPT
        : _ptr(p, std::move(del))
    {}

    ORK_MOVE_PTR(value_ptr)

    // The big payoff
    ORK_INLINE value_ptr(const value_ptr& x)
        : _ptr(x.get_deleter()(*x._ptr))
    {}
    template<class D2>
    ORK_INLINE value_ptr(const value_ptr<element_type, D2>& x)
        : _ptr(x.get_deleter()(*x._ptr))
    {}

    // The big payoff + conversion
    ORK_INLINE value_ptr(const std::unique_ptr<element_type, D>& x)
        : _ptr(x.get_deleter()(*x._ptr))
    {}
    template<class D2>
    ORK_INLINE value_ptr(const std::unique_ptr<element_type, D2>& x)
        : _ptr(x.get_deleter()(*x._ptr))
    {}

    // Destructor is implicitly defined
public:
    ORK_INLINE value_ptr& operator=(nullptr_t) ORK_NO_EXCEPT
    {
        _ptr = nullptr;
        return *this;
    }

    // The big payoff
    ORK_INLINE value_ptr& operator=(const value_ptr& x)
    {
        _ptr.reset(x.get_deleter()(*x));
        return *this;
    }
    template<class D2>
    ORK_INLINE value_ptr& operator=(const value_ptr<element_type, D2>& x)
    {
        _ptr.reset(x.get_deleter()(*x));
        return *this;
    }

    // The big payoff + conversion; this will throw if null, unless the type is contsructible from nullptr
    ORK_INLINE value_ptr& operator=(const std::unique_ptr<element_type, D>& x)
    {
        _ptr.reset(x.get_deleter()(*x));
        return *this;
    }
    template<class D2>
    ORK_INLINE value_ptr& operator=(const std::unique_ptr<element_type, D2>& x)
    {
        _ptr.reset(x.get_deleter()(*x));
        return *this;
    }

public:
    ORK_GET_PTR(value_ptr)

    ORK_INLINE deleter_type& get_deleter() ORK_NO_EXCEPT
    {
        return _ptr.get_deleter();
    }
    ORK_INLINE const deleter_type& get_deleter() const ORK_NO_EXCEPT
    {
        return _ptr.get_deleter();
    }

    ORK_INLINE pointer release() ORK_NO_EXCEPT
    {
        return _ptr.release();
    }
    // The reset interface is different than std, but I prefer overloads to default params
    ORK_INLINE void reset() ORK_NO_EXCEPT
    {
        _ptr.reset();
    }
    template<typename T2>
    ORK_INLINE void reset(T2* p) ORK_NO_EXCEPT
    {
        _ptr.reset(p);
    }
};


/*
The std::unique_ptr provides a templated deleter and an array specialization.
The std::shared_ptr takes a deleter object and provides a weak_ptr interface.
This class is a mixture of the two, a shared pointer with a templated deleter.
Thus every instance is guaranteed to use the same type of deleter.
Neither an array specialization nor a weak_ptr interface is provided.
*/
template<typename T, typename D = std::default_delete<T>>
class shared_ptr {
public:
    ORK_PTR_TYPES(shared_ptr)

private:
    std::shared_ptr<element_type> _ptr;

public:
    ORK_CONSTEXPR ORK_INLINE shared_ptr() ORK_NO_EXCEPT
        : _ptr(nullptr, deleter_type())
    {}
    ORK_CONSTEXPR ORK_INLINE explicit shared_ptr(std::nullptr_t) ORK_NO_EXCEPT
        : shared_ptr()
    {}
    ORK_INLINE explicit shared_ptr(pointer ptr) ORK_NO_EXCEPT
        : _ptr(ptr, deleter_type())
    {}
    template<typename Y>
    ORK_INLINE explicit shared_ptr(Y* ptr)
        : _ptr(ptr, deleter_type())
    {}

    ORK_INLINE shared_ptr(const shared_ptr& ptr) ORK_NO_EXCEPT : _ptr(ptr._ptr)
    {}
    template<typename Y>
    ORK_INLINE shared_ptr(const shared_ptr<Y>& ptr) ORK_NO_EXCEPT : _ptr(ptr._ptr)
    {}

    ORK_MOVE_PTR(shared_ptr)
public:
    ORK_INLINE shared_ptr& operator=(const shared_ptr& ptr) ORK_NO_EXCEPT
    {
        _ptr = ptr._ptr;
        return *this;
    }
    template<typename Y>
    ORK_INLINE shared_ptr& operator=(const shared_ptr<Y>& ptr) ORK_NO_EXCEPT
    {
        _ptr = ptr._ptr;
        return *this;
    }

    // The reset interface is different than std, but I prefer overloads to default params
    ORK_INLINE void reset() ORK_NO_EXCEPT
    {
        _ptr.reset(deleter_type());
    }
    template<typename T2>
    ORK_INLINE void reset(T2* ptr)
    {
        _ptr.reset(ptr, deleter_type());
    }

    ORK_GET_PTR(shared_ptr)

    ORK_INLINE long use_count() const ORK_NO_EXCEPT
    {
        return _ptr.use_count();
    }
    ORK_INLINE bool unique() const ORK_NO_EXCEPT
    {
        return _ptr.unique();
    }
};


#define ORK_PTR_OP(PTR, OP) \
    template<typename T, typename D, typename T2, typename D2> \
    ORK_INLINE bool operator OP(const PTR<T, D>& lhs, const PTR<T2, D2>& rhs) \
        ORK_NO_EXCEPT \
    { \
        return lhs.get() OP rhs.get(); \
    } \
    template<typename T, typename D> \
    ORK_INLINE bool operator OP(const PTR<T, D>& lhs, nullptr_t) ORK_NO_EXCEPT \
    { \
        return lhs.get() OP nullptr; \
    } \
    template<typename T, typename D> \
    ORK_INLINE bool operator OP(nullptr_t, const PTR<T, D>& rhs) ORK_NO_EXCEPT \
    { \
        return nullptr OP rhs.get(); \
    }

#define ORK_PTR_OPS(PTR) \
    ORK_PTR_OP(PTR, ==) \
    ORK_PTR_OP(PTR, !=) \
    ORK_PTR_OP(PTR, <) \
    ORK_PTR_OP(PTR, <=) \
    ORK_PTR_OP(PTR, >) \
    ORK_PTR_OP(PTR, >=)

ORK_PTR_OPS(value_ptr)
ORK_PTR_OPS(shared_ptr)


// TODO: Atomic stuff


} // namespace ork


namespace std {

#define ORK_PTR_SWAP(PTR) \
    template<class T, class D, class T2, class D2> \
    ORK_INLINE void swap(ork::PTR<T, D>& x, ork::PTR<T2, D2>& y) ORK_NO_EXCEPT \
    { \
        x.swap(y); \
    }

ORK_PTR_SWAP(value_ptr)
ORK_PTR_SWAP(shared_ptr)

} // namespace std
