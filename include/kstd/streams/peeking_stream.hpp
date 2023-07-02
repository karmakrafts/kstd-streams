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
 * @since 29/03/2023
 */

#pragma once

#include "kstd/option.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename F>//
    struct PeekingStream final : public Stream<typename S::ValueType, S, PeekingStream<S, F>> {
        using Self = PeekingStream<S, F>;
        using ValueType = typename S::ValueType;

        private:
        F _function;

        public:
        constexpr PeekingStream(S streamable, F&& function) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _function(utils::forward<F>(function)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(!element) {
                return make_empty<ValueType>();
            }

            _function(*element);
            return element;
        }
    };
}// namespace kstd::streams