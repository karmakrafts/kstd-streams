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

#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename LS, typename RS, typename LM, typename RM> //
    KSTD_REQUIRES((kstd::concepts::Function<LM, LS(typename S::value_type & )> && kstd::concepts::Function<RM, RS(typename S::value_type & )>))
    struct FlatZippingStream final : public Stream<std::pair<typename LS::value_type, typename RS::value_type>, S, FlatZippingStream<S, LS, RS, LM, RM>> {
        using self_type = FlatZippingStream<S, LS, RS, LM, RM>;
        using left_type = typename LS::value_type;
        using right_type = typename RS::value_type;
        using value_type = std::pair<left_type, right_type>;

        private:

        LM _left_mapper;
        RM _right_mapper;
        std::optional<std::pair<LS, RS>> _current;

        public:

        KSTD_STREAM_CONSTRUCTOR FlatZippingStream(S streamable, LM&& left_mapper, RM&& right_mapper) noexcept :
                Stream<value_type, S, self_type>(std::move(streamable)),
                _left_mapper(std::forward<LM>(left_mapper)),
                _right_mapper(std::forward<RM>(right_mapper)),
                _current(std::nullopt) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            while (true) {
                if (_current) {
                    auto& [left_stream, right_stream] = *_current;
                    auto left_element = left_stream.next();
                    auto right_element = right_stream.next();

                    if (!left_element || !right_element) {
                        _current.reset();
                        continue;
                    }

                    return std::make_optional(std::make_pair(std::move(*left_element), std::move(*right_element)));
                }
                else {
                    auto element = this->_streamable.next();

                    if (!element) {
                        return std::nullopt;
                    }

                    auto& value = *element;
                    _current = std::make_optional(std::make_pair(std::move(_left_mapper(value)), std::move(_right_mapper(value))));
                    continue;
                }
            }
        }
    };
}