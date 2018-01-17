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


#define ORK_EMPTY /**/


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


#define ORK_EMPTY_ /*Empty*/


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
	#define ORK_NO_MOVE_(TYPE) ORK_EMPTY_
	#define ORK_MOVE_(TYPE) ORK_EMPTY_
#endif

#define ORK_NON_COPYABLE(TYPE) ORK_NO_COPY_(TYPE) ORK_NO_MOVE_(TYPE)
#if ORK_GCC || ORK_MSC > 1800//VS 2015 or later
#	define ORK_MOVE_ONLY(TYPE) ORK_NO_COPY_(TYPE) ORK_MOVE_(TYPE)
#	define ORK_MOVEABLE(TYPE) ORK_COPY_(TYPE) ORK_MOVE_(TYPE)
#endif


/*
The macros below provide more control over construction, including insertion of
code in both the constructor and assignment operator.
These macros also support VS 2012.
*/
//Construction
#define ORK_CMOVE_(M) M(std::move(rhs.M))
#define ORK_CMOVE_1_(M1)				ORK_CMOVE_(M1)
#define ORK_CMOVE_2_(M1,M2)				ORK_CMOVE_1_(M1),			ORK_CMOVE_(M2)
#define ORK_CMOVE_3_(M1,M2,M3)			ORK_CMOVE_2_(M1,M2),		ORK_CMOVE_(M3)
#define ORK_CMOVE_4_(M1,M2,M3,M4)		ORK_CMOVE_3_(M1,M2,M3),		ORK_CMOVE_(M4)
#define ORK_CMOVE_5_(M1,M2,M3,M4,M5)	ORK_CMOVE_4_(M1,M2,M3,M4),	ORK_CMOVE_(M5)

//Assignment
#define ORK_AMOVE_(M) M=std::move(rhs.M);
#define ORK_AMOVE_1_(M1)				ORK_AMOVE_(M1)
#define ORK_AMOVE_2_(M1,M2)				ORK_AMOVE_1_(M1)			ORK_AMOVE_(M2)
#define ORK_AMOVE_3_(M1,M2,M3)			ORK_AMOVE_2_(M1,M2)			ORK_AMOVE_(M3)
#define ORK_AMOVE_4_(M1,M2,M3,M4)		ORK_AMOVE_3_(M1,M2,M3)		ORK_AMOVE_(M4)
#define ORK_AMOVE_5_(M1,M2,M3,M4,M5)	ORK_AMOVE_4_(M1,M2,M3,M4)	ORK_AMOVE_(M5)


/*
This constructor somewhat surreptitiously inserts noexcept
but a throwing move constructor is pointless anyway
*/
#define ORK_MOVE_CONSTRUCT_(TYPE,CODE)\
public:\
	TYPE(TYPE&&rhs)ORK_NO_EXCEPT:CODE{
/*Constructor body here*/
#define ORK_MOVE_ASSIGN_(TYPE,CODE)\
	}\
public:\
	TYPE&operator=(TYPE&&rhs)ORK_NO_EXCEPT{\
		if(this!=&rhs){\
			CODE
/*Assignment body here*/
#define ORK_MOVE_END_\
		}\
		return *this;\
	}


/*
These macros wrap up functionality.
The first inserts the same code in the constuctor and assigment operator.
The second inserts no code.
*/
#define ORK_MOVE_CON1_(TYPE,M1)				ORK_MOVE_CONSTRUCT_(TYPE,ORK_CMOVE_1_(M1))
#define ORK_MOVE_CON2_(TYPE,M1,M2)			ORK_MOVE_CONSTRUCT_(TYPE,ORK_CMOVE_2_(M1,M2))
#define ORK_MOVE_CON3_(TYPE,M1,M2,M3)		ORK_MOVE_CONSTRUCT_(TYPE,ORK_CMOVE_3_(M1,M2,M3))
#define ORK_MOVE_CON4_(TYPE,M1,M2,M3,M4)	ORK_MOVE_CONSTRUCT_(TYPE,ORK_CMOVE_4_(M1,M2,M3,M4))
#define ORK_MOVE_CON5_(TYPE,M1,M2,M3,M4,M5)	ORK_MOVE_CONSTRUCT_(TYPE,ORK_CMOVE_5_(M1,M2,M3,M4,M5))

