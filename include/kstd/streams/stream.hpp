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
#include "draining_streamable.hpp"

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
#include "taking_stream.hpp"
#include "sorting_stream.hpp"

namespace kstd::streams {
    template<typename T, concepts::Streamable S, typename IMPL> //
    class Stream {
        protected:

        [[maybe_unused]] S _streamable;

        [[nodiscard]] constexpr auto get_self() noexcept -> IMPL& {
            static_assert(concepts::Streamable<IMPL>, "Implementation is not streamable");
            return static_cast<IMPL&>(*this);
        }

        public:

        explicit constexpr Stream(S streamable) noexcept:
                _streamable(std::move(streamable)) {
        }

        template<concepts::Streamable S2>
        [[nodiscard]] constexpr auto chain(S2 other) noexcept -> ChainingStream<IMPL, S2> {
            return ChainingStream<IMPL, S2>(std::move(get_self()), std::move(other));
        }

        template<concepts::Streamable S2>
        [[nodiscard]] constexpr auto pre_chain(S2 other) noexcept -> ChainingStream<S2, IMPL> {
            return ChainingStream<S2, IMPL>(std::move(other), std::move(get_self()));
        }

        template<concepts::Function<void(T&)> F>
        [[nodiscard]] constexpr auto filter(F&& filter) noexcept -> FilteringStream<IMPL, F> {
            return FilteringStream<IMPL, F>(std::move(get_self()), std::forward<F>(filter));
        }

