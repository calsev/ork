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


#define ORK_STR_(X) #X
#define ORK_STR(X) ORK_STR_(X)


#define ORK_CAT_(X,Y) X##Y
#define ORK_CAT(X,Y) ORK_CAT_(X,Y)
#define ORK_CAT3(X,Y,Z) ORK_CAT(X,ORK_CAT(Y,Z))
#define ORK_CAT4(W,X,Y,Z) ORK_CAT(W,ORK_CAT3(X,Y,Z))
#define ORK_CAT5(V,W,X,Y,Z) ORK_CAT(V,ORK_CAT4(W,X,Y,Z))


#define ORK_WIDEN_(X) L##X
#define ORK_WIDEN(X) ORK_WIDEN_(X)


#if UNICODE
typedef std::wstring string;
typedef std::wstringstream string_stream;
typedef wchar_t letr;
typedef std::wostream o_stream;
typedef std::wistream i_stream;
typedef std::wostringstream o_string_stream;
typedef std::wistringstream i_string_stream;
#define ORK(X) ORK_WIDEN(X)//The shortest prefixed macro
#define ORK_CIN std::wcin
#define ORK_COUT std::wcout
#define ORK_CERR std::wcerr
#define ORK_CLOG std::wclog
#else
typedef std::string string;
typedef std::stringstream string_stream;
typedef char letr;
typedef std::ostream o_stream;
typedef std::istream i_stream;
typedef std::ostringstream o_string_stream;
typedef std::istringstream i_string_stream;
#define ORK(X) X//The shortest prefixed macro
#define ORK_CIN std::cin
#define ORK_COUT std::cout
#define ORK_CERR std::cerr
#define ORK_CLOG std::clog
#endif
typedef std::string bstring;//Byte string, so that searches can identify unhandled unicode
typedef std::stringstream b_string_stream;
typedef std::ostream bo_stream;
typedef std::istream bi_stream;
typedef std::ostringstream bo_string_stream;
typedef std::istringstream bi_string_stream;
#define BORK(X) X//Byte Text, so that searches can identify unhandled unicode


#define ORK_FILEN ORK(__FILE__)
#define ORK_LINE ORK(ORK_STR(__LINE__))
#if 1
	#ifdef _MSC_VER
		#ifdef __FUNCTION__
			#define ORK_FUNC __FUNCTION__
		#else
			#define ORK_FUNC ORK("(unknown_function)")
		#endif
	#else
		#ifdef BOOST_CURRENT_FUNCTION
			#define ORK_FUNC BOOST_CURRENT_FUNCTION
		#else
			#define ORK_FUNC ORK("(unknown_function)")
		#endif
	#endif
#else
	#if defined _MSC_VER
		#define ORK_FUNC __FUNCSIG__
	#elif defined __GNUC__
		#define ORK_FUNC __PRETTY_FUNCTION__
	#else
		#error "Compiler not supported"
	#endif
#endif
#define ORK_FLOC ORK_CAT4(ORK_FILEN, ORK("("), ORK_LINE, ORK(")"))


#if UNICODE
typedef boost::filesystem::wofstream of_stream;
typedef boost::filesystem::wifstream if_stream;
typedef boost::filesystem::wfstream f_stream;
#define ORK_GEN_STR generic_wstring
#define ORK_STRING wstring
#else
typedef boost::filesystem::ofstream of_stream;
typedef boost::filesystem::ifstream if_stream;
typedef boost::filesystem::fstream f_stream;
#define ORK_GEN_STR generic_string
#define ORK_STRING string
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
#define ORK_STR_2_BYTE(ORK_STRING) ork::g_string_converter().to_bytes(ORK_STRING)
#define ORK_BYTE_2_STR(ORK_STRING) ork::g_string_converter().from_bytes(ORK_STRING)
#else
#define ORK_STR_2_BYTE(ORK_STRING) ORK_STRING
#define ORK_BYTE_2_STR(ORK_STRING) ORK_STRING
#endif

}//namespace ork