#define ORK_MOVE_OP1_(TYPE,M1)				ORK_MOVE_ASSIGN_(TYPE,ORK_AMOVE_1_(M1))
#define ORK_MOVE_OP2_(TYPE,M1,M2)			ORK_MOVE_ASSIGN_(TYPE,ORK_AMOVE_2_(M1,M2))
#define ORK_MOVE_OP3_(TYPE,M1,M2,M3)		ORK_MOVE_ASSIGN_(TYPE,ORK_AMOVE_3_(M1,M2,M3))
#define ORK_MOVE_OP4_(TYPE,M1,M2,M3,M4)		ORK_MOVE_ASSIGN_(TYPE,ORK_AMOVE_4_(M1,M2,M3,M4))
#define ORK_MOVE_OP5_(TYPE,M1,M2,M3,M4,M5)	ORK_MOVE_ASSIGN_(TYPE,ORK_AMOVE_5_(M1,M2,M3,M4,M5))

#define ORK_MOVE_1_(TYPE,M1,CODE)				ORK_MOVE_CON1_(TYPE,M1)				CODE ORK_MOVE_OP1_(TYPE,M1)				CODE ORK_MOVE_END_
#define ORK_MOVE_2_(TYPE,M1,M2,CODE)			ORK_MOVE_CON2_(TYPE,M1,M2)			CODE ORK_MOVE_OP2_(TYPE,M1,M2)			CODE ORK_MOVE_END_
#define ORK_MOVE_3_(TYPE,M1,M2,M3,CODE)			ORK_MOVE_CON3_(TYPE,M1,M2,M3)		CODE ORK_MOVE_OP3_(TYPE,M1,M2,M3)		CODE ORK_MOVE_END_
#define ORK_MOVE_4_(TYPE,M1,M2,M3,M4,CODE)		ORK_MOVE_CON4_(TYPE,M1,M2,M3,M4)	CODE ORK_MOVE_OP4_(TYPE,M1,M2,M3,M4)	CODE ORK_MOVE_END_
#define ORK_MOVE_5_(TYPE,M1,M2,M3,M4,M5,CODE)	ORK_MOVE_CON5_(TYPE,M1,M2,M3,M4,M5)	CODE ORK_MOVE_OP5_(TYPE,M1,M2,M3,M4,M5)	CODE ORK_MOVE_END_


#define ORK_MOVEABLE_1(TYPE,M1)				ORK_COPY_(TYPE) ORK_MOVE_1_(TYPE,M1,ORK_EMPTY_)
#define ORK_MOVEABLE_2(TYPE,M1,M2)			ORK_COPY_(TYPE) ORK_MOVE_2_(TYPE,M1,M2,ORK_EMPTY_)
#define ORK_MOVEABLE_3(TYPE,M1,M2,M3)		ORK_COPY_(TYPE) ORK_MOVE_3_(TYPE,M1,M2,M3,ORK_EMPTY_)
#define ORK_MOVEABLE_4(TYPE,M1,M2,M3,M4)	ORK_COPY_(TYPE) ORK_MOVE_4_(TYPE,M1,M2,M3,M4,ORK_EMPTY_)
#define ORK_MOVEABLE_5(TYPE,M1,M2,M3,M4,M5)	ORK_COPY_(TYPE) ORK_MOVE_5_(TYPE,M1,M2,M3,M4,M5,ORK_EMPTY_)

#define ORK_MOVE_ONLY_1(TYPE,M1)				ORK_NO_COPY_(TYPE) ORK_MOVE_1_(TYPE,M1,ORK_EMPTY_)
#define ORK_MOVE_ONLY_2(TYPE,M1,M2)				ORK_NO_COPY_(TYPE) ORK_MOVE_2_(TYPE,M1,M2,ORK_EMPTY_)
#define ORK_MOVE_ONLY_3(TYPE,M1,M2,M3)			ORK_NO_COPY_(TYPE) ORK_MOVE_3_(TYPE,M1,M2,M3,ORK_EMPTY_)
#define ORK_MOVE_ONLY_4(TYPE,M1,M2,M3,M4)		ORK_NO_COPY_(TYPE) ORK_MOVE_4_(TYPE,M1,M2,M3,M4,ORK_EMPTY_)
#define ORK_MOVE_ONLY_5(TYPE,M1,M2,M3,M4,M5)	ORK_NO_COPY_(TYPE) ORK_MOVE_5_(TYPE,M1,M2,M3,M4,M5,ORK_EMPTY_)


/*
Begin enum section: defining enums with an iterable container and string conversions in two lines
*/
#define ORK_EVAL(...) __VA_ARGS__


#define ORK_STR_(X) #X
#define ORK_STR(X) ORK_STR_(X)


