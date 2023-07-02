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

#include "kstd/option.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename C>//
    struct OwningIteratorStreamable final {
        using Self = OwningIteratorStreamable<C>;
        using Iterator = typename C::const_iterator;
        using ValueType = typename Iterator::value_type;

        private:
        C _container;
        Iterator _current;
        Iterator _end;

        public:
        explicit constexpr OwningIteratorStreamable(C container) noexcept :
                _container(utils::move(container)),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable() noexcept :
                _container(),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable(const Self& other) noexcept :
                _container(other._container),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        constexpr OwningIteratorStreamable(Self&& other) noexcept :
                _container(utils::move(other._container)),
                _current(_container.cbegin()),
                _end(_container.cend()) {
        }

        ~OwningIteratorStreamable() noexcept = default;

        constexpr auto operator=(const Self& other) noexcept -> Self& {
            _container = other._container;
            _current = _container.cbegin();
            _end = _container.cend();
            return *this;
        }

        constexpr auto operator=(Self&& other) noexcept -> Self& {
            _container = utils::move(other._container);
            _current = _container.cbegin();
            _end = _container.cend();
            return *this;
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            if(_current == _end) {
                return make_empty<ValueType>();
            }

            return make_value<ValueType>(*_current++);
        }
    };
}// namespace kstd::streams