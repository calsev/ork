/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#include"ork/ork.hpp"
#define ORK_STL_INC_FILE <algorithm>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <cstring>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <cwchar>
#include"ork/core/stl_include.inl"
#define ORK_STL_INC_FILE <sstream>
#include"ork/core/stl_include.inl"

#include"ork/string_utils.hpp"


namespace ork {


ORK_INLINE char lower(const char c) {
	return static_cast<char>(::tolower(c));
}
ORK_INLINE wchar_t wlower(const wchar_t c) {
	return static_cast<wchar_t>(::towlower(c));
}


ORK_INLINE char upper(const char c) {
	return static_cast<char>(::toupper(c));
}
ORK_INLINE wchar_t wupper(const wchar_t c) {
	return static_cast<wchar_t>(::towupper(c));
}


void to_lower(bstring&str) {
	std::transform(str.begin(), str.end(), str.begin(), lower);
}
void to_lower(wstring&str) {
	std::transform(str.begin(), str.end(), str.begin(), wlower);
}
bstring to_lower_copy(const bstring&str) {
	bstring retval(str);
	to_lower(retval);
	return retval;
}
wstring to_lower_copy(const wstring&str) {
	wstring retval(str);
	to_lower(retval);
	return retval;
}


void to_upper(bstring&str) {
	std::transform(str.begin(), str.end(), str.begin(), upper);
}
void to_upper(wstring&str) {
	std::transform(str.begin(), str.end(), str.begin(), wupper);
}
bstring to_upper_copy(const bstring&str) {
	bstring retval(str);
	to_upper(retval);
	return std::move(retval);
}
wstring to_upper_copy(const wstring&str) {
	wstring retval(str);
	to_upper(retval);
	return std::move(retval);
}


void to_sentence_case(bstring&str) {
	to_lower(str);
	if(!str.empty()) {
		str[0] = upper(str[0]);
	}
}
void to_sentence_case(wstring&str) {
	to_lower(str);
	if(!str.empty()) {
		str[0] = wupper(str[0]);
	}
}
bstring to_sentence_case_copy(const bstring&str) {
	bstring retval(str);
	to_sentence_case(retval);
	return std::move(retval);
}
wstring to_sentence_case_copy(const wstring&str) {
	wstring retval(str);
	to_sentence_case(retval);
	return std::move(retval);
}


string to_string(const bool val) {
	return val ? ORK("true") : ORK("false");
}
bstring to_bstring(const bool val) {
	return val ? BORK("true") : BORK("false");
}
wstring to_wstring(const bool val) {
	return val ? WORK("true") : WORK("false");
}

#define ORK_TO_BOOL(STRING) \
bool to_bool(const STRING&val_) {\
	const STRING val{to_lower_copy(val_)};\
	if(val == ORK_BOOL_STR("t")) {\
		return true;\
	}\
	if(val == ORK_BOOL_STR("true")) {\
		return true;\
	}\
	if(val == ORK_BOOL_STR("y")) {\
		return true;\
	}\
	if(val == ORK_BOOL_STR("yes")) {\
		return true;\
	}\
	if(val == ORK_BOOL_STR("1")) {\
		return true;\
	}\
	if(val == ORK_BOOL_STR("f")) {\
		return false;\
	}\
	if(val == ORK_BOOL_STR("false")) {\
		return false;\
	}\
	if(val == ORK_BOOL_STR("n")) {\
		return false;\
	}\
	if(val == ORK_BOOL_STR("no")) {\
		return false;\
	}\
	if(val == ORK_BOOL_STR("0")) {\
		return false;\
	}\
	ORK_THROW(ORK("Invalid bool value"));\
}

#define ORK_BOOL_STR(STR) BORK(STR)
ORK_TO_BOOL(bstring)
#undef ORK_BOOL_STR
#define ORK_BOOL_STR(STR) WORK(STR)
ORK_TO_BOOL(wstring)


#if ORK_UNICODE
#   define ORK_STR_FUNC(BFUNC, WFUNC) WFUNC
#else
#   define ORK_STR_FUNC(BFUNC, WFUNC) BFUNC
#endif


#undef ORK_STRING_CONV_ARG
#define ORK_STRING_CONV_ARG(TYPE) TYPE
#define ORK_STRING_CONVERSION_DEF(TYPE, TO_B, TO_W, SB, SW)\
string to_string(const ORK_STRING_CONV_ARG(TYPE) val) {\
	return ORK_STR_FUNC(TO_B, TO_W)(val);\
}\
bstring to_bstring(const ORK_STRING_CONV_ARG(TYPE) val) {\
	return TO_B(val);\
}\
wstring to_wstring(const ORK_STRING_CONV_ARG(TYPE) val) {\
	return TO_W(val);\
}\
TYPE ORK_CAT(to_, TYPE)(const bstring& val) {\
	return SB(val);\
}\
TYPE ORK_CAT(to_, TYPE)(const wstring& val) {\
	return SW(val);\
}\
template<> TYPE from_string<TYPE>(const bstring&str) {\
	return ORK_CAT(to_, TYPE)(str);\
}\
template<> TYPE from_string<TYPE>(const wstring&str) {\
	return ORK_CAT(to_, TYPE)(str);\
}

//We could provide wcsto versions for pointers
ORK_STRING_CONVERSION_DEF(int, std::to_string, std::to_wstring, std::stoi, std::stoi);
ORK_STRING_CONVERSION_DEF(unsigned, std::to_string, std::to_wstring, std::stoul, std::stoul);
ORK_STRING_CONVERSION_DEF(size_t, std::to_string, std::to_wstring, std::stoull, std::stoull);
ORK_STRING_CONVERSION_DEF(float, std::to_string, std::to_wstring, std::stof, std::stof);
ORK_STRING_CONVERSION_DEF(double, std::to_string, std::to_wstring, std::stod, std::stod);

string to_string(const bstring& val) {
	return ORK_BYTE_2_STR(val);
}
bstring to_bstring(const bstring& val) {
	return val;
}
wstring to_wstring(const bstring& val) {
	return ork::g_string_converter().byte2wide(val);
}
template<> bstring from_string<bstring>(const bstring&str) {
	return str;
}
template<> bstring from_string<bstring>(const wstring&str) {
	return g_string_converter().wide2byte(str);
}

string to_string(const wstring& val) {
	return ORK_WIDE_2_STR(val);
}
bstring to_bstring(const wstring& val) {
	return ork::g_string_converter().wide2byte(val);
}
wstring to_wstring(const wstring& val) {
	return val;
}
template<> wstring from_string<wstring>(const bstring&str) {
	return ork::g_string_converter().byte2wide(str);
}
template<> wstring from_string<wstring>(const wstring&str) {
	return str;
}


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


/*
Base 64 codes are an arbitrary list of printable, non-whitespace, ASCII characters.
However, this particular encoding is MIME-compatible.
*/
const bstring codes_64 = BORK(//26 + 26 + 12 = 64
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/"
);

/*
Because 64 chars can represent only 6 bits, bytes in input must be expanded.
MIME expands 3 bytes to 4 characters (3*8 = 24 = 4*6).
Output is padded with the character below to 4 byte alignment, again MIME-compatible.
Due to byte expansion, 1 '=' indicates 2 bytes and 2 '=' indicates 1 byte.
*/
const char fill_char = BORK('=');

ORK_INLINE bool is_base64(const char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

/*
Here is the expansion.
Just select the bits from first to last
	bytes ordered high to low
	high bit @ 7, low bit @ 0 - adjacent to the next lower byte
So this makes sense in memory also, if we are running on a big-endian (the good endian) machine
*/
std::array<unsigned char, 4>bytes2chars(const std::array<unsigned char, 3>&buf3) {
	//Rather than complicate the expressions with casts, we do int arithmetic, then cast at the end
	std::array<int, 4>vals = {
		  (buf3[0] & 0xfc) >> 2//[7...2], shifted to [5...0]
		, ((buf3[0] & 0x03) << 4) + ((buf3[1] & 0xf0) >> 4)//prev [1...0] shifted to [5...4], and [7...4] shifted to [3...0]
		, ((buf3[1] & 0x0f) << 2) + ((buf3[2] & 0xc0) >> 6)//prev [3...0] shifted to [5...2], and [7...6] shifted to [1...0]
		, buf3[2] & 0x3f//prev [5...0] stay put
	};
	return {
		  static_cast<unsigned char>(vals[0])
		, static_cast<unsigned char>(vals[1])
		, static_cast<unsigned char>(vals[2])
		, static_cast<unsigned char>(vals[3])
	};
}
std::array<unsigned char, 3>chars2bytes(const std::array<unsigned char, 4>&buf4) {
	std::array<int, 3>vals = {
		  (buf4[0] << 2) + ((buf4[1] & 0x30) >> 4)//prev [5...0] shifted to [7...2], and [5...4] shifted to [1..0]
		, ((buf4[1] & 0xf) << 4) + ((buf4[2] & 0x3c) >> 2)//prev [3...0] shifted to [7...4], and [5...2] shifted to [3...0]
		, ((buf4[2] & 0x3) << 6) + buf4[3]//prev [1...0] shifted to [7...6], and [5...0] stay put
	};
	return {
		  static_cast<unsigned char>(vals[0])
		, static_cast<unsigned char>(vals[1])
		, static_cast<unsigned char>(vals[2])
	};
}


/*
Hex is a simple byte-to-charactr mapping with no expansion or padding needed
*/
const bstring codes_hex = BORK(
	"0123456789ABCDEF"
);

ORK_INLINE bool is_hex(const unsigned char c) {
	return isxdigit(c) != 0;
}


void encode_bytes(const size_t byte_index, std::array<unsigned char, 3>&buf3bytes, std::string&retval) {
	for(size_t i = byte_index; i < 3; ++i) {
		buf3bytes[i] = 0;//Pad the buffer with zeros
	}
	const std::array<unsigned char, 4>buf4chars = bytes2chars(buf3bytes);
	LOOPI(byte_index + 1) {//There is always one more byte than the index
		retval += codes_64[buf4chars[i]];//Encode the chars
	}
	LOOPI(3 - byte_index) {//Add (3 - byte_index) padding bytes
		retval += fill_char;
	}
}


bstring encode(const unsigned char*str, const size_t size, const encoding enc) {
	if(enc != encoding::base_64) {
		ORK_THROW(ORK("A new encoding!"));
	}
	bstring retval;
	size_t byte_index = 0;//Rolls over every 3
	std::array<unsigned char, 3>buf3bytes = {0, 0, 0};//Holds the current bytes (3 per 4-character set)

	LOOPI(size) {
		buf3bytes[byte_index++] = *(str++);
		if(byte_index == 3) {//Post decement, so we have a full set of 3 source bytes
			encode_bytes(byte_index, buf3bytes, retval);
			byte_index = 0;//Next 3 bytes please
		}
	}
	if(byte_index != 0) {//Decode the last (size mod 3) bytes
		encode_bytes(byte_index, buf3bytes, retval);
	}

	return retval;
}


void decode_bytes(const size_t byte_index, std::array<unsigned char, 4>&buf4chars, std::string&retval) {
	for(size_t i = byte_index; i < 4; ++i) {
		buf4chars[i] = 0;//Pad the buffer with zeros
	}
	LOOPI(4) {//Decode the chars
		buf4chars[i] = static_cast<unsigned char>(codes_64.find(static_cast<char>(buf4chars[i])));
	}
	const std::array<unsigned char, 3>buf3bytes = chars2bytes(buf4chars);
	LOOPI(byte_index - 1) {//Copy non-padding bytes
		retval += static_cast<char>(buf3bytes[i]);
	}
}


bstring decode(const bstring& str, const encoding enc) {
	if(enc != encoding::base_64) {
		ORK_THROW(ORK("Wow, a new encoding!"));
	}
	bstring retval;
	size_t byte_index = 0;//Rolls over every 4
	std::array<unsigned char, 4>buf4chars = {0, 0, 0, 0};//Holds the current chars

	for(size_t i = 0; i < str.size() && is_base64(str[i]); ++i) {
		buf4chars[byte_index++] = static_cast<unsigned char>(str[i]);
		if(byte_index == 4) {//Post decement, so we have a full set of 4 source chars
			decode_bytes(byte_index, buf4chars, retval);
			byte_index = 0;//Next 4 chars please
		}
	}
	if(byte_index != 0) {//Decode the last (size mod 4) chars
		decode_bytes(byte_index, buf4chars, retval);
	}

	return retval;
}


}//namespace ork
