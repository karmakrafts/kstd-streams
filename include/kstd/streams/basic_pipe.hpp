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
 * @since 15/07/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/option.hpp>

namespace kstd::streams {
    template<typename S>
    struct BasicPipe {
        // clang-format off
        using supplier_type = S;
        using in_type       = typename supplier_type::in_type;
        using out_type      = typename supplier_type::out_type;
        using value_type    = out_type;
        using self          = BasicPipe<supplier_type>;
        // clang-format on

        protected:
        supplier_type _supplier;// NOLINT

        public:
        KSTD_DEFAULT_MOVE_COPY(BasicPipe, self, constexpr);

        BasicPipe(supplier_type supplier) noexcept :
                _supplier(std::move(supplier)) {
        }

        ~BasicPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<out_type> {
            return _supplier.get_next();
        }
    };
}// namespace kstd::streams