/*
This file is part of the ORK_STR library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#error This header can only be included from ork/common.hpp!
#endif

#include"ork/detail/text.hpp"


namespace ork {


#define WARN_(MSG) ORK_CAT3(ORK_FLOC, ORK(": Warning: "), ORK(MSG))
#if defined _MSC_VER
#define VSWARNING(MSG) message(WARN_(MSG))
#else
#define VSWARNING(MSG) 
#endif


//Make the code execute as one statement; used because it could be more exotic(per-platform) in the future.
#define STMT(CODE) {CODE} //do{CODE}while(false);

#if defined _MSC_VER
#define INLINE __forceinline
#elif defined __GNUC__
#define INLINE __attribute__((always_inline)) inline
#else
#error "Compiler not supported"
#endif


#if defined __GNUC__ || _MSC_VER > 1800
#define NO_EXCEPT noexcept
#else
#define NO_EXCEPT//throw()
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


#define _EMPTY /*Empty*/


/*
Copy and move semantics
*/

#define _COPY(TYPE)\
	TYPE(const TYPE&)=default;\
	TYPE&operator=(const TYPE&)=default;

#if defined __GNUC__ || _MSC_VER > 1700//VS 2013 or later
	#define _NO_COPY(TYPE)\
			TYPE(const TYPE&)=delete;\
			TYPE&operator=(const TYPE&)=delete;
	#define _NO_MOVE(TYPE)\
			TYPE(TYPE&&)=delete;\
			TYPE&operator=(TYPE&&)=delete;
	#define _MOVE(TYPE)\
		TYPE(TYPE&&)=default;\
		TYPE&operator=(TYPE&&)=default;
#else
	#define _NO_COPY(TYPE)\
			private:\
				TYPE(const TYPE&);\
				TYPE&operator=(const TYPE&);
	#define _NO_MOVE(TYPE) _EMPTY
	#define _MOVE(TYPE) _EMPTY
#endif

#define NON_COPYABLE(TYPE) _NO_COPY(TYPE) _NO_MOVE(TYPE)
#if defined __GNUC__ || _MSC_VER > 1700//VS 2013 or later
#define MOVE_ONLY(TYPE) _NO_COPY(TYPE) _MOVE(TYPE)
#define MOVEABLE(TYPE) _COPY(TYPE) _MOVE(TYPE)
#endif


/*
The macros below provide more control over construction, including insertion of
code in both the constructor and assignment operator.
These macros also support VS 2012.
*/
//Construction
#define _CMOVE(M) M(std::move(rhs.M))
#define _CMOVE_1(M1)										_CMOVE(M1)
#define _CMOVE_2(M1,M2)				_CMOVE_1(M1),			_CMOVE(M2)
#define _CMOVE_3(M1,M2,M3)			_CMOVE_2(M1,M2),		_CMOVE(M3)
#define _CMOVE_4(M1,M2,M3,M4)		_CMOVE_3(M1,M2,M3),		_CMOVE(M4)
#define _CMOVE_5(M1,M2,M3,M4,M5)	_CMOVE_4(M1,M2,M3,M4),	_CMOVE(M5)

//Assignment
#define _AMOVE(M) M=std::move(rhs.M);
#define _AMOVE_1(M1)										_AMOVE(M1)
#define _AMOVE_2(M1,M2)				_AMOVE_1(M1)			_AMOVE(M2)
#define _AMOVE_3(M1,M2,M3)			_AMOVE_2(M1,M2)			_AMOVE(M3)
#define _AMOVE_4(M1,M2,M3,M4)		_AMOVE_3(M1,M2,M3)		_AMOVE(M4)
#define _AMOVE_5(M1,M2,M3,M4,M5)	_AMOVE_4(M1,M2,M3,M4)	_AMOVE(M5)


/*
This constructor somewhat surreptitiously inserts noexcept
but a throwing move constructor is pointless anyway
*/
#define _MOVE_CONSTRUCT(TYPE,CODE)\
public:\
	TYPE(TYPE&&rhs)NO_EXCEPT:CODE{
/*Constructor body here*/
#define _MOVE_ASSIGN(TYPE,CODE)\
	}\
public:\
	TYPE&operator=(TYPE&&rhs)NO_EXCEPT{\
		if(this!=&rhs){\
			CODE
/*Assignment body here*/
#define _MOVE_END\
		}\
		return *this;\
	}


