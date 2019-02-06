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
#include <SFML/Graphics/RenderTextureImplDefault.hpp>
#include <SFML/System/Err.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
RenderTextureImplDefault::RenderTextureImplDefault() :
m_width  (0),
m_height (0)
{

}

////////////////////////////////////////////////////////////
unsigned int RenderTextureImplDefault::getMaximumAntialiasingLevel()
{
    // If the system is so old that it doesn't support FBOs, chances are it is
    // also using either a software renderer or some CPU emulated support for AA
    // In order to not cripple performance in this rare case, we just return 0 here
    return 0;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplDefault::create(unsigned int width, unsigned int height, unsigned int, const ContextSettings& settings)
{
    // Store the dimensions
    m_width = width;
    m_height = height;

    return true;
}


////////////////////////////////////////////////////////////
bool RenderTextureImplDefault::activate(bool active)
{
    //TODO: Needed?
    return true;
}


////////////////////////////////////////////////////////////
void RenderTextureImplDefault::updateTexture(unsigned int textureId)
{
    //TODO
}

} // namespace priv

} // namespace sf
