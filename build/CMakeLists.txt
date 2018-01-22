#This is a non-interactive build script that makes exactly what we require.
#All cache variables are set then hidden.
#GUI mode is invoked to detect if any variables have been added in child projects.

cmake_minimum_required(VERSION 3.8 FATAL_ERROR) #Required for C++17

include("macros.cmake")
ork_pre_project_config()
project(third_party)


##########
#Language Section
##########

set_latest_language_versions()
set_as_internal(CMAKE_CXX_EXTENSIONS OFF)

#Must be tested after project
get_compiler_name(COMPILER)
if(${COMPILER} MATCHES "vc")
	if(NOT COMPLILER MATCHES "vc1[2-9]")
		message(FATAL_ERROR "Error: Only Visual Studio 2013 and above are supported")
	endif()
elseif(${COMPILER} MATCHES "gcc|gxx")
	# All recent versions of GCC are OK
else()
	message(FATAL_ERROR "Error: Only MSVC and GCC supported")
endif()


mark_as_internal(CMAKE_CXX_FLAGS)
mark_as_internal(CMAKE_CXX_FLAGS_DEBUG)
mark_as_internal(CMAKE_CXX_FLAGS_MINSIZEREL)
mark_as_internal(CMAKE_CXX_FLAGS_RELEASE)
mark_as_internal(CMAKE_CXX_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_CXX_STANDARD_LIBRARIES)

mark_as_internal(CMAKE_C_FLAGS)
mark_as_internal(CMAKE_C_FLAGS_DEBUG)
mark_as_internal(CMAKE_C_FLAGS_MINSIZEREL)
mark_as_internal(CMAKE_C_FLAGS_RELEASE)
mark_as_internal(CMAKE_C_FLAGS_RELWITHDEBINFO)

mark_as_internal(CMAKE_C_STANDARD_LIBRARIES)


append_debug_flag("-D_DEBUG -DDEBUG")
append_release_flag("-D_NDEBUG -DNDEBUG")
replace_or_append_release_flag_compilers("/O([0-4]|x)" "-O[0-3]" "/Ox" "-O3")
replace_or_append_release_flag_compilers("/Z(7|i|l)" "IMPOSSIBLE" "/Zi" "")
replace_or_append_release_flag_compilers("/Ob[0-2]" "IMPOSSIBLE" "/Ob2" "")


#Fast behavior is bad for CAD kernels
detect_option_flag(ORK_FAST_FLOAT "Use fast floating point operations (Do not use in CAD applications)" OFF)
enable_fast_floating_point(ORK_FAST_FLOAT)


mark_as_internal(CMAKE_RC_COMPILER)
mark_as_internal(CMAKE_RC_FLAGS)
if(CMAKE_RC_FLAGS_DEBUG)#Not always defined
	mark_as_internal(CMAKE_RC_FLAGS_DEBUG)
else()
	mark_empty_internal(CMAKE_RC_FLAGS_DEBUG)
endif()
mark_empty_internal(CMAKE_RC_FLAGS_MINSIZEREL)
mark_empty_internal(CMAKE_RC_FLAGS_RELEASE)
mark_empty_internal(CMAKE_RC_FLAGS_RELWITHDEBINFO)


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


set_as_internal(CMAKE_SKIP_INSTALL_RPATH ON)
set_as_internal(CMAKE_SKIP_RPATH ON)
set_as_internal(CMAKE_VERBOSE_MAKEFILE OFF)


##########
#Options Section
##########

set_as_internal(CMAKE_DEBUG_POSTFIX "d")

if(DEFINED ORK_INSTALL_PREFIX AND ORK_INSTALL_PREFIX) #We set this previously
	file(TO_CMAKE_PATH "${ORK_INSTALL_PREFIX}" ORK_INSTALL_PREFIX)
elseif(DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_INSTALL_PREFIX)
	detect_option_path(ORK_INSTALL_PREFIX "Path of install directory" "${CMAKE_INSTALL_PREFIX}")
else()
	detect_option_path(ORK_INSTALL_PREFIX "Path of install directory" "${CMAKE_SOURCE_DIR}/../bin")
endif()
set_as_internal(CMAKE_INSTALL_PREFIX "${ORK_INSTALL_PREFIX}")

detect_option(ORK_INSTALL_BIN_SUFFIX "Subdirectory for installation of executables and shared libraries" bin)
detect_option(ORK_INSTALL_INC_SUFFIX "Subdirectory for installation of headers" inc)
detect_option(ORK_INSTALL_LIB_SUFFIX "Subdirectory for installation of static libraries and archives" lib)
set_as_internal(ORK_INSTALL_BIN_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_BIN_SUFFIX}")
set_as_internal(ORK_INSTALL_INC_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_INC_SUFFIX}")
set_as_internal(ORK_INSTALL_LIB_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_LIB_SUFFIX}")


