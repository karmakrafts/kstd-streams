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
 * @since 23/07/2023
 */

#pragma once

namespace kstd::streams::comparators {
    constexpr auto deref_less_than = [](auto* lhs, auto* rhs) noexcept -> bool {
        return (*lhs) < (*rhs);
    };

    constexpr auto deref_greater_than = [](auto* lhs, auto* rhs) noexcept -> bool {
        return (*lhs) > (*rhs);
    };
}// namespace kstd::streams::comparators