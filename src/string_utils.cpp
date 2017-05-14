/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<cstring>
#include<cwchar>
#include"ork/string_utils.hpp"


namespace ork {


size_t str_length(const char*str) {
	if(str == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::strlen(str);
}
size_t str_length(const wchar_t*str) {
	if(str == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::wcslen(str);
}


}//namespace ork
