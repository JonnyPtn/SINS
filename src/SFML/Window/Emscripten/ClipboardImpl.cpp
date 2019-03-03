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
#include <SFML/Window/Unix/ClipboardImpl.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Err.hpp>
#include <X11/Xatom.h>
#include <vector>


namespace
{
    // Filter the events received by windows (only allow those matching a specific window)
    Bool checkEvent(::Display*, XEvent* event, XPointer userData)
    {
        // Just check if the event matches the window
        return event->xany.window == reinterpret_cast< ::Window >(userData);
    }
}

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
String ClipboardImpl::getString()
{
    return getInstance().getStringImpl();
}


////////////////////////////////////////////////////////////
void ClipboardImpl::setString(const String& text)
{
    getInstance().setStringImpl(text);
}


////////////////////////////////////////////////////////////
void ClipboardImpl::processEvents()
{
    getInstance().processEventsImpl();
}


////////////////////////////////////////////////////////////
ClipboardImpl::ClipboardImpl() :
m_window (0),
m_requestResponded(false)
{
    // Create a hidden window that will broker our clipboard interactions with X
    //m_window = XCreateSimpleWindow(m_display, DefaultRootWindow(m_display), 0, 0, 1, 1, 0, 0, 0);
}


////////////////////////////////////////////////////////////
ClipboardImpl::~ClipboardImpl()
{
}


////////////////////////////////////////////////////////////
ClipboardImpl& ClipboardImpl::getInstance()
{
    static ClipboardImpl instance;

    return instance;
}


////////////////////////////////////////////////////////////
String ClipboardImpl::getStringImpl()
{
    // Check if anybody owns the current selection
    if (XGetSelectionOwner(m_display, m_clipboard) == None)
    {
        m_clipboardContents.clear();

        return m_clipboardContents;
    }

    // Process any already pending events
    processEvents();

    m_requestResponded = false;

    // Request the current selection to be converted to UTF-8 (or STRING
    // if UTF-8 is not available) and written to our window property
    XConvertSelection(
        m_display,
        m_clipboard,
        (m_utf8String != None) ? m_utf8String : XA_STRING,
        m_targetProperty,
        m_window,
        CurrentTime
    );

    Clock clock;

    // Wait for a response for up to 1000ms
    while (!m_requestResponded && (clock.getElapsedTime().asMilliseconds() < 1000))
        processEvents();

    // If no response was received within the time period, clear our clipboard contents
    if (!m_requestResponded)
        m_clipboardContents.clear();

    return m_clipboardContents;
}


////////////////////////////////////////////////////////////
void ClipboardImpl::setStringImpl(const String& text)
{
    m_clipboardContents = text;

    // Set our window as the current owner of the selection
    XSetSelectionOwner(m_display, m_clipboard, m_window, CurrentTime);

    // Check if setting the selection owner was successful
    if (XGetSelectionOwner(m_display, m_clipboard) != m_window)
        err() << "Cannot set clipboard string: Unable to get ownership of X selection" << std::endl;
}


////////////////////////////////////////////////////////////
void ClipboardImpl::processEventsImpl()
{
    XEvent event;

    // Pick out the events that are interesting for this window
    //while (XCheckIfEvent(m_display, &event, &checkEvent, reinterpret_cast<XPointer>(m_window)))
    //    m_events.push_back(event);

    // Handle the events for this window that we just picked out
    while (!m_events.empty())
    {
        event = m_events.front();
        m_events.pop_front();
        processEvent(event);
    }
}


////////////////////////////////////////////////////////////
void ClipboardImpl::processEvent(XEvent& windowEvent)
{
}

} // namespace priv

} // namespace sf
