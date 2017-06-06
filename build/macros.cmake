
if(ORK_MACROS_INCLUDED)
	return()
endif()
set(ORK_MACROS_INCLUDED 1)

####################
#Variable section
####################

macro(ensure_non_empty _var)
	if(NOT ${_var})
		message(FATAL_ERROR "Error: ${${_var}} not defined")
	endif()
endmacro()

macro(mark_as_internal _var)
	ensure_non_empty(${_var})
	mark_as_advanced(${_var})
	set(${_var} ${${_var}} CACHE INTERNAL "" FORCE)
endmacro()

macro(set_as_internal _var _val)
	mark_as_advanced(${_var})
	set(${_var} ${_val} CACHE INTERNAL "" FORCE)
endmacro()

macro(append_string_internal _var _str)
	ensure_non_empty(${_var})
	mark_as_advanced(${_var})
	if(NOT DEFINED ORK_CMAKE_RUN_ONCE)
		message(FATAL_ERROR "Error: ORK_CMAKE_RUN_ONCE not defined")
	elseif(${ORK_CMAKE_RUN_ONCE})
		set(${_var} "${${_var}} ${_str}" CACHE INTERNAL "" FORCE)
	endif()
endmacro()

macro(append_as_internal _var _val)
	ensure_non_empty(${_var})
	mark_as_advanced(${_var})
	if(NOT DEFINED ORK_CMAKE_RUN_ONCE)
		message(FATAL_ERROR "Error: ORK_CMAKE_RUN_ONCE not defined")
	elseif(${ORK_CMAKE_RUN_ONCE})
		set(${_var} "${${_var}} ${${_val}}" CACHE INTERNAL "" FORCE)
	endif()
endmacro()

macro(mark_empty_internal _var)
	if(${_var})
		message(FATAL_ERROR "Error: ${${_var}} defined")
	endif()
	mark_as_advanced(${_var})
	set(${_var} "" CACHE INTERNAL "" FORCE)
endmacro()

####################
#Option section
####################

macro(detect_option_flag _var _msg _val)
	if(DEFINED ${_var})
		option(${_var} "${_msg}" ${${_var}})
	else()
		option(${_var} "${_msg}" ${_val})
	endif()
endmacro()

macro(detect_option_path _var _msg _val)
	if(NOT DEFINED ${_var})
		set(${_var} ${_val} PATH "${_msg}")
	endif()
endmacro()

macro(detect_path _var _val1 _val2)
	if(EXISTS "${_val1}")
		set_as_internal(${_var} "${_val1}")
	elseif(EXISTS "${_val2}")
		set_as_internal(${_var} "${_val2}")
	else()
		message(FATAL_ERROR "Cannot find path ${_val1} or ${_val2}")
	endif()
endmacro()

macro(find_3p_path _build _var _val1 _val2)
	if(${_build})
		detect_path(${_var} "${ORK_3P_SOURCE_DIR}/${_val1}" "${ORK_3P_SOURCE_DIR}/${_val2}")
	endif()
endmacro()

####################
#Install section
####################

macro(parse_version _ver _prefix)
	set(VERSION_REGEX "[0-9]+\\.[0-9]+\\.[0-9]+")
	if(${_ver} MATCHES ${VERSION_REGEX})
		string(REGEX MATCHALL "[0-9]+|-([A-Za-z0-9_]+)" VERSION_PARTS ${_ver})
		list(GET VERSION_PARTS 0 ${_prefix}_MAJOR)
		list(GET VERSION_PARTS 1 ${_prefix}_MINOR)
		list(GET VERSION_PARTS 2 ${_prefix}_PATCH)
	else()
		message(FATAL_ERROR "Invalid version: ${${_ver}}")
	endif()
endmacro()

