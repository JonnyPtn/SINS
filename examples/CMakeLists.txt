
if(SFML_OS_EMSCRIPTEN)
    add_subdirectory(emscripten)
else()
    # CLI based examples
    if (NOT SFML_OS_IOS)
        if(SFML_BUILD_NETWORK)
            add_subdirectory(ftp)
            add_subdirectory(sockets)
        endif()
        if(SFML_BUILD_NETWORK AND SFML_BUILD_AUDIO)
            add_subdirectory(voip)
        endif()
        if(SFML_BUILD_AUDIO)
            add_subdirectory(sound)
            add_subdirectory(sound_capture)
        endif()
    endif()

    # GUI based examples
    if(SFML_BUILD_WINDOW)
        add_subdirectory(window)
    endif()

    if(SFML_BUILD_GRAPHICS)

        if (NOT SFML_OS_IOS)
            add_subdirectory(joystick)
        endif()

        add_subdirectory(shader)
        add_subdirectory(island)

    endif()
    if(SFML_BUILD_GRAPHICS AND SFML_BUILD_AUDIO)
        add_subdirectory(pong)
    endif()
endif()