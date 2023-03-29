// Copyright $year.today Karma Krafts & associates
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

#include <optional>
#include <functional>
#include <type_traits>
#include <tuple>
#include "stream_fwd.hpp"

namespace cxxs {
    template<typename S, typename LM, typename L, typename RM, typename R> //
    requires(std::is_convertible_v<LM, std::function<L(typename S::value_type&)>>&& std::is_convertible_v<RM, std::function<R(typename S::value_type & )>>)
    struct ZippingStream final : public Stream<std::pair<L, R>, S, ZippingStream<S, LM, L, RM, R>> {
        using self_type = ZippingStream<S, LM, L, RM, R>;
        using value_type = std::pair<L, R>;

        private:

        LM _left_mapper;
        RM _right_mapper;

        public:

        constexpr ZippingStream(S streamable, LM&& left_mapper, RM&& right_mapper) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)),
                _left_mapper(std::forward<LM>(left_mapper)),
                _right_mapper(std::forward<RM>(right_mapper)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (!element) {
                return std::nullopt;
            }

            auto& value = *element;
            return std::make_optional(std::make_pair(std::move(_left_mapper(value)), std::move(_right_mapper(value))));
        }
    };
}