
project(bzip2 C)

set(VERSION "1.0.6")

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4244") #Narrowing conversion
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267") #Narrowing conversion
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996") #POSIX names deprecated
endif()


set(BZ2_PUBLIC_HDRS
	"${3P_BZ2_SRC_DIR}/bzlib.h"
)
set(BZ2_PRIVATE_HDRS
	"${3P_BZ2_SRC_DIR}/bzlib_private.h"
)
set(BZ2_SRCS
	"${3P_BZ2_SRC_DIR}/blocksort.c"
	"${3P_BZ2_SRC_DIR}/bzlib.c"
	"${3P_BZ2_SRC_DIR}/compress.c"
	"${3P_BZ2_SRC_DIR}/crctable.c"
	"${3P_BZ2_SRC_DIR}/decompress.c"
	"${3P_BZ2_SRC_DIR}/huffman.c"
	"${3P_BZ2_SRC_DIR}/randtable.c"
)
set(BZ2_DLL_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/bz2.def"
)


add_library(bz2 SHARED ${BZ2_SRCS} ${BZ2_DLL_SRCS} ${BZ2_PUBLIC_HDRS} ${BZ2_PRIVATE_HDRS})

target_include_directories(
	bz2
	PUBLIC $<BUILD_INTERFACE:${3P_BZ2_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)

install_shared(bz2 ${BZ2_PUBLIC_HDRS})