/**
 * @author Alexander Hinze
 * @since 03/04/2023
 */

#pragma once

#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace cxxs {
    template<typename T, template<typename, typename...> typename C> requires(concepts::is_iterable<C<T>> && concepts::has_erase<C<T>>)
    struct DrainingStreamable final {
        using self_type = DrainingStreamable<T, C>;
        using value_type = T;

        private:

        C<T>& _container;

        public:

        explicit constexpr DrainingStreamable(C<T>& container) noexcept:
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