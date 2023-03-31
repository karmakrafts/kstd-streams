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

#include "concepts.hpp"
#include "stream_fwd.hpp"

#include "iterator_streamable.hpp"
#include "owning_iterator_streamable.hpp"
#include "singlet_streamable.hpp"
#include "counting_streamable.hpp"

#include "basic_stream.hpp"
#include "chaining_stream.hpp"
#include "filtering_stream.hpp"
#include "limiting_stream.hpp"
#include "mapping_stream.hpp"
#include "flat_mapping_stream.hpp"
#include "distinct_stream.hpp"
#include "zipping_stream.hpp"
#include "flat_zipping_stream.hpp"
#include "peeking_stream.hpp"
#include "dropping_stream.hpp"

namespace cxxs {
    template<typename T, typename S, typename IMPL> //
    requires(concepts::is_streamable<S>)
    class Stream {
        protected:

        [[maybe_unused]] S _streamable;

        [[nodiscard]] constexpr auto get_self() noexcept -> IMPL& {
            static_assert(concepts::is_streamable<IMPL>, "Implementation is not streamable");
            return static_cast<IMPL&>(*this);
        }

        public:

        explicit constexpr Stream(S streamable) noexcept:
                _streamable(std::move(streamable)) {
        }

        template<typename S2>
        requires(concepts::is_streamable<S2>)
        [[nodiscard]] constexpr auto chain(S2 other) noexcept -> ChainingStream<IMPL, S2> {
            return ChainingStream<IMPL, S2>(std::move(get_self()), std::move(other));
        }

        template<typename S2>
        requires(concepts::is_streamable<S2>)
        [[nodiscard]] constexpr auto pre_chain(S2 other) noexcept -> ChainingStream<S2, IMPL> {
            return ChainingStream<S2, IMPL>(std::move(other), std::move(get_self()));
        }

        template<typename F>
        requires(std::is_convertible_v<F, std::function<void(T&)>>)
        [[nodiscard]] constexpr auto filter(F&& filter) noexcept -> FilteringStream<IMPL, F> {
            return FilteringStream<IMPL, F>(std::move(get_self()), std::forward<F>(filter));
        }

        [[nodiscard]] constexpr auto filter_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter([](auto& value) {
                return value != nullptr;
            });
        }

