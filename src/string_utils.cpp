/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include<cstring>
#include<cwchar>
#include"ork/string_utils.hpp"


namespace ork {


size_t str_length(const char*const str) {
	if(str == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::strlen(str);
}
size_t str_length(const wchar_t*const str) {
	if(str == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::wcslen(str);
}


bool str_equal(const char*const lhs, const char*const rhs) {
	if(lhs == nullptr || rhs == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::strcmp(lhs, rhs) == 0;
}
bool str_equal(const wchar_t*const lhs, const wchar_t*const rhs) {
	if(lhs == nullptr || rhs == nullptr) {
		ORK_THROW(ORK("String length called for null string"));
	}
	return std::wcscmp(lhs, rhs) == 0;
}


}//namespace ork
