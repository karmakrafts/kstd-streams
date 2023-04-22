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

#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, concepts::Function<bool(typename S::value_type&)> P> //
    struct DroppingStream final : public Stream<typename S::value_type, S, DroppingStream<S, P>> {
        using self_type = DroppingStream<S, P>;
        using value_type = typename S::value_type;

        private:

        P _predicate;
        bool _has_dropped;

        public:

        constexpr DroppingStream(S streamable, P&& predicate) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)),
                _predicate(std::forward<P>(predicate)),
                _has_dropped(false) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (_has_dropped) {
                return element;
            }

            if (!element) {
                return std::nullopt;
            }

            while (element && _predicate(*element)) {
                element = this->_streamable.next();
            }

            _has_dropped = true;
            return element;
        }
    };
}