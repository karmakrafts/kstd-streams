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
 * @since 17/07/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>

namespace kstd::streams {
    template<typename SUPPLIER>
    struct SupplierPipe final {
        // clang-format off
        using SupplierType  = SUPPLIER;
        using Self          = SupplierPipe<SupplierType>;
        using ValueType     = typename decltype(std::declval<SupplierType&&>()())::value_type;
        // clang-format on

        private:
        SupplierType _supplier;

        public:
        KSTD_DEFAULT_MOVE_COPY(SupplierPipe, Self, constexpr)

        constexpr SupplierPipe() noexcept :
                _supplier {} {
        }

        explicit constexpr SupplierPipe(SupplierType supplier) noexcept :
                _supplier(std::move(supplier)) {
        }

        ~SupplierPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<ValueType> {
            return _supplier();
        }
    };
}// namespace kstd::streams