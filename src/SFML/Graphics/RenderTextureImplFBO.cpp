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
#include <SFML/Graphics/RenderTextureImplFBO.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Err.hpp>
#include <utility>
#include <set>
#include <mutex>


namespace
{
    // Set to track all active FBO mappings
    // This is used to free active FBOs while their owning
    // RenderTextureImplFBO is still alive
    std::set<std::map<sf::Uint64, unsigned int>*> frameBuffers;

    // Set to track all stale FBOs
    // This is used to free stale FBOs after their owning
    // RenderTextureImplFBO has already been destroyed
    // An FBO cannot be destroyed until it's containing context
    // becomes active, so the destruction of the RenderTextureImplFBO
    // has to be decoupled from the destruction of the FBOs themselves
    std::set<std::pair<sf::Uint64, unsigned int> > staleFrameBuffers;

    // Mutex to protect both active and stale frame buffer sets
    std::mutex mutex;

    // This function is called either when a RenderTextureImplFBO is
    // destroyed or via contextDestroyCallback when context destruction
    // might trigger deletion of its contained stale FBOs
    //void destroyStaleFBOs()
    //{
    //    sf::Uint64 contextId = sf::Context::getActiveContextId();
    //
    //    for (std::set<std::pair<sf::Uint64, unsigned int> >::iterator iter = staleFrameBuffers.begin(); iter != staleFrameBuffers.end();)
    //    {
    //        if (iter->first == contextId)
    //        {
    //            GLuint frameBuffer = static_cast<GLuint>(iter->second);
    //            glCheck(GLEXT_glDeleteFramebuffers(1, &frameBuffer));
    //
    //            staleFrameBuffers.erase(iter++);
    //        }
    //        else
    //        {
    //            ++iter;
    //        }
    //    }
    //}

    // Callback that is called every time a context is destroyed
    //void contextDestroyCallback(void* arg)
    //{
    //    std::lock_guard<std::mutex> lock(mutex);
    //
    //    sf::Uint64 contextId = sf::Context::getActiveContextId();
    //
    //    // Destroy active frame buffer objects
    //    for (std::set<std::map<sf::Uint64, unsigned int>*>::iterator frameBuffersIter = frameBuffers.begin(); frameBuffersIter != frameBuffers.end(); ++frameBuffersIter)
    //    {
    //        for (std::map<sf::Uint64, unsigned int>::iterator iter = (*frameBuffersIter)->begin(); iter != (*frameBuffersIter)->end(); ++iter)
    //        {
    //            if (iter->first == contextId)
    //            {
    //                GLuint frameBuffer = static_cast<GLuint>(iter->second);
    //                glCheck(GLEXT_glDeleteFramebuffers(1, &frameBuffer));
    //
    //                // Erase the entry from the RenderTextureImplFBO's map
    //                (*frameBuffersIter)->erase(iter);
    //
    //                break;
    //            }
    //        }
    //    }
    //
    //    // Destroy stale frame buffer objects
    //    destroyStaleFBOs();
    //}
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
RenderTextureImplFBO::RenderTextureImplFBO() :
m_depthStencilBuffer(0),
m_colorBuffer       (0),
m_width             (0),
m_height            (0),
m_textureId         (0),
m_multisample       (false),
m_stencil           (false)
{
    std::lock_guard<std::mutex> lock(mutex);

    // Insert the new frame buffer mapping into the set of all active mappings
    frameBuffers.insert(&m_frameBuffers);
    frameBuffers.insert(&m_multisampleFrameBuffers);
}


////////////////////////////////////////////////////////////
RenderTextureImplFBO::~RenderTextureImplFBO()
{
    std::lock_guard<std::mutex> lock(mutex);

    // Remove the frame buffer mapping from the set of all active mappings
    frameBuffers.erase(&m_frameBuffers);
    frameBuffers.erase(&m_multisampleFrameBuffers);

    // Destroy the color buffer
    if (m_colorBuffer)
    {
        //TODO ??
        //GLuint colorBuffer = static_cast<GLuint>(m_colorBuffer);
        //glCheck(GLEXT_glDeleteRenderbuffers(1, &colorBuffer));
    }

    // Destroy the depth/stencil buffer
    if (m_depthStencilBuffer)
    {
        // TODO ??
        //GLuint depthStencilBuffer = static_cast<GLuint>(m_depthStencilBuffer);
        //glCheck(GLEXT_glDeleteRenderbuffers(1, &depthStencilBuffer));
    }

    // Move all frame buffer objects to stale set
    for (std::map<Uint64, unsigned int>::iterator iter = m_frameBuffers.begin(); iter != m_frameBuffers.end(); ++iter)
        staleFrameBuffers.insert(std::make_pair(iter->first, iter->second));

    for (std::map<Uint64, unsigned int>::iterator iter = m_multisampleFrameBuffers.begin(); iter != m_multisampleFrameBuffers.end(); ++iter)
        staleFrameBuffers.insert(std::make_pair(iter->first, iter->second));
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::isAvailable()
{
    // TODO: needed?
    return false;
}


////////////////////////////////////////////////////////////
unsigned int RenderTextureImplFBO::getMaximumAntialiasingLevel()
{
    //TODO something?
    return 0;
}


////////////////////////////////////////////////////////////
void RenderTextureImplFBO::unbind()
{
    //TODO: something?
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::create(unsigned int width, unsigned int height, unsigned int textureId, const ContextSettings& settings)
{
    // Store the dimensions
    m_width = width;
    m_height = height;

    {
        //TODO: create a framebuffer something something
    }

    // Save our texture ID in order to be able to attach it to an FBO at any time
    m_textureId = textureId;

    return false;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::createFrameBuffer()
{
    //TODO: something or other
    return true;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplFBO::activate(bool active)
{
    // Unbind the FBO if requested
    if (!active)
    {
        return true;
    }
    //TODO: needed?
    return true;
}


////////////////////////////////////////////////////////////
void RenderTextureImplFBO::updateTexture(unsigned int)
{
   //TODO
}

} // namespace priv

} // namespace sf
