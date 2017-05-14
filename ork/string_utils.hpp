/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"


namespace ork {


ORK_ORK_EXT(size_t) str_length(const char*const);
ORK_ORK_EXT(size_t) str_length(const wchar_t*const);


ORK_ORK_EXT(bool) str_equal(const char*const, const char*const);
ORK_ORK_EXT(bool) str_equal(const wchar_t*const, const wchar_t*const);


}//namespace ork
