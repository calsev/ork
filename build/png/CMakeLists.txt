
project(libpng C)

set(VERSION "1.6.9")

add_definitions(-DPNG_BUILD_DLL)
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)


configure_file(
	"${3P_PNG_SRC_DIR}/scripts/pnglibconf.h.prebuilt"
	"${3P_PNG_BIN_DIR}/pnglibconf.h"
)


set(PNG_PUBLIC_HDRS
	"${3P_PNG_SRC_DIR}/png.h"
	"${3P_PNG_SRC_DIR}/pngconf.h"
	"${3P_PNG_BIN_DIR}/pnglibconf.h"
)
set(PNG_PRIVATE_HDRS
	"${3P_PNG_SRC_DIR}/pngdebug.h"
	"${3P_PNG_SRC_DIR}/pnginfo.h"
	"${3P_PNG_SRC_DIR}/pngpriv.h"
	"${3P_PNG_SRC_DIR}/pngstruct.h"
)
set(PNG_SRCS
	"${3P_PNG_SRC_DIR}/png.c"
	"${3P_PNG_SRC_DIR}/pngerror.c"
	"${3P_PNG_SRC_DIR}/pngget.c"
	"${3P_PNG_SRC_DIR}/pngmem.c"
	"${3P_PNG_SRC_DIR}/pngpread.c"
	"${3P_PNG_SRC_DIR}/pngread.c"
	"${3P_PNG_SRC_DIR}/pngrio.c"
	"${3P_PNG_SRC_DIR}/pngrtran.c"
	"${3P_PNG_SRC_DIR}/pngrutil.c"
	"${3P_PNG_SRC_DIR}/pngset.c"
	"${3P_PNG_SRC_DIR}/pngtrans.c"
	"${3P_PNG_SRC_DIR}/pngwio.c"
	"${3P_PNG_SRC_DIR}/pngwrite.c"
	"${3P_PNG_SRC_DIR}/pngwtran.c"
	"${3P_PNG_SRC_DIR}/pngwutil.c"
)


add_library(png SHARED ${PNG_SRCS} ${PNG_PUBLIC_HDRS} ${PNG_PRIVATE_HDRS})

target_link_libraries(png PRIVATE zlib)

target_include_directories(
	png
	PUBLIC $<BUILD_INTERFACE:${3P_PNG_BIN_DIR}>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_PNG_SRC_DIR}>
	INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)

install_shared(png ${PNG_PUBLIC_HDRS})
