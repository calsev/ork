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


double to_double(const char*const str) {
	return std::strtod(str, nullptr);
}
double to_double(const wchar_t*const str) {
	return std::wcstod(str, nullptr);
}
double to_double(const bstring&str) {
	return std::strtod(str.c_str(), nullptr);
}
double to_double(const wstring&str) {
	return std::wcstod(str.c_str(), nullptr);
}


//We could also static cast double versions
float to_float(const char*const str) {
	return std::strtof(str, nullptr);
}
float to_float(const wchar_t*const str) {
	return std::wcstof(str, nullptr);
}
float to_float(const bstring&str) {
	return std::strtof(str.c_str(), nullptr);
}
float to_float(const wstring&str) {
	return std::wcstof(str.c_str(), nullptr);
}


}//namespace ork