        template<typename M, typename R = std::invoke_result_t<M, T&>>
        requires(std::is_convertible_v<M, std::function<R(T&)>>)
        [[nodiscard]] constexpr auto map(M&& mapper) noexcept -> MappingStream<R, IMPL, M> {
            return MappingStream<R, IMPL, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename M, typename RS = std::invoke_result_t<M, T&>>
        requires(std::is_convertible_v<M, std::function<RS(T&)>> && concepts::is_streamable<RS>)
        [[nodiscard]] constexpr auto flat_map(M&& mapper) noexcept -> FlatMappingStream<IMPL, RS, M> {
            return FlatMappingStream<IMPL, RS, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename LM, typename L = std::invoke_result_t<LM, T&>, typename RM, typename R = std::invoke_result_t<RM, T&>>
        requires(std::is_convertible_v<LM, std::function<L(T&)>> && std::is_convertible_v<RM, std::function<R(T&)>>)
        [[nodiscard]] constexpr auto zip(LM&& left_mapper, RM&& right_mapper) noexcept -> ZippingStream<IMPL, LM, L, RM, R> {
            return ZippingStream<IMPL, LM, L, RM, R>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<typename LM, typename LS = std::invoke_result_t<LM, T&>, typename RM, typename RS = std::invoke_result_t<RM, T&>>
        requires(std::is_convertible_v<LM, std::function<LS(T&)>> && std::is_convertible_v<RM, std::function<RS(T&)>>)
        [[nodiscard]] constexpr auto flat_zip(LM&& left_mapper, RM&& right_mapper) noexcept -> FlatZippingStream<IMPL, LM, LS, RM, RS> {
            return FlatZippingStream<IMPL, LM, LS, RM, RS>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<typename F>
        requires(std::is_convertible_v<F, std::function<void(T&)>>)
        [[nodiscard]] constexpr auto peek(F&& function) noexcept -> PeekingStream<IMPL, F> {
            return PeekingStream<IMPL, F>(std::move(get_self()), std::forward<F>(function));
        }

        template<typename P>
        requires(std::is_convertible_v<P, std::function<bool(T&)>>)
        [[nodiscard]] constexpr auto drop_while(P&& predicate) noexcept -> DroppingStream<IMPL, P> {
            return DroppingStream<IMPL, P>(std::move(get_self()), std::forward<P>(predicate));
        }

        [[nodiscard]] constexpr auto limit(size_t max_count) noexcept -> LimitingStream<IMPL> {
            return LimitingStream<IMPL>(std::move(get_self()), max_count);
        }

        [[nodiscard]] constexpr auto distinct() noexcept -> DistinctStream<IMPL> {
            return DistinctStream<IMPL>(std::move(get_self()));
        }

        [[nodiscard]] constexpr auto skip(size_t count) noexcept -> IMPL& {
            auto& self = get_self();

            for (size_t i = 0; i < count; i++) {
                if (!self.next()) {
                    break;
                }
            }

            return self;
        }

        [[nodiscard]] constexpr auto find_first() noexcept -> std::optional<T> {
            return get_self().next();
        }

        [[nodiscard]] constexpr auto find_last() noexcept -> std::optional<T> {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                auto next = self.next();

                if (!next) {
                    break;
                }

                element = std::move(next);
            }

            return element;
        }

        template<typename F>
        requires(std::is_convertible_v<F, std::function<T(T, T)>>)
        [[nodiscard]] constexpr auto reduce(F&& function) noexcept -> std::optional<T> {
            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto acc = std::move(*element);
            auto next = self.next();

            while (next) {
                acc = std::move(function(std::move(acc), std::move(*next)));
                next = self.next();
            }

            return std::make_optional(acc);
        }

        [[nodiscard]] constexpr auto sum() noexcept -> std::optional<T> {
            static_assert(concepts::has_add<T>, "Stream value type doesn't implement operator+");

            return reduce([](auto a, auto b) {
                return a + b;
            });
        }

        [[nodiscard]] constexpr auto min() noexcept -> std::optional<T> {
            static_assert(concepts::has_lth<T> || concepts::has_gth<T>, "Stream value type doesn't implement operator< or operator>");

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if constexpr (concepts::has_lth<T>) {
                    if (value < result) {
                        result = std::move(value);
                    }
                }
                else {
                    if (result > value) {
                        result = std::move(value);
                    }
                }

                element = self.next();
            }

            return std::make_optional(result);
        }

        [[nodiscard]] constexpr auto max() noexcept -> std::optional<T> {
            static_assert(concepts::has_lth<T> || concepts::has_gth<T>, "Stream value type doesn't implement operator< or operator>");

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if constexpr (concepts::has_gth<T>) {
                    if (value > result) {
                        result = std::move(value);
                    }
                }
                else {
                    if (result < value) {
                        result = std::move(value);
                    }
                }

                element = self.next();
            }

            return std::make_optional(result);
        }

        [[nodiscard]] constexpr auto count() noexcept -> size_t {
            size_t result = 0;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                result++;
                element = self.next();
            }

            return result;
        }

