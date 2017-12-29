/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"


namespace ork {


ORK_ORK_EXT(string) to_string(const bool val);
ORK_ORK_EXT(bool) string2bool(const string&val);


ORK_ORK_EXT(size_t) str_length(const char*const);
ORK_ORK_EXT(size_t) str_length(const wchar_t*const);


ORK_ORK_EXT(bool) str_equal(const char*const, const char*const);
ORK_ORK_EXT(bool) str_equal(const wchar_t*const, const wchar_t*const);


ORK_ORK_EXT(double) to_double(const char*const);
ORK_ORK_EXT(double) to_double(const wchar_t*const);
ORK_ORK_EXT(double) to_double(const bstring&);
ORK_ORK_EXT(double) to_double(const wstring&);

ORK_ORK_EXT(float) to_float(const char*const);
ORK_ORK_EXT(float) to_float(const wchar_t*const);
ORK_ORK_EXT(float) to_float(const bstring&);
ORK_ORK_EXT(float) to_float(const wstring&);


enum class encoding {
	  base_64
	//, hex
};


ORK_ORK_EXT(bstring) encode(const unsigned char*str, const size_t size, const encoding enc);
ORK_ORK_EXT(bstring) decode(const bstring&str, const encoding enc);


}//namespace ork