#define ORK_GET_ARG_00_(A0, ...) A0
#define ORK_GET_ARG_01_(A0, A1, ...) A1
#define ORK_GET_ARG_02_(A0, A1, A2, ...) A2
#define ORK_GET_ARG_03_(A0, A1, A2, A3, ...) A3
#define ORK_GET_ARG_04_(A0, A1, A2, A3, A4, ...) A4
#define ORK_GET_ARG_05_(A0, A1, A2, A3, A4, A5, ...) A5
#define ORK_GET_ARG_06_(A0, A1, A2, A3, A4, A5, A6, ...) A6
#define ORK_GET_ARG_07_(A0, A1, A2, A3, A4, A5, A6, A7, ...) A7
#define ORK_GET_ARG_08_(A0, A1, A2, A3, A4, A5, A6, A7, A8, ...) A8
#define ORK_GET_ARG_09_(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, ...) A9


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
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_CAT_09_, ORK_CAT_08_, ORK_CAT_07_, ORK_CAT_06_, ORK_CAT_05_, \
		ORK_CAT_04_, ORK_CAT_03_, ORK_CAT_02_, ORK_CAT_01_, ORK_CAT_00_) \
	(__VA_ARGS__))


#define ORK_CALL_EACH_00_(CALL)
#define ORK_CALL_EACH_01_(CALL, A1) CALL(A1)
#define ORK_CALL_EACH_02_(CALL, A1, A2) CALL(A1) CALL(A2)
#define ORK_CALL_EACH_03_(CALL, A1, A2, A3) CALL(A1) CALL(A2) CALL(A3)
#define ORK_CALL_EACH_04_(CALL, A1, A2, A3, A4) CALL(A1) CALL(A2) CALL(A3) CALL(A4)
#define ORK_CALL_EACH_05_(CALL, A1, A2, A3, A4, A5) CALL(A1) CALL(A2) CALL(A3) CALL(A4) CALL(A5)
#define ORK_CALL_EACH_06_(CALL, ARG, ...) CALL(ARG) ORK_EVAL(ORK_CALL_EACH_05_(CALL, __VA_ARGS__))
#define ORK_CALL_EACH_07_(CALL, ARG, ...) CALL(ARG) ORK_EVAL(ORK_CALL_EACH_06_(CALL, __VA_ARGS__))
#define ORK_CALL_EACH_08_(CALL, ARG, ...) CALL(ARG) ORK_EVAL(ORK_CALL_EACH_07_(CALL, __VA_ARGS__))
#define ORK_CALL_EACH_09_(CALL, ARG, ...) CALL(ARG) ORK_EVAL(ORK_CALL_EACH_08_(CALL, __VA_ARGS__))

#define ORK_CALL_EACH(CALL, ...) ORK_EVAL( \
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_CALL_EACH_09_, ORK_CALL_EACH_08_, ORK_CALL_EACH_07_, ORK_CALL_EACH_06_, ORK_CALL_EACH_05_, \
		ORK_CALL_EACH_04_, ORK_CALL_EACH_03_, ORK_CALL_EACH_02_, ORK_CALL_EACH_01_, ORK_CALL_EACH_00_) \
	(CALL, __VA_ARGS__))


#define ORK_LIST_00_(PRE, DELIM)
#define ORK_LIST_01_(PRE, DELIM, A1) PRE A1
#define ORK_LIST_02_(PRE, DELIM, A1, A2) PRE A1 DELIM PRE A2 
#define ORK_LIST_03_(PRE, DELIM, A1, A2, A3) PRE A1 DELIM PRE A2 DELIM PRE A3
#define ORK_LIST_04_(PRE, DELIM, A1, A2, A3, A4) PRE A1 DELIM PRE A2 DELIM PRE A3 DELIM PRE A4 
#define ORK_LIST_05_(PRE, DELIM, ARG, ...) PRE ARG DELIM ORK_EVAL(ORK_LIST_04_(PRE, DELIM, __VA_ARGS__))
#define ORK_LIST_06_(PRE, DELIM, ARG, ...) PRE ARG DELIM ORK_EVAL(ORK_LIST_05_(PRE, DELIM, __VA_ARGS__))
#define ORK_LIST_07_(PRE, DELIM, ARG, ...) PRE ARG DELIM ORK_EVAL(ORK_LIST_06_(PRE, DELIM, __VA_ARGS__))
#define ORK_LIST_08_(PRE, DELIM, ARG, ...) PRE ARG DELIM ORK_EVAL(ORK_LIST_07_(PRE, DELIM, __VA_ARGS__))
#define ORK_LIST_09_(PRE, DELIM, ARG, ...) PRE ARG DELIM ORK_EVAL(ORK_LIST_08_(PRE, DELIM, __VA_ARGS__))

