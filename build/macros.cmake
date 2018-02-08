
if(ORK_MACROS_INCLUDED)
	return()
endif()
set(ORK_MACROS_INCLUDED 1)

####################
#Basic compiler identification
####################

macro(get_compiler_name _var)
	if(MSVC)
		if(MSVC11)
			set(${_var} "vc11")
		elseif(MSVC12)
			set(${_var} "vc12")
		elseif(MSVC14)
			set(${_var} "vc14")
		elseif(MSVC15)
			set(${_var} "vc15")
		endif()
	elseif(DEFINED CMAKE_COMPILER_IS_GNUCC)
		set(${_var} "gcc")
	elseif(DEFINED CMAKE_COMPILER_IS_GNUCXX)
		set(${_var} "gxx")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(${_var} "clang")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		set(${_var} "icc")
	else()
		message(FATAL_ERROR "Compiler not recognized")
	endif()
endmacro()

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
	set(${_var} "${${_var}}" CACHE INTERNAL "" FORCE)
endmacro()

macro(set_as_internal _var _val)
	mark_as_advanced(${_var})
	set(${_var} "${_val}" CACHE INTERNAL "" FORCE)
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
#Global section
####################

macro(ork_pre_project_config)
	set(CMAKE_SUPPRESS_REGENERATION OFF) #Projects can be run on machines without CMake installed
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
	set(ORK_USE_SOLUTION_FOLDERS ON)


	if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	  message(FATAL_ERROR "Prevented in-tree built. Call cmake from outside source tree.")
	endif()


	if(NOT ${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		message(FATAL_ERROR "Error: Only x64 targets are supported")
	endif()


	#Used for concatenating to avoid repeats
	if(NOT ORK_CMAKE_RAN_INTERNAL)
		set(ORK_CMAKE_RUN_ONCE ON)
	else()
		set(ORK_CMAKE_RUN_ONCE OFF)
	endif()
	set_as_internal(ORK_CMAKE_RAN_INTERNAL ON)


	enable_language(CXX)
	enable_language(C)
	enable_language(ASM_MASM)
	#Must be set before PROJECT to hide
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
	mark_as_internal(CMAKE_CONFIGURATION_TYPES)
	
	set(ORK_PRE_PROJECT_CONFIGURED ON)
endmacro()

####################
#Option section
####################

macro(detect_option_flag _var _msg _val)
	if(DEFINED ${_var})
		option(${_var} "${_msg}" "${${_var}}")
	else()
		option(${_var} "${_msg}" "${_val}")
	endif()
endmacro()

macro(detect_option _var _msg _val)
	if(NOT DEFINED ${_var})
		set(${_var} "${_val}" CACHE STRING "${_msg}")
	endif()
endmacro()

macro(detect_option_path _var _msg _val)
	if(NOT DEFINED ${_var})
		set(${_var} "${_val}" CACHE PATH "${_msg}")
	endif()
endmacro()

macro(detect_path _var _val1 _val2)
	if(EXISTS "${_val1}")
		set(${_var} "${_val1}")
	elseif(EXISTS "${_val2}")
		set(${_var} "${_val2}")
	else()
		message(FATAL_ERROR "Cannot find path ${_val1} or ${_val2}")
	endif()
endmacro()

macro(find_3p_path _build _var _val1 _val2)
	if(NOT ${_build})
		# Do nothing
	elseif(${_var})
		# Allow override
	else()
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

macro(install_exe _targ)
	if(NOT SKIP_INSTALL_ALL)
		install(
			TARGETS ${_targ}
			RUNTIME DESTINATION "${ORK_INSTALL_BIN_DIR}" COMPONENT Runtime
		)
	endif()
endmacro()

macro(install_bin _targ)
	if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL)
		install(
			TARGETS ${_targ}
			ARCHIVE DESTINATION "${ORK_INSTALL_LIB_DIR}" COMPONENT Development
		)
		install(
			FILES $<TARGET_PDB_FILE:${_targ}> DESTINATION "${ORK_INSTALL_BIN_DIR}" COMPONENT Development OPTIONAL
		)
	endif()
	if(NOT SKIP_INSTALL_RUNTIME AND NOT SKIP_INSTALL_ALL)
		install(
			TARGETS ${_targ}
			RUNTIME DESTINATION "${ORK_INSTALL_BIN_DIR}" COMPONENT Runtime
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

macro(ork_install_config)
	if(DEFINED ORK_INSTALL_PREFIX AND ORK_INSTALL_PREFIX) #We set this previously
		file(TO_CMAKE_PATH "${ORK_INSTALL_PREFIX}" ORK_INSTALL_PREFIX)
	elseif(DEFINED CMAKE_INSTALL_PREFIX AND CMAKE_INSTALL_PREFIX)
		detect_option_path(ORK_INSTALL_PREFIX "Path of install directory" "${CMAKE_INSTALL_PREFIX}")
	else()
		detect_option_path(ORK_INSTALL_PREFIX "Path of install directory" "${CMAKE_SOURCE_DIR}/../bin")
	endif()
	set_as_internal(CMAKE_INSTALL_PREFIX "${ORK_INSTALL_PREFIX}")

	detect_option(ORK_INSTALL_BIN_SUFFIX "Subdirectory for installation of executables and shared libraries" bin)
	detect_option(ORK_INSTALL_INC_SUFFIX "Subdirectory for installation of headers" inc)
	detect_option(ORK_INSTALL_LIB_SUFFIX "Subdirectory for installation of static libraries and archives" lib)
	set(ORK_INSTALL_BIN_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_BIN_SUFFIX}")
	set(ORK_INSTALL_INC_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_INC_SUFFIX}")
	set(ORK_INSTALL_LIB_DIR "${ORK_INSTALL_PREFIX}/${ORK_INSTALL_LIB_SUFFIX}")


	#Build output, not that it is usually used without installing
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${ORK_INSTALL_LIB_SUFFIX}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${ORK_INSTALL_BIN_SUFFIX}")

	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${ORK_INSTALL_LIB_SUFFIX}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${ORK_INSTALL_BIN_SUFFIX}")
	
	set(ORK_INSTALL_CONFIGURED ON)
endmacro()

####################
#Compiler section
####################

macro(append_all_compiler_flag _flag)
	if(NOT ${_flag} MATCHES "^(/|-)")
		message(FATAL_ERROR "All MSVC and GCC flag start with a slash or dash: ${_flag}")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_flag}")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_flag}")
