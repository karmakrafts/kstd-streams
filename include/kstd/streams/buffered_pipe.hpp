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
        using PipeType          = PIPE;
        using CallbackType      = CALLBACK;
        using Self              = BufferedPipe<PipeType, CallbackType>;
        using BufferType        = std::vector<
                                    std::remove_cv_t<std::remove_reference_t<typename PipeType::ValueType>>>;
        using DelegateIterator  = typename BufferType::iterator;
        using DelegatePipeType  = IteratorPipe<DelegateIterator>;
        using ValueType         = typename DelegatePipeType::ValueType;
        // clang-format on

        static_assert(std::is_convertible_v<CallbackType, std::function<void(BufferType&)>>,
                      "Callback signature does not match");

        private:
        BufferType _buffer;
        DelegatePipeType _pipe;

        public:
        KSTD_DEFAULT_MOVE_COPY(BufferedPipe, Self, constexpr)

        constexpr BufferedPipe() noexcept :
                _buffer {},
                _pipe {} {
        }

        constexpr BufferedPipe(PipeType pipe, CallbackType&& callback) noexcept :
                _buffer {} {
            auto element = pipe.get_next();
            while(element) {
                _buffer.push_back(*element);
                element = pipe.get_next();
            }
            callback(_buffer);
            _pipe = DelegatePipeType {_buffer.begin(), _buffer.end()};
        }

        ~BufferedPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<ValueType> {
            return _pipe.get_next();
        }
    };

    // clang-format off
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string>::iterator>,
            decltype([](auto&) {})>::ValueType,
            std::string&>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string>::const_iterator>,
            decltype([](auto&) {})>::ValueType,
            std::string&>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string*>::iterator>,
            decltype([](auto&) {})>::ValueType,
            std::string*>);
    static_assert(std::is_same_v<
            typename BufferedPipe<IteratorPipe<typename std::vector<std::string*>::const_iterator>,
            decltype([](auto&) {})>::ValueType,
            std::string*>);
    // clang-format on
}// namespace kstd::streams