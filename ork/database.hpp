/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include<vector>
#include"ork/ork.hpp"


namespace ork {


ORK_ORK_EXT(bstring) uuid2string(const char*const value);
ORK_ORK_EXT(std::vector<char>) string2uuid(const bstring&val);

#define ORK_SQ_ALWAYS(CODE, MSG) ORK_STMT(\
	const int sqlite_status(CODE);\
	if(sqlite_status != SQLITE_OK){\
		ORK_THROW(\
			MSG << ORK(":") << sqlite_status\
		);\
	}\
)

}//namespace ork