endmacro()

macro(append_debug_flag _flag)
	if(NOT ${_flag} MATCHES "^(/|-)")
		message(FATAL_ERROR "All MSVC and GCC flag start with a slash or dash: ${_flag}")
	endif()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${_flag}")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${_flag}")
endmacro()

macro(append_release_flag _flag)
	if(NOT ${_flag} MATCHES "^(/|-)")
		message(FATAL_ERROR "All MSVC and GCC flag start with a slash or dash: ${_flag}")
	endif()
	set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${_flag}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${_flag}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${_flag}")
	set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${_flag}")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${_flag}")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${_flag}")
endmacro()

macro(replace_or_append_compiler_flag _flags_var _regex_str _flag_str)
	if(NOT ${_flag_str} MATCHES "^(/|-)")
		message(FATAL_ERROR "All MSVC and GCC flag start with a slash or dash: ${_flag_str}")
	endif()
	if(_regex_str MATCHES "IMPOSIBLE")
		#Useful for one-line compiler-specific options
	elseif(${_flags_var} MATCHES ${_regex_str})
		string(REGEX REPLACE ${_regex_str} ${_flag_str} ${_flags_var} "${${_flags_var}}")
	else()
		set(${_flags_var} "${${_flags_var}} ${_flag_str}")
	endif()
endmacro()

