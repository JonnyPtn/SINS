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
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Err.hpp>

#include <bgfx/bgfx.h>

namespace sf
{
////////////////////////////////////////////////////////////
struct RenderTexture::impl
{
    bgfx::FrameBufferHandle handle;
    bgfx::ViewId            view;
};
    
////////////////////////////////////////////////////////////
RenderTexture::RenderTexture() = default;


////////////////////////////////////////////////////////////
RenderTexture::~RenderTexture() = default;

////////////////////////////////////////////////////////////
bool RenderTexture::create(unsigned int width, unsigned int height, const ContextSettings& settings)
{
    // Create the framebuffer
    m_impl = std::make_unique<impl>();
    m_impl->handle = bgfx::createFrameBuffer(width, height, bgfx::TextureFormat::RGBA8);
    
    m_texture.create(bgfx::getTexture(m_impl->handle).idx, width, height);

    // We can now initialize the render target part
    RenderTarget::initialize();

     bgfx::setViewFrameBuffer(m_id, m_impl->handle);

    return bgfx::isValid(m_impl->handle);
}


////////////////////////////////////////////////////////////
void RenderTexture::setSmooth(bool smooth)
{
    m_texture.setSmooth(smooth);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isSmooth() const
{
    return m_texture.isSmooth();
}


////////////////////////////////////////////////////////////
void RenderTexture::setRepeated(bool repeated)
{
    m_texture.setRepeated(repeated);
}


////////////////////////////////////////////////////////////
bool RenderTexture::isRepeated() const
{
    return m_texture.isRepeated();
}


////////////////////////////////////////////////////////////
bool RenderTexture::generateMipmap()
{
    return m_texture.generateMipmap();
}


////////////////////////////////////////////////////////////
void RenderTexture::display()
{
    //TODO Jonny
}


////////////////////////////////////////////////////////////
Vector2u RenderTexture::getSize() const
{
    return m_texture.getSize();
}


////////////////////////////////////////////////////////////
const Texture& RenderTexture::getTexture() const
{
    return m_texture;
}

} // namespace sf
