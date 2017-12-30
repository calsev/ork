
project(ork_test CXX)

set(VERSION "${ORK_VERSION_STR}")


get_compiler_name(COMPILER)
if(COMPILER MATCHES "vc")
	if(CMAKE_CXX_FLAGS MATCHES "/W([0-4]|all)")
		string(REGEX REPLACE "/W([0-4]|all)" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
elseif(COMPILER MATCHES "gcc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
	message(FATAL_ERROR "Compiler not recognized: ${COMPILER}")
endif()


set(ORK_TEST_SRCS
	"${3P_ORK_SRC_DIR}/src/test/test_main.cpp"
)

link_directories("${3P_BOOST_LIB_DIR}") #Must be before target
add_executable(ork_test ${ORK_TEST_SRCS})

target_link_libraries(ork_test PUBLIC ork)
if(ORK_USE_JSON)
	target_link_libraries(ork_test PUBLIC jsoncpp)
endif()
if(ORK_USE_PUGI)
	target_link_libraries(ork_test PUBLIC pugixml)
endif()

target_include_directories(
	ork_test
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
	PUBLIC "${3P_BOOST_SRC_DIR}"
	PUBLIC "${3P_CATCH_SRC_DIR}/single_include"
)
if(ORK_USE_GLM)
	target_include_directories(
		ork_test
		PUBLIC "${3P_GLM_SRC_DIR}"
	)
endif()


#We do not install test executable
