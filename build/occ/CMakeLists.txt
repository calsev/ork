
project(occ)


if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()
if(ORK_BUILD_TBB)
	add_definitions(-DHAVE_TBB)
	add_definitions(-D__TBB_NO_IMPLICIT_LINKAGE)
	add_definitions(-D__TBBMALLOC_NO_IMPLICIT_LINKAGE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNo_Exception")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNo_Exception")

#Not sure why this is done
string(REGEX REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/D_WINDOWS" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/DWIN32" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-DDEBUG" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REGEX REPLACE "-DDEBUG" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")

#Generate a response file for all object files to avoid Windows CL character limits
SET(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
SET(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)


include("${3P_OCC_SRC_DIR}/adm/cmake/occt_macros.cmake)

# get current OCCT version
OCC_VERSION(OCC_VERSION_MAJOR OCC_VERSION_MINOR OCC_VERSION_MAINTENANCE OCC_VERSION_DEVELOPMENT OCC_VERSION_STRING_EXT)

set_property(GLOBAL PROPERTY OCC_VERSION_MAJOR ${OCC_VERSION_MAJOR})
set_property(GLOBAL PROPERTY OCC_VERSION_MINOR ${OCC_VERSION_MINOR})
set_property(GLOBAL PROPERTY OCC_VERSION_MAINTENANCE ${OCC_VERSION_MAINTENANCE})

set(BUILD_WITH_DEBUG OFF)#Enables extended messages
if(BUILD_WITH_DEBUG)
	add_definitions(-DOCCT_DEBUG)
else()
	
endif()

#list ALL_OCC_MODULES will contain all modules
#list ${module_name}_TOOLKITS will contain all toolkits used by that module
file_2_strings("${3P_OCC_SRC_DIR}/adm/MODULES" FILE_CONTENT)
foreach(curr_line ${FILE_CONTENT})
	separate_arguments(curr_line)
	
	#The first line is the module itself
	list(GET curr_line 0 module_name)
	list(REMOVE_AT curr_line 0)
	
	list(APPEND ALL_OCC_MODULES "${module_name}")
	set("${module_name}_TOOLKITS" "${curr_line}")
endforeach()

#Now build the geometry kernel only
macro(build_module module_name_)
	list(FIND "${module_name_}" ALL_OCC_MODULES module_index)
	if(NOT ${module_index} GREATER -1)
		message(FATAL_ERROR "Module not found: ${module_name_}")
	endif()
	set_as_internal(BUILD_MODULE_${module_name_} ON)
endmacro()
build_module("FoundationClasses")
build_module("ModelingData")
build_module("ModelingAlgorithms")
build_module("ApplicationFramework")
build_module("DataExchange")

#Aggregate all used toolkits
foreach(module_name ${ALL_OCC_MODULES})
	if(BUILD_MODULE_${module_name})
		list(APPEND BUILD_TOOLKITS "${${module_name}_TOOLKITS}")
	endif()
endforeach()
list(REMOVE_DUPLICATES BUILD_TOOLKITS)

#src/${kit_name_}/EXTERNLIB will exist if the toolkit has dependencies, and will list these one per line
#CSF_ variables are ignored
function(get_local_toolkit_deps kit_name_ var_)
	if_file_2_strings("${3P_OCC_SRC_DIR}/src/${kit_name_}/EXTERNLIB" file_lines_)

	set(_local_deps)
	foreach(curr_line_ ${file_lines_})
		string(REGEX MATCH "^TK" is_tk_ "${curr_line_}")
		if(NOT "${is_tk_}" STREQUAL "")
			list(APPEND _local_deps "${curr_line_}")
		endif()
	endforeach()

	set(${var_} "${_local_deps}" PARENT_SCOPE)
endfunction()

function(get_toolkit_deps kit_name_ var_)
	#_all_deps is initialized with first level dependencies
	get_local_toolkit_deps("${kit_name_}" _all_deps)
	list(LENGTH _all_deps _deps_len)
	
	set(_list_index 0)
	while(_list_index LESS _deps_len)
	list(GET _all_deps "${_list_index}" _curr_toolkit)
		get_local_toolkit_deps("${_curr_toolkit}" _curr_toolkit_deps)
		
		#Append any new toolkits
		foreach(_curr_dep ${_curr_toolkit_deps})
			list(FIND "${_curr_dep}" _all_deps _curr_dep_index)
			if("${_curr_dep_index}" GREATER -1)
				list(APPEND _all_deps "${_curr_dep}")
			endif()
		endforeach()
		
		#Update loop vars
		MATH(EXPR _list_index "${_list_index}+1")
		list(LENGTH _all_deps _deps_len)
	endwhile()

	set(${var_} "${_all_deps}" PARENT_SCOPE)
endfunction()


set(RAW_BUILD_TOOLKITS)
foreach(toolkit ${BUILD_TOOLKITS})
	get_toolkit_deps(${toolkit} deps)
	list(APPEND RAW_BUILD_TOOLKITS "${toolkit}" "${deps}")
endforeach()
list(REMOVE_DUPLICATES RAW_BUILD_TOOLKITS)
set(BUILD_TOOLKITS "${RAW_BUILD_TOOLKITS}")



enable_structured_exceptions()

list(REMOVE_ITEM BUILD_TOOLKITS TKIVtk)
list(REMOVE_ITEM BUILD_TOOLKITS TKIVtkDraw)


include_directories(${CMAKE_BINARY_DIR}/inc)

# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES("${CMAKE_BINARY_DIR}" "${BUILD_TOOLKITS}")


foreach(toolkit ${BUILD_TOOLKITS})
	if(EXISTS "${3P_OCC_SRC_DIR}/src/${toolkit}/CMakeLists.txt")
		add_subdirectory("${3P_OCC_SRC_DIR}/src/${toolkit}")
	endif()
endforeach()


if(ORK_BUILD_TBB)
	target_link_libraries(occ PRIVATE tbb)
	if(ORK_BUILD_TBB_MALLOC)
		target_link_libraries(occ PRIVATE tbbmalloc)
	endif()
endif()


install_file_or_directory("${3P_OCC_SRC_DIR}/LICENSE_LGPL_21.txt" "${ORK_INSTALL_PREFIX}")
install_file_or_directory("${3P_OCC_SRC_DIR}/OCCT_LGPL_EXCEPTION.txt" "${ORK_INSTALL_PREFIX}")
