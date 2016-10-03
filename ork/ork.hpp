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

#include"ork/detail/text.hpp"
#include"ork/detail/tools.hpp"
#include"ork/detail/exception.hpp"
#include"ork/detail/file_system.hpp"
#include"ork/detail/log.hpp"


namespace ork {

#define ORK_MAJOR 0
#define ORK_MINOR 1
#define ORK_REV 0

#define ORK_VER CAT(ORK_MINOR, ORK_REV)//1.0 is 100
#define ORK_VER_STR_ STR(CAT5(ORK_MAJOR, ., ORK_MINOR, ., ORK_REV))
#define ORK_VER_STR_W WIDEN(ORK_VER_STR_)
#define ORK_VER_STR_B ORK_VER_STR_
#ifdef UNICODE
#define ORK_VER_STR ORK_VER_STR_W
#else
#define ORK_VER_STR ORK_VER_STR_B
#endif


/*
Auto linking, at least in Visual Studio
Only link if using the current version of Visual Studio
*/
#if defined _MSC_VER
#ifdef _DEBUG
#pragma comment(lib,"lib_ork-d")
#else
#pragma comment(lib,"lib_ork")
#endif
#endif


/*
For backtrace generation
*/
#if defined _MSC_VER
#pragma comment(lib,"DbgHelp.lib")
#endif

//Put this somewhere
string to_string(const bool val);
bool string2bool(string val);


}//namespace ork
#endif ORK_ORK_HPP
