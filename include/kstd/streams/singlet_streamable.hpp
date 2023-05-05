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
#include "stream_macros.hpp"

namespace kstd::streams {
    template<typename T> //
    struct SingletStreamable final {
        using value_type = T;

    private:

        value_type _element;
        bool _has_data;

    public:

        explicit KSTD_STREAM_CONSTRUCTOR SingletStreamable(value_type element) noexcept : _element(std::move(element)), _has_data(true) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            if (!_has_data) {
                return std::nullopt;
            }

            _has_data = false;
            return std::make_optional(std::move(_element));
        }
    };
}