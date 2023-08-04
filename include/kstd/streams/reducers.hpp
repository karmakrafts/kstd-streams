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
 * @since 24/07/2023
 */

#pragma once

namespace kstd::streams::reducers {
    constexpr auto add = [](auto& current, auto& value) noexcept -> auto {
        return current + value;
    };

    constexpr auto subtract = [](auto& current, auto& value) noexcept -> auto {
        return current - value;
    };

    constexpr auto multiply = [](auto& current, auto& value) noexcept -> auto {
        return current * value;
    };

    constexpr auto divide = [](auto& current, auto& value) noexcept -> auto {
        return current / value;
    };

    constexpr auto modulo = [](auto& current, auto& value) noexcept -> auto {
        return current % value;
    };

    constexpr auto length_sq = [](auto& current, auto& value) noexcept -> auto {
        return current + (value * value);
    };
}// namespace kstd::streams::reducers