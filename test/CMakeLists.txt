
cmake_minimum_required(VERSION 3.8 FATAL_ERROR) #Required for C++17

project(ork_test CXX)

set(REPO_ROOT "$ENV{REPO_ROOT}")

include("${REPO_ROOT}/ork/build/macros.cmake")

set(SKIP_INSTALL_HEADERS ON)
set(SKIP_INSTALL_LIBRARIES ON)

message("Ork build unicode: $ENV{ORK_UNICODE}")
if($ENV{ORK_UNICODE})
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

set_as_internal(ORK_INSTALL_PREFIX "${REPO_ROOT}/ork_test_$ENV{VS_TOOLSET}")

set_as_internal(ORK_3P_SOURCE_DIR "${REPO_ROOT}")

set_as_internal(3P_BOOST_LIB_DIR "${REPO_ROOT}/boost_$ENV{VS_TOOLSET}/lib")


set_as_internal(ORK_USE_BOOST ON)
set_as_internal(ORK_USE_CATCH ON)
set_as_internal(ORK_USE_GLM ON)
set_as_internal(ORK_USE_JSON ON)
set_as_internal(ORK_USE_PUGI ON)
set_as_internal(ORK_USE_YAML ON)


##Level 0
set_as_internal(ORK_BUILD_BOOST ON)
set_as_internal(ORK_BUILD_CATCH ON)
set_as_internal(ORK_BUILD_GLM ON)
set_as_internal(ORK_BUILD_JSON ON)
set_as_internal(ORK_BUILD_PUGI ON)
set_as_internal(ORK_BUILD_YAML ON)

##Level 1
set_as_internal(ORK_BUILD_ORK ON)

add_subdirectory("${ORK_3P_SOURCE_DIR}/ork/build" "3p")

set(VERSION "${ORK_VERSION_STR}")

# Catch is not up to the challenge of Wall
replace_or_append_all_flag_compilers("/W([0-4]|all)" "IMPOSSIBLE" "/W4" "")


set(ORK_TEST_SRCS
	"${ORK_3P_SOURCE_DIR}/ork/src/test/test_main.cpp"
)
source_group("src" FILES ${ORK_TEST_SRCS})


set_advanced_warnings()
set_vector_architecture()

link_directories("${3P_BOOST_LIB_DIR}") #Must be before target
add_executable(ork_test ${ORK_TEST_SRCS})

target_link_libraries(ork_test PRIVATE ork)


install_exe(ork_test)
