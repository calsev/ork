/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_TOOLS_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_TOOLS_HPP
#define ORK_TOOLS_HPP


namespace ork {


//Make the code execute as one statement; used because it could be more exotic(per-platform) in the future.
#define ORK_STMT(CODE) {CODE} //do{CODE}while(false);


#ifndef ORK_DEPRECATED
#	if ORK_GCC
#		define ORK_DEPRECATED __attribute__((deprecated))
#	else
#		define ORK_DEPRECATED __declspec(deprecated)
#	endif
#endif


/*
Sometimes you gotta C
Usage: ORK_C_LINK ORK_API int ORK_CALL my_func(int arg);
*/
#ifndef ORK_C_LINK
#	ifdef __cplusplus
#		define ORK_C_LINK extern "C"
#	else
#		define ORK_C_LINK extern
#	endif
#endif


#ifndef ORK_INLINE//Generates a warning if the function cannot be inlined
#	if ORK_MSC
#		define ORK_INLINE __forceinline
#	elif ORK_GCC
#		define ORK_INLINE __attribute__((always_inline)) inline
#	else
#		error Compiler not supported
#	endif
#endif


#ifndef ORK_NO_EXCEPT
#	if ORK_GCC || ORK_MSC > 1800//2015
#		define ORK_NO_EXCEPT noexcept
#	else
#		define ORK_NO_EXCEPT//throw()
#	endif
#endif


#ifndef ORK_CONSTEXPR
#	if ORK_GCC || ORK_MSC > 1900//2015
#		define ORK_CONSTEXPR constexpr
#	else
#		define ORK_CONSTEXPR 
#	endif
#endif


#ifndef ORK_UNUSED
#	if ORK_GCC
#		define ORK_UNUSED __attribute__((unused))
#	else
#		define ORK_UNUSED
#	endif
#endif


#define ORK_UNREACHABLE ORK_THROW(ORK("Unreachable"));


/*
Copy and move semantics
*/

#define ORK_COPY_(TYPE)\
	TYPE(const TYPE&)=default;\
	TYPE&operator=(const TYPE&)=default;

#if ORK_GCC || ORK_MSC > 1800//VS 2015 or later
#define ORK_NO_COPY_(TYPE)\
			TYPE(const TYPE&)=delete;\
			TYPE&operator=(const TYPE&)=delete;
#define ORK_NO_MOVE_(TYPE)\
			TYPE(TYPE&&)=delete;\
			TYPE&operator=(TYPE&&)=delete;
#define ORK_MOVE_(TYPE)\
		TYPE(TYPE&&)=default;\
		TYPE&operator=(TYPE&&)=default;
#else
#define ORK_NO_COPY_(TYPE)\
			private:\
				TYPE(const TYPE&);\
				TYPE&operator=(const TYPE&);
#define ORK_NO_MOVE_(TYPE) ORK_EMPTY
#define ORK_MOVE_(TYPE) ORK_EMPTY
#endif

#define ORK_NON_COPYABLE(TYPE) ORK_NO_COPY_(TYPE) ORK_NO_MOVE_(TYPE)
#if ORK_GCC || ORK_MSC > 1800//VS 2015 or later
#	define ORK_MOVE_ONLY(TYPE) ORK_NO_COPY_(TYPE) ORK_MOVE_(TYPE)
#	define ORK_MOVEABLE(TYPE) ORK_COPY_(TYPE) ORK_MOVE_(TYPE)
#endif


/***
Begin Looping section.
Macros to simplify basic loops.
This may seem like overkill,but the loop headers are easier to type,
less error prone,and consistently optimized.
***/
#define LOOP_(LIMIT,INDEX)for(size_t INDEX=0;INDEX!=LIMIT;++INDEX)
#define LOOPI(LIMIT)LOOP_(LIMIT,i)
#define LOOPJ(LIMIT)LOOP_(LIMIT,j)
#define LOOPK(LIMIT)LOOP_(LIMIT,k)
#define LOOPL(LIMIT)LOOP_(LIMIT,l)

#define LOOPR(LIMIT,INDEX)for(size_t INDEX=LIMIT;INDEX--!=0;/*Do Nothing*/)
#define LOOPRI(LIMIT)LOOPR(LIMIT,i)
#define LOOPRJ(LIMIT)LOOPR(LIMIT,j)
#define LOOPRK(LIMIT)LOOPR(LIMIT,k)
#define LOOPRL(LIMIT)LOOPR(LIMIT,l)

/*Loops over vectors, the limit variable must be obviously constant for the compiler to vectorize anything.*/
#define LOOPV(SERIES,INDEX)for(size_t INDEX=0,limit##INDEX=SERIES.size();INDEX!=limit##INDEX;++INDEX)
#define LOOPVI(SERIES)LOOPV(SERIES,i)
#define LOOPVJ(SERIES)LOOPV(SERIES,j)
#define LOOPVK(SERIES)LOOPV(SERIES,k)
#define LOOPVL(SERIES)LOOPV(SERIES,l)


/*
Basic macros section; the building blocks
*/

#define ORK_EMPTY /*Empty*/

//Expand arguments, used mostly because of MSC
#define ORK_EVAL(...) __VA_ARGS__

#define ORK_STR_(X) #X
#define ORK_STR(X) ORK_STR_(X)

//These basic dumb macros are used to generate variadic macros
#define ORK_CAT_00_()
#define ORK_CAT_01_(A1) A1
#define ORK_CAT_02_(A1, A2) A1##A2
#define ORK_CAT_03_(A1, A2, A3) A1##A2##A3
#define ORK_CAT_04_(A1, A2, A3, A4) A1##A2##A3##A4
#define ORK_CAT_05_(A1, A2, A3, A4, A5) A1##A2##A3##A4##A5
#define ORK_CAT_06_(A1, A2, A3, A4, A5, A6) A1##A2##A3##A4##A5##A6
#define ORK_CAT_07_(A1, A2, A3, A4, A5, A6, A7) A1##A2##A3##A4##A5##A6##A7
#define ORK_CAT_08_(A1, A2, A3, A4, A5, A6, A7, A8) A1##A2##A3##A4##A5##A6##A7##A8
#define ORK_CAT_09_(A1, A2, A3, A4, A5, A6, A7, A8, A9) A1##A2##A3##A4##A5##A6##A7##A8##A9

#define ORK_CAT(...) ORK_EVAL( \
	ORK_ARG_9( \
		__VA_ARGS__, ORK_CAT_09_, ORK_CAT_08_, ORK_CAT_07_, ORK_CAT_06_, ORK_CAT_05_, \
		ORK_CAT_04_, ORK_CAT_03_, ORK_CAT_02_, ORK_CAT_01_, ORK_CAT_00_) \
	(__VA_ARGS__))

