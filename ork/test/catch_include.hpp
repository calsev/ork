/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include"ork/ork.hpp"

#if ORK_MSC
#pragma warning(push)
#pragma warning(disable:4061) //enum val not explicitly handled by a case label
#pragma warning(disable:4365) //return' signed/unsigned mismatch
#pragma warning(disable:4371) //layout changed
#pragma warning(disable:4623) //default constructor implicitly deleted
#pragma warning(disable:4625) //implicit copy constructor
#pragma warning(disable:4626) //implicit copy assignment
#pragma warning(disable:5026) //implicit move constructor
#pragma warning(disable:5027) //implicit move assignment
#pragma warning(disable:5039) //pointer or reference to throwing function passed to extern C function
#endif

#include"catch.hpp"

#if ORK_NEED_STL_SUPPRESS
#pragma warning(pop)
#endif
