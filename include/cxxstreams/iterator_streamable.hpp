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
 * @since 27/03/2023
 */

#pragma once

#include <optional>
#include "concepts.hpp"

namespace cxxstreams {
    template<typename I>
    requires(concepts::is_iterator<I>)
    struct IteratorStreamable final {
        using value_type = typename I::value_type;

        private:

        I _current;
        I _end;

        public:

        constexpr IteratorStreamable(I begin, I end) noexcept :
                _current(std::move(begin)),
                _end(std::move(end))
        {}

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            if(_current == _end) {
                return std::nullopt;
            }

            auto& result = *_current;
            ++_current;
            return std::make_optional(std::move(result));
        }
    };
}