#Build output, not that it is usually used without installing
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${ORK_INSTALL_LIB_SUFFIX}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${ORK_INSTALL_BIN_SUFFIX}")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${ORK_INSTALL_LIB_SUFFIX}")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${ORK_INSTALL_BIN_SUFFIX}")


##Level 0
detect_option_flag(ORK_BUILD_BOOST "Enable building boost interface" OFF)
detect_option_flag(ORK_BUILD_BZ2 "Enable building bzip" OFF)
detect_option_flag(ORK_BUILD_CATCH "Enable building Catch2" OFF)
detect_option_flag(ORK_BUILD_JPEG "Enable building libjpeg" OFF)
detect_option_flag(ORK_BUILD_GLM "Enable building GLM interface" OFF) #Not actually building but using
detect_option_flag(ORK_BUILD_HARFBUZZ "Enable building harfbuzz" OFF)
detect_option_flag(ORK_BUILD_JSON "Enable building jsoncpp" OFF)
detect_option_flag(ORK_BUILD_PUGI "Enable building pugixml" OFF)
detect_option_flag(ORK_BUILD_TBB "Enable building TBB" OFF)
detect_option_flag(ORK_BUILD_YAML "Enable building yaml-cpp" OFF)
detect_option_flag(ORK_BUILD_ZLIB "Enable building zlib" OFF)

##Level 1
detect_option_flag(ORK_BUILD_ORK "Enable building ork" OFF)
detect_option_flag(ORK_BUILD_PNG "Enable building libpng" OFF)

##Level 2
detect_option_flag(ORK_BUILD_FT2 "Enable building freetype" OFF)

##Level 3
detect_option_flag(ORK_BUILD_OCE "Enable building OpenCascade" OFF)
detect_option_flag(ORK_BUILD_OSG "Enable building OpenSceneGraph" OFF)


detect_option_path(ORK_3P_SOURCE_DIR "Path of third-party directory (location of submodules)" "${CMAKE_CURRENT_SOURCE_DIR}/../..")


if(ORK_BUILD_TBB)
	detect_option_flag(ORK_BUILD_TBB_MALLOC "Build TBB malloc library" ON)
	detect_option_flag(ORK_BUILD_TBB_MALLOC_PROXY "Build TBB malloc proxy library" ON)
endif()


##########
#Assembly requires explicit enable
##########
if(ORK_BUILD_TBB)
	enable_language(ASM_MASM)
	mark_as_internal(CMAKE_ASM_MASM_COMPILER)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_DEBUG)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_MINSIZEREL)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELEASE)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELWITHDEBINFO)
endif()


##########
#Paths Section
##########

##Level 0
find_3p_path(ORK_BUILD_BOOST 3P_BOOST_SRC_DIR "boost" "boost")
find_3p_path(ORK_BUILD_BZ2 3P_BZ2_SRC_DIR "bz2" "bzip")
find_3p_path(ORK_BUILD_CATCH 3P_CATCH_SRC_DIR "catch" "Catch2")
find_3p_path(ORK_BUILD_JPEG 3P_JPEG_SRC_DIR "jpeg" "jpeg-turbo")
find_3p_path(ORK_BUILD_GLM 3P_GLM_SRC_DIR "glm" "glm")
find_3p_path(ORK_BUILD_HARFBUZZ 3P_HARFBUZZ_SRC_DIR "harfbuzz" "harfbuzz")
find_3p_path(ORK_BUILD_JSON 3P_JSON_SRC_DIR "json" "jsoncpp")
find_3p_path(ORK_BUILD_PUGI 3P_PUGI_SRC_DIR "pugi" "pugixml")
find_3p_path(ORK_BUILD_TBB 3P_TBB_SRC_DIR "tbb" "tbb")
find_3p_path(ORK_BUILD_YAML 3P_YAML_SRC_DIR "yaml" "yaml-cpp")
find_3p_path(ORK_BUILD_ZLIB 3P_ZLIB_SRC_DIR "zlib" "libz")

##Level 1
find_3p_path(ORK_BUILD_ORK 3P_ORK_SRC_DIR "ork" "ork")
find_3p_path(ORK_BUILD_PNG 3P_PNG_SRC_DIR "png" "libpng")

##Level 2
find_3p_path(ORK_BUILD_FT2 3P_FT2_SRC_DIR "ft2" "freetype")

##Level 3
find_3p_path(ORK_BUILD_OCE 3P_OCE_SRC_DIR "oce" "occt")
find_3p_path(ORK_BUILD_OSG 3P_OSG_SRC_DIR "osg" "openscenegraph")


