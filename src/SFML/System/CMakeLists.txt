
set(INCROOT ${PROJECT_SOURCE_DIR}/include/SFML/System)
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/SFML/System)

# all source files
set(SRC
    ${SRCROOT}/Angle.cpp
    ${INCROOT}/Angle.hpp
    ${SRCROOT}/Clock.cpp
    ${INCROOT}/Clock.hpp
    ${SRCROOT}/Err.cpp
    ${INCROOT}/Err.hpp
    ${INCROOT}/Export.hpp
    ${INCROOT}/InputStream.hpp
    ${INCROOT}/NativeActivity.hpp
    ${INCROOT}/NonCopyable.hpp
    ${SRCROOT}/String.cpp
    ${INCROOT}/String.hpp
    ${INCROOT}/String.inl
    ${SRCROOT}/Time.cpp
    ${INCROOT}/Time.hpp
    ${INCROOT}/Utf.hpp
    ${INCROOT}/Utf.inl
    ${INCROOT}/Vector2.hpp
    ${INCROOT}/Vector2.inl
    ${INCROOT}/Vector3.hpp
    ${INCROOT}/Vector3.inl
    ${SRCROOT}/FileInputStream.cpp
    ${INCROOT}/FileInputStream.hpp
    ${SRCROOT}/MemoryInputStream.cpp
    ${INCROOT}/MemoryInputStream.hpp
)
source_group("" FILES ${SRC})

# add platform specific sources
if(SFML_OS_ANDROID)
    set(PLATFORM_SRC ${PLATFORM_SRC}
        ${SRCROOT}/Android/Activity.hpp
        ${SRCROOT}/Android/Activity.cpp
        ${SRCROOT}/Android/NativeActivity.cpp
        ${SRCROOT}/Android/ResourceStream.cpp
        ${SRCROOT}/Android/ResourceStream.cpp
    )

    source_group("unix" FILES ${PLATFORM_SRC})
endif()

# define the sfml-system target
sfml_add_library(sfml-system
                 SOURCES ${SRC} ${PLATFORM_SRC})

# setup dependencies
if(SFML_OS_LINUX)
    target_link_libraries(sfml-system PRIVATE rt)
elseif(SFML_OS_WINDOWS)
    target_link_libraries(sfml-system PRIVATE winmm)
elseif(SFML_OS_ANDROID)
    target_link_libraries(sfml-system PRIVATE android log)
endif()
