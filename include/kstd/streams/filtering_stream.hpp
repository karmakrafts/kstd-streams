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
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename F>//
    struct FilteringStream final : public Stream<typename S::ValueType, S, FilteringStream<S, F>> {
        using Self = FilteringStream<S, F>;
        using ValueType = typename S::ValueType;

        private:
        F _filter;

        public:
        constexpr FilteringStream(S streamable, F&& filter) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _filter(utils::forward<F>(filter)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(!element) {
                return make_empty<ValueType>();
            }

            while(element && !_filter(element.borrow())) {
                element = this->_streamable.next();
            }

            return element;
        }
    };
}// namespace kstd::streams