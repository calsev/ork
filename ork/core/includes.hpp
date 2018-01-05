/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_INCLUDES_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_INCLUDES_HPP
#define ORK_INCLUDES_HPP


#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4365) //return' signed/unsigned mismatch
#pragma warning(disable:4571) //catch(...) semantics changed for structured exceptions
#pragma warning(disable:4625) //implicit copy constructor
#pragma warning(disable:4626) //implicit copy assignment
#pragma warning(disable:4774) //format string argument is not a string literal
#pragma warning(disable:5026) //implicit move constructor
#pragma warning(disable:5027) //implicit move assignment
#endif

#include<array>
#include<deque>
#include<iosfwd>
#include<memory>
#include<string>

#if ORK_MSC
#pragma warning(pop)
#endif


#endif//ORK_INCLUDES_HPP