#define ORK_LIST(PRE, DELIM, ...) ORK_EVAL( \
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_LIST_09_, ORK_LIST_08_, ORK_LIST_07_, ORK_LIST_06_, ORK_LIST_05_, \
		ORK_LIST_04_, ORK_LIST_03_, ORK_LIST_02_, ORK_LIST_01_, ORK_LIST_00_) \
	(PRE, DELIM, __VA_ARGS__))


#define ORK_COMMA_LIST_00_(PRE)
#define ORK_COMMA_LIST_01_(PRE, A1) PRE A1
#define ORK_COMMA_LIST_02_(PRE, A1, A2) PRE A1, PRE A2
#define ORK_COMMA_LIST_03_(PRE, A1, A2, A3) PRE A1, PRE A2, PRE A3
#define ORK_COMMA_LIST_04_(PRE, A1, A2, A3, A4) PRE A1, PRE A2, PRE A3, PRE A4
#define ORK_COMMA_LIST_05_(PRE, A1, A2, A3, A4, A5) PRE A1, PRE A2, PRE A3, PRE A4, PRE A5
#define ORK_COMMA_LIST_06_(PRE, ARG, ...) PRE ARG, ORK_EVAL(ORK_COMMA_LIST_05_(PRE, __VA_ARGS__))
#define ORK_COMMA_LIST_07_(PRE, ARG, ...) PRE ARG, ORK_EVAL(ORK_COMMA_LIST_06_(PRE, __VA_ARGS__))
#define ORK_COMMA_LIST_08_(PRE, ARG, ...) PRE ARG, ORK_EVAL(ORK_COMMA_LIST_07_(PRE, __VA_ARGS__))
#define ORK_COMMA_LIST_09_(PRE, ARG, ...) PRE ARG, ORK_EVAL(ORK_COMMA_LIST_08_(PRE, __VA_ARGS__))

#define ORK_COMMA_LIST(PRE, ...) ORK_EVAL(\
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_COMMA_LIST_09_, ORK_COMMA_LIST_08_, ORK_COMMA_LIST_07_, ORK_COMMA_LIST_06_, ORK_COMMA_LIST_05_,	\
		ORK_COMMA_LIST_04_, ORK_COMMA_LIST_03_, ORK_COMMA_LIST_02_, ORK_COMMA_LIST_01_, ORK_COMMA_LIST_00_) \
	(PRE, __VA_ARGS__))


#define ORK_COUNT_ARGS_00_() 0
#define ORK_COUNT_ARGS_01_(A1) 1
#define ORK_COUNT_ARGS_02_(A1, A2) 2
#define ORK_COUNT_ARGS_03_(A1, A2, A3) 3
#define ORK_COUNT_ARGS_04_(A1, A2, A3, A4) 4
#define ORK_COUNT_ARGS_05_(A1, A2, A3, A4, A5) 5
#define ORK_COUNT_ARGS_06_(A1, A2, A3, A4, A5, A6) 6
#define ORK_COUNT_ARGS_07_(A1, A2, A3, A4, A5, A6, A7) 7
#define ORK_COUNT_ARGS_08_(A1, A2, A3, A4, A5, A6, A7, A8) 8
#define ORK_COUNT_ARGS_09_(A1, A2, A3, A4, A5, A6, A7, A8, A9) 9

#define ORK_COUNT_ARGS(...) ORK_EVAL( \
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_COUNT_ARGS_09_, ORK_COUNT_ARGS_08_, ORK_COUNT_ARGS_07_, ORK_COUNT_ARGS_06_, ORK_COUNT_ARGS_05_, \
		ORK_COUNT_ARGS_04_, ORK_COUNT_ARGS_03_, ORK_COUNT_ARGS_02_, ORK_COUNT_ARGS_01_, ORK_COUNT_ARGS_00_) \
	(__VA_ARGS__))


#define ORK_MAKE_STRING_(PRE, STR, ARG) ORK_EVAL(const ork::ORK_CAT(PRE, string) ORK_CAT(PRE, ARG){STR(ORK_STR(ARG))};)
#define ORK_STRING_LIST_00_(PRE, STR)
#define ORK_STRING_LIST_01_(PRE, STR, ARG) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG))
#define ORK_STRING_LIST_02_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_01_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_03_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_02_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_04_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_03_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_05_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_04_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_06_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_05_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_07_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_06_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_08_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_07_(PRE, STR, __VA_ARGS__))
#define ORK_STRING_LIST_09_(PRE, STR, ARG, ...) ORK_EVAL(ORK_MAKE_STRING_(PRE, STR, ARG) ORK_STRING_LIST_08_(PRE, STR, __VA_ARGS__))

