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
#include <type_traits>

namespace cxxs {
    template<typename T> //
    requires(std::is_move_assignable_v<T>)
    struct SingletStreamable final {
        using value_type = T;

        private:

        value_type _element;
        bool _is_consumed;

        public:

        explicit constexpr SingletStreamable(value_type element) noexcept:
                _element(std::move(element)),
                _is_consumed(false) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            if (_is_consumed) {
                return std::nullopt;
            }

            _is_consumed = true;
            return std::make_optional(std::move(_element));
        }
    };
}