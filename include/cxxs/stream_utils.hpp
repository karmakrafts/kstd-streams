/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include <type_traits>
#include "stream_fwd.hpp"
#include "concepts.hpp"

namespace cxxs {
    template<concepts::Streamable S, template<typename, typename...> typename C>
    requires(std::is_default_constructible_v<C<typename S::value_type>>)
    constexpr auto collect_into(S& streamable, C<typename S::value_type>& container) noexcept -> void {
        auto element = streamable.next();

        while (element) {
            container.push_back(element.value());
            element = streamable.next();
        }
    }

    template<concepts::Streamable S, template<typename, typename...> typename C>
    requires(std::is_default_constructible_v<C<typename S::value_type>>)
    constexpr auto collect(S& streamable) noexcept -> C<typename S::value_type> {
        C<typename S::value_type> result;
        collect_into(streamable, result);
        return result;
    }
}