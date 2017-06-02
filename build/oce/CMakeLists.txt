
project(oce)

add_definitions(-D_OCC64)
if(ORK_BUILD_TBB)
else()
	add_definitions(-DIGNORE_NO_ATOMICS)
endif()
add_definitions(-DOCE_DISABLE_TKSERVICE_FONT)#AIS_Dimension.cxx uses this flag to remove font-dependent code
add_definitions(-DHAVE_OCE_PATHS_H)
add_definitions(-DHAVE_CONFIG_H)
if(MSVC)
	add_definitions(-DWNT -DCSFDB)
	add_definitions("-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4800") #Coercion to bool
endif()
set(OCE_ENABLE_DEB_FLAG OFF)#Debug builds link debug OCC?
if(OCE_ENABLE_DEB_FLAG)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} OCCT_DEBUG=1")
endif()


set(OCE_BIN_BUILD_INC_DIR "${3P_OCE_BIN_DIR}/build_inc")
set(OCE_BIN_INSTALL_INC_DIR "${3P_OCE_BIN_DIR}/install_inc")
set(OCE_INSTALL_INC_DIR "${INSTALL_INC_DIR}/oce")


#Header used only to build OCE
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_build_config.h.cmake"
	"${OCE_BIN_BUILD_INC_DIR}/oce-config.h"
)
#Header installed on the system
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_install_config.h.cmake"
	"${OCE_BIN_INSTALL_INC_DIR}/oce-config.h"
)
#installation paths are written into a separate file (which is not installed)
#so that only few files have to be recompiled when installation paths are modified
configure_file(
	"${3P_OCE_SRC_DIR}/adm/cmake/config/oce_build_paths.h.cmake"
	"${OCE_BIN_INSTALL_INC_DIR}/oce-paths.h"
)


set(OCE_CONFIG_HDRS
	"${OCE_BIN_INSTALL_INC_DIR}/oce-config.h"
)
file(GLOB OCE_PUBLIC_HDRS
	"${OCE_CONFIG_HDRS}"
	#${3P_OCE_SRC_DIR}/inc/*.h
	#${3P_OCE_SRC_DIR}/inc/*.hxx
	#${3P_OCE_SRC_DIR}/inc/*.gxx
	#${3P_OCE_SRC_DIR}/inc/*.lxx
)
set(OCE_PRIVATE_HDRS)
set(OCE_SRCS)

set(OCE_MODULES
	
)

set(OCE_INCLUDE_DIRECTORIES "${OCE_BIN_BUILD_INC_DIR} ${OCE_BIN_INSTALL_INC_DIR} ${3P_OCE_SRC_DIR}/inc")#Order matters for config header
foreach(module ${OCE_MODULES})
	if(NOT IS_DIRECTORY "${3P_OCE_SRC_DIR}/src/${module}")
		message(FATAL_ERROR "Source directory does not exist: ${3P_OCE_SRC_DIR}/src/${module}")
	endif()
	file(GLOB module_srcs
		"${3P_OCE_SRC_DIR}/src/${module}/*.cxx"
		"${3P_OCE_SRC_DIR}/src/${module}/*.c"
		"${3P_OCE_SRC_DIR}/drv/${module}/*.cxx"
		"${3P_OCE_SRC_DIR}/drv/${module}/*.c"
	)
	file(GLOB module_hdrs
		"${3P_OCE_SRC_DIR}/src/${module}/*.h"
		"${3P_OCE_SRC_DIR}/src/${module}/*.hxx"
		"${3P_OCE_SRC_DIR}/src/${module}/*.gxx"
		"${3P_OCE_SRC_DIR}/src/${module}/*.lxx"
	)
	source_group("${module}" FILES "${module_srcs}")
	
	set(OCE_PUBLIC_HDRS "${OCE_PUBLIC_HDRS} ${module_hdrs}")
	set(OCE_SRCS "${OCE_SRCS} ${module_srcs}")
	set(OCE_INCLUDE_DIRECTORIES "${OCE_INCLUDE_DIRECTORIES} ${3P_OCE_SRC_DIR}/src/${module}")
	if(IS_DIRECTORY "${3P_OCE_SRC_DIR}/drv/${module}")
	    set(OCE_INCLUDE_DIRECTORIES "${OCE_INCLUDE_DIRECTORIES} ${3P_OCE_SRC_DIR}/drv/${module}")
	endif()
	
	add_definitions("-D__${module}_DLL")
endforeach()


add_library(oce SHARED ${OCE_PUBLIC_HDRS} ${OCE_PRIVATE_HDRS} ${OCE_SRCS})


#set_property(TARGET oce APPEND PROPERTY INCLUDE_DIRECTORIES "${OCE_INCLUDE_DIRECTORIES}")
target_include_directories(
	oce
	PUBLIC $<BUILD_INTERFACE:${OCE_INCLUDE_DIRECTORIES}>
	INTERFACE $<INSTALL_INTERFACE:${OCE_INSTALL_INC_DIR}>
)


if(ORK_BUILD_TBB)
	target_link_libraries(oce PRIVATE tbb)
	if(ORK_BUILD_TBB_MALLOC)
		target_link_libraries(oce PRIVATE tbbmalloc)
	endif()
endif()


install_bin(oce)
install_headers("${OCE_BIN_INSTALL_INC_DIR}/oce-config.h" "${OCE_INSTALL_INC_DIR}")
install_headers("${OCE_PUBLIC_HDRS}" "${OCE_INSTALL_INC_DIR}")
