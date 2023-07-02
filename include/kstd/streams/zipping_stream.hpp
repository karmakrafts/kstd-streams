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
#include "kstd/tuple.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename L, typename R, typename LM, typename RM>//
    struct ZippingStream final : public Stream<Pair<L, R>, S, ZippingStream<S, L, R, LM, RM>> {
        using Self = ZippingStream<S, L, R, LM, RM>;
        using LeftType = L;
        using RightType = R;
        using ValueType = Pair<LeftType, RightType>;

        private:
        LM _left_mapper;
        RM _right_mapper;

        public:
        constexpr ZippingStream(S streamable, LM&& left_mapper, RM&& right_mapper) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _left_mapper(utils::forward<LM>(left_mapper)),
                _right_mapper(utils::forward<RM>(right_mapper)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(!element) {
                return {};
            }

            auto& value = element.borrow();
            return ValueType(utils::move(_left_mapper(value)), utils::move(_right_mapper(value)));
        }
    };
}// namespace kstd::streams