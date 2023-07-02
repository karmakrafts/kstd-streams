/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename C>
        requires(std::is_same_v<typename S::ValueType, typename C::value_type>)
    constexpr auto collect_into(S& streamable, C& container) noexcept -> void {
        auto element = streamable.next();

        while(element) {
            container.push_back(element.get());
            element = streamable.next();
        }
    }

    template<typename S, typename C>
        requires(std::is_same_v<typename S::ValueType, typename C::ValueType>)
    constexpr auto collect(S& streamable) noexcept -> C {
        C result;
        collect_into(streamable, result);
        return result;
    }
}// namespace kstd::streams