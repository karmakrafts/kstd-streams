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

#include <kstd/option.hpp>
#include <kstd/utils.hpp>

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename R, typename S, typename M>//
    struct MappingStream final : public Stream<R, S, MappingStream<R, S, M>> {
        using Self = MappingStream<R, S, M>;
        using ValueType = R;

        private:
        M _mapper;

        public:
        constexpr MappingStream(S streamable, M&& mapper) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _mapper(utils::forward<M>(mapper)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            auto element = this->_streamable.next();

            if(!element) {
                return make_empty<ValueType>();
            }

            return make_value<ValueType>(utils::move(_mapper(element.borrow())));
        }
    };
}// namespace kstd::streams