set_as_internal(3P_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}")

if(ORK_BUILD_BOOST)
	# If CMake supported target_link_directories then interface libraries would be a snap
	# As is stands, every target that uses boost must use link_directories to add this path
	detect_path(3P_BOOST_LIB_DIR "${3P_BOOST_SRC_DIR}/lib" "${3P_BOOST_SRC_DIR}/lib/x64")
	
	# We go ahead and add these so we get some efficiency of scale for boost dependents
	append_all_compiler_flag("-DBOOST_ALL_DYN_LINK") #Link boost shared libs
	append_all_compiler_flag("-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE") # Pops up every new release of a compiler
endif()


##Level 0
set_as_internal(3P_BOOST_BIN_DIR "${3P_BIN_DIR}/boost")
set_as_internal(3P_BZ2_BIN_DIR "${3P_BIN_DIR}/bz2")
#Catch
set_as_internal(3P_JPEG_BIN_DIR "${3P_BIN_DIR}/jpeg")
set_as_internal(3P_GLM_BIN_DIR "${3P_BIN_DIR}/glm")
#harfbuzz
set_as_internal(3P_JSON_BIN_DIR "${3P_BIN_DIR}/json")
set_as_internal(3P_PUGI_BIN_DIR "${3P_BIN_DIR}/pugi")
set_as_internal(3P_TBB_BIN_DIR "${3P_BIN_DIR}/tbb")
set_as_internal(3P_YAML_BIN_DIR "${3P_BIN_DIR}/yaml")
set_as_internal(3P_ZLIB_BIN_DIR "${3P_BIN_DIR}/zlib")

##Level 1
set_as_internal(3P_ORK_BIN_DIR "${3P_BIN_DIR}/ork")
set_as_internal(3P_PNG_BIN_DIR "${3P_BIN_DIR}/png")

##Level 2
set_as_internal(3P_ORK_TEST_BIN_DIR "${3P_BIN_DIR}/ork_test")
set_as_internal(3P_FT2_BIN_DIR "${3P_BIN_DIR}/ft2")

##Level 3
set_as_internal(3P_OCE_BIN_DIR "${3P_BIN_DIR}/oce")
set_as_internal(3P_OSG_BIN_DIR "${3P_BIN_DIR}/osg")


##########
#Build Section
##########

##Level 0

if(ORK_BUILD_BOOST)
	add_subdirectory("./boost" "${3P_BOOST_BIN_DIR}")
endif()
if(ORK_BUILD_BZ2)
	add_subdirectory("./bz2" "${3P_BZ2_BIN_DIR}")
endif()
#Catch nothing to build
if(ORK_BUILD_JPEG)
	add_subdirectory("./jpeg" "${3P_JPEG_BIN_DIR}")
endif()
if(ORK_BUILD_GLM)
	add_subdirectory("./glm" "${3P_GLM_BIN_DIR}")
endif()
#Harfbuzz under construction
if(ORK_BUILD_JSON)
	add_subdirectory("./json" "${3P_JSON_BIN_DIR}")
endif()
if(ORK_BUILD_PUGI)
	add_subdirectory("./pugi" "${3P_PUGI_BIN_DIR}")
endif()
if(ORK_BUILD_TBB)
	add_subdirectory("./tbb" "${3P_TBB_BIN_DIR}")
endif()
if(ORK_BUILD_YAML)
	add_subdirectory("./yaml" "${3P_YAML_BIN_DIR}")
endif()
if(ORK_BUILD_ZLIB)
	add_subdirectory("./zlib" "${3P_ZLIB_BIN_DIR}")
endif()

##Level 1

if(ORK_BUILD_ORK)
	add_subdirectory("./ork" "${3P_ORK_BIN_DIR}")
endif()
if(ORK_BUILD_PNG)
	if(NOT ORK_BUILD_ZLIB)
		message(FATAL_ERROR "libpng requires zlib but zlib is not configured to build")
	endif()
	add_subdirectory("./png" "${3P_PNG_BIN_DIR}")
endif()

##Level 2

if(ORK_BUILD_FT2)
	if(NOT ORK_BUILD_PNG)
		message(FATAL_ERROR "freetype requires libpng but libpng is not configured to build")
	endif()
	add_subdirectory("./ft2" "${3P_FT2_BIN_DIR}")
endif()

##Level 3
if(ORK_BUILD_OCE)
	add_subdirectory("./oce" "${3P_OCE_BIN_DIR}")
endif()
if(ORK_BUILD_OSG)
	add_subdirectory("./osg" "${3P_OSG_BIN_DIR}")
endif()

add_subdirectory("./cleanup" "${3P_BIN_DIR}/cleanup")#Must be a subdirectory to guarantee order
