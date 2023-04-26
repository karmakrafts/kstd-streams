/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include <type_traits>
#include "stream_fwd.hpp"
#include "concepts.hpp"

namespace kstd::streams {
    template<concepts::Streamable S, concepts::Pushable C>
    requires(std::is_same_v<typename S::value_type, typename C::value_type>)
    constexpr auto collect_into(S& streamable, C& container) noexcept -> void {
        auto element = streamable.next();

        while (element) {
            container.push_back(element.value());
            element = streamable.next();
        }
    }

    template<concepts::Streamable S, concepts::Pushable C>
    requires(std::is_same_v<typename S::value_type, typename C::value_type>)
    constexpr auto collect(S& streamable) noexcept -> C {
        C result;
        collect_into(streamable, result);
        return result;
    }
}