macro(replace_or_append_debug_flag _regex_str _flag_str)
	replace_or_append_compiler_flag(CMAKE_CXX_FLAGS_DEBUG ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_C_FLAGS_DEBUG ${_regex_str} ${_flag_str})
endmacro()

macro(replace_or_append_release_flag _regex_str _flag_str)
	replace_or_append_compiler_flag(CMAKE_CXX_FLAGS_MINSIZEREL ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_CXX_FLAGS_RELEASE ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_C_FLAGS_MINSIZEREL ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_C_FLAGS_RELEASE ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_C_FLAGS_RELWITHDEBINFO ${_regex_str} ${_flag_str})
endmacro()

macro(replace_or_append_all_flag _regex_str _flag_str)
	replace_or_append_compiler_flag(CMAKE_CXX_FLAGS ${_regex_str} ${_flag_str})
	replace_or_append_compiler_flag(CMAKE_C_FLAGS ${_regex_str} ${_flag_str})
endmacro()

macro(replace_or_append_all_flag_compilers _vc_regex_str _gcc_regex_str _vc_flag_str _gcc_flag_str)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		replace_or_append_all_flag(${_vc_regex_str} ${_vc_flag_str})
	elseif(COMPILER MATCHES "gcc")
		replace_or_append_all_flag(${_gcc_regex_str} ${_gcc_flag_str})
	else()
		message(FATAL_ERROR "Error: Only MSVC and GCC supported")
	endif()
endmacro()

macro(replace_or_append_release_flag_compilers _vc_regex_str _gcc_regex_str _vc_flag_str _gcc_flag_str)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		replace_or_append_release_flag(${_vc_regex_str} ${_vc_flag_str})
	elseif(COMPILER MATCHES "gcc")
		replace_or_append_release_flag(${_gcc_regex_str} ${_gcc_flag_str})
	else()
		message(FATAL_ERROR "Error: Only MSVC and GCC supported")
	endif()
endmacro()

macro(replace_or_append_debug_flag_compilers _vc_regex_str _gcc_regex_str _vc_flag_str _gcc_flag_str)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		replace_or_append_debug_flag(${_vc_regex_str} ${_vc_flag_str})
	elseif(COMPILER MATCHES "gcc")
		replace_or_append_debug_flag(${_gcc_regex_str} ${_gcc_flag_str})
	else()
		message(FATAL_ERROR "Error: Only MSVC and GCC supported")
	endif()
endmacro()

macro(set_latest_language_versions)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
endmacro()

macro(enable_fast_floating_point _enable)
	get_compiler_name(COMPILER)
	if(${_enable})
		replace_or_append_all_flag_compilers("/fp:[fast|precise]" "-f[fast|rounding]-math" "/fp:fast" "-ffast-math")
	else()
		replace_or_append_all_flag_compilers("/fp:[fast|precise]" "-f[fast|rounding]-math" "/fp:precise" "-frounding-math")
	endif()
endmacro()

macro(disable_language_extensions _disable)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		if(${_disable})
			append_all_compiler_flag("/Za")
		else()
			#/Ze is deprecated
		endif()
	elseif(COMPILER MATCHES "gcc")
		message(FATAL_ERROR "TODO: Need to implement extensions for GCC")
	else()
		message(FATAL_ERROR "Compiler not recognized: ${COMPILER}")
	endif()
endmacro()

