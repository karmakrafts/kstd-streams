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

#include "kstd/concepts.hpp"
#include "kstd/types.hpp"
#include "concepts.hpp"
#include "stream_macros.hpp"

namespace kstd::streams {
    template<typename I> //
    KSTD_REQUIRES(kstd::concepts::Iterator<I>)
    struct IteratorStreamable;

    template<typename C> //
    KSTD_REQUIRES(kstd::concepts::ConstIterable<C>)
    struct OwningIteratorStreamable;

    template<typename C> //
    KSTD_REQUIRES(kstd::concepts::Iterable<C> && concepts::Erasable<C>)
    struct DrainingStreamable;

    template<typename T> //
    struct SingletStreamable;

    template<typename T> //
    struct CountingStreamable;

    template<typename T, typename S, typename IMPL> //
    KSTD_REQUIRES(concepts::Streamable<S>)
    class Stream;

    template<typename S> //
    struct BasicStream;

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstIterable<C>)
    [[nodiscard]] constexpr auto stream(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>>;

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstIterable<C>)
    [[nodiscard]] constexpr auto owning(C container) noexcept -> BasicStream<OwningIteratorStreamable<C>>;

    template<typename C>
    KSTD_REQUIRES(kstd::concepts::ConstReverseIterable<C>)
    [[nodiscard]] constexpr auto reverse(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>>;

    template<typename C>
    KSTD_REQUIRES((kstd::concepts::Iterable<C> && concepts::Erasable<C>))
    [[nodiscard]] constexpr auto draining(C& container) noexcept -> BasicStream<DrainingStreamable<C>>;

    template<typename T>
    KSTD_REQUIRES(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto singlet(T value) noexcept -> BasicStream<SingletStreamable<T>>;

    template<typename T>
    KSTD_REQUIRES(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto counting(T value, usize max_count) noexcept -> BasicStream<CountingStreamable<T>>;
}