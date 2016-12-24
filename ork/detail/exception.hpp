/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif


#include<exception>
#include<deque>
#include<string>
#include<ostream>
#include"ork/detail/config_internal.hpp"
#include"ork/detail/text.hpp"

namespace ork {


typedef std::runtime_error exception;


struct stack_frame {
	bstring function;
	bstring filename;
	unsigned line;
};


struct backtrace {
	std::deque<stack_frame> frames;
	bstring str() const;
};


backtrace generate_backtrace(const unsigned skip_frames = 0);
bo_stream &operator << (bo_stream& stream, const backtrace& trace);


/*
Exceptions do not support unicode!
*/
#define ORK_EXCEP(MSG)\
	throw ork::exception(ORK_STR_2_BYTE(MSG))

//Endlines are needed to flush the buffer before backtrace, for some reason
#define ORK_THROW(MSG) ORK_STMT(\
	ORK_LOG(ork::severity_level::fatal)<< MSG << std::endl << std::endl << ork::generate_backtrace().str() << ORK('\n');\
	ork::g_log().flush_all();\
	ork::string_stream throw_msg;\
	throw_msg << ORK("[FATAL]: ") << ORK_LOC_BLOCK << MSG;/*Generate more or less the header from logging*/\
	throw_msg << std::endl << std::endl << ork::generate_backtrace().str() << ORK('\n');\
	ORK_EXCEP(throw_msg.str());\
)

}//namespace ork
