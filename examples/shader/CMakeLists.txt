
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
