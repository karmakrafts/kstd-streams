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
 * @since 22/07/2023
 */

#pragma once

#include <kstd/option.hpp>
#include <type_traits>

namespace kstd::streams::filters {
    constexpr auto non_zero = [](auto value) noexcept -> bool {
        using Type = decltype(value);
        static_assert(std::is_integral_v<Type> || std::is_floating_point_v<Type>);
        return value != static_cast<Type>(0);
    };

    constexpr auto non_null = [](auto value) noexcept -> bool {
        static_assert(std::is_pointer_v<decltype(value)>);
        return value != nullptr;
    };

    constexpr auto odd = [](auto value) noexcept -> bool {
        static_assert(std::is_integral_v<decltype(value)>);
        return (value & 1) != 0;
    };

    constexpr auto even = [](auto value) noexcept -> bool {
        static_assert(std::is_integral_v<decltype(value)>);
        return (value & 1) == 0;
    };
}// namespace kstd::streams::filters