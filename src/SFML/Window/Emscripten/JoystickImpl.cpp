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
#include <SFML/Window/JoystickImpl.hpp>
#include <SFML/System/Err.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include <string>
#include <cstring>

namespace
{
    struct JoystickRecord
    {
        std::string deviceNode;
        std::string systemPath;
        bool plugged;
    };

    typedef std::vector<JoystickRecord> JoystickList;
    JoystickList joystickList;
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
JoystickImpl::JoystickImpl() :
m_file(-1)
{
    //std::fill(m_mapping, m_mapping + ABS_MAX + 1, 0);
}


////////////////////////////////////////////////////////////
void JoystickImpl::initialize()
{
}


////////////////////////////////////////////////////////////
void JoystickImpl::cleanup()
{
}


////////////////////////////////////////////////////////////
bool JoystickImpl::isConnected(unsigned int index)
{
    if (index >= joystickList.size())
        return false;

    // Then check if the joystick is connected
    return joystickList[index].plugged;
}

////////////////////////////////////////////////////////////
bool JoystickImpl::open(unsigned int index)
{
    if (index >= joystickList.size())
        return false;

    if (joystickList[index].plugged)
    {
        std::string devnode = joystickList[index].deviceNode;

        // Open the joystick's file descriptor (read-only and non-blocking)
        m_file = ::open(devnode.c_str(), O_RDONLY | O_NONBLOCK);
        if (m_file >= 0)
        {
            return true;
        }
        else
        {
            err() << "Failed to open joystick " << devnode << ": " << errno << std::endl;
        }
    }

    return false;
}


////////////////////////////////////////////////////////////
void JoystickImpl::close()
{
    ::close(m_file);
    m_file = -1;
}


////////////////////////////////////////////////////////////
JoystickCaps JoystickImpl::getCapabilities() const
{
    JoystickCaps caps;

    if (m_file < 0)
        return caps;

    // Get the number of buttons
    char buttonCount;
    caps.buttonCount = buttonCount;
    if (caps.buttonCount > Joystick::ButtonCount)
        caps.buttonCount = Joystick::ButtonCount;

    return caps;
}


////////////////////////////////////////////////////////////
Joystick::Identification JoystickImpl::getIdentification() const
{
    return m_identification;
}


////////////////////////////////////////////////////////////
JoystickState JoystickImpl::JoystickImpl::update()
{
    return {};
}

} // namespace priv

} // namespace sf
