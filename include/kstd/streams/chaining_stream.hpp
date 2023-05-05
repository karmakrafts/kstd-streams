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
#include <type_traits>
#include "stream_fwd.hpp"

namespace kstd::streams {
    template<typename S1, typename S2> //
    KSTD_REQUIRES((std::same_as<typename S1::value_type, typename S2::value_type>))
    struct ChainingStream final : public Stream<typename S1::value_type, S1, ChainingStream<S1, S2>> {
        using self_type = ChainingStream<S1, S2>;
        using value_type = typename S1::value_type;

    private:

        S2 _other_streamable;

    public:

        KSTD_STREAM_CONSTRUCTOR ChainingStream(S1 streamable, S2 other_streamable) noexcept :
                Stream<value_type, S1, self_type>(std::move(streamable)),
                _other_streamable(std::move(other_streamable)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (!element) {
                element = _other_streamable.next();

                if (!element) {
                    return std::nullopt;
                }
            }

            return element;
        }
    };
}