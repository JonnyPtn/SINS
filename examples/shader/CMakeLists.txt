
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/shader)

# all source files
set(SRC
    ${SRCROOT}/Effect.hpp
    ${SRCROOT}/Shader.cpp)

# all resources
if (SFML_OS_IOS)
    set(RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/sansation.ttf
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/background.jpg
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/devices.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/logo.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/sfml.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/text-background.png)
    set_source_files_properties(${RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

# define the shader target
sfml_add_example(shader GUI_APP
                 SOURCES ${SRC}
                 BUNDLE_RESOURCES ${RESOURCES}
                 DEPENDS sfml-graphics
                 RESOURCES_DIR resources)

if(SFML_OS_LINUX OR SFML_OS_FREEBSD)
	target_link_libraries(shader PRIVATE pthread dl GL)
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

foreach(FRAG_SHADER ${FRAG_SHADERS})
add_custom_command(TARGET shader POST_BUILD
    COMMAND ${SHADERC_COMMAND} -f ${FRAG_SHADER} -o ${FRAG_SHADER}.bin --type fragment ${SHADERC_FLAGS}
    COMMENT "Compiling ${FRAG_SHADER}")
endforeach()

# Need to use different flag for vert shaders on DX11
if (SFML_OS_WINDOWS)
    set(SHADERC_FLAGS --platform windows -p vs_5_0 -O 3)
endif()

foreach(VERT_SHADER ${VERT_SHADERS})
add_custom_command(TARGET shader POST_BUILD
    COMMAND ${SHADERC_COMMAND} -f ${VERT_SHADER} -o ${VERT_SHADER}.bin --type vertex ${SHADERC_FLAGS}
    COMMENT "Compiling ${VERT_SHADER}")
endforeach()