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

#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"
#include "kstd/option.hpp"

namespace kstd::streams {
    template<typename S, typename LS, typename RS, kstd::concepts::Function<LS(typename S::value_type&)> LM, kstd::concepts::Function<RS(typename S::value_type&)> RM> //
    struct FlatZippingStream final : public Stream<std::pair<typename LS::value_type, typename RS::value_type>, S, FlatZippingStream<S, LS, RS, LM, RM>> {
        using self_type = FlatZippingStream<S, LS, RS, LM, RM>;
        using left_type = typename LS::value_type;
        using right_type = typename RS::value_type;
        using value_type = std::pair<left_type, right_type>;

        private:

        LM _left_mapper;
        RM _right_mapper;
        Option<std::pair<LS, RS>> _current;

        public:

        constexpr FlatZippingStream(S streamable, LM&& left_mapper, RM&& right_mapper) noexcept :
                Stream<value_type, S, self_type>(std::move(streamable)),
                _left_mapper(std::forward<LM>(left_mapper)),
                _right_mapper(std::forward<RM>(right_mapper)),
                _current() {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<value_type> {
            while (true) {
                if (_current) {
                    auto& [left_stream, right_stream] = _current.borrow_value();
                    auto left_element = left_stream.next();
                    auto right_element = right_stream.next();

                    if (!left_element || !right_element) {
                        _current = {};
                        continue;
                    }

                    return std::make_pair(std::move(*left_element), std::move(*right_element));
                }
                else {
                    auto element = this->_streamable.next();

                    if (!element) {
                        return {};
                    }

                    auto& value = element.borrow_value();
                    _current = std::make_pair(std::move(_left_mapper(value)), std::move(_right_mapper(value)));
                    continue;
                }
            }
        }
    };
}