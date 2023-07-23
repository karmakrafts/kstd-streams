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
 * @since 21/07/2023
 */

#pragma once

#include <functional>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>
#include <vector>

#include "iterator_pipe.hpp"

namespace kstd::streams {
    template<typename PIPE, typename CALLBACK>
    struct BufferedPipe final {
        // clang-format off
        using pipe_type             = PIPE;
        using callback_type         = CALLBACK;
        using self                  = BufferedPipe<pipe_type, callback_type>;
        using buffer_type           = std::vector<
                                        std::remove_cv_t<std::remove_reference_t<typename pipe_type::value_type>>>;
        using delegate_iterator     = typename buffer_type::iterator;
        using delegate_pipe_type    = IteratorPipe<delegate_iterator>;
        using value_type            = typename delegate_pipe_type::value_type;
        // clang-format on

        static_assert(std::is_convertible_v<callback_type, std::function<void(buffer_type&)>>,
                      "Callback signature does not match");

        private:
        buffer_type _buffer;
        delegate_pipe_type _pipe;

        public:
        KSTD_DEFAULT_MOVE_COPY(BufferedPipe, self, constexpr)

        constexpr BufferedPipe() noexcept :
                _buffer {},
                _pipe {} {
        }

        constexpr BufferedPipe(pipe_type pipe, callback_type&& callback) noexcept :
                _buffer {} {
            auto element = pipe.get_next();
            while(element) {
                _buffer.push_back(*element);
                element = pipe.get_next();
            }
            callback(_buffer);
            _pipe = delegate_pipe_type {_buffer.begin(), _buffer.end()};
        }

        ~BufferedPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<value_type> {
            return _pipe.get_next();
        }
    };

    // clang-format off
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string>::iterator>,
            decltype([](auto&) {})>::value_type,
            std::string&>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string>::const_iterator>,
            decltype([](auto&) {})>::value_type,
            std::string&>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string*>::iterator>,
            decltype([](auto&) {})>::value_type,
            std::string*>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string*>::const_iterator>,
            decltype([](auto&) {})>::value_type,
            std::string*>);
    // clang-format on
}// namespace kstd::streams