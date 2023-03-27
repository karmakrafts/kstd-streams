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

#include <functional>
#include <optional>
#include "stream_fwd.hpp"
#include "concepts.hpp"

namespace cxxs {
    template<typename S, typename F> //
    requires(concepts::is_streamable<S> && std::is_convertible_v<F, std::function<void(typename S::value_type&)>>)
    struct FilteringStream final : public Stream<typename S::value_type, S, FilteringStream<S, F>> {
        using self_type = FilteringStream<S, F>;
        using value_type = typename S::value_type;

        private:

        F _filter;

        public:

        constexpr FilteringStream(S streamable, F&& filter) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)),
                _filter(std::forward<F>(filter)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            if (!element) {
                return std::nullopt;
            }

            while (element && !_filter(*element)) {
                element = this->_streamable.next();
            }

            return element;
        }
    };
}