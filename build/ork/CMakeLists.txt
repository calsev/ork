
project(ork CXX)

file(READ "${3P_ORK_SRC_DIR}/ork/ork.hpp" ORK_ORK_H)

string(
	REGEX MATCH
	"#define ORK_VERSION_MAJOR [0-9]+"
	ORK_VERSION_MAJOR "${ORK_ORK_H}"
)
string(
	REGEX MATCH
	"#define ORK_VERSION_MINOR [0-9]+"
	ORK_VERSION_MINOR "${ORK_ORK_H}"
)
string(
	REGEX MATCH
	"#define ORK_VERSION_PATCH [0-9]+"
	ORK_VERSION_PATCH "${ORK_ORK_H}"
)

string(
	REGEX REPLACE
	"#define ORK_VERSION_MAJOR ([0-9]+)" "\\1"
	ORK_VERSION_MAJOR "${ORK_VERSION_MAJOR}"
)
string(
	REGEX REPLACE
	"#define ORK_VERSION_MINOR ([0-9]+)" "\\1"
	ORK_VERSION_MINOR "${ORK_VERSION_MINOR}"
)
string(
	REGEX REPLACE
	"#define ORK_VERSION_PATCH ([0-9]+)" "\\1"
	ORK_VERSION_PATCH "${ORK_VERSION_PATCH}"
)

set(ORK_VERSION_STR "${ORK_VERSION_MAJOR}.${ORK_VERSION_MINOR}.${ORK_VERSION_PATCH}")
set(VERSION "${ORK_VERSION_STR}")


detect_option_flag(ORK_BUILD_SHARED "Building shared library instead of static" ON)
detect_option_flag(ORK_AUTO_LINK "Enable auto-linking in Visual Studio" ON)
detect_option_flag(ORK_AUTO_LINK_3P "If auto-linking, also auto-link any built dependencies" ON)

detect_option_flag(ORK_USE_BOOST "Enable Boost usage" OFF)
detect_option_flag(ORK_USE_GLM "Enable GLM usage" OFF)
detect_option_flag(ORK_USE_JSON "Enable JsonCPP usage" OFF)
detect_option_flag(ORK_USE_PUGI "Enable Pugi XML usage" OFF)
detect_option_flag(ORK_USE_YAML "Enable yaml-cpp usage" OFF)
if(ORK_USE_BOOST AND NOT ORK_BUILD_BOOST)
	message(FATAL_ERROR "Ork using Boost but Boost is not being built")
endif()
if(ORK_USE_GLM AND NOT ORK_BUILD_GLM)
	message(FATAL_ERROR "Ork using GLM but GLM is not being built")
endif()
if(ORK_USE_JSON AND NOT ORK_BUILD_JSON)
	message(FATAL_ERROR "Ork using JSON but JSON is not being built")
endif()
if(ORK_USE_PUGI AND NOT ORK_BUILD_PUGI)
	message(FATAL_ERROR "Ork using Pugi but Pugi is not being built")
endif()
if(ORK_USE_YAML AND NOT ORK_BUILD_YAML)
	message(FATAL_ERROR "Ork using YAML but YAML is not being built")
endif()


set_advanced_warnings()
set_vector_architecture()
disable_language_extensions(OFF)
add_definitions(-DORK_BUILD_ORK) #Enable exports, disable self-linking


