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
    template<typename S1, typename S2>//
    struct ChainingStream final : public Stream<typename S1::ValueType, S1, ChainingStream<S1, S2>> {
        using Self = ChainingStream<S1, S2>;
        using ValueType = typename S1::ValueType;

        private:
        S2 _other_streamable;

        public:
        constexpr ChainingStream(S1 streamable, S2 other_streamable) noexcept :
                Stream<ValueType, S1, Self>(utils::move(streamable)),
                _other_streamable(utils::move(other_streamable)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(!element) {
                element = _other_streamable.next();

                if(!element) {
                    return make_empty<ValueType>();
                }
            }

            return element;
        }
    };
}// namespace kstd::streams