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

#include <fmt/format.h>
#include <kstd/non_zero.hpp>
#include <kstd/option.hpp>

namespace kstd::streams::mappers {
    constexpr auto dereference = [](auto* value) noexcept -> auto& {
        return *value;
    };

    constexpr auto address_of = [](auto& value) noexcept -> auto* {
        return &value;
    };

    constexpr auto as_option = [](auto& value) noexcept -> Option<decltype(value)> {
        return value;
    };

    constexpr auto as_non_zero = [](auto& value) noexcept -> NonZero<decltype(value)> {
        return NonZero<decltype(value)> {value};
    };

    constexpr auto first = [](auto& value) noexcept -> auto& {
        return value.first;
    };

    constexpr auto second = [](auto& value) noexcept -> auto& {
        return value.second;
    };

    template<typename... ARGS>
    [[nodiscard]] constexpr auto format(const std::string& format, ARGS&&... args) noexcept {
        return [&format, &args...](auto& value) -> std::string {
            if constexpr(sizeof...(ARGS) > 0) {
                return fmt::format(fmt::runtime(format), fmt::arg("x", value), std::forward<ARGS>(args)...);
            }
            else {
                return fmt::format(fmt::runtime(format), value, std::forward<ARGS>(args)...);
            }
        };
    }
}// namespace kstd::streams::mappers