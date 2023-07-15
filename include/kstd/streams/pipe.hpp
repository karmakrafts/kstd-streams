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
 * @since 14/07/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>
#include <utility>

namespace kstd::streams {
    template<typename S, typename IMPL>
    struct IdentityPipe;

    template<typename S, typename T>
    struct Pipe final : public IdentityPipe<S, Pipe<S, T>> {
        // clang-format off
        using supplier_type     = S;
        using sleeve_type       = T;
        using in_type           = typename supplier_type::out_type;
        using self              = Pipe<supplier_type, sleeve_type>;
        // clang-format on

        private:
        sleeve_type _sleeve;

        public:
        // clang-format on
        using out_type = std::decay_t<decltype(_sleeve(std::declval<supplier_type&>()))>;
        // clang-format off

        KSTD_DEFAULT_MOVE_COPY(Pipe, self, constexpr)

        constexpr Pipe(supplier_type supplier, sleeve_type&& sleeve) noexcept :
                IdentityPipe<supplier_type, self>(std::move(supplier)),
                _sleeve(std::forward<sleeve_type>(sleeve)) {
        }

        ~Pipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<out_type> {
            return _sleeve(this->_supplier);
        }
    };

    template<typename S, typename T>
    Pipe(S, T&&) -> Pipe<S, T>;
}// namespace kstd::streams