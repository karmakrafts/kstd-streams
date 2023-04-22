/**
 * @author Alexander Hinze
 * @since 03/04/2023
 */

#pragma once

#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<concepts::Iterable C> //
    requires(concepts::Erasable<C>)
    struct DrainingStreamable final {
        using self_type = DrainingStreamable<C>;
        using value_type = typename C::iterator::value_type;

        private:

        C& _container;

        public:

        explicit constexpr DrainingStreamable(C& container) noexcept:
                _container(container) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto begin = _container.begin();
            auto end = _container.end();

            if (begin == end) {
                return std::nullopt;
            }

            const auto result = std::make_optional(*begin);
            _container.erase(begin);
            return result;
        }
    };
}