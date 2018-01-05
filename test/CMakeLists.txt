
cmake_minimum_required(VERSION 3.8 FATAL_ERROR) #Required for C++17

project(ork_test)

set(REPO_ROOT "$ENV{REPO_ROOT}")

include("${REPO_ROOT}/ork/build/macros.cmake")

message("Ork build unicode: $ENV{ORK_UNICODE}")
if($ENV{ORK_UNICODE})
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

set(ORK_EXTERNAL_BOOST ON)
set_as_internal(ORK_3P_BOOST_SRC_DIR "${REPO_ROOT}/boost_$ENV{VS_TOOLSET}")

set_as_internal(ORK_INSTALL_PREFIX "${REPO_ROOT}/ork_test_$ENV{VS_TOOLSET}/bin")

set_as_internal(ORK_3P_SOURCE_DIR "${REPO_ROOT}")


set_as_internal(ORK_USE_CATCH ON)
set_as_internal(ORK_USE_GLM ON)
set_as_internal(ORK_USE_JSON ON)
set_as_internal(ORK_USE_PUGI ON)
set_as_internal(ORK_USE_YAML ON)

set_as_internal(ORK_AUTO_LINK ON)
set_as_internal(ORK_AUTO_LINK_3P ON)
set_as_internal(ORK_BUILD_SHARED ON)


##Level 0
set_as_internal(ORK_BUILD_BZ2 OFF)
set_as_internal(ORK_BUILD_CATCH ON)
set_as_internal(ORK_BUILD_JPEG OFF)
set_as_internal(ORK_BUILD_GLM ON)
set_as_internal(ORK_BUILD_HARFBUZZ OFF)
set_as_internal(ORK_BUILD_JSON ON)
set_as_internal(ORK_BUILD_PUGI ON)
set_as_internal(ORK_BUILD_TBB OFF)
set_as_internal(ORK_BUILD_YAML ON)
set_as_internal(ORK_BUILD_ZLIB OFF)

##Level 1
set_as_internal(ORK_BUILD_ORK ON)
set_as_internal(ORK_BUILD_PNG OFF)

##Level 2
set_as_internal(ORK_BUILD_FT2 OFF)

##Level 3
set_as_internal(ORK_BUILD_OCE OFF)
set_as_internal(ORK_BUILD_OSG OFF)

add_subdirectory("${ORK_3P_SOURCE_DIR}/ork/build" "3p")
