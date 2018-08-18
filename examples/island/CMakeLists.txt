
set(SRCROOT ${PROJECT_SOURCE_DIR}/examples/island)

# all source files
set(SRC ${SRCROOT}/Island.cpp)

# define the island target
sfml_add_example(island GUI_APP
                 SOURCES ${SRC}
                 DEPENDS sfml-graphics sfml-window sfml-system
                 RESOURCES_DIR resources)

if(SFML_OS_LINUX OR SFML_OS_FREEBSD)
    # TODO: replace with Threads::Threads if CMake version >= 3.1
    find_package(Threads REQUIRED)

    if(THREADS_HAVE_PTHREAD_ARG)
        target_compile_options(island PRIVATE "-pthread")
    endif()

    if(CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(island
                              PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
    endif()
endif()
