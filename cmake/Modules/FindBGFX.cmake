#
# Try to find Ogg/Vorbis libraries and include paths.
# Once done this will define
#
# VORBIS_FOUND
# VORBIS_INCLUDE_DIRS
# VORBIS_LIBRARIES
#

#Because I'm offline right now this is a workaround
set(bgfx_BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/bgfx-build)
set(bgfx_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/bgfx-src)

find_path(BGFX_INCLUDE_DIR bgfx/bgfx.h HINTS ${bgfx_SOURCE_DIR}/bgfx/include)
find_path(BX_INCLUDE_DIR bx/bx.h HINTS ${bgfx_SOURCE_DIR}/bx/include)
find_path(BIMG_INCLUDE_DIR bimg/bimg.h HINTS ${bgfx_SOURCE_DIR}/bimg/include)

find_library(BGFX_LIBRARY NAMES bgfx HINTS ${bgfx_BINARY_DIR}/install/lib)
find_library(BX_LIBRARY NAMES bx HINTS ${bgfx_BINARY_DIR}/install/lib)
find_library(BIMG_LIBRARY NAMES bimg HINTS ${bgfx_BINARY_DIR}/install/lib)
set(BGFX_LIBRARIES ${BGFX_LIBRARY} ${BX_LIBRARY} ${BIMG_LIBRARY})

include(FindPackageHandleStandardArgs)

set(BGFX_INCLUDE_DIRS ${BGFX_INCLUDE_DIR} ${BX_INCLUDE_DIR} ${BIMG_INCLUDE_DIR})

find_package_handle_standard_args(BGFX DEFAULT_MSG BGFX_LIBRARIES BGFX_INCLUDE_DIRS )
