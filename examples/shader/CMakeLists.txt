
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/shader)

# all source files
set(SRC
    ${SRCROOT}/Effect.hpp
    ${SRCROOT}/Shader.cpp)

# define the shader target
sfml_add_example(shader GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics
                 RESOURCES_DIR resources)

if(SFML_OS_LINUX OR SFML_OS_FREEBSD)
	target_link_libraries(shader PRIVATE pthread dl GL ${CMAKE_BINARY_DIR}/bgfx-install/lib/libbx.a)
endif()

# Platform specific flags for shaderc
if (SFML_OS_WINDOWS)
    # Assuming DX11
    set(SHADERC_FLAGS --platform windows -p ps_5_0 -O 3)
elseif(SFML_OS_MACOSX)
    # Assuming Metal
    set(SHADERC_FLAGS --platform osx -p metal)
endif()

# Collect shaders by type
file(GLOB FRAG_SHADERS resources/*.frag.sc)
file(GLOB VERT_SHADERS resources/*.vert.sc)

foreach(FRAG_SHADER ${FRAG_SHADERS})
add_custom_command(TARGET shader POST_BUILD
    COMMAND $<TARGET_FILE:shaderc> -f ${FRAG_SHADER} -o ${FRAG_SHADER}.bin --type fs ${SHADERC_FLAGS}
    COMMENT "Compiling ${FRAG_SHADER}")
endforeach()

# Need to use different flag for vert shaders on DX11
if (SFML_OS_WINDOWS)
set(SHADERC_FLAGS --platform windows -p vs_5_0 -O 3)
endif()

foreach(VERT_SHADER ${VERT_SHADERS})
add_custom_command(TARGET shader POST_BUILD
    COMMAND $<TARGET_FILE:shaderc> -f ${VERT_SHADER} -o ${VERT_SHADER}.bin --type vs ${SHADERC_FLAGS}
    COMMENT "Compiling ${VERT_SHADER}")
endforeach()
