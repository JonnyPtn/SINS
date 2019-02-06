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

#ifndef SFML_COMPATIBILITY_HPP
#define SFML_COMPATIBILITY_HPP

#ifdef SFML_COMPAT
    #define SFML_COMPAT_ENUM enum
#else
    #define SFML_COMPAT_ENUM enum class
#endif

#ifdef SFML_COMPAT
    #define SFML_COMPAT_ALIAS(newName, aliasExpr) static constexpr auto newName = aliasExpr;
#else
    #define SFML_COMPAT_ALIAS(newName, aliasExpr)
#endif

#endif // SFML_COMPATIBILITY_HPP
