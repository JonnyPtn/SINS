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
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/System/Err.hpp>
#include <cassert>
#include <cstring>
#include <mutex>

#include <bgfx/bgfx.h>

namespace
{
    std::mutex idMutex;

    // Thread-safe unique identifier generator,
    // is used for states cache (see RenderTarget)
    sf::Uint64 getUniqueId()
    {
        std::lock_guard<std::mutex> lock(idMutex);

        static sf::Uint64 id = 1; // start at 1, zero is "no texture"

        return id++;
    }
}


namespace sf
{

////////////////////////////////////////////////////////////
struct  Texture::impl
{
    bgfx::TextureHandle texture = { bgfx::kInvalidHandle };
};


////////////////////////////////////////////////////////////
Texture::Texture() :
m_size         (0, 0),
m_actualSize   (0, 0),
m_isSmooth     (false),
m_sRgb         (false),
m_isRepeated   (false),
m_pixelsFlipped(false),
m_fboAttachment(false),
m_hasMipmap    (false),
m_cacheId      (getUniqueId()),
m_impl         (std::make_unique<impl>())
{
}


////////////////////////////////////////////////////////////
Texture::Texture(const Texture& copy) :
m_size         (0, 0),
m_actualSize   (0, 0),
m_isSmooth     (copy.m_isSmooth),
m_sRgb         (copy.m_sRgb),
m_isRepeated   (copy.m_isRepeated),
m_pixelsFlipped(false),
m_fboAttachment(false),
m_hasMipmap    (false),
m_cacheId      (getUniqueId()),
m_impl(std::make_unique<impl>())
{
    if (bgfx::isValid(copy.m_impl->texture))
    {
        if (create(copy.getSize().x, copy.getSize().y))
        {
            update(copy);
        }
        else
        {
            err() << "Failed to copy texture, failed to create new texture" << std::endl;
        }
    }
}


////////////////////////////////////////////////////////////
Texture::~Texture()
{
    if (bgfx::isValid(m_impl->texture))
        bgfx::destroy(m_impl->texture);
}


////////////////////////////////////////////////////////////
bool Texture::create(unsigned int width, unsigned int height)
{
    // Check if texture parameters are valid before creating it
    if ((width == 0) || (height == 0))
    {
        err() << "Failed to create texture, invalid size (" << width << "x" << height << ")" << std::endl;
        return false;
    }

    // Compute the internal texture dimensions depending on NPOT textures support
    Vector2u actualSize(getValidSize(width), getValidSize(height));

    // Check the maximum texture size
    unsigned int maxSize = getMaximumSize();
    if ((actualSize.x > maxSize) || (actualSize.y > maxSize))
    {
        err() << "Failed to create texture, its internal size is too high "
              << "(" << actualSize.x << "x" << actualSize.y << ", "
              << "maximum is " << maxSize << "x" << maxSize << ")"
              << std::endl;
        return false;
    }

    // All the validity checks passed, we can store the new texture settings
    m_size.x        = width;
    m_size.y        = height;
    m_actualSize    = actualSize;
    m_pixelsFlipped = false;
    m_fboAttachment = false;
    m_hasMipmap = false;

    // Create the texture if it doesn't exist yet
    if (bgfx::isValid(m_impl->texture))
    {
        bgfx::destroy(m_impl->texture);
    }
    m_impl->texture = bgfx::createTexture2D(width, height, m_hasMipmap, 1, bgfx::TextureFormat::RGBA8);

    m_cacheId = getUniqueId();

    return true;
}

////////////////////////////////////////////////////////////
bool Texture::create(uint16_t nativeHandle, unsigned int width, unsigned int height)
{
    m_impl->texture = {nativeHandle};
    m_size = {width, height};
    return bgfx::isValid(m_impl->texture);
}

////////////////////////////////////////////////////////////
bool Texture::loadFromFile(const std::string& filename, const IntRect& area)
{
    Image image;
    return image.loadFromFile(filename) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromMemory(const void* data, std::size_t size, const IntRect& area)
{
    Image image;
    return image.loadFromMemory(data, size) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromStream(InputStream& stream, const IntRect& area)
{
    Image image;
    return image.loadFromStream(stream) && loadFromImage(image, area);
}


////////////////////////////////////////////////////////////
bool Texture::loadFromImage(const Image& image, const IntRect& area)
{
    // Retrieve the image size
    int width = static_cast<int>(image.getSize().x);
    int height = static_cast<int>(image.getSize().y);

    // Load the entire image if the source area is either empty or contains the whole image
    if (area.width == 0 || (area.height == 0) ||
       ((area.left <= 0) && (area.top <= 0) && (area.width >= width) && (area.height >= height)))
    {
        // Load the entire image
        if (create(image.getSize().x, image.getSize().y))
        {
            update(image);

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // Load a sub-area of the image

        // Adjust the rectangle to the size of the image
        IntRect rectangle = area;
        if (rectangle.left   < 0) rectangle.left = 0;
        if (rectangle.top    < 0) rectangle.top  = 0;
        if (rectangle.left + rectangle.width > width)  rectangle.width  = width - rectangle.left;
        if (rectangle.top + rectangle.height > height) rectangle.height = height - rectangle.top;

        // Create the texture and upload the pixels
        if (create(rectangle.width, rectangle.height))
        {
            // Copy the pixels to the texture, row by row
            const Uint8* pixels = image.getPixelsPtr() + 4 * (rectangle.left + (width * rectangle.top));
            const auto size = rectangle.width * rectangle.height * 4;
            bgfx::updateTexture2D(m_impl->texture, 0, 0, rectangle.left, rectangle.top, rectangle.width, rectangle.height, bgfx::makeRef(pixels, size));
            m_hasMipmap = false;

            return true;
        }
        else
        {
            return false;
        }
    }
}


////////////////////////////////////////////////////////////
Vector2u Texture::getSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
Image Texture::copyToImage() const
{
    // Easy case: empty texture
    if (!bgfx::isValid(m_impl->texture))
        return Image();

    // Create an array of pixels
    std::vector<Uint8> pixels(m_size.x * m_size.y * 4);

    // Danger?
    //std::memcpy(pixels.data(), bgfx::getDirectAccessPtr(m_impl->texture), pixels.size());

    // Create the image
    Image image;
    image.create(m_size.x, m_size.y, pixels.data());

    return image;
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* pixels)
{
    // Update the whole texture
    update(pixels, m_size.x, m_size.y, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Uint8* pixels, unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
    assert(x + width <= m_size.x);
    assert(y + height <= m_size.y);

    if (pixels && bgfx::isValid(m_impl->texture))
    {
        const auto size = width * height * 4;
        auto newTexture = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8);
        if (bgfx::isValid(newTexture))
        {
            bgfx::updateTexture2D(newTexture, 0, 0, 0, 0, width, height, bgfx::copy(pixels, size));
            if (bgfx::getCaps()->supported & BGFX_CAPS_TEXTURE_BLIT)
            {
                bgfx::blit(0, m_impl->texture, x, y, newTexture);
            }
            else
            {
                sf::err() << "Texture blits not supported :(";
            }
        }
    
        m_hasMipmap = false;
        m_pixelsFlipped = false;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::update(const Texture& texture)
{
    // Update the whole texture
    update(texture, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Texture& texture, unsigned int x, unsigned int y)
{
    assert(x + texture.m_size.x <= m_size.x);
    assert(y + texture.m_size.y <= m_size.y);

    if (!bgfx::isValid(m_impl->texture))
        return;

    bgfx::blit(0, m_impl->texture, x, y, texture.m_impl->texture);
}


////////////////////////////////////////////////////////////
void Texture::update(const Image& image)
{
    // Update the whole texture
    update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Image& image, unsigned int x, unsigned int y)
{
    update(image.getPixelsPtr(), image.getSize().x, image.getSize().y, x, y);
}


////////////////////////////////////////////////////////////
void Texture::update(const Window& window)
{
    update(window, 0, 0);
}


////////////////////////////////////////////////////////////
void Texture::update(const Window& window, unsigned int x, unsigned int y)
{
    assert(x + window.getSize().x <= m_size.x);
    assert(y + window.getSize().y <= m_size.y);

    if (bgfx::isValid(m_impl->texture))
    {
        // Not sure what to do here... capture in bgfx would happen on the next frame,
        // Need to work out how to do it immediately. Presumably can get the backbuffer data somehow
        m_hasMipmap = false;
        m_pixelsFlipped = true;
        m_cacheId = getUniqueId();
    }
}


////////////////////////////////////////////////////////////
void Texture::setSmooth(bool smooth)
{
    if (smooth != m_isSmooth)
    {
        m_isSmooth = smooth;

        if (bgfx::isValid(m_impl->texture))
        {
            // TODO
        }
    }
}


////////////////////////////////////////////////////////////
bool Texture::isSmooth() const
{
    return m_isSmooth;
}


////////////////////////////////////////////////////////////
void Texture::setSrgb(bool sRgb)
{
    m_sRgb = sRgb;
}


////////////////////////////////////////////////////////////
bool Texture::isSrgb() const
{
    return m_sRgb;
}


////////////////////////////////////////////////////////////
void Texture::setRepeated(bool repeated)
{
    if (repeated != m_isRepeated)
    {
        m_isRepeated = repeated;

        if (bgfx::isValid(m_impl->texture))
        {
            //TODO
        }
    }
}


////////////////////////////////////////////////////////////
bool Texture::isRepeated() const
{
    return m_isRepeated;
}


////////////////////////////////////////////////////////////
bool Texture::generateMipmap()
{
    if (!bgfx::isValid(m_impl->texture))
        return false;

   //TODO

    m_hasMipmap = true;

    return true;
}


////////////////////////////////////////////////////////////
void Texture::invalidateMipmap()
{
    if (!m_hasMipmap)
        return;

    //TODO
    m_hasMipmap = false;
}


////////////////////////////////////////////////////////////
void Texture::bind(const Texture* texture, CoordinateType coordinateType)
{
   //TODO: Needed?
}


////////////////////////////////////////////////////////////
unsigned int Texture::getMaximumSize()
{
    static const auto size = []
    {
        auto caps = bgfx::getCaps();

        return static_cast<unsigned int>(caps->limits.maxTextureSize);
    }();

    return size;
}


////////////////////////////////////////////////////////////
Texture& Texture::operator =(const Texture& right)
{
    Texture temp(right);

    swap(temp);

    return *this;
}


////////////////////////////////////////////////////////////
void Texture::swap(Texture& right)
{
    std::swap(m_size,          right.m_size);
    std::swap(m_actualSize,    right.m_actualSize);
    std::swap(m_impl->texture, right.m_impl->texture);
    std::swap(m_isSmooth,      right.m_isSmooth);
    std::swap(m_sRgb,          right.m_sRgb);
    std::swap(m_isRepeated,    right.m_isRepeated);
    std::swap(m_pixelsFlipped, right.m_pixelsFlipped);
    std::swap(m_fboAttachment, right.m_fboAttachment);
    std::swap(m_hasMipmap,     right.m_hasMipmap);

    m_cacheId = getUniqueId();
    right.m_cacheId = getUniqueId();
}


////////////////////////////////////////////////////////////
unsigned int Texture::getNativeHandle() const
{
    return m_impl->texture.idx;
}


////////////////////////////////////////////////////////////
unsigned int Texture::getValidSize(unsigned int size)
{
    //TODO: needed?
    return size;
}

} // namespace sf
