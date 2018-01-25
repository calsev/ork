
cmake_minimum_required(VERSION 3.8 FATAL_ERROR) #Required for C++17

set(REPO_ROOT "$ENV{REPO_ROOT}")
include("${REPO_ROOT}/ork/build/macros.cmake")


ork_pre_project_config()
project(ork_test CXX)
ork_post_project_config()


set(SKIP_INSTALL_HEADERS ON)
set(SKIP_INSTALL_LIBRARIES ON)

set_as_internal(ORK_INSTALL_PREFIX "${REPO_ROOT}/ork_test_$ENV{VS_TOOLSET}")
ork_install_config()

set_as_internal(ORK_3P_SOURCE_DIR "${REPO_ROOT}")
set_as_internal(3P_BOOST_SRC_DIR "${REPO_ROOT}/boost_$ENV{VS_TOOLSET}")


set_as_internal(ORK_USE_BOOST ON)
set_as_internal(ORK_USE_CATCH ON)
set_as_internal(ORK_USE_GLM ON)
set_as_internal(ORK_USE_JSON ON)
set_as_internal(ORK_USE_PUGI ON)


##Level 0
set_as_internal(ORK_BUILD_BOOST ON)
set_as_internal(ORK_BUILD_CATCH ON)
set_as_internal(ORK_BUILD_GLM ON)
set_as_internal(ORK_BUILD_JSON ON)
set_as_internal(ORK_BUILD_PUGI ON)

##Level 1
set_as_internal(ORK_BUILD_ORK ON)


message("Ork build unicode: $ENV{ORK_UNICODE}")
if($ENV{ORK_UNICODE})
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

add_subdirectory("${ORK_3P_SOURCE_DIR}/ork/build" "3p")


set_advanced_warnings()
set_vector_architecture()
set_catch_warnings()

set(ORK_TEST_HDRS
	"${ORK_3P_SOURCE_DIR}/ork/ork/test/catch_include.hpp"
)
set(ORK_TEST_SRCS
	"${ORK_3P_SOURCE_DIR}/ork/src/test/color.cpp"
	"${ORK_3P_SOURCE_DIR}/ork/src/test/test_main.cpp"
)
source_group("inc" FILES ${ORK_TEST_HDRS})
source_group("src" FILES ${ORK_TEST_SRCS})


add_executable(ork_test ${ORK_TEST_HDRS} ${ORK_TEST_SRCS})
set_target_properties(ork_test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${ORK_INSTALL_PREFIX}/bin")

target_link_libraries(ork_test PRIVATE ork)


install_exe(ork_test)
