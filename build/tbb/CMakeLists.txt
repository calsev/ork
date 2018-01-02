
project(tbb CXX)


add_definitions(-DDO_ITT_NOTIFY)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
add_definitions(-D_WIN32_WINNT=0x0600)
add_definitions(-DUSE_WINTHREAD)

if(MSVC)
	append_all_compiler_flag("/volatile:iso")
	append_all_compiler_flag("/wd4018") #Compare sign mismatch
	append_all_compiler_flag("/wd4146") #Negation of unsigned
	append_all_compiler_flag("/wd4244") #Narrowing conversion
	append_all_compiler_flag("/wd4267") #Narrowing conversion
	append_all_compiler_flag("/wd4800") #Coercion to bool
endif()


execute_process(
	COMMAND cmd " /C date /T"
	OUTPUT_VARIABLE _configure_date
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version_string.ver.in" "${3P_TBB_BIN_DIR}/version_string.ver" @ONLY)


file(GLOB TBB_SRCS
	"${3P_TBB_SRC_DIR}/src/tbb/*.cpp"
	"${3P_TBB_SRC_DIR}/src/old/*.cpp"
)
list(APPEND TBB_SRCS "${3P_TBB_SRC_DIR}/src/rml/client/rml_tbb.cpp")
file(GLOB TBB_SRC_REM "${3P_TBB_SRC_DIR}/src/old/test*.cpp")
list(REMOVE_ITEM TBB_SRCS ${TBB_SRC_REM})

set(TBB_MALLOC_SRCS
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/backend.cpp"
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/large_objects.cpp"
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/backref.cpp"
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/tbbmalloc.cpp"
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/frontend.cpp"
	"${3P_TBB_SRC_DIR}/src/tbb/itt_notify.cpp"
)
set(TBB_PROXY_SRCS
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/proxy.cpp"
	"${3P_TBB_SRC_DIR}/src/tbbmalloc/tbb_function_replacement.cpp"
)


#Assembly sources
list(APPEND TBB_SRCS
	"${3P_TBB_SRC_DIR}/src/tbb/intel64-masm/atomic_support.asm"
	"${3P_TBB_SRC_DIR}/src/tbb/intel64-masm/itsx.asm"
	"${3P_TBB_SRC_DIR}/src/tbb/intel64-masm/intel64_misc.asm"
)
list(APPEND TBB_MALLOC_SRCS
	"${3P_TBB_SRC_DIR}/src/tbb/intel64-masm/atomic_support.asm"
)
set(CMAKE_ASM_MASM_FLAGS "/DEM64T=1 ${CMAKE_ASM_MASM_FLAGS}")


set(ENABLE_RTTI "/GR ") #Catch C++ exceptions only (C no throw, no asynch), RTTI
set(DISABLE_RTTI "/GR- ") #Catch no exceptions, no RTTI


#Create def for shared libs
set(ARCH_PREFIX "win64")
add_custom_command(
	OUTPUT tbb.def
	COMMAND ${CMAKE_CXX_COMPILER} /TC /EP ${3P_TBB_SRC_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def -I ${3P_TBB_SRC_DIR}/include > tbb.def
	MAIN_DEPENDENCY ${3P_TBB_SRC_DIR}/src/tbb/${ARCH_PREFIX}-tbb-export.def
)
add_custom_command(
	OUTPUT tbbmalloc.def
	COMMAND ${CMAKE_CXX_COMPILER} /TC /EP ${3P_TBB_SRC_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def -I ${3P_TBB_SRC_DIR}/include > tbbmalloc.def
	MAIN_DEPENDENCY ${3P_TBB_SRC_DIR}/src/tbbmalloc/${ARCH_PREFIX}-tbbmalloc-export.def
)
add_custom_target(tbb_def_files DEPENDS tbb.def tbbmalloc.def)
set_target_properties(tbb_def_files PROPERTIES FOLDER "TBB")


add_library(tbb SHARED ${TBB_SRCS})
set_target_properties(tbb PROPERTIES FOLDER "TBB")
target_compile_definitions(tbb PUBLIC "__TBB_NO_IMPLICIT_LINKAGE")
target_compile_definitions(tbb PRIVATE "__TBB_BUILD")
set_property(TARGET tbb APPEND_STRING PROPERTY COMPILE_FLAGS ${ENABLE_RTTI})
set_property(TARGET tbb APPEND PROPERTY LINK_FLAGS "/DEF:${3P_TBB_BIN_DIR}/tbb.def")
set_source_files_properties(${3P_TBB_SRC_DIR}/src/tbb/scheduler.cpp COMPILE_FLAGS "/wd4458 ") #Declaration hides class member
add_dependencies(tbb tbb_def_files)

target_include_directories(
	tbb
	PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
	PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
	PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
	INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


if(ORK_BUILD_TBB_MALLOC)
	add_library(tbbmalloc SHARED ${TBB_MALLOC_SRCS})
	set_target_properties(tbbmalloc PROPERTIES FOLDER "TBB")
	target_compile_definitions(tbbmalloc PUBLIC "__TBB_NO_IMPLICIT_LINKAGE" "__TBBMALLOC_NO_IMPLICIT_LINKAGE")
	target_compile_definitions(tbbmalloc PRIVATE "__TBBMALLOC_BUILD")
	set_property(TARGET tbbmalloc APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
	set_property(TARGET tbbmalloc APPEND PROPERTY LINK_FLAGS "/DEF:${3P_TBB_BIN_DIR}/tbbmalloc.def")
	add_dependencies(tbbmalloc tbb_def_files)
	
	target_include_directories(
		tbbmalloc
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/include>
		PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
		PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
		INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
	)
endif()

if(ORK_BUILD_TBB_MALLOC_PROXY)
	add_library(tbbmalloc_proxy SHARED ${TBB_PROXY_SRCS})
	set_target_properties(tbbmalloc_proxy PROPERTIES FOLDER "TBB")
	target_compile_definitions(tbbmalloc_proxy PRIVATE "__TBBMALLOC_BUILD")
	set_property(TARGET tbbmalloc_proxy APPEND_STRING PROPERTY COMPILE_FLAGS ${DISABLE_RTTI})
	target_link_libraries(tbbmalloc_proxy tbbmalloc)
	
	target_include_directories(
		tbbmalloc_proxy
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/include>
		PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
		PRIVATE $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
		INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
	)
endif()


install_bin(tbb)
if(ORK_BUILD_TBB_MALLOC)
	install_bin(tbbmalloc)
endif()
if(ORK_BUILD_TBB_MALLOC_PROXY)
	install_bin(tbbmalloc_proxy)
endif()
install_header_dir("${3P_TBB_SRC_DIR}/include/tbb" "${ORK_INSTALL_INC_DIR}/tbb" "compat|internal|machine")
