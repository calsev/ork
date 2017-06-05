
project(pugixml)

set(VERSION "1.8.1")


add_definitions(-DPUGIXML_BUILD_DLL)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275") #DLL Export class inherits from non-DLL class (std::exception)
endif()


#Some build systems (e.g. MSVC) always search the directory containing the file containing the #include statement first.
#Because the default configuration header is in the same directory as the header that includes it,
#it will always be found before the generated header.
#This is typically a problem only during build, as the default config header will not be installed.
#We rename the config header in the source tree to avoid this.
#We then rename the header back post-build to keep the process clean under source control.
file(READ "${3P_PUGI_SRC_DIR}/src/pugiconfig.hpp" PUGI_CONFIG_H)
#Why are these not defined!?
string(
	REGEX REPLACE
	"(#define HEADER_PUGICONFIG_HPP)"
"\\1\n\n\
#if PUGIXML_BUILD_DLL\n\
#	define PUGIXML_API __declspec(dllexport)//Export public symbols\n\
#else\n\
#	define PUGIXML_API __declspec(dllimport)//Import public symbols\n\
#endif\n\
"
	PUGI_CONFIG_H "${PUGI_CONFIG_H}"
)
file(WRITE "${3P_PUGI_BIN_DIR}/pugiconfig.hpp" "${PUGI_CONFIG_H}")


set(PUGI_PUBLIC_HDRS
	"${3P_PUGI_SRC_DIR}/src/pugixml.hpp"
	"${3P_PUGI_BIN_DIR}/pugiconfig.hpp"
)
set(PUGI_PRIVATE_HDRS
)
set(PUGI_SRCS
	"${3P_PUGI_SRC_DIR}/src/pugixml.cpp"
)


add_library(pugixml SHARED ${PUGI_SRCS} ${PUGI_PUBLIC_HDRS} ${PUGI_PRIVATE_HDRS})


add_custom_command( #Rename the default config header
	TARGET pugixml PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	"${3P_PUGI_SRC_DIR}/src/pugiconfig.hpp" "${3P_PUGI_SRC_DIR}/src/pugiconfig.included.hpp")

add_custom_command( #Rename the default config header back
	TARGET pugixml POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	"${3P_PUGI_SRC_DIR}/src/pugiconfig.included.hpp" "${3P_PUGI_SRC_DIR}/src/pugiconfig.hpp")


target_include_directories(
	pugixml
	PUBLIC $<BUILD_INTERFACE:${3P_PUGI_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_PUGI_SRC_DIR}/src>
	INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)

install_bin(pugixml)
install_headers("${3P_PUGI_SRC_DIR}/src/pugixml.hpp" "${ORK_INSTALL_INC_DIR}")
install_headers("${3P_PUGI_BIN_DIR}/pugiconfig.hpp" "${ORK_INSTALL_INC_DIR}")
