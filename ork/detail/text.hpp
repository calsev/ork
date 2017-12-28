/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif


#include<string>
#include<sstream>
#include<iostream>
#include<codecvt>
#include<mutex>


#if !ORK_MSC
#	include"boost/current_function.hpp"
#endif


#if defined UNICODE || defined _UNICODE//Avoid checking both everywhere
#	define ORK_UNICODE 1
#else
#	define ORK_UNICODE 0
#endif


#if ORK_GCC || ORK_MSC >= 1900
#   define ORK_STL_HAS_FILE 1
#else
#   define ORK_STL_HAS_FILE 0
#endif


#if ORK_STL_HAS_FILE
#   include<experimental/filesystem>
	namespace ork {
		namespace file = std::experimental::filesystem::v1;
	}//namespace ork
#else
	//Some boost configuration here
#   define BOOST_SYSTEM_NO_DEPRECATED 1
#   define BOOST_FILESYSTEM_NO_DEPRECATED 1
#   include"boost/filesystem/fstream.hpp"
	namespace ork {
		namespace file = boost::filesystem;
	}//namespace ork
#endif


//Forward declaration to keep boost out of headers
namespace file {

class wofstream;
class wifstream;
class wfstream;
class ofstream;
class ifstream;
class fstream;

}//namespace file


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



#if ORK_UNICODE
	typedef std::wstring string;
	typedef std::wstringstream string_stream;
	typedef wchar_t char_t;
	typedef std::wostream o_stream;
	typedef std::wistream i_stream;
	typedef std::wostringstream o_string_stream;
	typedef std::wistringstream i_string_stream;
	typedef std::wstreambuf streambuf;
	#define ORK(X) ORK_WIDEN(X)//The shortest prefixed macro
	#define ORK_CIN std::wcin
	#define ORK_COUT std::wcout
	#define ORK_CERR std::wcerr
	#define ORK_CLOG std::wclog
#else
	typedef std::string string;
	typedef std::stringstream string_stream;
	typedef char char_t;
	typedef std::ostream o_stream;
	typedef std::istream i_stream;
	typedef std::ostringstream o_string_stream;
	typedef std::istringstream i_string_stream;
	typedef std::streambuf streambuf;
	#define ORK(X) X//The shortest prefixed macro
	#define ORK_CIN std::cin
	#define ORK_COUT std::cout
	#define ORK_CERR std::cerr
	#define ORK_CLOG std::clog
#endif
typedef std::wstring wstring;
typedef std::string bstring;//Byte string, so that searches can identify unhandled unicode
typedef std::stringstream b_string_stream;
typedef std::ostream bo_stream;
typedef std::istream bi_stream;
typedef std::ostringstream bo_string_stream;
typedef std::istringstream bi_string_stream;
#define BORK(X) X//Byte Text, so that searches can identify unhandled unicode


#define ORK_FILEN ORK(__FILE__)
#define ORK_LINE ORK(ORK_STR(__LINE__))


#if ORK_MSC
#	ifdef __FUNCTION__
#		define ORK_FUNC __FUNCTION__ //__FUNCSIG__
#	else
#		define ORK_FUNC ORK("(unknown_function)")
#	endif
#elif ORK_GCC
#	ifdef BOOST_CURRENT_FUNCTION
#		define ORK_FUNC BOOST_CURRENT_FUNCTION //__PRETTY_FUNCTION__
#	else
#		define ORK_FUNC ORK("(unknown_function)")
#	endif
#else
#	error Compiler not supported
#endif


#define ORK_FLOC ORK_CAT4(ORK_FILEN, ORK("("), ORK_LINE, ORK(")"))


#if ORK_UNICODE
	typedef file::wofstream of_stream;
	typedef file::wifstream if_stream;
	typedef file::wfstream f_stream;
	#define ORK_GEN_STR generic_wstring
	#define ORK_STRING wstring
#else
	typedef file::ofstream of_stream;
	typedef file::ifstream if_stream;
	typedef file::fstream f_stream;
	#define ORK_GEN_STR generic_string
	#define ORK_STRING string
#endif


class string_converter_type {//Just a thread-safe wrapper for std::wstring_convert
private:
	typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>string_converter;
	typedef std::mutex mutex_type;
	typedef std::lock_guard<mutex_type>lock_type;
private:
	string_converter m_converter;
	std::mutex m_mutex;
public:
	ORK_ORK_API bstring to_bytes(const wchar_t s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	ORK_ORK_API bstring to_bytes(const wchar_t*s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	ORK_ORK_API bstring to_bytes(const wstring&s) {
		lock_type lock(m_mutex); return m_converter.to_bytes(s);
	}
	ORK_ORK_API bstring to_bytes(const wchar_t*first, const wchar_t*last) {
		lock_type lock(m_mutex); return m_converter.to_bytes(first, last);
	}

	ORK_ORK_API wstring from_bytes(const char s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	ORK_ORK_API wstring from_bytes(const char*s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	ORK_ORK_API wstring from_bytes(const bstring&s) {
		lock_type lock(m_mutex); return m_converter.from_bytes(s);
	}
	ORK_ORK_API wstring from_bytes(const char*first, const char*last) {
		lock_type lock(m_mutex); return m_converter.from_bytes(first, last);
	}
};
ORK_ORK_EXT(string_converter_type&) g_string_converter();


#if ORK_UNICODE
#	define ORK_STR_2_BYTE(ORK_STRING) ork::g_string_converter().to_bytes(ORK_STRING)
#	define ORK_BYTE_2_STR(ORK_STRING) ork::g_string_converter().from_bytes(ORK_STRING)
#else
#	define ORK_STR_2_BYTE(ORK_STRING) ORK_STRING
#	define ORK_BYTE_2_STR(ORK_STRING) ORK_STRING
#endif

}//namespace ork
