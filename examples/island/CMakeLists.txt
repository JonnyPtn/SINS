
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