        template<typename F>
        requires(std::is_convertible_v<F, std::function<void(T&)>>)
        constexpr auto for_each(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                function(*element);
                element = self.next();
            }
        }

        template<typename F>
        requires(std::is_convertible_v<F, std::function<void(T&, size_t)>>)
        constexpr auto for_each_indexed(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element) {
                function(*element, index++);
                element = self.next();
            }
        }

        template<typename P>
        requires(std::is_convertible_v<P, std::function<bool(T&)>>)
        [[nodiscard]] constexpr auto all_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (!predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        template<typename P>
        requires(std::is_convertible_v<P, std::function<bool(T&)>>)
        [[nodiscard]] constexpr auto any_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (predicate(*element)) {
                    return true;
                }

                element = self.next();
            }

            return false;
        }

        template<typename P>
        requires(std::is_convertible_v<P, std::function<bool(T&)>>)
        [[nodiscard]] constexpr auto none_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if (predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        [[nodiscard]] constexpr auto deref_all() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return map([](auto& value) {
                return *value;
            });
        }

        [[nodiscard]] constexpr auto deref_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter_not_null().map([](auto& value) {
                return *value;
            });
        }

        template<template<typename, typename...> typename C>
        requires(std::is_default_constructible_v<C<T>> && (concepts::has_add_assign<C<T>> || concepts::has_push_back<C<T>>))
        [[nodiscard]] constexpr auto collect() noexcept -> C <T> {
            C<T> result;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                if constexpr (concepts::has_add_assign<C<T>>) {
                    result += std::move(*element);
                }
                else {
                    result.push_back(std::move(*element));
                }

                element = self.next();
            }

            return result;
        }

        template<size_t EXTENT, template<typename, size_t, typename...> typename SEQ>
        requires(EXTENT > 0)
        [[nodiscard]] constexpr auto collect_sequence() noexcept -> SEQ <T, EXTENT> {
            SEQ<T, EXTENT> result;
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element && index < EXTENT) {
                result[index++] = std::move(*element);
                element = self.next();
            }

            return result;
        }

        constexpr auto collect_to_memory(T* elements, size_t max_count) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element && index < max_count) {
                *(elements + index) = std::move(*element);
                element = self.next();
                index++;
            }
        }

        // Chain operators (append)

        template<typename S2>
        requires(concepts::is_streamable<S2>)
        [[nodiscard]] constexpr auto operator |(S2 other) noexcept -> decltype(auto) {
            return chain(std::move(other));
        }

        template<template<typename, typename...> typename C>
        requires(concepts::is_const_iterable<C<T>>)
        [[nodiscard]] constexpr auto operator |(const C<T>& container) noexcept -> decltype(auto) {
            return chain(stream(container));
        }

        template<template<typename, typename...> typename C>
        requires(concepts::is_const_iterable<C<T>>)
        [[nodiscard]] constexpr auto operator |(C<T>&& container) noexcept -> decltype(auto) {
            return chain(owning(std::forward<C<T>>(container)));
        }

        // Pre-chain operators (prepend)

        template<typename S2>
        requires(concepts::is_streamable<S2>)
        [[nodiscard]] constexpr auto operator ||(S2 other) noexcept -> decltype(auto) {
            return pre_chain(std::move(other));
        }

        template<template<typename, typename...> typename C>
        requires(concepts::is_const_iterable<C<T>>)
        [[nodiscard]] constexpr auto operator ||(const C<T>& container) noexcept -> decltype(auto) {
            return pre_chain(stream(container));
        }

        template<template<typename, typename...> typename C>
        requires(concepts::is_const_iterable<C<T>>)
        [[nodiscard]] constexpr auto operator ||(C<T>&& container) noexcept -> decltype(auto) {
            return pre_chain(owning(std::forward<C<T>>(container)));
        }

        // Deref operator (find first)

        [[nodiscard]] constexpr auto operator *() noexcept -> std::optional<T> {
            return find_first();
        }
    };

    template<typename T, template<typename, typename...> typename C>
    requires(std::is_copy_assignable_v<T> && concepts::is_const_iterable<C<T>>)
    [[nodiscard]] constexpr auto stream(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>> {
        return BasicStream(IteratorStreamable(container.cbegin(), container.cend()));
    }

    template<typename T, template<typename, typename...> typename C>
    requires(std::is_copy_assignable_v<T> && concepts::is_const_iterable<C<T>>)
    [[nodiscard]] constexpr auto owning(C<T> container) noexcept -> BasicStream<OwningIteratorStreamable<T, C>> {
        return BasicStream(OwningIteratorStreamable(std::move(container)));
    }

    template<typename T, template<typename, typename...> typename C>
    requires(std::is_copy_assignable_v<T> && concepts::is_const_reverse_iterable<C<T>>)
    [[nodiscard]] constexpr auto reverse(const C<T>& container) noexcept -> BasicStream<IteratorStreamable<typename C<T>::const_iterator>> {
        return BasicStream(IteratorStreamable(container.crbegin(), container.crend()));
    }

    template<typename T>
    requires(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto singlet(T value) noexcept -> BasicStream<SingletStreamable<T>> {
        return BasicStream(SingletStreamable(std::move(value)));
    }

    template<typename T>
    requires(std::is_copy_assignable_v<T>)
    [[nodiscard]] constexpr auto counting(T value, size_t max_count) noexcept -> BasicStream<CountingStreamable<T>> {
        return BasicStream(CountingStreamable(std::move(value), max_count));
    }
}