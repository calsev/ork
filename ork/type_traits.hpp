/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"

#define ORK_STL_INC_FILE <type_traits>
#include "ork/core/stl_include.inl"


namespace ork {


/*
These are optimal types for function signatures
*/
template<typename T>
struct signature_traits {
private:
    static_assert(
        std::is_enum<T>::value || std::is_arithmetic<T>::value || std::is_class<T>::value,
        "Yeah, this trait is not general");
    using non_cv_type = typename std::remove_cv<T>::type;
    using val_t = non_cv_type;
    using cval_t = typename std::add_const<val_t>::type;
    using cref_t = typename std::add_lvalue_reference<cval_t>::type;
    static const auto fund_t = std::is_enum<T>::value || std::is_arithmetic<T>::value;

public:
    using value_type = val_t;
    using const_value_type = cval_t;

    using reference_type = typename std::add_lvalue_reference<val_t>::type;
    using const_reference_type = cref_t;

    using rreference_type = typename std::add_rvalue_reference<val_t>::type;

    using pointer_type = typename std::add_pointer<val_t>::type;
    using const_pointer_type = typename std::add_pointer<cval_t>::type;

    // Parameter to a function
    using const_param_type = typename std::conditional<fund_t, cval_t, cref_t>::type;
    // Wrapper return type
    using const_wrap_type = typename std::conditional<fund_t, val_t, cref_t>::type;
};

// For use in templates
#define ORK_VAL_T typename ork::signature_traits<T>::value_type
#define ORK_CVAL_T typename ork::signature_traits<T>::const_value_type

#define ORK_REF_T typename ork::signature_traits<T>::reference_type
#define ORK_CREF_T typename ork::signature_traits<T>::const_reference_type

#define ORK_RREF_T typename ork::signature_traits<T>::rreference_type

#define ORK_PTR_T typename ork::signature_traits<T>::pointer_type
#define ORK_CPTR_T typename ork::signature_traits<T>::const_pointer_type

#define ORK_CPARAM_T typename ork::signature_traits<T>::const_param_type
#define ORK_CWRAP_T typename ork::signature_traits<T>::const_wrap_type

// For use in macros
#define ORK_VAL(TYPE) ork::signature_traits<TYPE>::value_type
#define ORK_CVAL(TYPE) ork::signature_traits<TYPE>::const_value_type

#define ORK_REF(TYPE) ork::signature_traits<TYPE>::reference_type
#define ORK_CREF(TYPE) ork::signature_traits<TYPE>::const_reference_type

#define ORK_RREF(TYPE) ork::signature_traits<TYPE>::rreference_type

#define ORK_PTR(TYPE) ork::signature_traits<TYPE>::pointer_type
#define ORK_CPTR(TYPE) ork::signature_traits<TYPE>::const_pointer_type

#define ORK_CPARAM(TYPE) ork::signature_traits<TYPE>::const_param_type
#define ORK_CWRAP(TYPE) ork::signature_traits<TYPE>::const_wrap_type


} // namespace ork
