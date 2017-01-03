#This is a non-interactive build script that makes exactly what we require.
#All cache variables are exposed.
#GUI mode is invoked to detect if any variables have been added in child projects.

cmake_minimum_required(VERSION 3.2) #Just a guess, must be 3.x


macro(ensure_non_empty _var)
	if(NOT ${_var})
		message(FATAL_ERROR "Error: ${${_var}} not defined")
	endif()
endmacro()

macro(mark_as_internal _var)
	ensure_non_empty(${_var})
	mark_as_advanced(${_var})
	set(${_var} ${${_var}} CACHE INTERNAL "" FORCE)
endmacro()

macro(set_as_internal _var _val)
	mark_as_advanced(${_var})
	set(${_var} ${_val} CACHE INTERNAL "" FORCE)
endmacro()

macro(mark_empty_internal _var)
	if(${_var})
		message(FATAL_ERROR "Error: ${${_var}} defined")
	endif()
	mark_as_advanced(${_var})
	set(${_var} "" CACHE INTERNAL "" FORCE)
endmacro()

macro(detect_option_flag _var _msg _val)
	if(DEFINED ${_var})
		option(${_var} "${_msg}" ${${_var}})
	else()
		option(${_var} "${_msg}" ${_val})
	endif()
endmacro()

macro(detect_option_path _var _msg _val)
	if(NOT DEFINED ${_var})
		set(${_var} ${_val} PATH "${_msg}")
	endif()
endmacro()

macro(install_bin _targ)
	if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
		install(
			TARGETS ${_targ}
			RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
			ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
		)
	endif()
endmacro()

macro(install_shared _targ _head)
	install_bin(${_targ})
	if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
		install(
			FILES ${_head}
			DESTINATION "${INSTALL_INC_DIR}"
		)
	endif()
endmacro()


##Level 0
detect_option_flag(ORK_BUILD_BZ2 "Enable building bzip" OFF)
detect_option_flag(ORK_BUILD_JPEG "Enable building libjpeg" OFF)
detect_option_flag(ORK_BUILD_HARFBUZZ "Enable building harfbuzz" OFF)
detect_option_flag(ORK_BUILD_PUGI "Enable building pugixml" ON)
detect_option_flag(ORK_BUILD_ZLIB "Enable building zlib" OFF)

##Level 1
detect_option_flag(ORK_BUILD_ORK "Enable building ork" ON)
detect_option_flag(ORK_BUILD_PNG "Enable building libpng" OFF)

##Level 2
detect_option_flag(ORK_BUILD_FT2 "Enable building freetype" OFF)


if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Call cmake from outside source tree.")
endif()


if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	message(FATAL_ERROR "Error: Only x64 targets are supported")
endif()


#Used for concatenating to avoid repeats
if(NOT ORK_CMAKE_RAN_INTERNAL)
set(ORK_CMAKE_RUN_ONCE "ON")
endif()
set_as_internal(ORK_CMAKE_RAN_INTERNAL "ON")


#Must be set before PROJECT to hide
set(CMAKE_CONFIGURATION_TYPES Debug Release)
mark_as_internal(CMAKE_CONFIGURATION_TYPES)


project(third_party)


#Must be tested after project
if(MSVC_VERSION)
	if(${MSVC_VERSION} LESS 1800)
		message(FATAL_ERROR "Error: Only Visual Studio 2013 and above are supported")
	endif()
elseif(NOT CMAKE_C_COMPILER_ID)
	message(FATAL_ERROR "Compiler not found")
elseif(${CMAKE_C_COMPILER_ID} EQUAL GNU)
	#Nothing to do here
else()
	message(FATAL_ERROR "Error: Only MSVC and GCC supported")
endif()


set(CMAKE_CXX_STANDARD "11" CACHE INTERNAL "" FORCE)