macro(install_bin _targ)
	if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
		install(
			TARGETS ${_targ}
			RUNTIME DESTINATION "${ORK_INSTALL_BIN_DIR}" COMPONENT Runtime
			ARCHIVE DESTINATION "${ORK_INSTALL_LIB_DIR}" COMPONENT Development
		)
		install(
			FILES $<TARGET_PDB_FILE:${_targ}> DESTINATION "${ORK_INSTALL_BIN_DIR}" COMPONENT Development OPTIONAL
		)
	endif()
endmacro()

macro(install_headers _head _dest)
	if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
		install(
			FILES ${_head}
			DESTINATION "${_dest}"
			COMPONENT Development
		)
	endif()
endmacro()

macro(install_header_dir _dir _dest _exclude)
	if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
		install(
			DIRECTORY "${_dir}/" #Note trailing slash
			DESTINATION "${_dest}"
			COMPONENT Development
			PATTERN "${_exclude}" EXCLUDE
		)
	endif()
endmacro()

macro(install_shared _targ _head)
	install_bin(${_targ})
	install_headers(${_head} "${ORK_INSTALL_INC_DIR}")
endmacro()

macro(install_file_or_directory _file_or_dir _dest)
	if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL)
		if(IS_DIRECTORY "${_file_or_dir}")
			install(
				DIRECTORY "${_file_or_dir}/" #Note trailing slash
				DESTINATION "${_dest}"
				COMPONENT Runtime
			)
		else()
			install(
				FILES "${_file_or_dir}"
				DESTINATION  "${_dest}"
				COMPONENT Runtime
			)
		endif()
	endif()
endmacro()

####################
#Compiler section
####################

macro(get_compiler_name _var)
	if(MSVC)
		if(MSVC11)
			set(_var vc11)
		elseif(MSVC12)
			set(_var vc12)
		elseif(MSVC14)
			set(_var vc14)
		endif()
	elseif(DEFINED CMAKE_COMPILER_IS_GNUCC)
		set(_var gcc)
	elseif(DEFINED CMAKE_COMPILER_IS_GNUCXX)
		set(_var gxx)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(_var clang)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		set(_var icc)
	else()
		message(FATAL_ERROR "Compiler not recognized")
	endif()
endmacro()

#MSVC exception logic
set(EH_STRINGS "EH{a|s}{c|r}?\-?")
macro(enable_structured_exceptions)
	string (REGEX MATCH "${EH_STRINGS}" HAS_EH_FLAG "${CMAKE_CXX_FLAGS}")
	if(HAS_EH_FLAG)
		string(REGEX REPLACE "${EH_STRINGS}" "EHa" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	elseif(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHa")
	endif()
endmacro()
macro(enable_synchronous_exceptions)
	string (REGEX MATCH "${EH_STRINGS}" HAS_EH_FLAG "${CMAKE_CXX_FLAGS}")
	if(HAS_EH_FLAG)
		string(REGEX REPLACE "${EH_STRINGS}" "EHs" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	elseif(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
	endif()
endmacro()
macro(enable_only_cxx_exceptions)
	string (REGEX MATCH "${EH_STRINGS}" HAS_EH_FLAG "${CMAKE_CXX_FLAGS}")
	if(HAS_EH_FLAG)
		string(REGEX REPLACE "${EH_STRINGS}" "EHsc" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	elseif(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	endif()
endmacro()

####################
#File section
####################

function(file_2_strings path_to_file_ var_)
	if(NOT EXISTS "${path_to_file_}")
		message(FATAL_ERROR "File does not exist: ${path_to_file_}")
	endif()
	if(IS_DIRECTORY "${path_to_file_}")
		message(FATAL_ERROR "Cannot convert directory to strings: ${path_to_file_}")
	endif()
	file(STRINGS "${path_to_file_}" _content)
	set(${var_} "${_content}" PARENT_SCOPE)
endfunction()

function(if_file_2_strings path_to_file_ var_)
	set(_content)
	if(EXISTS "${path_to_file_}")
		file(STRINGS "${path_to_file_}" _content)
	endif()
	set(${var_} "${_content}" PARENT_SCOPE)
endfunction()
