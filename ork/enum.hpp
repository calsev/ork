/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include "ork/ork.hpp"


namespace ork {




#if ORK_UNICODE
#    define ORK_SAME_STR_ wstring
#else
#    define ORK_SAME_STR_ bstring
#endif


// Data must be formatted as (ENUM, PRE)
#define ORK_STR_2_ENUM_IF_(DATA, I, ARG) \
    if(str == ORK_CAT(ORK_ARG_0 DATA, _ns)::ORK_CAT(ORK_ARG_1 DATA, ARG)) { \
        return ORK_ARG_0 DATA::ARG; \
    }

#define ORK_STR_2_ENUM_(API, ENUM, PRE, CONV, ...) \
    ORK_EVAL(API(ENUM) ORK_CAT(to_, ENUM)(const ork::ORK_CAT(PRE, string) & str) { \
        ORK_MAP(ORK_STR_2_ENUM_IF_, ORK_SPACE, (ENUM, PRE), __VA_ARGS__) \
        ORK_THROW(ORK("Invalid ") << ORK_STR(ENUM) << ORK(": ") << CONV(str)); \
    } template<> API(ENUM) from_string<ENUM>(const ork::ORK_CAT(PRE, string) & str) { \
        return ORK_CAT(to_, ENUM)(str); \
    })


#define ORK_ENUM_DEF_(API, ENUM, ...) \
    API(ORK_ENUM_SET_(ENUM, __VA_ARGS__)&) ORK_CAT(ENUM, _set)() \
    { \
        static ORK_ENUM_SET_(ENUM, __VA_ARGS__) \
            val = {{ORK_MAP(ORK_PREFIX, ORK_COMMA, ENUM::, __VA_ARGS__)}}; \
        return val; \
    }; \
    namespace ORK_CAT(ENUM, _ns) \
    { \
        ORK_MAP(ORK_MAKE_STRING_, ORK_SEMI_COLON, (b, BORK), __VA_ARGS__); \
        ORK_MAP(ORK_MAKE_STRING_, ORK_SEMI_COLON, (w, WORK), __VA_ARGS__); \
    } \
    ORK_ENUM_2_STR_(API, ENUM, b, __VA_ARGS__) \
    ORK_ENUM_2_STR_(API, ENUM, w, __VA_ARGS__) \
    API(const ork::string&) to_string(const ENUM val) \
    { \
        return ORK_CAT(to_, ORK_SAME_STR_)(val); \
    } \
    ORK_STR_2_ENUM_(API, ENUM, b, ORK_BYTE_2_STR, __VA_ARGS__) \
    ORK_STR_2_ENUM_(API, ENUM, w, ORK_WIDE_2_STR, __VA_ARGS__)

#define ORK_ENUM_DEF(ENUM, ...) ORK_ENUM_DEF_(ORK_EXT, ENUM, __VA_ARGS__)
#define ORK_ORK_ENUM_DEF(ENUM, ...) \
    ORK_ENUM_DEF_(ORK_ORK_EXT, ENUM, __VA_ARGS__)


} // namespace ork
