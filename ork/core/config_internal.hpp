/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#ifndef ORK_ORK_HPP
#    error This header can only be included from ork/ork.hpp!
#endif
#ifdef ORK_CONFIG_INTERNAL_HPP
#    error This header can only be included from ork/ork.hpp!
#endif

#ifndef ORK_CONFIG_INTERNAL_HPP
#    define ORK_CONFIG_INTERNAL_HPP


#    if defined _MSC_VER
#        define ORK_MSC _MSC_VER
#        define ORK_GCC 0
#    elif define __GNUC__
#        define ORK_MSC 0
#        define ORK_GCC \
            (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#    else
#        error Compiler not supported!
#    endif


// We only support native compilers
#    if defined _WIN32 && !ORK_MSC
#        error Cygwin not supported
#    endif


#    if ORK_MSC && ORK_MSC < 1800
#        error Visual Studio 2013 or greater required
#    endif
#    if ORK_GCC && ORK_GCC < 40300
#        error GCC 4.3 or greater required
#    endif


#    if !defined ORK_DEBUG
#        if defined DEBUG || defined _DEBUG
#            if defined NDEBUG || defined _NDEBUG
#                error Cannot define debug and not debug
#            endif
#            define ORK_DEBUG 1
#            define ORK_NDEBUG 0
#        else
#            define ORK_DEBUG 0
#            define ORK_NDEBUG 1
#        endif
#    endif


#    ifndef ORK_CALL
#        if ORK_MSC
// Default for x64, __cdecl is default for x86
#            define ORK_CALL __fastcall
#        elif ORK_GCC
#            define ORK_CALL
#        else
#            error Compiler not supported
#        endif
#    endif


#    ifndef ORK_C_CALL
#        if ORK_MSC
#            define ORK_C_CALL __cdecl
#        elif ORK_GCC
#            define ORK_C_CALL
#        else
#            error Compiler not supported
#        endif
#    endif


/*
ORK_ORK versions are for internal use by ork
Plain versions are for use by clients
*/
#    ifndef ORK_BUILD_ORK
#        define ORK_BUILD_ORK 0
#    endif
#    ifndef ORK_ORK_API
#        if !ORK_ORK_DLL
#            define ORK_ORK_API
#            define ORK_ORK_LOCAL
#        elif ORK_MSC
#            if ORK_BUILD_ORK // Define to compile Ork DLL
#                define ORK_ORK_API __declspec(dllexport)
#            else // Default for using DLL
#                define ORK_ORK_API __declspec(dllimport)
#            endif
#            define ORK_ORK_LOCAL // Default in VS
#        elif ORK_GCC
#            define ORK_ORK_API __attribute__((visibility("default")))
#            define ORK_ORK_LOCAL \
                __attribute__((visibility("hidden"))) // Too bad this is not default
#        else
#            error Compiler not supported
#        endif
#    endif
#    ifndef ORK_DLL
#        define ORK_DLL 0
#    endif
#    ifndef ORK_API
#        if !ORK_DLL
#            define ORK_API
#            define ORK_LOCAL
#        elif ORK_MSC
#            if ORK_BUILD_DLL // Define to compile client DLL
#                define ORK_API __declspec(dllexport)
#            else // Default for using DLL
#                define ORK_API __declspec(dllimport)
#            endif
#            define ORK_LOCAL // Default in VS
#        elif ORK_GCC
#            define ORK_API __attribute__((visibility("default")))
#            define ORK_LOCAL \
                __attribute__((visibility("hidden"))) // Too bad this is not default
#        else
#            error Compiler not supported
#        endif
#    endif


/*
For global exports
*/
#    ifndef ORK_ORK_EXTERN
#        define ORK_ORK_EXTERN extern ORK_ORK_API
#    endif
#    ifndef ORK_EXTERN
#        define ORK_EXTERN extern ORK_API
#    endif


/*
For template exports
*/
#    ifndef ORK_EXT_TEMP
#        if !ORK_BUILD_ORK && ORK_MSC
#            define ORK_EXT_TEMP extern template
#        else
#            define ORK_EXT_TEMP template
#        endif
#    endif


/*
Convenience macro for functions
*/
#    ifndef ORK_ORK_EXT // External
#        define ORK_ORK_EXT(RETVAL) ORK_ORK_API RETVAL ORK_CALL
#    endif
#    ifndef ORK_ORK_INT // Internal
#        define ORK_ORK_INT(RETVAL) ORK_ORK_LOCAL RETVAL ORK_CALL
#    endif
#    ifndef ORK_EXT // External
#        define ORK_EXT(RETVAL) ORK_API RETVAL ORK_CALL
#    endif
#    ifndef ORK_INT // Internal
#        define ORK_INT(RETVAL) ORK_LOCAL RETVAL ORK_CALL
#    endif


#    if defined UNICODE || defined _UNICODE // Avoid checking both everywhere
#        define ORK_UNICODE 1
#    else
#        define ORK_UNICODE 0
#    endif


// Switch for transition from boost to stl
#    if ORK_GCC || ORK_MSC >= 1900
#        define ORK_STL_HAS_FILE 1
#    else
#        define ORK_STL_HAS_FILE 0
#    endif

#    if ORK_MSC > 1900 // Things fell apart in VS 2017
#        define ORK_NEED_STL_SUPPRESS 1
#    else
#        define ORK_NEED_STL_SUPPRESS 0
#    endif


#endif // ORK_CONFIG_INTERNAL_HPP
