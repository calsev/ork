
project(ork_test CXX)

set(VERSION "${ORK_VERSION_STR}")


get_compiler_name(COMPILER)
if(COMPILER MATCHES "vc")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4623") #default constructor was implicitly defined as deleted (catch)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4625") #copy constructor was implicitly defined as deleted (catch)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4626") #assignment operator was implicitly defined as deleted (catch)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4628") #digraphs not supported with -Ze (catch)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5026") #move constructor was implicitly defined as deleted (catch)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5027") #move assignment operator was implicitly defined as deleted (catch)
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
	PUBLIC "${3P_CATCH_SRC_DIR}/include"
)
if(ORK_USE_GLM)
	target_include_directories(
		ork_test
		PUBLIC "${3P_GLM_SRC_DIR}"
	)
endif()


#We do not install test executable
