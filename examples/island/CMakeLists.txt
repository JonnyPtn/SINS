
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/island)

# all source files
set(SRC ${SRCROOT}/Island.cpp)

# define the island target
sfml_add_example(island GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics sfml-window sfml-system
                 RESOURCES_DIR resources)

if(SFML_OS_LINUX OR SFML_OS_FREEBSD)
	target_link_libraries(island PRIVATE pthread dl GL)
endif()

# Platform specific flags for shaderc
if (SFML_OS_WINDOWS)
    # Assuming DX11
    set(SHADERC_FLAGS --platform windows -p ps_5_0 -O 3)
elseif(SFML_OS_MACOSX)
    # Assuming Metal
    set(SHADERC_FLAGS --platform osx -p metal)
elseif(SFML_OS_IOS)
    set(SHADERC_FLAGS --platform ios)
endif()

# Collect shaders by type
file(GLOB FRAG_SHADERS resources/*.frag.sc)
file(GLOB VERT_SHADERS resources/*.vert.sc)

# We ideally need to compile the shaders on the host system
if (NOT SFML_OS_EMSCRIPTEN)
    set(SHADERC_COMMAND $<TARGET_FILE:shaderc>)
else()
    # On emscripten we look for a preinstalled shaderc and use that
    find_program(SHADERC_COMMAND shaderc)
endif()

set(TERRAIN_FRAG_SHADER ${CMAKE_CURRENT_LIST_DIR}/resources/terrain.frag.sc)
add_custom_command(TARGET island POST_BUILD
    COMMAND ${SHADERC_COMMAND} -f ${TERRAIN_FRAG_SHADER} -o ${TERRAIN_FRAG_SHADER}.bin --type fragment ${SHADERC_FLAGS}
    COMMENT "Compiling ${TERRAIN_FRAG_SHADER}")

set(TERRAIN_VERT_SHADER ${CMAKE_CURRENT_LIST_DIR}/resources/terrain.vert.sc)
add_custom_command(TARGET island POST_BUILD
    COMMAND ${SHADERC_COMMAND} -f ${TERRAIN_VERT_SHADER} -o ${TERRAIN_VERT_SHADER}.bin --type vertex ${SHADERC_FLAGS}
    COMMENT "Compiling ${TERRAIN_VERT_SHADER}")