
project(ft2 C)

set(VERSION "2.7.0")

add_definitions(-DFT2_BUILD_LIBRARY)
if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4244") #Narrowing conversion
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267") #Narrowing conversion
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4312") #Weird cast to void*
endif()


#Create the options file (configuration file is not modified on Windows)
file(READ "${3P_FT2_SRC_DIR}/include/freetype/config/ftoption.h" FTOPTION_H)
if(ORK_BUILD_ZLIB)
	string(
		REGEX REPLACE
		"/\\* +(#define +FT_CONFIG_OPTION_SYSTEM_ZLIB) +\\*/" "\\1"
		FTOPTION_H "${FTOPTION_H}"
	)
endif()
if(ORK_BUILD_BZ2)
	string(
		REGEX REPLACE
		"/\\* +(#define +FT_CONFIG_OPTION_USE_BZIP2) +\\*/" "\\1"
		FTOPTION_H "${FTOPTION_H}"
	)
endif()
if(ORK_BUILD_PNG)
	string(
		REGEX REPLACE
		"/\\* +(#define +FT_CONFIG_OPTION_USE_PNG) +\\*/" "\\1"
		FTOPTION_H "${FTOPTION_H}"
	)
endif()
if(ORK_BUILD_HARFBUZZ)
	string(
		REGEX REPLACE
		"/\\* +(#define +FT_CONFIG_OPTION_USE_HARFBUZZ) +\\*/" "\\1"
		FTOPTION_H "${FTOPTION_H}"
	)
endif()
#Why are these not defined!?
string(
	REPLACE
	"/* #define FT_EXPORT_DEF(x)  x */"
"\
/* #define FT_EXPORT_DEF(x)  x */\n\
#if FT2_BUILD_LIBRARY\n\
#    define FT_EXPORT_DEF(x)  __declspec(dllexport) x __cdecl\n\
#else\n\
#    define FT_EXPORT_DEF(x)  __declspec(dllimport) x __cdecl\n\
#endif\n\
#define FT_EXPORT(x) extern FT_EXPORT_DEF(x)\n\
"
	FTOPTION_H "${FTOPTION_H}"
)
file(WRITE "${3P_FT2_BIN_DIR}/include/freetype/config/ftoption.h" "${FTOPTION_H}")


