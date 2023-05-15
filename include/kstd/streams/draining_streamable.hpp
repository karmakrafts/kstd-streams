/**
 * @author Alexander Hinze
 * @since 03/04/2023
 */

#pragma once

#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"
#include "kstd/option.hpp"

namespace kstd::streams {
    template<kstd::concepts::Iterable C> //
    requires(concepts::Erasable<C>)
    struct DrainingStreamable final {
        using self_type = DrainingStreamable<C>;
        using value_type = typename C::iterator::value_type;

        private:

        C& _container;

        public:

        explicit constexpr DrainingStreamable(C& container) noexcept :
                _container(container) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<value_type> {
            auto begin = _container.begin();
            auto end = _container.end();

            if (begin == end) {
                return make_empty<value_type>();
            }

            auto result = make_value<value_type>(*begin);
            _container.erase(begin);
            return result;
        }
    };
}