#Configure header, done with regex to avoid a separate 'in' header
file(READ "${3P_ORK_SRC_DIR}/ork/config/config.hpp" ORK_CONFIG_H)
if(NOT ORK_BUILD_SHARED)
	string(
		REGEX REPLACE
		"(ORK_ORK_DLL) 1" "\\1 0"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
	set(ORK_LINK_MODE "STATIC")
else()
	set(ORK_LINK_MODE "SHARED")
endif()
if(NOT ORK_AUTO_LINK)
	string(
		REGEX REPLACE
		"(ORK_AUTO_LINK) 1" "\\1 0"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(NOT ORK_AUTO_LINK_3P)
	string(
		REGEX REPLACE
		"(ORK_AUTO_LINK_3P) 1" "\\1 0"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(ORK_USE_BOOST)
	string(
		REGEX REPLACE
		"(ORK_USE_BOOST) 0" "\\1 1"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(ORK_USE_GLM)
	string(
		REGEX REPLACE
		"(ORK_USE_GLM) 0" "\\1 1"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(ORK_USE_JSON)
	string(
		REGEX REPLACE
		"(ORK_USE_JSON) 0" "\\1 1"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(ORK_USE_PUGI)
	string(
		REGEX REPLACE
		"(ORK_USE_PUGI) 0" "\\1 1"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
if(ORK_USE_YAML)
	string(
		REGEX REPLACE
		"(ORK_USE_YAML) 0" "\\1 1"
		ORK_CONFIG_H "${ORK_CONFIG_H}"
	)
endif()
file(WRITE "${3P_ORK_SRC_DIR}/version.txt" "${VERSION}\n")
file(WRITE "${3P_ORK_BIN_DIR}/ork/config/config.hpp" "${ORK_CONFIG_H}")


set(ORK_CONFIG_HDRS
	"${3P_ORK_BIN_DIR}/ork/config/config.hpp"
)
set(ORK_CORE_HDRS
	"${3P_ORK_SRC_DIR}/ork/core/boost_include.inl"
	"${3P_ORK_SRC_DIR}/ork/core/config_internal.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/exception.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/file_system.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/includes.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/log.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/stl_include.inl"
	"${3P_ORK_SRC_DIR}/ork/core/text.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/tools.hpp"
	"${3P_ORK_SRC_DIR}/ork/core/tools_generated.hpp"
)
set(ORK_PUBLIC_HDRS
	"${3P_ORK_SRC_DIR}/ork/color.hpp"
	"${3P_ORK_SRC_DIR}/ork/command_line.hpp"
	"${3P_ORK_SRC_DIR}/ork/database.hpp"
	"${3P_ORK_SRC_DIR}/ork/distribution.hpp"
	"${3P_ORK_SRC_DIR}/ork/file_utils.hpp"
	"${3P_ORK_SRC_DIR}/ork/filter.hpp"
	"${3P_ORK_SRC_DIR}/ork/geometry.hpp"
	"${3P_ORK_SRC_DIR}/ork/glm.hpp"
	"${3P_ORK_SRC_DIR}/ork/html.hpp"
	"${3P_ORK_SRC_DIR}/ork/memory.hpp"
	"${3P_ORK_SRC_DIR}/ork/orientation.hpp"
	"${3P_ORK_SRC_DIR}/ork/ork.hpp"
	"${3P_ORK_SRC_DIR}/ork/parse.hpp"
	"${3P_ORK_SRC_DIR}/ork/string_utils.hpp"
	"${3P_ORK_SRC_DIR}/ork/tagger.hpp"
	"${3P_ORK_SRC_DIR}/ork/text_io.hpp"
	"${3P_ORK_SRC_DIR}/ork/text_serializable.hpp"
)
set(ORK_PRIVATE_HDRS
	"${3P_ORK_SRC_DIR}/ork/private/text_includes.hpp"
)
set(ORK_SRCS
	"${3P_ORK_SRC_DIR}/src/color.cpp"
	"${3P_ORK_SRC_DIR}/src/command_line.cpp"
	"${3P_ORK_SRC_DIR}/src/common.cpp"
	"${3P_ORK_SRC_DIR}/src/distribution.cpp"
	"${3P_ORK_SRC_DIR}/src/geometry.cpp"
	"${3P_ORK_SRC_DIR}/src/glm.cpp"
	"${3P_ORK_SRC_DIR}/src/html.cpp"
	"${3P_ORK_SRC_DIR}/src/log.cpp"
	"${3P_ORK_SRC_DIR}/src/orientation.cpp"
	"${3P_ORK_SRC_DIR}/src/string_utils.cpp"
	"${3P_ORK_SRC_DIR}/src/tagger.cpp"
	"${3P_ORK_SRC_DIR}/src/text_io.cpp"
	"${3P_ORK_SRC_DIR}/src/text_serializable.cpp"
)

source_group("config" FILES ${ORK_CONFIG_HDRS})
source_group("core" FILES ${ORK_CORE_HDRS})
source_group("ork" FILES ${ORK_PUBLIC_HDRS})
source_group("private" FILES ${ORK_PRIVATE_HDRS})
source_group("src" FILES ${ORK_SRCS})


if(ORK_USE_BOOST)
	link_directories("${3P_BOOST_LIB_DIR}") #Must be before target
endif()
add_library(ork ${ORK_LINK_MODE} ${ORK_CONFIG_HDRS} ${ORK_CORE_HDRS} ${ORK_PUBLIC_HDRS} ${ORK_PRIVATE_HDRS} ${ORK_SRCS})


if(ORK_USE_BOOST)
	target_link_libraries(ork PUBLIC boost)
endif()
if(ORK_USE_CATCH)
	target_link_libraries(ork PUBLIC catch)
endif()
if(ORK_USE_GLM)
	target_link_libraries(ork PUBLIC glm)
endif()
if(ORK_USE_JSON)
	target_link_libraries(ork PUBLIC jsoncpp)
endif()
if(ORK_USE_PUGI)
	target_link_libraries(ork PUBLIC pugixml)
endif()
if(ORK_USE_YAML)
	target_link_libraries(ork PUBLIC yamlcpp)
endif()


target_include_directories(
	ork
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_ORK_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


install_bin(ork)
install_header_dir("${3P_ORK_SRC_DIR}/ork" "${ORK_INSTALL_INC_DIR}/ork" "config")
install_headers("${3P_ORK_BIN_DIR}/ork/config/config.hpp" "${ORK_INSTALL_INC_DIR}/ork/config")