        template<typename M, typename R = std::invoke_result_t<M, T&>>
        requires(concepts::Function<M, R(T&)>)
        [[nodiscard]] constexpr auto map(M&& mapper) noexcept -> MappingStream<R, IMPL, M> {
            return MappingStream<R, IMPL, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename M, typename RS = std::invoke_result_t<M, T&>>
        requires(concepts::Function<M, RS(T&)> && concepts::Streamable<RS>)
        [[nodiscard]] constexpr auto flat_map(M&& mapper) noexcept -> FlatMappingStream<IMPL, RS, M> {
            return FlatMappingStream<IMPL, RS, M>(std::move(get_self()), std::forward<M>(mapper));
        }

        template<typename LM, typename L = std::invoke_result_t<LM, T&>, typename RM, typename R = std::invoke_result_t<RM, T&>>
        requires(concepts::Function<LM, L(T&)> && concepts::Function<RM, R(T&)>)
        [[nodiscard]] constexpr auto zip(LM&& left_mapper, RM&& right_mapper) noexcept -> ZippingStream <IMPL, L, R, LM, RM> {
            return ZippingStream<IMPL, L, R, LM, RM>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<typename LM, typename LS = std::invoke_result_t<LM, T&>, typename RM, typename RS = std::invoke_result_t<RM, T&>>
        requires(concepts::Function<LM, LS(T&)> && concepts::Function<RM, RS(T&)>)
        [[nodiscard]] constexpr auto flat_zip(LM&& left_mapper, RM&& right_mapper) noexcept -> FlatZippingStream<IMPL, LS, RS, LM, RM> {
            return FlatZippingStream<IMPL, LS, RS, LM, RM>(std::move(get_self()), std::forward<LM>(left_mapper), std::forward<RM>(right_mapper));
        }

        template<concepts::Function<void(T&)> F>
        [[nodiscard]] constexpr auto peek(F&& function) noexcept -> PeekingStream<IMPL, F> {
            return PeekingStream<IMPL, F>(std::move(get_self()), std::forward<F>(function));
        }

        template<concepts::Function<bool(T&)> P>
        [[nodiscard]] constexpr auto drop_while(P&& predicate) noexcept -> DroppingStream<IMPL, P> {
            return DroppingStream<IMPL, P>(std::move(get_self()), std::forward<P>(predicate));
        }

        template<concepts::Function<bool(T&)> P>
        [[nodiscard]] constexpr auto take_while(P&& predicate) noexcept -> TakingStream <IMPL, P> {
            return TakingStream<IMPL, P>(std::move(get_self()), std::forward<P>(predicate));
        }

        template<concepts::Function<bool(const T&, const T&)> C>
        [[nodiscard]] constexpr auto sorted(C&& comparator) noexcept -> SortingStream<IMPL, C> {
            return SortingStream<IMPL, C>(std::move(get_self()), std::forward<C>(comparator));
        }

        [[nodiscard]] constexpr auto limit(size_t max_count) noexcept -> LimitingStream<IMPL> {
            return LimitingStream<IMPL>(std::move(get_self()), max_count);
        }

        [[nodiscard]] constexpr auto distinct() noexcept -> DistinctStream<IMPL> {
            return DistinctStream<IMPL>(std::move(get_self()));
        }

        [[nodiscard]] constexpr auto sorted() noexcept -> decltype(auto) {
            static_assert(concepts::LessThanComparable<T> || concepts::GreaterThanComparable<T>, "Stream value type doesn't implement operator< or operator>");

            return sorted([](const auto& a, const auto& b) {
                if constexpr (concepts::LessThanComparable<T>) {
                    return a < b;
                }
                else {
                    return !(a > b); // NOLINT
                }
            });
        }

        [[nodiscard]] constexpr auto filter_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter([](auto& value) {
                return value != nullptr;
            });
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

        template<concepts::Function<T(T, T)> F>
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
            static_assert(concepts::Addable<T>, "Stream value type doesn't implement operator+");

            return reduce([](auto a, auto b) {
                return a + b;
            });
        }

        [[nodiscard]] constexpr auto min() noexcept -> std::optional<T> {
            static_assert(concepts::LessThanComparable<T> || concepts::GreaterThanComparable<T>, "Stream value type doesn't implement operator< or operator>");

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if constexpr (concepts::LessThanComparable<T>) {
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
            static_assert(concepts::LessThanComparable<T> || concepts::LessThanComparable<T>, "Stream value type doesn't implement operator< or operator>");

            auto& self = get_self();
            auto element = self.next();

            if (!element) {
                return std::nullopt;
            }

            auto result = std::move(*element);
            element = self.next();

            while (element) {
                auto value = std::move(*element);

                if constexpr (concepts::GreaterThanComparable<T>) {
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

        template<concepts::Function<void(T&)> F>
        constexpr auto for_each(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                function(*element);
                element = self.next();
            }
        }

        template<concepts::Function<void(T&, size_t)> F>
        constexpr auto for_each_indexed(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            size_t index = 0;

            while (element) {
                function(*element, index++);
                element = self.next();
            }
        }

        template<concepts::Function<bool(T&)> P>
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

        template<concepts::Function<bool(T&)> P>
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

        template<concepts::Function<bool(T&)> P>
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
        requires(concepts::Pushable<C<T>> && std::is_default_constructible_v<C<T>>)
        [[nodiscard]] constexpr auto collect() noexcept -> C <T> {
            C<T> result;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                result.push_back(std::move(*element));
                element = self.next();
            }

            return result;
        }

        template<template<typename, typename, typename...> typename M, typename KM, typename K = std::invoke_result_t<KM, T&>, typename VM, typename V = std::invoke_result_t<VM, T&>>
        requires(concepts::Function<KM, K(T&)> && concepts::Function<VM, V(T&)> && std::is_default_constructible_v<M<K, V>> && concepts::Indexable<K, V, M>)
        [[nodiscard]] constexpr auto collect_map(KM&& key_mapper, VM&& value_mapper) noexcept -> M <K, V> {
            M<K, V> result;
            auto& self = get_self();
            auto element = self.next();

            while (element) {
                auto& value = *element;
                result[key_mapper(value)] = std::move(value_mapper(value));
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

        [[nodiscard]] constexpr auto evaluate() noexcept -> decltype(auto) {
            return owning(collect<std::vector>());
        }

        // Chain operators (append)

        [[nodiscard]] constexpr auto operator |(concepts::Streamable auto other) noexcept -> decltype(auto) {
            return chain(std::move(other));
        }

        [[nodiscard]] constexpr auto operator |(const concepts::ConstIterable auto& container) noexcept -> decltype(auto) {
            return chain(stream(container));
        }

        [[nodiscard]] constexpr auto operator |(concepts::ConstIterable auto&& container) noexcept -> decltype(auto) {
            return chain(owning(std::forward(container)));
        }

        // Pre-chain operators (prepend)

        [[nodiscard]] constexpr auto operator ||(concepts::Streamable auto other) noexcept -> decltype(auto) {
            return pre_chain(std::move(other));
        }

        [[nodiscard]] constexpr auto operator ||(const concepts::ConstIterable auto& container) noexcept -> decltype(auto) {
            return pre_chain(stream(container));
        }

        [[nodiscard]] constexpr auto operator ||(concepts::ConstIterable auto&& container) noexcept -> decltype(auto) {
            return pre_chain(owning(std::forward(container)));
        }

        // Deref operator (find first)

        [[nodiscard]] constexpr auto operator *() noexcept -> std::optional<T> {
            return find_first();
        }
    };

    template<concepts::ConstIterable C>
    [[nodiscard]] constexpr auto stream(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        return BasicStream(IteratorStreamable(container.cbegin(), container.cend()));
    }

    template<concepts::ConstIterable C>
    [[nodiscard]] constexpr auto owning(C container) noexcept -> BasicStream<OwningIteratorStreamable<C>> {
        return BasicStream(OwningIteratorStreamable(std::move(container)));
    }

    template<concepts::ConstReverseIterable C>
    [[nodiscard]] constexpr auto reverse(const C& container) noexcept -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        return BasicStream(IteratorStreamable(container.crbegin(), container.crend()));
    }

    template<concepts::Iterable C>
    requires(concepts::Erasable<C>)
    [[nodiscard]] constexpr auto draining(C& container) noexcept -> BasicStream<DrainingStreamable<C>> {
        return BasicStream(DrainingStreamable(container));
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