macro(set_advanced_warnings)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		replace_or_append_all_flag("/W[[0-4]|all]" "/Wall")
		replace_or_append_all_flag("/WX\-?" "/WX")
		append_all_compiler_flag("/sdl") #Security Development Lifecycle checks
		append_debug_flag("/RTCc") #Smaller type checks
		add_definitions(-D_ALLOW_RTCc_IN_STL) #Acknowledge STL does not support this check
		append_all_compiler_flag("/Zc:rvalueCast") #C++11 R-value cast check
		
		append_all_compiler_flag("/wd4201") #nameless struct/union (GLM)
		append_all_compiler_flag("/wd4464") #relative include path contains '..' (GLM)
		append_all_compiler_flag("/wd4514") #unreferenced inline function has been removed
		append_all_compiler_flag("/wd4571") #catch(...) semantics changed
		append_all_compiler_flag("/wd4710") #function not inlined (the entire std lib)
		append_all_compiler_flag("/wd4820") #Padding added to type
		
		append_release_flag("/wd4711") #Function selected for automatic inline expansion
	elseif(COMPILER MATCHES "gcc")
		append_all_compiler_flag("-Wall -Wextra -Werror -Wshadow -Wconversion -Wsign-conversion -pedantic")
	else()
		message(FATAL_ERROR "Compiler not recognized: ${COMPILER}")
	endif()
endmacro()

macro(set_catch_warnings)
	# Catch is not up to the challenge of Wall
	replace_or_append_all_flag_compilers("/W([0-4]|all)" "IMPOSSIBLE" "/W4" "")
	# These warnings are generated by catch test macros
	if(MSVC)
		append_all_compiler_flag("/wd4868") #compiler evaluation order in initializer list
		append_all_compiler_flag("/wd4571") #catch(...) semantics changed for structured exceptions
		append_all_compiler_flag("/wd4623") #default constructor implicitly deleted
	endif()
endmacro()

macro(set_vector_architecture)
	replace_or_append_all_flag_compilers("/arch:[AVX|SSE][0-9]" "-m[avx|sse][0-9]" "/arch:AVX2" "-mavx2")
endmacro()

macro(set_source_listing)
	get_compiler_name(COMPILER)
	if(COMPILER MATCHES "vc")
		replace_or_append_all_flag("/FA[c|s|u]" "/FAs")
	elseif(COMPILER MATCHES "gcc")
		message(FATAL_ERROR "TODO:  Implement source listing for GCC")
	else()
		message(FATAL_ERROR "Compiler not recognized: ${COMPILER}")
	endif()
endmacro()

#MSVC exception logic
set(EH_STRINGS "EH{a|s}{c|r}?\-?")
macro(enable_structured_exceptions)
	replace_or_append_all_flag_compilers(${EH_STRINGS} "IMPOSSIBLE" "/EHa" "")
endmacro()
macro(enable_synchronous_exceptions)
	replace_or_append_all_flag_compilers(${EH_STRINGS} "IMPOSSIBLE" "/EHs" "")
endmacro()
macro(enable_only_cxx_exceptions)
	replace_or_append_all_flag_compilers(${EH_STRINGS} "IMPOSSIBLE" "/EHsc" "")
endmacro()

####################
#Language section
####################

