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
    template<typename S, kstd::concepts::Function<void(typename S::value_type&)> F> //
    struct PeekingStream final : public Stream<typename S::value_type, S, PeekingStream<S, F>> {
        using self_type = PeekingStream<S, F>;
        using value_type = typename S::value_type;

        private:

        F _function;

        public:

        constexpr PeekingStream(S streamable, F&& function) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)),
                _function(std::forward<F>(function)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (!element) {
                return std::nullopt;
            }

            _function(*element);
            return element;
        }
    };
}