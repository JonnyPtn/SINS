
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

# Compile the shaders
add_custom_command(TARGET shader POST_BUILD
                    COMMAND $<TARGET_FILE:shaderc> -f ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixelate.sc -o ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixelate.bin --type fs --platform windows -p ps_5_0 -O 3
                    COMMENT "Compiling shaders...")