macro(ork_post_project_config)
	set_latest_language_versions()
	set(CMAKE_CXX_EXTENSIONS OFF)

	#Must be tested after project
	get_compiler_name(COMPILER)
	if(${COMPILER} MATCHES "vc")
		if(NOT ${COMPLILER} MATCHES "vc1[2-9]")
			message(FATAL_ERROR "Error: Only Visual Studio 2013 and above are supported")
		endif()
	elseif(${COMPILER} MATCHES "gcc|gxx")
		# All recent versions of GCC are OK
	else()
		message(FATAL_ERROR "Error: Only MSVC and GCC supported")
	endif()

	#explicit enable is required to avoid errors
	mark_as_internal(CMAKE_CXX_FLAGS)
	mark_as_internal(CMAKE_CXX_FLAGS_DEBUG)
	mark_as_internal(CMAKE_CXX_FLAGS_MINSIZEREL)
	mark_as_internal(CMAKE_CXX_FLAGS_RELEASE)
	mark_as_internal(CMAKE_CXX_FLAGS_RELWITHDEBINFO)

	mark_as_internal(CMAKE_CXX_STANDARD_LIBRARIES)

	mark_as_internal(CMAKE_C_FLAGS)
	mark_as_internal(CMAKE_C_FLAGS_DEBUG)
	mark_as_internal(CMAKE_C_FLAGS_MINSIZEREL)
	mark_as_internal(CMAKE_C_FLAGS_RELEASE)
	mark_as_internal(CMAKE_C_FLAGS_RELWITHDEBINFO)

	mark_as_internal(CMAKE_C_STANDARD_LIBRARIES)

	mark_as_internal(CMAKE_ASM_MASM_COMPILER)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_DEBUG)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_MINSIZEREL)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELEASE)
	mark_empty_internal(CMAKE_ASM_MASM_FLAGS_RELWITHDEBINFO)

	append_debug_flag("-D_DEBUG -DDEBUG")
	append_release_flag("-D_NDEBUG -DNDEBUG")
	replace_or_append_release_flag_compilers("/O([0-4]|x)" "-O[0-3]" "/Ox" "-O3")
	replace_or_append_release_flag_compilers("/Z(7|i|l)" "IMPOSSIBLE" "/Zi" "")
	replace_or_append_release_flag_compilers("/Ob[0-2]" "IMPOSSIBLE" "/Ob2" "")

	#Fast behavior is bad for CAD kernels
	detect_option_flag(ORK_FAST_FLOAT "Use fast floating point operations (Do not use in CAD applications)" OFF)
	enable_fast_floating_point(ORK_FAST_FLOAT)

	mark_as_internal(CMAKE_RC_COMPILER)
	mark_as_internal(CMAKE_RC_FLAGS)
	if(CMAKE_RC_FLAGS_DEBUG)#Not always defined
		mark_as_internal(CMAKE_RC_FLAGS_DEBUG)
	else()
		mark_empty_internal(CMAKE_RC_FLAGS_DEBUG)
	endif()
	mark_empty_internal(CMAKE_RC_FLAGS_MINSIZEREL)
	mark_empty_internal(CMAKE_RC_FLAGS_RELEASE)
	mark_empty_internal(CMAKE_RC_FLAGS_RELWITHDEBINFO)


	mark_as_internal(CMAKE_LINKER)

	mark_as_internal(CMAKE_EXE_LINKER_FLAGS)
	mark_as_internal(CMAKE_EXE_LINKER_FLAGS_DEBUG)
	mark_as_internal(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
	ensure_non_empty(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)
	set_as_internal(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
	mark_as_internal(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)

	mark_as_internal(CMAKE_MODULE_LINKER_FLAGS)
	mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_DEBUG)
	mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL)
	ensure_non_empty(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)
	set_as_internal(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO}")
	mark_as_internal(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)

	mark_as_internal(CMAKE_SHARED_LINKER_FLAGS)
	mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_DEBUG)
	mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL)
	ensure_non_empty(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)
	set_as_internal(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")
	mark_as_internal(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO)

	mark_as_internal(CMAKE_STATIC_LINKER_FLAGS)
	mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_DEBUG)
	mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL)
	mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_RELEASE)
	mark_empty_internal(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO)

	set_as_internal(CMAKE_SKIP_INSTALL_RPATH ON)
	set_as_internal(CMAKE_SKIP_RPATH ON)
	set_as_internal(CMAKE_VERBOSE_MAKEFILE OFF)
	
	set(CMAKE_DEBUG_POSTFIX "d")
	
	set(ORK_POST_PROJECT_CONFIGURED ON)
endmacro()

####################
#File section
####################

function(get_sub_directories parent_directory_ var_)
	if(NOT IS_DIRECTORY "${parent_directory_}")
		message(FATAL_ERROR "Invalid directory: ${parent_directory_}")
	endif()
	file(GLOB _sub_paths "${parent_directory_}/*")
	
	foreach(_curr_path ${_sub_paths})
		if(IS_DIRECTORY "${_curr_path}")
			list(APPEND _dirs_only "${_curr_path}")
		endif()
	endforeach()
	
	set(${var_} "${_dirs_only}" PARENT_SCOPE)
endfunction()

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