file(GLOB FT2_PUBLIC_HDRS
	"${3P_FT2_SRC_DIR}/include/ft2build.h"
	"${3P_FT2_SRC_DIR}/include/freetype/*.h"
	"${3P_FT2_SRC_DIR}/include/freetype/config/ftconfig.h"
	"${3P_FT2_SRC_DIR}/include/freetype/config/ftheader.h"
	"${3P_FT2_SRC_DIR}/include/freetype/config/ftmodule.h"
	"${3P_FT2_SRC_DIR}/include/freetype/config/ftstdlib.h"
	
	"${3P_FT2_BIN_DIR}/include/freetype/config/ftoption.h"
)
file(GLOB FT2_PRIVATE_HDRS
	"${3P_FT2_SRC_DIR}/include/freetype/internal/*.h"
)
set(FT2_SRCS
	"${3P_FT2_SRC_DIR}/src/autofit/autofit.c"
	"${3P_FT2_SRC_DIR}/src/base/ftbase.c"
	"${3P_FT2_SRC_DIR}/src/base/ftbbox.c"
	"${3P_FT2_SRC_DIR}/src/base/ftbdf.c"
	"${3P_FT2_SRC_DIR}/src/base/ftbitmap.c"
	"${3P_FT2_SRC_DIR}/src/base/ftcid.c"
	"${3P_FT2_SRC_DIR}/src/base/ftfntfmt.c"
	"${3P_FT2_SRC_DIR}/src/base/ftfstype.c"
	"${3P_FT2_SRC_DIR}/src/base/ftgasp.c"
	"${3P_FT2_SRC_DIR}/src/base/ftglyph.c"
	"${3P_FT2_SRC_DIR}/src/base/ftgxval.c"
	"${3P_FT2_SRC_DIR}/src/base/ftinit.c"
	"${3P_FT2_SRC_DIR}/src/base/ftlcdfil.c"
	"${3P_FT2_SRC_DIR}/src/base/ftmm.c"
	"${3P_FT2_SRC_DIR}/src/base/ftotval.c"
	"${3P_FT2_SRC_DIR}/src/base/ftpatent.c"
	"${3P_FT2_SRC_DIR}/src/base/ftpfr.c"
	"${3P_FT2_SRC_DIR}/src/base/ftstroke.c"
	"${3P_FT2_SRC_DIR}/src/base/ftsynth.c"
	"${3P_FT2_SRC_DIR}/src/base/ftsystem.c"
	"${3P_FT2_SRC_DIR}/src/base/fttype1.c"
	"${3P_FT2_SRC_DIR}/src/base/ftwinfnt.c"
	"${3P_FT2_SRC_DIR}/src/bdf/bdf.c"
	"${3P_FT2_SRC_DIR}/src/bzip2/ftbzip2.c"
	"${3P_FT2_SRC_DIR}/src/cache/ftcache.c"
	"${3P_FT2_SRC_DIR}/src/cff/cff.c"
	"${3P_FT2_SRC_DIR}/src/cid/type1cid.c"
	"${3P_FT2_SRC_DIR}/src/gzip/ftgzip.c"
	"${3P_FT2_SRC_DIR}/src/lzw/ftlzw.c"
	"${3P_FT2_SRC_DIR}/src/pcf/pcf.c"
	"${3P_FT2_SRC_DIR}/src/pfr/pfr.c"
	"${3P_FT2_SRC_DIR}/src/psaux/psaux.c"
	"${3P_FT2_SRC_DIR}/src/pshinter/pshinter.c"
	"${3P_FT2_SRC_DIR}/src/psnames/psnames.c"
	"${3P_FT2_SRC_DIR}/src/raster/raster.c"
	"${3P_FT2_SRC_DIR}/src/sfnt/sfnt.c"
	"${3P_FT2_SRC_DIR}/src/smooth/smooth.c"
	"${3P_FT2_SRC_DIR}/src/truetype/truetype.c"
	"${3P_FT2_SRC_DIR}/src/type1/type1.c"
	"${3P_FT2_SRC_DIR}/src/type42/type42.c"
	"${3P_FT2_SRC_DIR}/src/winfonts/winfnt.c"
)
set(FT2_WIN_SRCS
	"${3P_FT2_SRC_DIR}/builds/windows/ftdebug.c"
)


add_library(ft2 SHARED ${FT2_SRCS} ${FT2_WIN_SRCS} ${FT2_PUBLIC_HDRS} ${FT2_PRIVATE_HDRS})

if(ORK_BUILD_ZLIB)
	target_link_libraries(ft2 PRIVATE zlib)
endif()
if(ORK_BUILD_BZ2)
	target_link_libraries(ft2 PRIVATE bz2)
endif()
if(ORK_BUILD_PNG)
	target_link_libraries(ft2 PRIVATE png)
endif()
if(ORK_BUILD_HARFBUZZ)
	target_link_libraries(ft2 PRIVATE harfbuzz)
endif()

target_include_directories(
	png
	PUBLIC $<BUILD_INTERFACE:${3P_FT2_BIN_DIR}/include>#Order matters for config header
	PUBLIC $<BUILD_INTERFACE:${3P_FT2_SRC_DIR}/include>
	INTERFACE $<INSTALL_INTERFACE:${INSTALL_INC_DIR}>
)

install_bin(ft2)
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL)
	install(
		DIRECTORY "${3P_FT2_SRC_DIR}/include/" #Note trailing slash
		DESTINATION "${INSTALL_INC_DIR}"
		PATTERN "internal" EXCLUDE
		PATTERN "ftoption.h" EXCLUDE
	)
	install(
		FILES "${3P_FT2_BIN_DIR}/include/freetype/config/ftoption.h"
		DESTINATION "${INSTALL_INC_DIR}/freetype/config"
	)
endif()
