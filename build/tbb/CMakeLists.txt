
project(tbb CXX)

detect_option_flag(TBB_BUILD_TBBMALLOC "Build TBB malloc library" ON)
detect_option_flag(TBB_BUILD_TBBMALLOC_PROXY "Build TBB malloc proxy library" ON)


add_definitions(-DDO_ITT_NOTIFY)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
add_definitions(-D_WIN32_WINNT=0x0600)
add_definitions(-DUSE_WINTHREAD)

if(MSVC)
	set(CMAKE_CXX_FLAGS "/volatile:iso ${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "/wd4018 ${CMAKE_CXX_FLAGS}") #Compare sign mismatch
	set(CMAKE_CXX_FLAGS "/wd4146 ${CMAKE_CXX_FLAGS}") #Negation of unsigned
	set(CMAKE_CXX_FLAGS "/wd4244 ${CMAKE_CXX_FLAGS}") #Narrowing conversion
	set(CMAKE_CXX_FLAGS "/wd4267 ${CMAKE_CXX_FLAGS}") #Narrowing conversion
	set(CMAKE_CXX_FLAGS "/wd4800 ${CMAKE_CXX_FLAGS}") #Coercion to bool
endif()


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


set(ENABLE_RTTI "/EHsc /GR ") #Catch C++ exceptions only (C no throw, no asynch), RTTI
set(DISABLE_RTTI "/EHs- /GR- ") #Catch no exceptions, no RTTI


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
target_compile_definitions(tbb PUBLIC "__TBB_NO_IMPLICIT_LINKAGE=1")
target_compile_definitions(tbb PRIVATE "__TBB_BUILD=1")
target_compile_options(tbb PRIVATE "${ENABLE_RTTI}")
set_property(TARGET tbb APPEND PROPERTY LINK_FLAGS "/DEF:${3P_TBB_BIN_DIR}/tbb.def")
set_source_files_properties(${3P_TBB_SRC_DIR}/src/tbb/scheduler.cpp COMPILE_FLAGS "/wd4458 ") #Declaration hides class member
add_dependencies(tbb tbb_def_files)

target_include_directories(
	tbb
	PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
	PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
	PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)


if(TBB_BUILD_TBBMALLOC)
	add_library(tbbmalloc SHARED ${TBB_MALLOC_SRCS})
	set_target_properties(tbbmalloc PROPERTIES FOLDER "TBB")
	target_compile_definitions(tbbmalloc PUBLIC "__TBB_NO_IMPLICIT_LINKAGE=1" "__TBBMALLOC_NO_IMPLICIT_LINKAGE=1")
	target_compile_definitions(tbb PRIVATE "__TBBMALLOC_BUILD=1")
	target_compile_options(tbb PRIVATE "${DISABLE_RTTI}")
	set_property(TARGET tbbmalloc APPEND PROPERTY LINK_FLAGS "/DEF:${3P_TBB_BIN_DIR}/tbbmalloc.def")
	add_dependencies(tbbmalloc tbb_def_files)
	
	target_include_directories(
		tbbmalloc
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
		INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
	)
endif()

if(TBB_BUILD_TBBMALLOC_PROXY)
	add_library(tbbmalloc_proxy SHARED ${TBB_PROXY_SRCS})
	set_target_properties(tbbmalloc_proxy PROPERTIES FOLDER "TBB")
	target_compile_definitions(tbbmalloc_proxy PRIVATE "__TBBMALLOC_BUILD=1")
	target_compile_options(tbbmalloc_proxy PRIVATE "${DISABLE_RTTI}")
	target_link_libraries(tbbmalloc_proxy tbbmalloc)
	
	target_include_directories(
		tbbmalloc_proxy
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_BIN_DIR}>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src>
		PUBLIC $<BUILD_INTERFACE:${3P_TBB_SRC_DIR}/src/rml/include>
		INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
	)
endif()


install_bin(tbb)
if(TBB_BUILD_TBBMALLOC)
	install_bin(tbbmalloc)
endif()
if(TBB_BUILD_TBBMALLOC_PROXY)
	install_bin(tbbmalloc_proxy)
endif()
install_header_dir("${3P_ORK_SRC_DIR}/tbb" "${INSTALL_INC_DIR}" "dummy_pattern")
