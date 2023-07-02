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
 * @since 28/03/2023
 */

#pragma once

#include <kstd/option.hpp>
#include <kstd/utils.hpp>

#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename RS, typename M>//
    struct FlatMappingStream final : public Stream<typename RS::ValueType, S, FlatMappingStream<S, RS, M>> {
        using Self = FlatMappingStream<S, RS, M>;
        using ValueType = typename RS::ValueType;

        private:
        M _mapper;
        Option<RS> _current;

        public:
        constexpr FlatMappingStream(S streamable, M&& mapper) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)),
                _mapper(utils::forward<M>(mapper)),
                _current() {
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            while(true) {
                if(_current) {
                    auto element = _current->next();

                    if(!element) {
                        _current = make_empty<RS>();
                        continue;
                    }

                    return element;
                }

                auto element = this->_streamable.next();

                if(!element) {
                    return make_empty<ValueType>();
                }

                _current = make_value<RS>(_mapper(element.get()));
            }
        }
    };
}// namespace kstd::streams