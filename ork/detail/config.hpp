/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#error This header can only be included from ork/ork.hpp!
#endif


#if defined _MSC_VER
#	define ORK_MSC _MSC_VER
#	define ORK_GCC 0
#elif define __GNUC__
#	define ORK_MSC 0
#	define ORK_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#	error Compiler not supported!
#endif

#if ORK_MSC && ORK_MSC < 1800
#error Visual Studio 2012 or greater required
#endif
#if ORK_GCC && ORK_GCC < 40300
#error GCC 4.3 or greater required
#endif


namespace ork {



}//namespace ork
