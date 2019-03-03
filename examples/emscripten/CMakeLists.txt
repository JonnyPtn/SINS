
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/emscripten)

# all source files
set(SRC ${SRCROOT}/Emscripten.cpp)

# define the window target
sfml_add_example(emscripten GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics sfml-audio
                 RESOURCES_DIR resources)
