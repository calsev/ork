
project(ork CXX)

set(VERSION "0.0.0") #No formal release yet

detect_option_flag(ORK_BUILD_SHARED "Building shared library instead of static" ON)
detect_option_flag(ORK_AUTO_LINK "Enable auto-linking in Visual Studio" ON)
detect_option_flag(ORK_AUTO_LINK_3P "If auto-linking, also auto-link any built dependencies" ON)
detect_option_flag(ORK_USE_GLM "Enable GLM usage" ON)
detect_option_flag(ORK_USE_JSON "Enable JsonCPP usage" OFF)
detect_option_flag(ORK_USE_PUGI "Enable Pugi XML usage" ON)
if(ORK_USE_GLM AND NOT ORK_BUILD_GLM)
	message(FATAL_ERROR "Ork using GLM but GLM is not being built")
endif()
if(ORK_USE_JSON AND NOT ORK_BUILD_JSON)
	message(FATAL_ERROR "Ork using JSON but JSON is not being built")
endif()
if(ORK_USE_PUGI AND NOT ORK_BUILD_PUGI)
	message(FATAL_ERROR "Ork using Pugi but Pugi is not being built")
endif()


add_definitions(-DORK_BUILD_ORK) #Enable exports, disable self-linking


#Configure header, done with regex to avoid a separate 'in' header
file(READ "${3P_ORK_SRC_DIR}/ork/config/config.hpp" ORK_CONFIG_H)
if(NOT ORK_BUILD_SHARED)
	string(
		REGEX REPLACE
		"(#	define ORK_ORK_DLL) 1" "\\1 0"
	)
	set(ORK_LINK_MODE "STATIC")
else()
	set(ORK_LINK_MODE "SHARED")
endif()
if(NOT ORK_AUTO_LINK)
	string(
		REGEX REPLACE
		"(#	define ORK_AUTO_LINK) 1" "\\1 0"
	)
endif()
if(NOT ORK_AUTO_LINK_3P)
	string(
		REGEX REPLACE
		"(#	define ORK_AUTO_LINK_3P) 1" "\\1 0"
	)
endif()
if(NOT ORK_USE_GLM)
	string(
		REGEX REPLACE
		"(#	define ORK_USE_GLM) 1" "\\1 0"
	)
endif()
if(ORK_USE_JSON)
	string(
		REGEX REPLACE
		"(#	define ORK_USE_JSON) 0" "\\1 1"
	)
endif()
if(NOT ORK_USE_PUGI)
	string(
		REGEX REPLACE
		"(#	define ORK_USE_PUGI) 1" "\\1 0"
	)
endif()
file(WRITE "${3P_ORK_BIN_DIR}/ork/config/config.hpp" "${ORK_CONFIG_H}")


file(GLOB ORK_PUBLIC_HDRS
	"${3P_ORK_SRC_DIR}/ork/*.hpp"
	"${3P_ORK_SRC_DIR}/ork/detail/*.hpp"
	"${3P_ORK_BIN_DIR}/ork/config/config.hpp"
)
file(GLOB ORK_PRIVATE_HDRS
	
)
file(GLOB ORK_SRCS
	"${3P_ORK_SRC_DIR}/src/*.cpp"
)


link_directories("${3P_BOOST_LIB_DIR}") #Must be before target
add_library(ork ${ORK_LINK_MODE} ${ORK_SRCS} ${ORK_PUBLIC_HDRS} ${ORK_PRIVATE_HDRS})

if(ORK_USE_JSON)
	target_link_libraries(ork PUBLIC jsoncpp)
endif()
if(ORK_USE_PUGI)
	target_link_libraries(ork PUBLIC pugixml)
endif()


target_include_directories(
	ork
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
	PUBLIC "${3P_GLM_SRC_DIR}"
	PUBLIC "${3P_BOOST_SRC_DIR}"
)

install_bin(ork)
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
	install(
		DIRECTORY "${3P_ORK_SRC_DIR}/ork/" #Note trailing slash
		DESTINATION "${INSTALL_INC_DIR}/ork"
		PATTERN "config" EXCLUDE
	)
	install(
		FILES "${3P_ORK_BIN_DIR}/ork/config/config.hpp"
		DESTINATION "${INSTALL_INC_DIR}/ork/config"
	)
endif()
