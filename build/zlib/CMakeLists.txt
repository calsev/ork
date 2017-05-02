
project(zlib C)

set(VERSION "1.2.8")

add_definitions(-DZLIB_DLL)
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()


configure_file(
	"${3P_ZLIB_SRC_DIR}/zconf.h.cmakein"
	"${3P_ZLIB_BIN_DIR}/zconf.h" @ONLY
)


set(ZLIB_PUBLIC_HDRS
	"${3P_ZLIB_BIN_DIR}/zconf.h"
	"${3P_ZLIB_SRC_DIR}/zlib.h"
)
set(ZLIB_PRIVATE_HDRS
	"${3P_ZLIB_SRC_DIR}/crc32.h"
	"${3P_ZLIB_SRC_DIR}/deflate.h"
	"${3P_ZLIB_SRC_DIR}/gzguts.h"
	"${3P_ZLIB_SRC_DIR}/inffast.h"
	"${3P_ZLIB_SRC_DIR}/inffixed.h"
	"${3P_ZLIB_SRC_DIR}/inflate.h"
	"${3P_ZLIB_SRC_DIR}/inftrees.h"
	"${3P_ZLIB_SRC_DIR}/trees.h"
	"${3P_ZLIB_SRC_DIR}/zutil.h"
)
set(ZLIB_SRCS
	"${3P_ZLIB_SRC_DIR}/adler32.c"
	"${3P_ZLIB_SRC_DIR}/compress.c"
	"${3P_ZLIB_SRC_DIR}/crc32.c"
	"${3P_ZLIB_SRC_DIR}/deflate.c"
	"${3P_ZLIB_SRC_DIR}/gzclose.c"
	"${3P_ZLIB_SRC_DIR}/gzlib.c"
	"${3P_ZLIB_SRC_DIR}/gzread.c"
	"${3P_ZLIB_SRC_DIR}/gzwrite.c"
	"${3P_ZLIB_SRC_DIR}/inflate.c"
	"${3P_ZLIB_SRC_DIR}/infback.c"
	"${3P_ZLIB_SRC_DIR}/inftrees.c"
	"${3P_ZLIB_SRC_DIR}/inffast.c"
	"${3P_ZLIB_SRC_DIR}/trees.c"
	"${3P_ZLIB_SRC_DIR}/uncompr.c"
	"${3P_ZLIB_SRC_DIR}/zutil.c"
)
set(ZLIB_DLL_SRCS
	"${3P_ZLIB_SRC_DIR}/win32/zlib1.rc" #Overrides custom build rule below.
)


add_library(zlib SHARED ${ZLIB_SRCS} ${ZLIB_DLL_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})

add_custom_command( #Rename the default config header
	TARGET zlib PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	"${3P_ZLIB_SRC_DIR}/zconf.h" "${3P_ZLIB_SRC_DIR}/zconf.h.included"
)
add_custom_command( #Rename the default config header back
	TARGET zlib POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rename
	"${3P_ZLIB_SRC_DIR}/zconf.h.included" "${3P_ZLIB_SRC_DIR}/zconf.h"
)

target_include_directories(
	zlib
	PUBLIC $<BUILD_INTERFACE:${3P_ZLIB_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_ZLIB_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)

install_shared(zlib ${ZLIB_PUBLIC_HDRS})
