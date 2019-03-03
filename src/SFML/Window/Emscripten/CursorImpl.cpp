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
#include <SFML/Window/Unix/CursorImpl.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <cassert>
#include <cstdlib>
#include <vector>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
CursorImpl::CursorImpl() :
m_cursor(None)
{
    // That's it.
}


////////////////////////////////////////////////////////////
CursorImpl::~CursorImpl()
{
    release();

    CloseDisplay(m_display);
}


////////////////////////////////////////////////////////////
bool CursorImpl::loadFromPixels(const Uint8* pixels, Vector2u size, Vector2u hotspot)
{
    // We assume everything went fine...
    return true;
}


////////////////////////////////////////////////////////////
bool CursorImpl::loadFromSystem(Cursor::Type type)
{
    release();

    unsigned int shape;
    switch (type)
    {
        default: return false;

        case Cursor::Arrow:          shape = XC_arrow;              break;
        case Cursor::Wait:           shape = XC_watch;              break;
        case Cursor::Text:           shape = XC_xterm;              break;
        case Cursor::Hand:           shape = XC_hand1;              break;
        case Cursor::SizeHorizontal: shape = XC_sb_h_double_arrow;  break;
        case Cursor::SizeVertical:   shape = XC_sb_v_double_arrow;  break;
        case Cursor::SizeAll:        shape = XC_fleur;              break;
        case Cursor::Cross:          shape = XC_crosshair;          break;
        case Cursor::Help:           shape = XC_question_arrow;     break;
        case Cursor::NotAllowed:     shape = XC_X_cursor;           break;
    }

    m_cursor = XCreateFontCursor(m_display, shape);
    return true;
}


////////////////////////////////////////////////////////////
void CursorImpl::release()
{
}


} // namespace priv

} // namespace sf

