
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/joystick)

# all source files
set(SRC ${SRCROOT}/Joystick.cpp)

# define the joystick target
sfml_add_example(joystick GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics
                 RESOURCES_DIR resources)

if(SFML_OS_LINUX OR SFML_OS_FREEBSD)
	target_link_libraries(joystick PRIVATE pthread dl GL)
endif()
