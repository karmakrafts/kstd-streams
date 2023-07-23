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

namespace kstd::streams {
    template<typename I>
    struct IteratorPipe final {
        static constexpr bool is_const = std::is_const_v<std::remove_pointer_t<typename I::iterator_type>>;

        // clang-format off
        using iterator      = I;
        using self          = IteratorPipe<iterator>;
        using value_type    = std::conditional_t<
                                std::is_pointer_v<typename iterator::value_type>,
                                typename iterator::value_type,
                                std::conditional_t<
                                    is_const,
                                    const std::remove_cv_t<std::remove_reference_t<typename iterator::value_type>>&,
                                    std::remove_cv_t<std::remove_reference_t<typename iterator::value_type>>&>>;
        // clang-format on

        private:
        iterator _current;
        iterator _end;

        public:
        KSTD_DEFAULT_MOVE_COPY(IteratorPipe, self, constexpr)

        constexpr IteratorPipe() noexcept :
                _current {},
                _end {} {
        }

        constexpr IteratorPipe(iterator begin, iterator end) noexcept :
                _current {begin},
                _end {end} {
        }

        ~IteratorPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<value_type> {
            if(_current == _end) {
                return {};
            }
            if constexpr(is_const) {
                value_type result = *_current;
                _current = std::next(_current);
                return result;
            }
            else {
                return *(_current++);
            }
        }
    };

    static_assert(std::is_same_v<typename IteratorPipe<typename std::vector<std::string>::iterator>::value_type,
                                 std::string&>);
    static_assert(!IteratorPipe<typename std::vector<std::string>::iterator>::is_const);
    static_assert(std::is_same_v<typename IteratorPipe<typename std::vector<std::string>::const_iterator>::value_type,
                                 const std::string&>);
    static_assert(IteratorPipe<typename std::vector<std::string>::const_iterator>::is_const);
    static_assert(std::is_same_v<typename IteratorPipe<typename std::vector<std::string*>::iterator>::value_type,
                                 std::string*>);
    static_assert(!IteratorPipe<typename std::vector<std::string*>::iterator>::is_const);
    static_assert(std::is_same_v<typename IteratorPipe<typename std::vector<std::string*>::const_iterator>::value_type,
                                 std::string*>);
    static_assert(IteratorPipe<typename std::vector<std::string*>::const_iterator>::is_const);
}// namespace kstd::streams