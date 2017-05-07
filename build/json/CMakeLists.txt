
project(jsoncpp CXX)

file(READ "${3P_JSON_SRC_DIR}/version" VERSION)


add_definitions(-DJSON_DLL_BUILD)
set(JSONCPP_USE_SECURE_MEMORY "0")
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275") #DLL Export class inherits from non-DLL class (std::exception)
endif()

macro(parse_version _ver _prefix)
	set(VERSION_REGEX "[0-9]+\\.[0-9]+\\.[0-9]+(-[a-zA-Z0-9_]+)?")
	if(${_ver} MATCHES ${VERSION_REGEX})
		string(REGEX MATCHALL "[0-9]+|-([A-Za-z0-9_]+)" VERSION_PARTS ${_ver})
		list(GET VERSION_PARTS 0 ${_prefix}_MAJOR)
		list(GET VERSION_PARTS 1 ${_prefix}_MINOR)
		list(GET VERSION_PARTS 2 ${_prefix}_PATCH)
	else()
		message(FATAL_ERROR "Invalid version: ${${_ver}}")
	endif()
endmacro()

parse_version(${VERSION} "JSONCPP_VERSION")

configure_file(
	"${3P_JSON_SRC_DIR}/src/lib_json/version.h.in"
	"${3P_JSON_BIN_DIR}/include/json/version.h" @ONLY
)


set(JSON_INC_DIR "${3P_JSON_SRC_DIR}/include/json")
set(JSON_PUBLIC_HDRS #For build
	"${JSON_INC_DIR}/assertions.h"
	"${JSON_INC_DIR}/config.h"
	"${JSON_INC_DIR}/features.h"
	"${JSON_INC_DIR}/forwards.h"
	"${JSON_INC_DIR}/reader.h"
	"${JSON_INC_DIR}/value.h"
	"${JSON_INC_DIR}/writer.h"
	"${3P_JSON_BIN_DIR}/include/json/version.h"
)
file(GLOB JSON_PRIVATE_HDRS
	
)
set(JSON_SRC_DIR "${3P_JSON_SRC_DIR}/src/lib_json")
set(JSON_SRCS
	"${JSON_SRC_DIR}/json_tool.h"
	"${JSON_SRC_DIR}/json_reader.cpp"
	"${JSON_SRC_DIR}/json_valueiterator.inl"
	"${JSON_SRC_DIR}/json_value.cpp"
	"${JSON_SRC_DIR}/json_writer.cpp"
	"${JSON_SRC_DIR}/version.h.in"
)


add_library(jsoncpp SHARED ${JSON_SRCS} ${JSON_PUBLIC_HDRS} ${JSON_PRIVATE_HDRS})

target_include_directories(
	jsoncpp
	PUBLIC $<BUILD_INTERFACE:${3P_JSON_BIN_DIR}/include>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_JSON_SRC_DIR}/include>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)


install_bin(jsoncpp)
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
	install(
		DIRECTORY "${3P_JSON_SRC_DIR}/include/json/" #Note trailing slash
		DESTINATION "${INSTALL_INC_DIR}/json"
		PATTERN "version.h" EXCLUDE
	)
	install(
		FILES "${3P_JSON_BIN_DIR}/include/json/version.h"
		DESTINATION "${INSTALL_INC_DIR}/json"
	)
endif()
