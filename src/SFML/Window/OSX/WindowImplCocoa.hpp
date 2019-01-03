////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Marco Antognini (antognini.marco@gmail.com),
//                         Laurent Gomila (laurent@sfml-dev.org)
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

#ifndef SFML_WINDOWIMPLCOCOA_HPP
#define SFML_WINDOWIMPLCOCOA_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowImpl.hpp>
#include <SFML/System/String.hpp>

////////////////////////////////////////////////////////////
/// Predefine OBJ-C classes
////////////////////////////////////////////////////////////
#ifdef __OBJC__

#import <SFML/Window/OSX/WindowImplDelegateProtocol.h>
typedef id<WindowImplDelegateProtocol,NSObject> WindowImplDelegateRef;

#else // If C++

typedef unsigned short unichar; // See NSString.h

typedef void* WindowImplDelegateRef;

#endif

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Mac OS X (Cocoa) implementation of WindowImpl
///
////////////////////////////////////////////////////////////
class WindowImplCocoa : public WindowImpl
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Construct the window implementation from an existing control
    ///
    /// \param handle Platform-specific handle of the control
    ///
    ////////////////////////////////////////////////////////////
    WindowImplCocoa(WindowHandle handle);

    ////////////////////////////////////////////////////////////
    /// \brief Create the window implementation
    ///
    /// \param mode  Video mode to use
    /// \param title Title of the window
    /// \param style Window style (resizeable, fixed, or fullscren)
    ///
    ////////////////////////////////////////////////////////////
    WindowImplCocoa(VideoMode mode, const String& title, unsigned long style);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~WindowImplCocoa() override;

    ////////////////////////////////////////////////////////////
    /// \brief Window Closed Event - called by the cocoa window object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void windowClosed(void);

    ////////////////////////////////////////////////////////////
    /// \brief Window Resized Event - called by the cocoa window object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param width new width
    /// \param height new height
    ///
    ////////////////////////////////////////////////////////////
    void windowResized(unsigned int width, unsigned int height);

    ////////////////////////////////////////////////////////////
    /// \brief Window Lost Focus Event - called by the cocoa window object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void windowLostFocus(void);

    ////////////////////////////////////////////////////////////
    /// \brief Window Get Focus Event - called by the cocoa window object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void windowGainedFocus(void);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse Down Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param button active button
    /// \param x mouse x position
    /// \param y mouse y position
    ///
    ////////////////////////////////////////////////////////////
    void mouseDownAt(Mouse::Button button, int x, int y);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse Up Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param button active button
    /// \param x mouse x position
    /// \param y mouse y position
    ///
    ////////////////////////////////////////////////////////////
    void mouseUpAt(Mouse::Button button, int x, int y);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse Moved Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param x mouse x position
    /// \param y mouse y position
    ///
    ////////////////////////////////////////////////////////////
    void mouseMovedAt(int x, int y);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse Wheel Scrolled Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param deltaX horizontal scrolling delta
    /// \param deltaY vertical scrolling delta
    /// \param x mouse x position
    /// \param y mouse y position
    ///
    ////////////////////////////////////////////////////////////
    void mouseWheelScrolledAt(float deltaX, float deltaY, int x, int y);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse In Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void mouseMovedIn(void);

    ////////////////////////////////////////////////////////////
    /// \brief Mouse Out Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    ////////////////////////////////////////////////////////////
    void mouseMovedOut(void);

    ////////////////////////////////////////////////////////////
    /// \brief Key Down Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param key active key
    ///
    ////////////////////////////////////////////////////////////
    void keyDown(Event::KeyEvent key);

    ////////////////////////////////////////////////////////////
    /// \brief Key Up Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param key active key
    ///
    ////////////////////////////////////////////////////////////
    void keyUp(Event::KeyEvent key);

    ////////////////////////////////////////////////////////////
    /// \brief Text Entred Event - called by the cocoa view object
    ///
    /// Send the event to SFML WindowImpl class.
    ///
    /// \param charcode Unicode input
    ///
    ////////////////////////////////////////////////////////////
    void textEntered(unichar charcode);

    ////////////////////////////////////////////////////////////
    /// \brief Change the type of the current process
    ///
    /// The type of the process is changed to become a full GUI app.
    /// Also ensure NSApp is constructed.
    ///
    ////////////////////////////////////////////////////////////
    static void setUpProcess(void);

public:

    ////////////////////////////////////////////////////////////
    /// \brief Get the OS-specific handle of the window
    ///
    /// \return Handle of the window
    ///
    ////////////////////////////////////////////////////////////
    WindowHandle getSystemHandle() const override;

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the window
    ///
    /// \return Position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    Vector2i getPosition() const override;

    ////////////////////////////////////////////////////////////
    /// \brief Change the position of the window on screen
    ///
    /// \param position New position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    void setPosition(const Vector2i& position) override;

    ////////////////////////////////////////////////////////////
    /// \brief Get the client size of the window
    ///
    /// \return Size of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    Vector2u getSize() const override;

    ////////////////////////////////////////////////////////////
    /// \brief Change the size of the rendering region of the window
    ///
    /// \param size New size, in pixels
    ///
    ////////////////////////////////////////////////////////////
    void setSize(const Vector2u& size) override;

    ////////////////////////////////////////////////////////////
    /// \brief Change the title of the window
    ///
    /// \param title New title
    ///
    ////////////////////////////////////////////////////////////
    void setTitle(const String& title) override;

    ////////////////////////////////////////////////////////////
    /// \brief Change the window's icon
    ///
    /// \param width  Icon's width, in pixels
    /// \param height Icon's height, in pixels
    /// \param pixels Pointer to the pixels in memory, format must be RGBA 32 bits
    ///
    ////////////////////////////////////////////////////////////
    void setIcon(unsigned int width, unsigned int height, const Uint8* pixels) override;

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the window
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    void setVisible(bool visible) override;

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the mouse cursor
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    void setMouseCursorVisible(bool visible) override;

    ////////////////////////////////////////////////////////////
    /// \brief Grab or release the mouse cursor
    ///
    /// \param grabbed True to grab, false to release
    ///
    ////////////////////////////////////////////////////////////
    void setMouseCursorGrabbed(bool grabbed) override;

    ////////////////////////////////////////////////////////////
    /// \brief Set the displayed cursor to a native system cursor
    ///
    /// \param cursor Native system cursor type to display
    ///
    ////////////////////////////////////////////////////////////
    void setMouseCursor(const CursorImpl& cursor) override;

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable automatic key-repeat
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    void setKeyRepeatEnabled(bool enabled) override;

    ////////////////////////////////////////////////////////////
    /// \brief Request the current window to be made the active
    ///        foreground window
    ///
    ////////////////////////////////////////////////////////////
    void requestFocus() override;

    ////////////////////////////////////////////////////////////
    /// \brief Check whether the window has the input focus
    ///
    /// \return True if window has focus, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    bool hasFocus() const override;

protected:

    ////////////////////////////////////////////////////////////
    /// \brief Process incoming events from the operating system
    ///
    ////////////////////////////////////////////////////////////
    void processEvents() override;

private:

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    WindowImplDelegateRef m_delegate;   ///< Implementation in Obj-C.
    bool                  m_showCursor; ///< Is the cursor displayed or hidden?
};

} // namespace priv

} // namespace sf


#endif // SFML_WINDOWIMPLCOCOA_HPP
