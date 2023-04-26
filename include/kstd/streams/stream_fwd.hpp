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

namespace kstd::streams {
    template<concepts::Iterator I> //
    struct IteratorStreamable;

    template<concepts::ConstIterable C> //
    struct OwningIteratorStreamable;

    template<concepts::Iterable C> //
    requires(concepts::Erasable<C>)
    struct DrainingStreamable;

    template<typename T> //
    requires(std::is_move_assignable_v<T>)
    struct SingletStreamable;

    template<typename T> //
    struct CountingStreamable;

    template<typename T, concepts::Streamable S, typename IMPL> //
    class Stream;

    template<typename S> //
    struct BasicStream;

    template<concepts::ConstIterable C>
    [[nodiscard]] constexpr auto stream(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>>;

    template<concepts::ConstIterable C>
    [[nodiscard]] constexpr auto owning(C container) noexcept -> BasicStream<OwningIteratorStreamable<C>>;

    template<concepts::ConstReverseIterable C>
    [[nodiscard]] constexpr auto reverse(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>>;

    template<concepts::Iterable C>
    requires(concepts::Erasable<C>)
    [[nodiscard]] constexpr auto draining(C& container) noexcept -> BasicStream<DrainingStreamable<C>>;

    template<typename T>
    requires(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto singlet(T value) noexcept -> BasicStream<SingletStreamable<T>>;

    template<typename T>
    requires(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto counting(T value, size_t max_count) noexcept -> BasicStream<CountingStreamable<T>>;
}