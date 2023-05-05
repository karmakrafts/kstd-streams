/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename C>
    KSTD_REQUIRES((concepts::Streamable<S> && concepts::Pushable<C> &&
                   std::is_same_v<typename S::value_type, typename C::value_type>))
    constexpr auto collect_into(S& streamable, C& container) noexcept -> void {
        auto element = streamable.next();

        while (element) {
            container.push_back(element.value());
            element = streamable.next();
        }
    }

    template<concepts::Streamable S, concepts::Pushable C>
    KSTD_REQUIRES((concepts::Streamable<S> && concepts::Pushable<C> &&
                   std::is_same_v<typename S::value_type, typename C::value_type>))
    constexpr auto collect(S& streamable) noexcept -> C {
        C result;
        collect_into(streamable, result);
        return result;
    }
}