//No include guard!

#if ORK_NEED_STL_SUPPRESS
#pragma warning(push)
#pragma warning(disable:4244) //conversion from 'wchar_t' to 'char'
#pragma warning(disable:4365) //return' signed/unsigned mismatch
#pragma warning(disable:4571) //catch(...) semantics changed for structured exceptions
#pragma warning(disable:4625) //implicit copy constructor
#pragma warning(disable:4626) //implicit copy assignment
#pragma warning(disable:4774) //format string argument is not a string literal
#pragma warning(disable:5026) //implicit move constructor
#pragma warning(disable:5027) //implicit move assignment
#pragma warning(disable:5039) //pointer or reference to throwing function passed to extern C function
#endif

#ifndef _USE_MATH_DEFINES
#   define _USE_MATH_DEFINES 1//M_PI
#endif
#include ORK_STL_INC_FILE
#undef ORK_STL_INC_FILE

#if ORK_NEED_STL_SUPPRESS
#pragma warning(pop)
#endif
