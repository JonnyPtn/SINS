////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
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
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTextureImplFBO.hpp>
#include <SFML/System/Sleep.hpp>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace sf
{
////////////////////////////////////////////////////////////
RenderWindow::RenderWindow() :
m_defaultFrameBuffer(0),
m_frameTimeLimit(Time::Zero)
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
RenderWindow::RenderWindow(VideoMode mode, const String& title, Uint32 style, const ContextSettings& settings) :
RenderTarget(settings),
m_defaultFrameBuffer(0),
m_frameTimeLimit(Time::Zero)
{
    // Don't call the base class constructor because it contains virtual function calls
    create(mode, title, style);
}


////////////////////////////////////////////////////////////
RenderWindow::RenderWindow(WindowHandle handle, const ContextSettings& settings) :
RenderTarget(settings),
m_defaultFrameBuffer(0),
m_frameTimeLimit(Time::Zero)
{
    // Don't call the base class constructor because it contains virtual function calls
    create(handle);
}


////////////////////////////////////////////////////////////
RenderWindow::~RenderWindow()
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
Vector2u RenderWindow::getSize() const
{
    return Window::getSize();
}

////////////////////////////////////////////////////////////
void RenderWindow::setVerticalSyncEnabled(bool enabled)
{
    //bgfx::reset(getSize().x, getSize().y, enabled ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
}

////////////////////////////////////////////////////////////
void RenderWindow::display()
{
    bgfx::frame();

    // Limit the framerate if needed
    if (m_frameTimeLimit != Time::Zero)
    {
        sleep(m_frameTimeLimit - m_clock.getElapsedTime());
        m_clock.restart();
    }
}

////////////////////////////////////////////////////////////
void RenderWindow::onCreate()
{
    // Set the window handle for bgfx then initialise the rendertarget
    bgfx::PlatformData pd;
    pd.nwh = (void*)getSystemHandle();
    pd.ndt = NULL;
    bgfx::setPlatformData(pd);
    //bgfx::renderFrame();
    RenderTarget::initialize();
}


////////////////////////////////////////////////////////////
void RenderWindow::onResize()
{
    // Update the current view (recompute the viewport, which is stored in relative coordinates)
    setView(getView());
}

////////////////////////////////////////////////////////////
void RenderWindow::setFramerateLimit(unsigned int limit)
{
    if (limit > 0)
        m_frameTimeLimit = seconds(1.f / limit);
    else
        m_frameTimeLimit = Time::Zero;
}

} // namespace sf
