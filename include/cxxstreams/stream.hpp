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
#include "filtering_stream.hpp"
#include "limiting_stream.hpp"

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

        template<typename F>
        requires(std::is_convertible_v<F, std::function<void(T&)>>)
        [[nodiscard]] constexpr auto filter(F&& filter) noexcept -> FilteringStream<IMPL, F> {
            return FilteringStream(std::move(get_self()), std::forward<F>(filter));
        }

        [[nodiscard]] constexpr auto limit(size_t max_count) noexcept -> LimitingStream<IMPL> {
            return LimitingStream(std::move(get_self()), max_count);
        }

        template<template<typename, typename...> typename C>
        requires(std::is_default_constructible_v<C<T>> && (concepts::has_add_assign<C<T>> || concepts::has_push_back<C<T>>))
        [[nodiscard]] constexpr auto collect() noexcept -> C<T> {
            C<T> result;
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                if constexpr(concepts::has_add_assign<C<T>>) {
                    result += std::move(*element);
                }
                else {
                    result.push_back(std::move(*element));
                }

                element = self.next();
            }

            return result;
        }
    };

    template<typename T, template<typename, typename...> typename C>
    requires(concepts::is_const_iterable<C<T>>)
    [[nodiscard]] constexpr auto make_stream(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>> {
        return BasicStream(IteratorStreamable(container.cbegin(), container.cend()));
    }

    template<typename T, template<typename, typename...> typename C>
    requires(concepts::is_const_reverse_iterable<C<T>>)
    [[nodiscard]] constexpr auto make_reverse_stream(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>> {
        return BasicStream(IteratorStreamable(container.cbegin(), container.cend()));
    }
}