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
 * @since 10/07/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/types.hpp>
#include <utility>

#include "ref_supplier.hpp"
#include "val_supplier.hpp"

namespace kstd::streams {
    /**
     * @tparam T The type of the elements being processed by this stream.
     * @tparam S The type of the supplier this stream takes its elements from.
     */
    template<typename S>
    struct Stream final {
        // clang-format off
        using supplier_type = S;
        using self          = Stream<supplier_type>;
        using value_type    = typename supplier_type::value_type;
        // clang-format on

        private:
        supplier_type _supplier;

        public:
        KSTD_NO_MOVE_COPY(Stream, self, constexpr);

        constexpr Stream(supplier_type supplier) noexcept :
                _supplier(std::move(_supplier)) {
        }

        ~Stream() noexcept = default;
    };

    template<typename C>
    [[nodiscard]] constexpr auto const_stream(const C& container) noexcept
            -> Stream<RefSupplier<typename C::const_iterator>> {
        using supplier = RefSupplier<typename C::const_iterator>;
        return Stream<supplier>(supplier(container.cbegin(), container.cend()));
    }

    template<typename C>
    [[nodiscard]] constexpr auto stream(const C& container) noexcept -> Stream<RefSupplier<typename C::iterator>> {
        using supplier = RefSupplier<typename C::iterator>;
        return Stream<supplier>(supplier(container.begin(), container.end()));
    }
}// namespace kstd::streams