// This is a primitive IF macro, that supports only 0, 1
#define ORK_IF_0_(T, ...) ORK_EVAL(__VA_ARGS__)
#define ORK_IF_1_(T, ...) T
#define ORK_IF_(C) ORK_CAT(ORK_IF_, C, _)

// If passed < 2 arguments, expands to 0, otherwise argument 2
#define ORK_CHECK_1_(...) ORK_EVAL(ORK_ARG_1(__VA_ARGS__, 0, 0))
#define ORK_APPEND_1_(...) ORK_EVAL(__VA_ARGS__, 1)

// If called, returns two arguments
#define ORK_IS_PAREN_() ORK_APPEND_1_(~)
// Parentheses after a macro name call that macro, otherwise the name is just another token:
// If X is parentheses, will generate two arguments: ~, 1
// Else, will generate one argument: ORK_IS_PAREN_ X
#define ORK_IS_PAREN(X) ORK_CHECK_1_(ORK_IS_PAREN_ X)

#define ORK_NOT_0_ ORK_APPEND_1_(~)
//Returns 1 for 0, 0 for everything else
#define ORK_NOT(X) ORK_CHECK_1_(ORK_CAT(ORK_NOT_, X, _))

// Transforms everything but 0 to 1
#define ORK_BOOL(X) ORK_NOT(ORK_NOT(X))

// If ... Else construct
#define ORK_IF(C) ORK_IF_(ORK_BOOL(C))
#define ORK_DISCARD(...)
//Conditional evaluation; trivial else clause
#define ORK_WHEN(C) ORK_IF(C)(ORK_EVAL, ORK_DISCARD)

#define ORK_EMPTY_()
#define ORK_DEFER_(X) X ORK_EMPTY_()
// Go two contexts deeper, to avoid macro expansion
#define ORK_CONTEXT_(...) __VA_ARGS__ ORK_DEFER_(ORK_EMPTY_)()

// Support max depth/length of 64
#define ORK_FLAT_2_(...) \
    ORK_EVAL(ORK_EVAL(ORK_EVAL(ORK_EVAL(__VA_ARGS__)))) // 2^2
#define ORK_FLAT_1_(...) \
    ORK_FLAT_2_(ORK_FLAT_2_(ORK_FLAT_2_(ORK_FLAT_2_(__VA_ARGS__)))) // 2^4
#define ORK_FLAT(...) \
    ORK_FLAT_1_(ORK_FLAT_1_(ORK_FLAT_1_(ORK_FLAT_1_(__VA_ARGS__)))) // 2^6

