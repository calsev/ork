/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"


namespace ork {


ORK_ORK_EXT(void) to_lower(bstring&);
ORK_ORK_EXT(void) to_lower(wstring&);
ORK_ORK_EXT(bstring) to_lower_copy(const bstring&);
ORK_ORK_EXT(wstring) to_lower_copy(const wstring&);

ORK_ORK_EXT(void) to_upper(bstring&);
ORK_ORK_EXT(void) to_upper(wstring&);
ORK_ORK_EXT(bstring) to_upper_copy(const bstring&);
ORK_ORK_EXT(wstring) to_upper_copy(const wstring&);

ORK_ORK_EXT(void) to_sentence_case(bstring&);
ORK_ORK_EXT(void) to_sentence_case(wstring&);
ORK_ORK_EXT(bstring) to_sentence_case_copy(const bstring&);
ORK_ORK_EXT(wstring) to_sentence_case_copy(const wstring&);


//There is no default implementation
template<typename T> ORK_INLINE T from_string(const bstring&str);
template<typename T> ORK_INLINE T from_string(const wstring&str);

#define ORK_STRING_CONV_ARG(TYPE) TYPE
#define ORK_STRING_CONVERSION(TYPE)\
ORK_ORK_EXT(string) to_string(const ORK_STRING_CONV_ARG(TYPE));\
ORK_ORK_EXT(bstring) to_bstring(const ORK_STRING_CONV_ARG(TYPE));\
ORK_ORK_EXT(wstring) to_wstring(const ORK_STRING_CONV_ARG(TYPE));\
ORK_ORK_EXT(TYPE) ORK_CAT(to_, TYPE)(const bstring&);\
ORK_ORK_EXT(TYPE) ORK_CAT(to_, TYPE)(const wstring&);\
template<> ORK_ORK_EXT(TYPE) from_string<TYPE>(const bstring&str);\
template<> ORK_ORK_EXT(TYPE) from_string<TYPE>(const wstring&str);

//Conversions are being instantiated lazily
ORK_STRING_CONVERSION(bool);
ORK_STRING_CONVERSION(int);
ORK_STRING_CONVERSION(unsigned);
ORK_STRING_CONVERSION(size_t);
ORK_STRING_CONVERSION(float);
ORK_STRING_CONVERSION(double);


#undef ORK_STRING_CONV_ARG
#define ORK_STRING_CONV_ARG(TYPE) TYPE&

ORK_STRING_CONVERSION(bstring);
ORK_STRING_CONVERSION(wstring);


ORK_ORK_EXT(size_t) str_length(const char*const);
ORK_ORK_EXT(size_t) str_length(const wchar_t*const);

ORK_ORK_EXT(bool) str_equal(const char*const, const char*const);
ORK_ORK_EXT(bool) str_equal(const wchar_t*const, const wchar_t*const);


enum class encoding {
	  base_64
	//, hex
};


ORK_ORK_EXT(bstring) encode(const unsigned char*str, const size_t size, const encoding enc);
ORK_ORK_EXT(bstring) decode(const bstring&str, const encoding enc);


}//namespace ork
