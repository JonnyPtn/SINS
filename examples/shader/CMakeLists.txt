
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