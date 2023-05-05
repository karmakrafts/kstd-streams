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

#include <optional>
#include <functional>
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename R, typename S, typename M> //
    KSTD_REQUIRES((kstd::concepts::Function<M, R(typename S::value_type & )>))
    struct MappingStream final : public Stream<R, S, MappingStream<R, S, M>> {
        using self_type = MappingStream<R, S, M>;
        using value_type = R;

    private:

        M _mapper;

    public:

        KSTD_STREAM_CONSTRUCTOR MappingStream(S streamable, M&& mapper) noexcept : Stream<value_type, S, self_type>(std::move(streamable)), _mapper(std::forward<M>(mapper)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (!element) {
                return std::nullopt;
            }

            return std::make_optional(std::move(_mapper(*element)));
        }
    };
}