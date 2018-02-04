/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#    error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_INCLUDES_HPP
#    error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_INCLUDES_HPP
#    define ORK_INCLUDES_HPP


#    define ORK_STL_INC_FILE <array>
#    include "ork/core/stl_include.inl"
#    define ORK_STL_INC_FILE <deque>
#    include "ork/core/stl_include.inl"
#    define ORK_STL_INC_FILE <iosfwd>
#    include "ork/core/stl_include.inl"
#    define ORK_STL_INC_FILE <memory>
#    include "ork/core/stl_include.inl"
#    define ORK_STL_INC_FILE <string>
#    include "ork/core/stl_include.inl"

#    if ORK_STL_HAS_FILE
#        define ORK_STL_INC_FILE <filesystem>
#        include "ork/core/stl_include.inl"
#    else
#        define ORK_BOOST_INC_FILE "boost/filesystem"
#        include "ork/core/boost_include.inl"
#    endif


#endif // ORK_INCLUDES_HPP
