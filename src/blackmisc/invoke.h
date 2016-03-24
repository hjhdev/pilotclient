/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INVOKE_H
#define BLACKMISC_INVOKE_H

#include <type_traits>

namespace BlackMisc
{
    //! \cond PRIVATE
    namespace Private
    {

        // Our own version of C++17 std::invoke().
        template <typename F, typename T, typename = std::enable_if_t<std::is_member_object_pointer<F>::value>>
        decltype(auto) invoke(F ptr, T &&object)
        {
            return std::forward<T>(object).*ptr;
        }
        template <typename F, typename T, typename... Ts, typename = std::enable_if_t<std::is_member_function_pointer<F>::value>>
        decltype(auto) invoke(F ptr, T &&object, Ts &&... args)
        {
            return (std::forward<T>(object).*ptr)(std::forward<Ts>(args)...);
        }
        template <typename F, typename... Ts, typename = std::enable_if_t<! std::is_member_pointer<std::decay_t<F>>::value>>
        decltype(auto) invoke(F &&func, Ts &&... args)
        {
            return std::forward<F>(func)(std::forward<Ts>(args)...);
        }

        // Like invoke() but ignores the first argument if callable is not a member function. For uniform calling of callables with slot semantics.
        template <typename F, typename T, typename... Ts, typename = std::enable_if_t<std::is_member_function_pointer<F>::value>>
        decltype(auto) invokeSlot(F ptr, T *object, Ts &&... args)
        {
            return (object->*ptr)(std::forward<Ts>(args)...);
        }
        template <typename F, typename T, typename... Ts, typename = std::enable_if_t<! std::is_member_pointer<std::decay_t<F>>::value>>
        decltype(auto) invokeSlot(F &&func, T *, Ts &&... args)
        {
            return std::forward<F>(func)(std::forward<Ts>(args)...);
        }

    }
    //! \endcond
}

#endif