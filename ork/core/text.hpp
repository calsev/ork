/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_TEXT_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_TEXT_HPP
#define ORK_TEXT_HPP


//Forward declaration to keep boost out of headers
#if ORK_STL_HAS_FILE

namespace ork {
namespace detail {
namespace fstream {

using wofstream = std::wofstream;
using wifstream = std::wifstream;
using wfstream = std::wfstream;
using ofstream = std::ofstream;
using ifstream = std::ifstream;
using fstream = std::fstream;

}//namespace fstream
}//namespace detail
}//namespace ork

#else

//Some boost configuration here
#define BOOST_SYSTEM_NO_DEPRECATED 1
#define BOOST_FILESYSTEM_NO_DEPRECATED 1

namespace boost {
namespace filesystem {

class wofstream;
class wifstream;
class wfstream;
class ofstream;
class ifstream;
class fstream;

}//filesystem
}//boost


namespace ork {
namespace detail {
namespace fstream {

using wofstream = boost::filesystem::wofstream;
using wifstream = boost::filesystem::wifstream;
using wfstream = boost::filesystem::wfstream;
using ofstream = boost::filesystem::ofstream;
using ifstream = boost::filesystem::ifstream;
using fstream = boost::filesystem::fstream;

}//namespace fstream
}//namespace detail
}//namespace ork

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
#define WORK(X) ORK_WIDEN(X)//Wide text, so that searches can identify unhandled unicode
#define BORK(X) X//Byte text, so that searches can identify unhandled unicode


#define ORK_FILEN ORK(__FILE__)
#define ORK_LINE ORK(ORK_STR(__LINE__))


#define _ORK_UID2(PREFIX, LINE, COUNT) ORK_CAT3(PREFIX, LINE, COUNT)
#define _ORK_UID1(PREFIX, LINE) _ORK_UID2(PREFIX, LINE, __COUNTER__)
#define ORK_UID(PREFIX) _ORK_UID1(PREFIX, __LINE__)


#if ORK_UNICODE
#   define ORK_FUNC_ ORK_STR(no_func_unicode_build)
#elif ORK_MSC
#   define ORK_FUNC_ __FUNCSIG__
#elif ORK_GCC
#   define ORK_FUNC_ __PRETTY_FUNCTION__
#else
#	error Compiler not supported
#endif

#define ORK_FUNC ORK(ORK_FUNC_)


#define ORK_FLOC_STREAM ORK_FILEN << ORK("(") << ORK_LINE << ORK(")")


#ifndef ORK_WARNING
#	if ORK_MSC
#		define ORK_WARNING(MSG) message(__FILE__ "(" ORK_STR(__LINE__) ") : Warning: " ORK_STR(MSG))
#	elif ORK_GCC
#		define ORK_WARNING(MSG) warning(ORK_STR(MSG))
#   else
#       error Compiler not supported
#	endif
#endif


#if ORK_UNICODE
	typedef detail::fstream::wofstream of_stream;
	typedef detail::fstream::wifstream if_stream;
	typedef detail::fstream::wfstream f_stream;
	#define ORK_GEN_STR generic_wstring
	#define ORK_STRING ork::wstring
#else
	typedef detail::fstream::ofstream of_stream;
	typedef detail::fstream::ifstream if_stream;
	typedef detail::fstream::fstream f_stream;
	#define ORK_GEN_STR generic_string
	#define ORK_STRING ork::bstring
#endif
typedef detail::fstream::wofstream wof_stream;
typedef detail::fstream::wifstream wif_stream;
typedef detail::fstream::wfstream wf_stream;
typedef detail::fstream::ofstream bof_stream;
typedef detail::fstream::ifstream bif_stream;
typedef detail::fstream::fstream bf_stream;


class string_converter_type {//Just a thread-safe wrapper for std::wstring_convert
private:
	struct impl;
private:
	std::unique_ptr<impl>_pimpl;
public:
	string_converter_type();
	~string_converter_type();
	ORK_MOVE_ONLY(string_converter_type)
public:
	ORK_ORK_API bstring wide2byte(const wchar_t s);
	ORK_ORK_API bstring wide2byte(const wchar_t*s);
	ORK_ORK_API bstring wide2byte(const wstring&s);
	ORK_ORK_API bstring wide2byte(const wchar_t*first, const wchar_t*last);

	ORK_ORK_API wstring byte2wide(const char s);
	ORK_ORK_API wstring byte2wide(const char*s);
	ORK_ORK_API wstring byte2wide(const bstring&s);
	ORK_ORK_API wstring byte2wide(const char*first, const char*last);
};
ORK_ORK_EXT(string_converter_type&) g_string_converter();


#if ORK_UNICODE
#	define ORK_STR_2_BYTE(ORK_STRING) ork::g_string_converter().wide2byte(ORK_STRING)
#	define ORK_STR_2_WIDE(ORK_STRING) ORK_STRING
#	define ORK_BYTE_2_STR(ORK_STRING) ork::g_string_converter().byte2wide(ORK_STRING)
#	define ORK_WIDE_2_STR(ORK_STRING) ORK_STRING
#else
#	define ORK_STR_2_BYTE(ORK_STRING) ORK_STRING
#	define ORK_STR_2_WIDE(ORK_STRING) ork::g_string_converter().byte2wide(ORK_STRING)
#	define ORK_BYTE_2_STR(ORK_STRING) ORK_STRING
#	define ORK_WIDE_2_STR(ORK_STRING) ork::g_string_converter().wide2byte(ORK_STRING)
#endif

}//namespace ork

#endif//ORK_TEXT_HPP
