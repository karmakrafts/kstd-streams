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
 * @since 31/03/2023
 */

#pragma once

#include "kstd/option.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename P>//
    struct DroppingStream final : public Stream<typename S::ValueType, S, DroppingStream<S, P>> {
        using Self = DroppingStream<S, P>;
        using ValueType = typename S::ValueType;

        private:
        P _predicate;
        bool _has_dropped;

        public:
        constexpr DroppingStream(S streamable, P&& predicate) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _predicate(utils::forward<P>(predicate)),
                _has_dropped(false) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(_has_dropped) {
                return element;
            }

            if(!element) {
                return make_empty<ValueType>();
            }

            while(element && _predicate(element.borrow())) {
                element = this->_streamable.next();
            }

            _has_dropped = true;
            return element;
        }
    };
}// namespace kstd::streams