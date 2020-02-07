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
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Err.hpp>
#include <cstring>
#include <mutex>

#include <bgfx/bgfx.h>

namespace
{
    std::mutex isAvailableMutex;
}


namespace sf
{

////////////////////////////////////////////////////////////

struct VertexBuffer::impl
{
    bgfx::DynamicVertexBufferHandle handle = { bgfx::kInvalidHandle };
};

////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer() :
m_size         (0),
m_primitiveType(PrimitiveType::Points),
m_usage        (Stream),
m_impl         (std::make_unique<impl>())
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(PrimitiveType type) :
m_size         (0),
m_primitiveType(type),
m_usage        (Stream),
m_impl(std::make_unique<impl>())
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(VertexBuffer::Usage usage) :
m_size         (0),
m_primitiveType(PrimitiveType::Points),
m_usage        (usage),
m_impl(std::make_unique<impl>())
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(PrimitiveType type, VertexBuffer::Usage usage) :
m_size         (0),
m_primitiveType(type),
m_usage        (usage),
m_impl(std::make_unique<impl>())
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(const VertexBuffer& copy) :
m_size         (0),
m_primitiveType(copy.m_primitiveType),
m_usage        (copy.m_usage),
m_impl(std::make_unique<impl>())
{
    if (bgfx::isValid(copy.m_impl->handle) && copy.m_size)
    {
        if (!create(copy.m_size))
        {
            err() << "Could not create vertex buffer for copying" << std::endl;
            return;
        }

        if (!update(copy))
            err() << "Could not copy vertex buffer" << std::endl;
    }
}


////////////////////////////////////////////////////////////
VertexBuffer::~VertexBuffer()
{
    if (bgfx::isValid(m_impl->handle))
    {
        bgfx::destroy(m_impl->handle);
    }
}


////////////////////////////////////////////////////////////
bool VertexBuffer::create(std::size_t vertexCount)
{
    if (!isAvailable())
        return false;

    if (!bgfx::isValid(m_impl->handle))
    {
        bgfx::VertexLayout vertexLayout;
        vertexLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        m_impl->handle = bgfx::createDynamicVertexBuffer(vertexCount, vertexLayout, BGFX_BUFFER_ALLOW_RESIZE);
    }

    if (!bgfx::isValid(m_impl->handle))
    {
        err() << "Could not create vertex buffer, generation failed" << std::endl;
        return false;
    }

    m_size = vertexCount;

    return true;
}


////////////////////////////////////////////////////////////
std::size_t VertexBuffer::getVertexCount() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const Vertex* vertices)
{
    return update(vertices, m_size, 0);
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const Vertex* vertices, std::size_t vertexCount, unsigned int offset)
{
    // Sanity checks
    if (!bgfx::isValid(m_impl->handle))
        return false;

    if (!vertices)
        return false;

    if (offset && (offset + vertexCount > m_size))
        return false;

    // Check if we need to resize or orphan the buffer
    if (vertexCount != m_size)
    {
        m_size = vertexCount;
        bgfx::destroy(m_impl->handle);
        create(vertexCount);
    }
    
    bgfx::update(m_impl->handle, offset, bgfx::copy(vertices, vertexCount * sizeof(Vertex)));

    return true;
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const VertexBuffer& vertexBuffer)
{

    if (!bgfx::isValid(m_impl->handle) || !bgfx::isValid(vertexBuffer.m_impl->handle))
        return false;


    //TODO: something here?

    return true;

}


////////////////////////////////////////////////////////////
VertexBuffer& VertexBuffer::operator =(const VertexBuffer& right)
{
    VertexBuffer temp(right);

    swap(temp);

    return *this;
}


////////////////////////////////////////////////////////////
void VertexBuffer::swap(VertexBuffer& right)
{
    std::swap(m_size,          right.m_size);
    std::swap(m_impl->handle,  right.m_impl->handle);
    std::swap(m_primitiveType, right.m_primitiveType);
    std::swap(m_usage,         right.m_usage);
}


////////////////////////////////////////////////////////////
unsigned int VertexBuffer::getNativeHandle() const
{
    return m_impl->handle.idx;
}


////////////////////////////////////////////////////////////
void VertexBuffer::bind(const VertexBuffer* vertexBuffer)
{
    if (!isAvailable())
        return;

    //TODO wat?
}


////////////////////////////////////////////////////////////
void VertexBuffer::setPrimitiveType(PrimitiveType type)
{
    m_primitiveType = type;
}


////////////////////////////////////////////////////////////
PrimitiveType VertexBuffer::getPrimitiveType() const
{
    return m_primitiveType;
}


////////////////////////////////////////////////////////////
void VertexBuffer::setUsage(VertexBuffer::Usage usage)
{
    m_usage = usage;
}


////////////////////////////////////////////////////////////
VertexBuffer::Usage VertexBuffer::getUsage() const
{
    return m_usage;
}


////////////////////////////////////////////////////////////
bool VertexBuffer::isAvailable()
{
    std::lock_guard<std::mutex> lock(isAvailableMutex);

    static bool checked = false;
    static bool available = false;

    if (!checked)
    {
        checked = true;

        //TODO: going to assume available?
        available = true;
    }

    return available;
}


////////////////////////////////////////////////////////////
void VertexBuffer::draw(RenderTarget& target, RenderStates states) const
{
    if (bgfx::isValid(m_impl->handle) && m_size)
        target.draw(*this, 0, m_size, states);
}

} // namespace sf