/*
These macros wrap up functionality.
The first inserts the same code in the constuctor and assigment operator.
The second inserts no code.
*/
#define _MOVE_CON1(TYPE,M1)				_MOVE_CONSTRUCT(TYPE,_CMOVE_1(M1))
#define _MOVE_CON2(TYPE,M1,M2)			_MOVE_CONSTRUCT(TYPE,_CMOVE_2(M1,M2))
#define _MOVE_CON3(TYPE,M1,M2,M3)		_MOVE_CONSTRUCT(TYPE,_CMOVE_3(M1,M2,M3))
#define _MOVE_CON4(TYPE,M1,M2,M3,M4)	_MOVE_CONSTRUCT(TYPE,_CMOVE_4(M1,M2,M3,M4))
#define _MOVE_CON5(TYPE,M1,M2,M3,M4,M5)	_MOVE_CONSTRUCT(TYPE,_CMOVE_5(M1,M2,M3,M4,M5))

#define _MOVE_OP1(TYPE,M1)				_MOVE_ASSIGN(TYPE,_AMOVE_1(M1))
#define _MOVE_OP2(TYPE,M1,M2)			_MOVE_ASSIGN(TYPE,_AMOVE_2(M1,M2))
#define _MOVE_OP3(TYPE,M1,M2,M3)		_MOVE_ASSIGN(TYPE,_AMOVE_3(M1,M2,M3))
#define _MOVE_OP4(TYPE,M1,M2,M3,M4)		_MOVE_ASSIGN(TYPE,_AMOVE_4(M1,M2,M3,M4))
#define _MOVE_OP5(TYPE,M1,M2,M3,M4,M5)	_MOVE_ASSIGN(TYPE,_AMOVE_5(M1,M2,M3,M4,M5))

#define _MOVE_1(TYPE,M1,CODE)				_MOVE_CON1(TYPE,M1)				CODE _MOVE_OP1(TYPE,M1)				CODE _MOVE_END
#define _MOVE_2(TYPE,M1,M2,CODE)			_MOVE_CON2(TYPE,M1,M2)			CODE _MOVE_OP2(TYPE,M1,M2)			CODE _MOVE_END
#define _MOVE_3(TYPE,M1,M2,M3,CODE)			_MOVE_CON3(TYPE,M1,M2,M3)		CODE _MOVE_OP3(TYPE,M1,M2,M3)		CODE _MOVE_END
#define _MOVE_4(TYPE,M1,M2,M3,M4,CODE)		_MOVE_CON4(TYPE,M1,M2,M3,M4)	CODE _MOVE_OP4(TYPE,M1,M2,M3,M4)	CODE _MOVE_END
#define _MOVE_5(TYPE,M1,M2,M3,M4,M5,CODE)	_MOVE_CON5(TYPE,M1,M2,M3,M4,M5)	CODE _MOVE_OP5(TYPE,M1,M2,M3,M4,M5)	CODE _MOVE_END


#define MOVEABLE_1(TYPE,M1)				_COPY(TYPE) _MOVE_1(TYPE,M1,_EMPTY)
#define MOVEABLE_2(TYPE,M1,M2)			_COPY(TYPE) _MOVE_2(TYPE,M1,M2,_EMPTY)
#define MOVEABLE_3(TYPE,M1,M2,M3)		_COPY(TYPE) _MOVE_3(TYPE,M1,M2,M3,_EMPTY)
#define MOVEABLE_4(TYPE,M1,M2,M3,M4)	_COPY(TYPE) _MOVE_4(TYPE,M1,M2,M3,M4,_EMPTY)
#define MOVEABLE_5(TYPE,M1,M2,M3,M4,M5)	_COPY(TYPE) _MOVE_5(TYPE,M1,M2,M3,M4,M5,_EMPTY)

#define MOVE_ONLY_1(TYPE,M1)				_NO_COPY(TYPE) _MOVE_1(TYPE,M1,_EMPTY)
#define MOVE_ONLY_2(TYPE,M1,M2)				_NO_COPY(TYPE) _MOVE_2(TYPE,M1,M2,_EMPTY)
#define MOVE_ONLY_3(TYPE,M1,M2,M3)			_NO_COPY(TYPE) _MOVE_3(TYPE,M1,M2,M3,_EMPTY)
#define MOVE_ONLY_4(TYPE,M1,M2,M3,M4)		_NO_COPY(TYPE) _MOVE_4(TYPE,M1,M2,M3,M4,_EMPTY)
#define MOVE_ONLY_5(TYPE,M1,M2,M3,M4,M5)	_NO_COPY(TYPE) _MOVE_5(TYPE,M1,M2,M3,M4,M5,_EMPTY)

}//namespace ork
