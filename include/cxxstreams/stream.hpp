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

#include "concepts.hpp"
#include "stream_fwd.hpp"

#include "iterator_streamable.hpp"
#include "singlet_streamable.hpp"

#include "basic_stream.hpp"

namespace cxxstreams {
    template<typename T, typename S, typename IMPL>
    requires(concepts::is_streamable<S>)
    class Stream {
        protected:

        S _streamable;

        [[nodiscard]] constexpr auto get_self() noexcept -> IMPL& {
            static_assert(concepts::is_streamable<IMPL>, "Implementation is not streamable");
            return static_cast<IMPL&>(*this);
        }

        public:

        explicit constexpr Stream(S streamable) noexcept :
                _streamable(std::move(streamable))
        {}
    };

    template<typename T, template<typename, typename...> typename C>
    requires(concepts::is_const_iterable<C<T>>)
    [[nodiscard]] constexpr auto make_stream(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>> {
        using S = IteratorStreamable<typename C<T>::const_iterator>;
        return BasicStream<S>(S(container.cbegin(), container.cend()));
    }
}