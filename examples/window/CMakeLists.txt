
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/window)

# all source files
set(SRC ${SRCROOT}/Window.cpp)

# define the window target
sfml_add_example(window GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-window)

if (APPLE AND NOT IOS)
    target_link_libraries(window PRIVATE "-framework OpenGL -Objc")
endif()
