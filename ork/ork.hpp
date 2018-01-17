/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#define ORK_ORK_HPP

/*
Cumulative header for the ork framework:
Types and utilities for text, files and logging
*/

#include"ork/config/config.hpp"
#include"ork/core/config_internal.hpp"
#include"ork/core/includes.hpp"
#include"ork/core/tools.hpp"
#include"ork/core/text.hpp"
#include"ork/core/exception.hpp"
#include"ork/core/file_system.hpp"
#include"ork/core/log.hpp"


namespace ork {


#define ORK_VERSION_MAJOR 0
#define ORK_VERSION_MINOR 2//2 digit
#define ORK_VERSION_PATCH 0//2 digits
#define ORK_VERSION (ORK_VERSION_MAJOR*10000 + ORK_VERSION_MINOR*100 + ORK_VERSION_PATCH)//1.0 is 10000


#define ORK_VERSION_STR_ ORK_STR(ORK_CAT(ORK_VERSION_MAJOR, ., ORK_VERSION_MINOR, ., ORK_VERSION_PATCH))
#define ORK_VERSION_STR_W ORK_WIDEN(ORK_VERSION_STR_)
#define ORK_VERSION_STR_B ORK_VERSION_STR_
#if ORK_UNICODE
#	define ORK_VERSION_STR ORK_VERSION_STR_W
#else
#	define ORK_VERSION_STR ORK_VERSION_STR_B
#endif


/*
Auto linking, at least in Visual Studio
*/
#if ORK_AUTO_LINK && ORK_MSC
#	if !ORK_BUILD_ORK
#		if ORK_DEBUG
#			pragma comment(lib,"orkd")
#		else
#			pragma comment(lib,"ork")
#		endif
#	endif
#if ORK_AUTO_LINK_3P
#	pragma comment(lib,"DbgHelp.lib")//For backtrace generation
#	if ORK_USE_PUGI
#		if ORK_DEBUG
#			pragma comment(lib,"pugixmld")
#		else
#			pragma comment(lib,"pugixml")
#		endif
#	endif
#	if ORK_USE_JSON
#		if ORK_DEBUG
#			pragma comment(lib,"jsoncppd")
#		else
#			pragma comment(lib,"jsoncpp")
#		endif
#	endif
#	if ORK_USE_YAML
#		if ORK_DEBUG
#			pragma comment(lib,"yamlcppd")
#		else
#			pragma comment(lib,"yamlcpp")
#		endif
#	endif
#endif
#endif


}//namespace ork
#endif//ORK_ORK_HPP
