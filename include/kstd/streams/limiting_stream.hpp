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
 * @since 27/03/2023
 */

#pragma once

#include "kstd/option.hpp"
#include "kstd/types.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S>//
    struct LimitingStream final : public Stream<typename S::ValueType, S, LimitingStream<S>> {
        using Self = LimitingStream<S>;
        using ValueType = typename S::ValueType;

        private:
        usize _max_count;
        usize _count;

        public:
        constexpr LimitingStream(S streamable, usize max_count) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _max_count(max_count),
                _count(0) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            if(_count == _max_count) {
                return make_empty<ValueType>();
            }

            ++_count;
            return this->_streamable.next();
        }
    };
}// namespace kstd::streams