/**
 * @author Alexander Hinze
 * @since 03/04/2023
 */

#pragma once

#include <kstd/option.hpp>

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename C>//
    struct DrainingStreamable final {
        using Self = DrainingStreamable<C>;
        using ValueType = typename C::iterator::value_type;

        private:
        C& _container;

        public:
        explicit constexpr DrainingStreamable(C& container) noexcept :
                _container(container) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto begin = _container.begin();
            auto end = _container.end();

            if(begin == end) {
                return make_empty<ValueType>();
            }

            auto result = make_value<ValueType>(*begin);
            _container.erase(begin);
            return result;
        }
    };
}// namespace kstd::streams