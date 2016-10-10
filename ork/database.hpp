/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#include"ork/ork.hpp"


namespace ork {


bstring uuid2string(const char*const value);
std::vector<char> string2uuid(const bstring&val);

#define ORK_SQ_ALWAYS(CODE, MSG) ORK_STMT(\
	const int sqlite_status(CODE);\
	if(sqlite_status != SQLITE_OK){\
		ORK_THROW(\
			MSG << ORK(":") << sqlite_status\
		);\
	}\
)

}//namespace ork

