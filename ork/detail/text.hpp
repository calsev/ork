/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#error This header can only be included from ork/common.hpp!
#endif


#include<string>
#include<sstream>
#include<iostream>
#ifndef _MSC_VER
#include"boost/current_function.hpp"
#endif


#if defined UNICODE || defined _UNICODE
#define UNICODE 1//Avoid checking both everywhere
#endif

#if UNICODE || 1
#include<codecvt>
#include<mutex>
#endif

#if 1
#include"boost/filesystem/fstream.hpp"
#else
//Forward declaration to keep boost out of headers
namespace boost {
namespace filesystem {
class wofstream;
class wifstream;
class wfstream;
class ofstream;
class ifstream;
class fstream;
}
}
#endif


namespace ork {


#define STR_(X) #X
#define STR(X) STR_(X)


#define CAT_(X,Y) X##Y
#define CAT(X,Y) CAT_(X,Y)
#define CAT3(X,Y,Z) CAT(X,CAT(Y,Z))
#define CAT4(W,X,Y,Z) CAT(W,CAT3(X,Y,Z))
#define CAT5(V,W,X,Y,Z) CAT(V,CAT4(W,X,Y,Z))


#define WIDEN_(X) L##X
#define WIDEN(X) WIDEN_(X)


#if UNICODE
typedef std::wstring string;
typedef std::wstringstream string_stream;
typedef wchar_t letr;
typedef std::wostream o_stream;
typedef std::wistream i_stream;
typedef std::wostringstream o_string_stream;
typedef std::wistringstream i_string_stream;
#define TXT(X) WIDEN(X)
#define CIN std::wcin
#define COUT std::wcout
#define CERR std::wcerr
#define CLOG std::wclog
#else
typedef std::string string;
typedef std::stringstream string_stream;
typedef char letr;
typedef std::ostream o_stream;
typedef std::istream i_stream;
typedef std::ostringstream o_string_stream;
typedef std::istringstream i_string_stream;
#define TXT(X) X
#define CIN std::cin
#define COUT std::cout
#define CERR std::cerr
#define CLOG std::clog
#endif
typedef std::string bstring;//Byte string, so that searches can identify unhandled unicode
typedef std::stringstream b_string_stream;
typedef std::ostream bo_stream;
typedef std::istream bi_stream;
typedef std::ostringstream bo_string_stream;
typedef std::istringstream bi_string_stream;
#define BTXT(X) X//Byte Text, so that searches can identify unhandled unicode


#define FILEN TXT(__FILE__)
#define LINE TXT(STR(__LINE__))
#if 1
	#ifdef _MSC_VER
		#ifdef __FUNCTION__
			#define FUNC __FUNCTION__
		#else
			#define FUNC TXT("(unknown_function)")
		#endif
	#else
		#ifdef BOOST_CURRENT_FUNCTION
			#define FUNC BOOST_CURRENT_FUNCTION
		#else
			#define FUNC TXT("(unknown_function)")
		#endif
	#endif
#else
	#if defined _MSC_VER
		#define FUNC __FUNCSIG__
	#elif defined __GNUC__
		#define FUNC __PRETTY_FUNCTION__
	#else
		#error "Compiler not supported"
	#endif
#endif
#define FLOC CAT4(FILEN, TXT("("), LINE, TXT(")"))


#if UNICODE
typedef boost::filesystem::wofstream of_stream;
typedef boost::filesystem::wifstream if_stream;
typedef boost::filesystem::wfstream f_stream;
#define GEN_STR generic_wstring
#define STRING wstring
#else
typedef boost::filesystem::ofstream of_stream;
typedef boost::filesystem::ifstream if_stream;
typedef boost::filesystem::fstream f_stream;
#define GEN_STR generic_string
#define STRING string
#endif


#if UNICODE || 1
class string_converter_type {//Just a thread-safe wrapper for std::wstring_convert
protected:
	typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>string_converter;
	typedef std::mutex mutex_type;
	typedef std::lock_guard<mutex_type>lock_type;
protected:
	string_converter m_converter;
	std::mutex m_mutex;
public:
	std::string to_bytes(const wchar_t s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	std::string to_bytes(const wchar_t*s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	std::string to_bytes(const std::wstring&s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	std::string to_bytes(const wchar_t*first, const wchar_t*last) {
		lock_type lock(m_mutex); return m_converter.to_bytes(first, last);
	}

	std::wstring from_bytes(const char s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	std::wstring from_bytes(const char*s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	std::wstring from_bytes(const std::string&s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	std::wstring from_bytes(const char*first, const char*last) {
		lock_type lock(m_mutex); return m_converter.from_bytes(first, last);
	}
};
string_converter_type&g_string_converter();
#endif

#if UNICODE
#define STR_2_BYTE(STRING) ork::g_string_converter().to_bytes(STRING)
#define BYTE_2_STR(STRING) ork::g_string_converter().from_bytes(STRING)
#else
#define STR_2_BYTE(STRING) STRING
#define BYTE_2_STR(STRING) STRING
#endif

}//namespace ork
