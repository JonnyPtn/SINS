
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/opengl)

# all source files
set(SRC ${SRCROOT}/OpenGL.cpp)

find_package(OpenGL REQUIRED)

# define the opengl target
sfml_add_example(opengl GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics ${OPENGL_gl_LIBRARY}
                 RESOURCES_DIR resources)
