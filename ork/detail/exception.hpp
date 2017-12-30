/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifdef ORK_EXCEPTION_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_EXCEPTION_HPP
#define ORK_EXCEPTION_HPP

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif


#include<deque>


#if ORK_MSC
#pragma warning(push)
#pragma warning(disable : 4251) //DLL interface for X needs DLL interface for Y (std::runtime_error)
#endif

namespace ork {

typedef std::runtime_error exception;

struct ORK_ORK_API stack_frame {
	bstring function;
	bstring filename;
	unsigned line;
};


struct ORK_ORK_API backtrace {
	std::deque<stack_frame> frames;
	bstring str() const;
};


ORK_ORK_EXT(backtrace) generate_backtrace(const unsigned skip_frames = 0);
ORK_ORK_EXT(bo_stream&) operator << (bo_stream& stream, const backtrace& trace);


/*
Exceptions do not support unicode!
*/
#define ORK_EXCEP(MSG)\
	throw ork::exception(ORK_STR_2_BYTE(MSG))

//Endlines are needed to flush the buffer before backtrace, for some reason
#define ORK_THROW(MSG) ORK_STMT(\
	ORK_LOG(ork::severity_level::fatal)<< MSG << std::endl << std::endl << ork::generate_backtrace().str() << ORK('\n');\
	ork::get_global_log().flush_all();\
	ork::string_stream throw_msg;\
	throw_msg << ORK("[FATAL]: ") << ORK_LOC_BLOCK << MSG;/*Generate more or less the header from logging*/\
	throw_msg << std::endl << std::endl << ork::generate_backtrace().str() << ORK('\n');\
	ORK_EXCEP(throw_msg.str());\
)

}//namespace ork

#if ORK_MSC
#pragma warning(pop)
#endif

#endif ORK_EXCEPTION_HPP
