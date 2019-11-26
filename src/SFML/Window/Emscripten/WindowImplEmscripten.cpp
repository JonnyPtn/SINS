////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Emscripten/WindowImplEmscripten.hpp>
#include <SFML/Window/Emscripten/ClipboardImpl.hpp>
#include <SFML/Window/Emscripten/InputImpl.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/System/Err.hpp>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <string>
#include <mutex>
#include <cstring>
#include <limits>
#include <iostream>

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(WindowHandle handle) :
m_screen         (0),
m_inputMethod    (nullptr),
m_inputContext   (nullptr),
m_isExternal     (true),
m_oldVideoMode   (0),
m_hiddenCursor   (0),
m_lastCursor     (None),
m_keyRepeat      (true),
m_previousSize   (-1, -1),
m_useSizeHints   (false),
m_fullscreen     (false),
m_cursorGrabbed  (false),
m_windowMapped   (false),
m_iconPixmap     (0),
m_iconMaskPixmap (0),
m_lastInputTime  (0)
{
    // Do some common initializations
    initialize();
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::WindowImplEmscripten(VideoMode mode, const String& title, unsigned long style) :
m_screen         (0),
m_inputMethod    (nullptr),
m_inputContext   (nullptr),
m_isExternal     (false),
m_oldVideoMode   (0),
m_hiddenCursor   (0),
m_lastCursor     (None),
m_keyRepeat      (true),
m_previousSize   (-1, -1),
m_useSizeHints   (false),
m_fullscreen     ((style & Style::Fullscreen) != 0),
m_cursorGrabbed  (m_fullscreen),
m_windowMapped   (false),
m_iconPixmap     (0),
m_iconMaskPixmap (0),
m_lastInputTime  (0)
{
    m_size = {mode.width, mode.height};
    initialize();
}


////////////////////////////////////////////////////////////
WindowImplEmscripten::~WindowImplEmscripten()
{
   
}

////////////////////////////////////////////////////////////
WindowHandle WindowImplEmscripten::getSystemHandle() const
{
    return std::numeric_limits<uint16_t>::max();
}

////////////////////////////////////////////////////////////
Vector2i WindowImplEmscripten::getPosition() const
{
    return Vector2i(0, 0);
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setPosition(const Vector2i& position)
{
}


////////////////////////////////////////////////////////////
Vector2u WindowImplEmscripten::getSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setSize(const Vector2u& size)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setTitle(const String& title)
{
    // Bare X11 has no Unicode window title support.
    // There is however an option to tell the window manager your Unicode title via hints.

    // Convert to UTF-8 encoding.
    std::basic_string<Uint8> utf8Title;
    Utf32::toUtf8(title.begin(), title.end(), std::back_inserter(utf8Title));

}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setIcon(unsigned int width, unsigned int height, const Uint8* pixels)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setVisible(bool visible)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursorVisible(bool visible)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursor(const CursorImpl& cursor)
{
    m_lastCursor = cursor.m_cursor;
    //XDefineCursor(m_display, m_window, m_lastCursor);
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setMouseCursorGrabbed(bool grabbed)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setKeyRepeatEnabled(bool enabled)
{
    m_keyRepeat = enabled;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::requestFocus()
{
}

////////////////////////////////////////////////////////////
bool WindowImplEmscripten::hasFocus() const
{
    return true;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::grabFocus()
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::setVideoMode(const VideoMode& mode)
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::resetVideoMode()
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::switchToFullscreen()
{
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::initialize()
{
    auto keyCb = [](int eventType, const EmscriptenKeyboardEvent *e, void* windowptr)->int
    {
        if (e)
        {
            switch(eventType)
            {
                case EMSCRIPTEN_EVENT_KEYDOWN:
                case EMSCRIPTEN_EVENT_KEYPRESS:
                {
                    sf::Event ev;
                    ev.type = sf::Event::KeyPressed;
                    switch(e->keyCode)
                    {
                        case 37: ev.key.code = sf::Keyboard::Key::Left; break;
                        case 38: ev.key.code = sf::Keyboard::Key::Up; break;
                        case 39: ev.key.code = sf::Keyboard::Key::Right; break;
                        case 40: ev.key.code = sf::Keyboard::Key::Down; break;
                        case ' ': ev.key.code = sf::Keyboard::Key::Space; break;
                        default: ev.key.code = static_cast<sf::Keyboard::Key>(e->keyCode); break;
                    }
                    static_cast<WindowImplEmscripten*>(windowptr)->m_events.emplace_back(ev);
                    return true;
                }
                case EMSCRIPTEN_EVENT_KEYUP:
                {
                    sf::Event ev;
                    ev.type = sf::Event::KeyReleased;
                    switch(e->keyCode)
                    {
                        case 37: ev.key.code = sf::Keyboard::Key::Left; break;
                        case 38: ev.key.code = sf::Keyboard::Key::Up; break;
                        case 39: ev.key.code = sf::Keyboard::Key::Right; break;
                        case 40: ev.key.code = sf::Keyboard::Key::Down; break;
                        case ' ': ev.key.code = sf::Keyboard::Key::Space; break;
                    }
                    static_cast<WindowImplEmscripten*>(windowptr)->m_events.emplace_back(ev);
                    return true;
                }
            }
        }
        return false;
    };

    emscripten_set_keypress_callback(NULL, this, true, keyCb);
    emscripten_set_keydown_callback(NULL, this, true, keyCb);
    emscripten_set_keyup_callback(NULL, this, true, keyCb); 

    auto mouseCb = [](int eventType, const EmscriptenMouseEvent *e, void* windowptr)->int
    {
        if (e)
        {
            switch(eventType)
            {
                case EMSCRIPTEN_EVENT_MOUSEDOWN:
                {
                    sf::Event ev;
                    ev.type = sf::Event::MouseButtonPressed;
                    ev.mouseButton.button = static_cast<sf::Mouse::Button>(e->button);
                    ev.mouseButton.x = e->clientX;
                    ev.mouseButton.y = e->clientY;
                    static_cast<WindowImplEmscripten*>(windowptr)->m_events.emplace_back(ev);
                    return true;
                }
                case EMSCRIPTEN_EVENT_MOUSEUP:
                {    
                    sf::Event ev;
                    ev.type = sf::Event::MouseButtonReleased;
                    ev.mouseButton.button = static_cast<sf::Mouse::Button>(e->button);
                    ev.mouseButton.x = e->clientX;
                    ev.mouseButton.y = e->clientY;
                    static_cast<WindowImplEmscripten*>(windowptr)->m_events.emplace_back(ev);
                    return true;
                }
            }
        }
        return false;
    };
    
    emscripten_set_mousedown_callback(NULL, this, true, mouseCb);
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::updateLastInputTime(::Time time)
{
    if (time && (time != m_lastInputTime))
    {
        /*Atom netWmUserTime = getAtom("_NET_WM_USER_TIME", true);

        if (netWmUserTime)
        {
            XChangeProperty(m_display,
                            m_window,
                            netWmUserTime,
                            XA_CARDINAL,
                            32,
                            PropModeReplace,
                            reinterpret_cast<const unsigned char*>(&time),
                            1);
        }*/

        m_lastInputTime = time;
    }
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::createHiddenCursor()
{
    // Create the cursor, using the pixmap as both the shape and the mask of the cursor
    XColor color;
    color.flags = DoRed | DoGreen | DoBlue;
    color.red = color.blue = color.green = 0;
}


////////////////////////////////////////////////////////////
void WindowImplEmscripten::cleanup()
{
    // Restore the previous video mode (in case we were running in fullscreen)
    resetVideoMode();

    // Unhide the mouse cursor (in case it was hidden)
    setMouseCursorVisible(true);
}

////////////////////////////////////////////////////////////
void WindowImplEmscripten::processEvents()
{
    // Pick out the events that are interesting for this window
    // Handle the events for this window that we just picked out
    while (!m_events.empty())
    {
        auto event = m_events.front();
        m_events.pop_front();
        pushEvent(event);
    }

    // Process clipboard window events
    priv::ClipboardImpl::processEvents();
}

} // namespace priv

} // namespace sf
