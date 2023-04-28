// Copyright 2023 Karma Krafts & associates
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @author Alexander Hinze
 * @since 29/03/2023
 */

#pragma once

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<kstd::concepts::ConstIterable C> //
    struct OwningIteratorStreamable final {
        using self_type = OwningIteratorStreamable<C>;
        using iterator = typename C::const_iterator;
        using value_type = typename iterator::value_type;

        private:

        C _container;
        iterator _current;
        iterator _end;

        public:

        explicit constexpr OwningIteratorStreamable(C container) noexcept:
                _container(std::move(container)),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable() noexcept:
                _container(),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable(const self_type& other) noexcept:
                _container(other._container),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable(self_type&& other) noexcept:
                _container(std::move(other._container)),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr auto operator =(const self_type& other) noexcept -> self_type& {
            _container = other._container;
            _current = _container.cbegin();
            _end = _container.cend();
            return *this;
        }

        constexpr auto operator =(self_type&& other) noexcept -> self_type& {
            _container = std::move(other._container);
            _current = _container.cbegin();
            _end = _container.cend();
            return *this;
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            if (_current == _end) {
                return std::nullopt;
            }

            return std::make_optional(*_current++);
        }
    };
}