#define ORK_PHX_(...) ORK_EVAL(ORK_PHX_ __VA_ARGS__)
#define ORK_SKIP_ORK_PHX_
#define ORK_UNPAREN(...) ORK_FLAT(ORK_CAT(ORK_SKIP_, ORK_PHX_ __VA_ARGS__))

#define ORK_MAP_3_() ORK_MAP_2_
// This macro intentionally does not evaluate to avoid direct recursion
#define ORK_MAP_2_(CALL, STITCH, DATA, I, ARG, PEEK, ...) \
    ORK_IF(ORK_IS_PAREN(PEEK)) \
    (CALL(DATA, I, ARG), \
     STITCH(DATA, I, CALL(DATA, I, ARG), ORK_CONTEXT_(ORK_MAP_3_)()(CALL, STITCH, DATA, ORK_CAT(ORK_INC_, I), PEEK, __VA_ARGS__)))

// The end-of-list marker, '()',  will call a macro
// Empty at the end to maintain compliant call when macro called.
/*
MAP is the basis of most high-level variadic macros.

DATA is passed to every call, and can be a tuple.  Example:
#define DATA (One, Two)

I is the iteration number

CALL is invoked as CALL(DATA, I, ARG). Example:
#define CALL(DATA, I, ARG) ORK_ARG_0 D--ARG--ORK_ARG_1 D

STITCH is invoked as STITCH(DATA, I, HEAD, TAIL) and is right
associative.  Example: #define STITCH(DATA, I, X, Y) (X *ORK_EVAL D, I* Y)

Completing the example:
ORK_MAP(CALL, STITCH, DATA, a, b, c) -> (One--a--Two *One, Two, 0* (One--b--Two *One, Two, 1* One--c--Two))
*/
#define ORK_MAP(CALL, STITCH, DATA, ...) \
    ORK_FLAT(ORK_MAP_2_(CALL, STITCH, DATA, 0, __VA_ARGS__, (), 0))


/*
Convenience definitions for variadic macros
*/

#define ORK_NUM_ARG_(...) ORK_EVAL(ORK_ARG_N_(__VA_ARGS__))
#define ORK_NUM_ARG(...) ORK_EVAL(ORK_NUM_ARG_(__VA_ARGS__, ORK_DESCENDING_N_))


// Common CALL paradigms
#define ORK_IDENTITY(DATA, I, ARG) ARG
#define ORK_PREFIX(DATA, I, ARG) DATA ARG
#define ORK_POSTFIX(DATA, I, ARG) ARG DATA

// Common STITCH paradigms
#define ORK_COMMA(DATA, I, X, Y) X, Y
#define ORK_SEMI_COLON(DATA, I, X, Y) X; Y
#define ORK_SPACE(DATA, I, X, Y) X Y


/*
Begin enum section: defining enums with an iterable container and string conversions in two lines
*/


#define ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) \
if(str == ORK_CAT(PRE, ARG)) {\
	return ENUM::ARG;\
}
#define ORK_STR_2_ENUM_LIST_00_(ENUM, PRE)
#define ORK_STR_2_ENUM_LIST_01_(ENUM, PRE, ARG) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG))
#define ORK_STR_2_ENUM_LIST_02_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_01_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_03_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_02_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_04_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_03_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_05_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_04_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_06_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_05_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_07_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_06_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_08_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_07_(ENUM, PRE, __VA_ARGS__))
#define ORK_STR_2_ENUM_LIST_09_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_STR_2_ENUM_IF_(ENUM, PRE, ARG) ORK_STR_2_ENUM_LIST_08_(ENUM, PRE, __VA_ARGS__))

#define ORK_STR_2_ENUM_LIST(ENUM, PRE, ...) ORK_EVAL(\
	ORK_ARG_9( \
		__VA_ARGS__, ORK_STR_2_ENUM_LIST_09_, ORK_STR_2_ENUM_LIST_08_, ORK_STR_2_ENUM_LIST_07_, ORK_STR_2_ENUM_LIST_06_, ORK_STR_2_ENUM_LIST_05_,	\
		ORK_STR_2_ENUM_LIST_04_, ORK_STR_2_ENUM_LIST_03_, ORK_STR_2_ENUM_LIST_02_, ORK_STR_2_ENUM_LIST_01_, ORK_STR_2_ENUM_LIST_00_) \
	(ENUM, PRE, __VA_ARGS__))


#define ORK_ENUM_SET_(ENUM, ...) const std::array<ENUM, ORK_NUM_ARG(__VA_ARGS__)>