#define ORK_STRING_LIST(PRE, STR, ...) ORK_EVAL(\
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_STRING_LIST_09_, ORK_STRING_LIST_08_, ORK_STRING_LIST_07_, ORK_STRING_LIST_06_, ORK_STRING_LIST_05_,	\
		ORK_STRING_LIST_04_, ORK_STRING_LIST_03_, ORK_STRING_LIST_02_, ORK_STRING_LIST_01_, ORK_STRING_LIST_00_) \
	(PRE, STR, __VA_ARGS__))


#define ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_EVAL(case ENUM::ARG: return ORK_CAT(PRE, ARG);)
#define ORK_CASE_LIST_00_(ENUM, PRE)
#define ORK_CASE_LIST_01_(ENUM, PRE, ARG) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG))
#define ORK_CASE_LIST_02_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_01_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_03_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_02_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_04_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_03_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_05_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_04_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_06_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_05_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_07_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_06_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_08_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_07_(ENUM, PRE, __VA_ARGS__))
#define ORK_CASE_LIST_09_(ENUM, PRE, ARG, ...) ORK_EVAL(ORK_MAKE_CASE_(ENUM, PRE, ARG) ORK_CASE_LIST_08_(ENUM, PRE, __VA_ARGS__))

#define ORK_CASE_LIST(ENUM, PRE, ...) ORK_EVAL(\
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_CASE_LIST_09_, ORK_CASE_LIST_08_, ORK_CASE_LIST_07_, ORK_CASE_LIST_06_, ORK_CASE_LIST_05_,	\
		ORK_CASE_LIST_04_, ORK_CASE_LIST_03_, ORK_CASE_LIST_02_, ORK_CASE_LIST_01_, ORK_CASE_LIST_00_) \
	(ENUM, PRE, __VA_ARGS__))


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
	ORK_GET_ARG_09_( \
		__VA_ARGS__, ORK_STR_2_ENUM_LIST_09_, ORK_STR_2_ENUM_LIST_08_, ORK_STR_2_ENUM_LIST_07_, ORK_STR_2_ENUM_LIST_06_, ORK_STR_2_ENUM_LIST_05_,	\
		ORK_STR_2_ENUM_LIST_04_, ORK_STR_2_ENUM_LIST_03_, ORK_STR_2_ENUM_LIST_02_, ORK_STR_2_ENUM_LIST_01_, ORK_STR_2_ENUM_LIST_00_) \
	(ENUM, PRE, __VA_ARGS__))


#define ORK_ENUM_SET_(ENUM, ...) const std::array<ENUM, ORK_COUNT_ARGS(__VA_ARGS__)>

#define ORK_ENUM_DECL(ENUM, ...)\
enum class ENUM {\
	ORK_COMMA_LIST(ORK_EMPTY, __VA_ARGS__)\
};\
extern ORK_ENUM_SET_(ENUM, __VA_ARGS__)& ORK_CAT(ENUM, _set)();\
const ork::string& to_string(const ENUM);\
const ork::bstring& to_bstring(const ENUM);\
const ork::wstring& to_wstring(const ENUM);\
ENUM ORK_CAT(to_, ENUM)(const ork::bstring&);\
ENUM ORK_CAT(to_, ENUM)(const ork::wstring&);


#define ORK_ENUM_2_STR_(ENUM, PRE, ...) ORK_EVAL( \
const ork::ORK_CAT(PRE, string)& ORK_CAT(to_, PRE, string)(const ENUM val) {\
	switch(val) {\
		ORK_CASE_LIST(ENUM, PRE, __VA_ARGS__)\
	};\
	ORK_UNREACHABLE\
})


#define ORK_ENUM_DEF(ENUM, ...) \
ORK_ENUM_SET_(ENUM, __VA_ARGS__)& ORK_CAT(ENUM, _set)() {\
	static ORK_ENUM_SET_(ENUM, __VA_ARGS__) val = {{\
		ORK_COMMA_LIST(ENUM::, __VA_ARGS__)\
	}};\
	return val;\
};\
ORK_STRING_LIST(b, BORK, __VA_ARGS__) \
ORK_STRING_LIST(w, WORK, __VA_ARGS__) \
ORK_ENUM_2_STR_(ENUM, b, __VA_ARGS__) \
ORK_ENUM_2_STR_(ENUM, w, __VA_ARGS__) \
const ork::string& to_string(const grid_visibility_mode val) { \
	return ORK_CAT(to_, ORK_STRING)(val); \
}
// TODO: The functions


}//namespace ork

#endif//ORK_TOOLS_HPP
