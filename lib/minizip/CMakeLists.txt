set(MINIZIP_SRC ioapi.c mztools.c unzip.c zip.c)
if (WIN32)
	set(MINIZIP_SRC ${MINIZIP_SRC} iowin32.c)
endif(WIN32)

#Mac OS X doesn't have the 64-bit FS apis :(
if (APPLE)
add_definitions(-DUSE_FILE32API)
endif(APPLE)

add_library(minizip STATIC ${MINIZIP_SRC})
