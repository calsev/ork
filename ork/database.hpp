/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"ork/ork.hpp"


namespace ork {


bstring uuid2string(const char*const value);
std::vector<char> string2uuid(const bstring&val);

#define SQ_ALWAYS_MSG(CODE, MSG) STMT(\
	const int sqlite_status(CODE);\
	if(sqlite_status != SQLITE_OK){\
		THROW_MSG(\
			MSG << TXT(":") << sqlite_status\
		);\
	}\
)

}//namespace ork

