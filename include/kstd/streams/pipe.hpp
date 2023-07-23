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

#include <functional>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>

namespace kstd::streams {
    template<typename PIPE, typename SLEEVE>
    struct Pipe final {
        // clang-format off
        using pipe_type     = PIPE;
        using sleeve_type   = SLEEVE;
        using self          = Pipe<pipe_type, sleeve_type>;
        using value_type    = typename decltype(std::declval<sleeve_type&&>()(std::declval<pipe_type&>()))::value_type;
        // clang-format on

        private:
        pipe_type _pipe;
        sleeve_type _sleeve;

        public:
        KSTD_DEFAULT_MOVE_COPY(Pipe, self, constexpr)

        constexpr Pipe() noexcept :
                _pipe {},
                _sleeve {} {
        }

        constexpr Pipe(pipe_type pipe, sleeve_type&& sleeve) noexcept :
                _pipe {std::move(pipe)},
                _sleeve {std::forward<sleeve_type>(sleeve)} {
        }

        ~Pipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<value_type> {
            return _sleeve(_pipe);
        }
    };
}// namespace kstd::streams