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

#include "concepts.hpp"

namespace cxxs {
    template<concepts::Iterator I> //
    struct IteratorStreamable;

    template<concepts::ConstIterable C> //
    struct OwningIteratorStreamable;

    template<typename T, concepts::Streamable S, typename IMPL> //
    class Stream;

    template<typename S> //
    struct BasicStream;

    template<typename T, template<typename, typename...> typename C>
    requires(std::is_copy_assignable_v<T> && concepts::ConstIterable<C<T>>)
    [[nodiscard]] constexpr auto stream(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>>;

    template<typename K, typename V, template<typename, typename, typename...> typename M>
    requires(std::is_copy_assignable_v<K> && std::is_copy_assignable_v<V> && concepts::ConstIterable<M<K, V>>)
    [[nodiscard]] constexpr auto stream(const M<K, V>& container) noexcept -> BasicStream<IteratorStreamable<typename M<K, V>::const_iterator>>;

    template<typename T, template<typename, typename...> typename C>
    requires(std::is_copy_assignable_v<T> && concepts::ConstIterable<C<T>>)
    [[nodiscard]] constexpr auto owning(C<T> container) noexcept -> BasicStream<OwningIteratorStreamable<C < T>>
    >;
}