ensure_non_empty(CMAKE_CXX_FLAGS)
if(ORK_CMAKE_RUN_ONCE)
	if(ORK_CMAKE_RUN_ONCE)
		mark_as_internal(CMAKE_CXX_FLAGS) #This was causing problems with boost
	elseif(MSVC)
		set_as_internal(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
	elseif(UNIX)
		set_as_internal(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
	else()
		message(FATAL_ERROR "Compiler not supported")
	endif()
endif()
mark_as_internal(CMAKE_CXX_FLAGS_DEBUG)
mark_as_internal(CMAKE_CXX_FLAGS_MINSIZEREL)
ensure_non_empty(CMAKE_CXX_FLAGS_RELEASE)
set_as_internal(CMAKE_CXX_FLAGS_RELEASE "/D_NDEBUG /MD /Zi /Ox /Ob2")#Default: /O2, we could use regex replace
mark_as_internal(CMAKE_CXX_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_CXX_STANDARD_LIBRARIES)


mark_as_internal(CMAKE_C_FLAGS)
mark_as_internal(CMAKE_C_FLAGS_DEBUG)
mark_as_internal(CMAKE_C_FLAGS_MINSIZEREL)
ensure_non_empty(CMAKE_C_FLAGS_RELEASE)
set_as_internal(CMAKE_C_FLAGS_RELEASE "/MD /Zi /Ox /Ob2 /DNDEBUG")
mark_as_internal(CMAKE_C_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_C_STANDARD_LIBRARIES)


set_as_internal(CMAKE_DEBUG_POSTFIX "d")
detect_option_path(ORK_INSTALL_PREFIX "Path of install directory" "${CMAKE_SOURCE_DIR}/../bin")
set_as_internal(CMAKE_INSTALL_PREFIX "${ORK_INSTALL_PREFIX}")
set_as_internal(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set_as_internal(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/inc")
set_as_internal(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")

detect_option_path(ORK_3P_SOURCE_DIR "Path of third-party directory (location of submodules)" "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set_as_internal(3P_BOOST_SRC_DIR "${ORK_3P_SOURCE_DIR}/boost")
set_as_internal(3P_BZ2_SRC_DIR "${ORK_3P_SOURCE_DIR}/bz2")
set_as_internal(3P_FT2_SRC_DIR "${ORK_3P_SOURCE_DIR}/ft2")
set_as_internal(3P_GLM_SRC_DIR "${ORK_3P_SOURCE_DIR}/glm")
set_as_internal(3P_JPEG_SRC_DIR "${ORK_3P_SOURCE_DIR}/jpeg")
set_as_internal(3P_ORK_SRC_DIR "${ORK_3P_SOURCE_DIR}/ork")
set_as_internal(3P_PNG_SRC_DIR "${ORK_3P_SOURCE_DIR}/png")
set_as_internal(3P_PUGI_SRC_DIR "${ORK_3P_SOURCE_DIR}/pugi")
set_as_internal(3P_ZLIB_SRC_DIR "${ORK_3P_SOURCE_DIR}/zlib")

set_as_internal(3P_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set_as_internal(3P_BZ2_BIN_DIR "${3P_BIN_DIR}/bz2")
set_as_internal(3P_FT2_BIN_DIR "${3P_BIN_DIR}/ft2")
set_as_internal(3P_JPEG_BIN_DIR "${3P_BIN_DIR}/jpeg")
set_as_internal(3P_ORK_BIN_DIR "${3P_BIN_DIR}/ork")
set_as_internal(3P_PNG_BIN_DIR "${3P_BIN_DIR}/png")
set_as_internal(3P_PUGI_BIN_DIR "${3P_BIN_DIR}/pugi")
set_as_internal(3P_ZLIB_BIN_DIR "${3P_BIN_DIR}/zlib")

set_as_internal(3P_BOOST_LIB_DIR "${ORK_3P_SOURCE_DIR}/deps/lib")
add_definitions(-DBOOST_ALL_DYN_LINK) #Link boost shared libs


#mark_as_internal(CMAKE_ASM_MASM_COMPILER)
#mark_empty_internal(CMAKE_ASM_MASM_FLAGS)
#mark_empty_internal(CMAKE_ASM_MASM_FLAGS_DEBUG)
#mark_empty_internal(CMAKE_ASM_MASM_FLAGS_MINSIZEREL)
#mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELEASE)
#mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELWITHDEBINFO)


mark_as_internal(CMAKE_RC_COMPILER)
mark_as_internal(CMAKE_RC_FLAGS)


mark_as_internal(CMAKE_LINKER)

mark_as_internal(CMAKE_EXE_LINKER_FLAGS)
mark_as_internal(CMAKE_EXE_LINKER_FLAGS_DEBUG)
mark_as_internal(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
ensure_non_empty(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)
set_as_internal(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
mark_as_internal(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_MODULE_LINKER_FLAGS)
mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_DEBUG)
mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL)
ensure_non_empty(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)
set_as_internal(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO}")
mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_SHARED_LINKER_FLAGS)
mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL)
ensure_non_empty(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)
set_as_internal(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")
mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_STATIC_LINKER_FLAGS)
mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_DEBUG)
mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL)
mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_RELEASE)
mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO)


set_as_internal(CMAKE_SKIP_INSTALL_RPATH "YES")
set_as_internal(CMAKE_SKIP_RPATH "YES")
set_as_internal(CMAKE_VERBOSE_MAKEFILE "NO")


##Level 0

if(ORK_BUILD_BZ2)
	add_subdirectory("./bz2" "${3P_BZ2_BIN_DIR}")
endif()
if(ORK_BUILD_JPEG)
	add_subdirectory("./jpeg" "${3P_JPEG_BIN_DIR}")
endif()
if(ORK_BUILD_PUGI)
	add_subdirectory("./pugi" "${3P_PUGI_BIN_DIR}")
endif()
if(ORK_BUILD_ZLIB)
	add_subdirectory("./zlib" "${3P_ZLIB_BIN_DIR}")
endif()

####Level 1

if(ORK_BUILD_ORK)
	add_subdirectory("./ork" "${3P_ORK_BIN_DIR}")
endif()

if(ORK_BUILD_PNG)
	if(NOT ORK_BUILD_ZLIB)
		message(FATAL_ERROR "libpng requires zlib but zlib is not configured to build")
	endif()
	add_subdirectory("./png" "${3P_PNG_BIN_DIR}")
endif()

####Level 2

if(ORK_BUILD_FT2)
	if(NOT ORK_BUILD_PNG)
		message(FATAL_ERROR "freetype requires libpng but libpng is not configured to build")
	endif()
	add_subdirectory("./ft2" "${3P_FT2_BIN_DIR}")
endif()

####Level 3

add_subdirectory("./cleanup")
