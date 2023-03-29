// Copyright $year.today Karma Krafts & associates
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
#include <unordered_set>
#include "stream_fwd.hpp"

namespace cxxs {
    template<typename S>
    struct DistinctStream final : public Stream<typename S::value_type, S, DistinctStream<S>> {
        using self_type = DistinctStream<S>;
        using value_type = typename S::value_type;

        private:

        std::unordered_set<value_type> _elements;

        public:

        explicit constexpr DistinctStream(S streamable) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)) {
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            auto element = this->_streamable.next();

            while (element && !_elements.insert(*element).second) {
                element = this->_streamable.next();
            }

            return element;
        }
    };
}