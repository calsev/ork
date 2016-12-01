/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#include"ork/detail/config.hpp"
#include"ork/detail/text.hpp"
#include"ork/detail/exception.hpp"


namespace ork {


#ifndef ORK_WARN_
#	define ORK_WARN_(MSG) ORK_CAT3(ORK_FLOC, ORK(": Warning: "), ORK(MSG))
#	if ORK_MSC
#		define ORK_VSWARNING(MSG) message(ORK_WARN_(MSG))
#	else
#		define ORK_VSWARNING(MSG) 
#	endif
#endif


//Make the code execute as one statement; used because it could be more exotic(per-platform) in the future.
#define ORK_STMT(CODE) {CODE} //do{CODE}while(false);


#ifndef ORK_DEPRECATED
#	if ORK_GCC
#		define ORK_DEPRECATED __attribute__((deprecated))
#	else
#		define ORK_DEPRECATED
#	endif
#endif


#ifndef ORK_INLINE
#	if ORK_MSC
#		define ORK_INLINE __forceinline
#	elif ORK_GCC
#		define ORK_INLINE __attribute__((always_inline)) inline
#	else
#		error Compiler not supported
#	endif
#endif


#if ORK_GCC || ORK_MSC > 1800
#define ORK_NO_EXCEPT noexcept
#define ORK_CONSTEXPR constexpr
#else
#define ORK_NO_EXCEPT//throw()
#define ORK_CONSTEXPR 
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

#if ORK_GCC || ORK_MSC > 1700//VS 2013 or later
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
#if ORK_GCC || ORK_MSC > 1700//VS 2013 or later
#define ORK_MOVE_ONLY(TYPE) ORK_NO_COPY_(TYPE) ORK_MOVE_(TYPE)
#define ORK_MOVEABLE(TYPE) ORK_COPY_(TYPE) ORK_MOVE_(TYPE)
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

}//namespace ork
