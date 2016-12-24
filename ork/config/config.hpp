/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once

#ifndef ORK_ORK_HPP
#	error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_AUTO_LINK//Auto link ork library
#	define ORK_AUTO_LINK 1
#endif

#ifndef ORK_AUTO_LINK_3P//Auto link third-party dependencies
#	define ORK_AUTO_LINK_3P ORK_AUTO_LINK
#endif

#ifndef ORK_USE_PUGI//Optional dependency
#	define ORK_USE_PUGI 1
#endif