#define ORK_ENUM_DECL_(API, ENUM, ...)\
enum class ENUM {\
	ORK_MAP(ORK_IDENTITY, ORK_COMMA, ORK_EMPTY, __VA_ARGS__)\
};\
API(ORK_ENUM_SET_(ENUM, __VA_ARGS__)&) ORK_CAT(ENUM, _set)();\
API(const ork::string&) to_string(const ENUM);\
API(const ork::bstring&) to_bstring(const ENUM);\
API(const ork::wstring&) to_wstring(const ENUM);\
API(ENUM) ORK_CAT(to_, ENUM)(const ork::bstring&);\
API(ENUM) ORK_CAT(to_, ENUM)(const ork::wstring&);\
template<> API(ENUM) from_string<ENUM>(const bstring&str); \
template<> API(ENUM) from_string<ENUM>(const wstring&str);


#define ORK_ENUM_DECL(ENUM, ...) ORK_ENUM_DECL_(ORK_EXT, ENUM, __VA_ARGS__)
#define ORK_ORK_ENUM_DECL(ENUM, ...) ORK_ENUM_DECL_(ORK_ORK_EXT, ENUM, __VA_ARGS__)


// Data must be formatted (PRE, STR)
#define ORK_MAKE_STRING_(DATA, I, ARG) ORK_EVAL(const ork::ORK_CAT(ORK_ARG_0 DATA, string) ORK_CAT(ORK_ARG_0 DATA, ARG){ORK_ARG_1 DATA(ORK_STR(ARG))};)


// Data must be formatted (ENUM, PRE)
#define ORK_CASE_(DATA, I, ARG) case ORK_ARG_0 DATA::ARG: return ORK_CAT(ORK_ARG_1 DATA, ARG)

#define ORK_ENUM_2_STR_(API, ENUM, PRE, ...) ORK_EVAL( \
API(const ork::ORK_CAT(PRE, string)&) ORK_CAT(to_, PRE, string)(const ENUM val) {\
	switch(val) {\
		ORK_MAP(ORK_CASE_, ORK_SEMI_COLON, (ENUM, PRE), __VA_ARGS__);\
	};\
	ORK_UNREACHABLE\
})


#if ORK_UNICODE
#   define ORK_SAME_STR_ wstring
#else
#   define ORK_SAME_STR_ bstring
#endif


#define ORK_STR_2_ENUM_(API, ENUM, PRE, CONV, ...) ORK_EVAL(\
API(ENUM) ORK_CAT(to_, ENUM)(const ork::ORK_CAT(PRE, string)& str) { \
	ORK_STR_2_ENUM_LIST(ENUM, PRE, __VA_ARGS__) \
	ORK_THROW(ORK("Invalid ") << ORK_STR(ENUM) << ORK(": ") << CONV(str));\
}\
template<> API(ENUM) from_string<ENUM>(const ork::ORK_CAT(PRE, string)& str) {\
	return ORK_CAT(to_, ENUM)(str);\
})


#define ORK_ENUM_DEF_(API, ENUM, ...) \
API(ORK_ENUM_SET_(ENUM, __VA_ARGS__)&) ORK_CAT(ENUM, _set)() {\
	static ORK_ENUM_SET_(ENUM, __VA_ARGS__) val = {{\
		ORK_MAP(ORK_PREFIX, ORK_COMMA, ENUM::, __VA_ARGS__)\
	}};\
	return val;\
};\
ORK_MAP(ORK_MAKE_STRING_, ORK_SEMI_COLON, (b, BORK), __VA_ARGS__); \
ORK_MAP(ORK_MAKE_STRING_, ORK_SEMI_COLON, (w, WORK), __VA_ARGS__); \
ORK_ENUM_2_STR_(API, ENUM, b, __VA_ARGS__) \
ORK_ENUM_2_STR_(API, ENUM, w, __VA_ARGS__) \
API(const ork::string&) to_string(const ENUM val) { \
	return ORK_CAT(to_, ORK_SAME_STR_)(val); \
} \
ORK_STR_2_ENUM_(API, ENUM, b, ORK_BYTE_2_STR, __VA_ARGS__)\
ORK_STR_2_ENUM_(API, ENUM, w, ORK_WIDE_2_STR, __VA_ARGS__)

#define ORK_ENUM_DEF(ENUM, ...) ORK_ENUM_DEF_(ORK_EXT, ENUM, __VA_ARGS__)
#define ORK_ORK_ENUM_DEF(ENUM, ...) ORK_ENUM_DEF_(ORK_ORK_EXT, ENUM, __VA_ARGS__)


}//namespace ork

#endif//ORK_TOOLS_HPP
