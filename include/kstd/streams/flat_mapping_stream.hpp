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

#include <memory>
#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S, typename RS, typename M> //
    KSTD_REQUIRES((concepts::Streamable<RS> && kstd::concepts::Function<M, RS(typename S::value_type & )>))
    struct FlatMappingStream final : public Stream<typename RS::value_type, S, FlatMappingStream<S, RS, M>> {
        using self_type = FlatMappingStream<S, RS, M>;
        using value_type = typename RS::value_type;

    private:

        M _mapper;
        std::optional<RS> _current;

    public:

        KSTD_STREAM_CONSTRUCTOR FlatMappingStream(S streamable, M&& mapper) noexcept :
                Stream<value_type, S, self_type>(std::move(streamable)),
                _mapper(std::forward<M>(mapper)),
                _current(std::nullopt) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            while (true) {
                if (_current) {
                    auto element = _current->next();

                    if (!element) {
                        _current.reset();
                        continue;
                    }

                    return element;
                }
                else {
                    auto element = this->_streamable.next();

                    if (!element) {
                        return std::nullopt;
                    }

                    _current = std::make_optional<RS>(std::move(_mapper(*element)));
                    continue;
                }
            }
        }
    };
}