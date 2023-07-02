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

#include "kstd/meta.hpp"
#include "kstd/option.hpp"
#include "kstd/utils.hpp"

#include "stream_fwd.hpp"

#include "counting_streamable.hpp"
#include "draining_streamable.hpp"
#include "iterator_streamable.hpp"
#include "owning_iterator_streamable.hpp"
#include "singlet_streamable.hpp"

#include "basic_stream.hpp"
#include "chaining_stream.hpp"
#include "distinct_stream.hpp"
#include "dropping_stream.hpp"
#include "filtering_stream.hpp"
#include "flat_mapping_stream.hpp"
#include "flat_zipping_stream.hpp"
#include "limiting_stream.hpp"
#include "mapping_stream.hpp"
#include "peeking_stream.hpp"
#include "sorting_stream.hpp"
#include "taking_stream.hpp"
#include "zipping_stream.hpp"

namespace kstd::streams {
    template<typename T, typename S, typename IMPL>//
    class Stream {
        protected:
        [[maybe_unused]] S _streamable;

        [[nodiscard]] constexpr auto get_self() noexcept -> IMPL& {
            return static_cast<IMPL&>(*this);
        }

        public:
        explicit constexpr Stream(S streamable) noexcept :
                _streamable(utils::move(streamable)) {
        }

        template<typename S2>
        [[nodiscard]] constexpr auto chain(S2 other) noexcept -> ChainingStream<IMPL, S2> {
            return ChainingStream<IMPL, S2>(utils::move(get_self()), utils::move(other));
        }

        template<typename S2>
        [[nodiscard]] constexpr auto pre_chain(S2 other) noexcept -> ChainingStream<S2, IMPL> {
            return ChainingStream<S2, IMPL>(utils::move(other), utils::move(get_self()));
        }

        template<typename F>
        [[nodiscard]] constexpr auto filter(F&& filter) noexcept -> FilteringStream<IMPL, F> {
            return FilteringStream<IMPL, F>(utils::move(get_self()), utils::forward<F>(filter));
        }

        template<typename M, typename R = std::invoke_result_t<M, T&>>
        [[nodiscard]] constexpr auto map(M&& mapper) noexcept -> MappingStream<R, IMPL, M> {
            return MappingStream<R, IMPL, M>(utils::move(get_self()), utils::forward<M>(mapper));
        }

        template<typename M, typename RS = std::invoke_result_t<M, T&>>
        [[nodiscard]] constexpr auto flat_map(M&& mapper) noexcept -> FlatMappingStream<IMPL, RS, M> {
            return FlatMappingStream<IMPL, RS, M>(utils::move(get_self()), utils::forward<M>(mapper));
        }

        template<typename LM, typename L = std::invoke_result_t<LM, T&>, typename RM,
                 typename R = std::invoke_result_t<RM, T&>>
        [[nodiscard]] constexpr auto zip(LM&& left_mapper, RM&& right_mapper) noexcept
                -> ZippingStream<IMPL, L, R, LM, RM> {
            return ZippingStream<IMPL, L, R, LM, RM>(utils::move(get_self()), utils::forward<LM>(left_mapper),
                                                     utils::forward<RM>(right_mapper));
        }

        template<typename LM, typename LS = std::invoke_result_t<LM, T&>, typename RM,
                 typename RS = std::invoke_result_t<RM, T&>>
        [[nodiscard]] constexpr auto flat_zip(LM&& left_mapper, RM&& right_mapper) noexcept
                -> FlatZippingStream<IMPL, LS, RS, LM, RM> {
            return FlatZippingStream<IMPL, LS, RS, LM, RM>(utils::move(get_self()), utils::forward<LM>(left_mapper),
                                                           utils::forward<RM>(right_mapper));
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek(F&& function) noexcept -> PeekingStream<IMPL, F> {
            return PeekingStream<IMPL, F>(utils::move(get_self()), utils::forward<F>(function));
        }

        template<typename P>
        [[nodiscard]] constexpr auto drop_while(P&& predicate) noexcept -> DroppingStream<IMPL, P> {
            return DroppingStream<IMPL, P>(utils::move(get_self()), utils::forward<P>(predicate));
        }

        template<typename P>
        [[nodiscard]] constexpr auto take_while(P&& predicate) noexcept -> TakingStream<IMPL, P> {
            return TakingStream<IMPL, P>(utils::move(get_self()), utils::forward<P>(predicate));
        }

        template<typename C>
        [[nodiscard]] constexpr auto sorted(C&& comparator) noexcept -> SortingStream<IMPL, C> {
            return SortingStream<IMPL, C>(utils::move(get_self()), utils::forward<C>(comparator));
        }

        [[nodiscard]] constexpr auto limit(usize max_count) noexcept -> LimitingStream<IMPL> {
            return LimitingStream<IMPL>(utils::move(get_self()), max_count);
        }

        [[nodiscard]] constexpr auto distinct() noexcept -> DistinctStream<IMPL> {
            return DistinctStream<IMPL>(utils::move(get_self()));
        }

        [[nodiscard]] constexpr auto sorted() noexcept -> decltype(auto) {
#ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>,
                          "Stream value type doesn't implement operator< or operator>");
#endif// KSTD_CONCEPTS_AVAILABLE

            return sorted([](const auto& a, const auto& b) {
                return a < b;
            });
        }

        [[nodiscard]] constexpr auto filter_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter([](auto* value) {
                return value != nullptr;
            });
        }

        [[nodiscard]] constexpr auto skip(usize count) noexcept -> IMPL& {
            auto& self = get_self();

            for(usize i = 0; i < count; i++) {
                if(!self.next()) {
                    break;
                }
            }

            return self;
        }

        [[nodiscard]] constexpr auto find_first() noexcept -> Option<T> {
            return get_self().next();
        }

        [[nodiscard]] constexpr auto find_last() noexcept -> Option<T> {
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                auto next = self.next();

                if(!next) {
                    break;
                }

                element = utils::move(next);
            }

            return element;
        }

        template<typename F>
        [[nodiscard]] constexpr auto reduce(F&& function) noexcept -> Option<T> {
            auto& self = get_self();
            auto element = self.next();

            if(!element) {
                return {};
            }

            auto acc = utils::move(*element);
            auto next = self.next();

            while(next) {
                acc = utils::move(function(utils::move(acc), utils::move(*next)));
                next = self.next();
            }

            return {utils::move(acc)};
        }

        [[nodiscard]] constexpr auto sum() noexcept -> Option<T> {
#ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::Addable<T>, "Stream value type doesn't implement operator+");
#endif// KSTD_CONCEPTS_AVAILABLE

            return reduce([](auto a, auto b) {
                return a + b;
            });
        }

        [[nodiscard]] constexpr auto min() noexcept -> Option<T> {
#ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>, "Stream value type doesn't implement operator<");
#endif// KSTD_CONCEPTS_AVAILABLE

            auto& self = get_self();
            auto element = self.next();

            if(!element) {
                return {};
            }

            auto result = utils::move(*element);
            element = self.next();

            while(element) {
                auto value = utils::move(*element);

                if(value < result) {
                    result = utils::move(value);
                }

                element = self.next();
            }

            return {utils::move(result)};
        }

        [[nodiscard]] constexpr auto max() noexcept -> Option<T> {
#ifdef KSTD_CONCEPTS_AVAILABLE
            static_assert(kstd::concepts::LessThanComparable<T>, "Stream value type doesn't implement operator<");
#endif// KSTD_CONCEPTS_AVAILABLE

            auto& self = get_self();
            auto element = self.next();

            if(!element) {
                return {};
            }

            auto result = utils::move(*element);
            element = self.next();

            while(element) {
                auto value = utils::move(*element);

                if(result < value) {
                    result = utils::move(value);
                }

                element = self.next();
            }

            return {utils::move(result)};
        }

        [[nodiscard]] constexpr auto count() noexcept -> usize {
            usize result = 0;
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                result++;
                element = self.next();
            }

            return result;
        }

        template<typename F>
        constexpr auto for_each(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                function(*element);
                element = self.next();
            }
        }

        template<typename F>
        constexpr auto for_each_indexed(F&& function) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            usize index = 0;

            while(element) {
                function(*element, index++);
                element = self.next();
            }
        }

        template<typename P>
        [[nodiscard]] constexpr auto all_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                if(!predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        template<typename P>
        [[nodiscard]] constexpr auto any_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                if(predicate(*element)) {
                    return true;
                }

                element = self.next();
            }

            return false;
        }

        template<typename P>
        [[nodiscard]] constexpr auto none_match(P&& predicate) noexcept -> bool {
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                if(predicate(*element)) {
                    return false;
                }

                element = self.next();
            }

            return true;
        }

        [[nodiscard]] constexpr auto deref_all() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return map([](auto* value) {
                return *value;
            });
        }

        [[nodiscard]] constexpr auto deref_not_null() noexcept -> decltype(auto) {
            static_assert(std::is_pointer_v<T>, "Stream value type is not a pointer");

            return filter_not_null().map([](auto value) {
                return *value;
            });
        }

        template<template<typename, typename...> typename C>
        [[nodiscard]] constexpr auto collect() noexcept -> C<T> {
            C<T> result;
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                result.push_back(element.get());
                element = self.next();
            }

            return result;
        }

        template<template<typename, typename, typename...> typename M, typename KM,
                 typename K = std::invoke_result_t<KM, T&>, typename VM, typename V = std::invoke_result_t<VM, T&>>
        [[nodiscard]] constexpr auto collect_map(KM&& key_mapper, VM&& value_mapper) noexcept -> M<K, V> {
            M<K, V> result;
            auto& self = get_self();
            auto element = self.next();

            while(element) {
                auto& value = element.borrow();
                result[key_mapper(value)] = utils::move(value_mapper(value));
                element = self.next();
            }

            return result;
        }

        template<usize EXTENT, template<typename, usize, typename...> typename SEQ>
        [[nodiscard]] constexpr auto collect_sequence() noexcept -> SEQ<T, EXTENT> {
            SEQ<T, EXTENT> result;
            auto& self = get_self();
            auto element = self.next();
            usize index = 0;

            while(element && index < EXTENT) {
                result[index++] = utils::move(element.get());
                element = self.next();
            }

            return result;
        }

        constexpr auto collect_to_memory(T* elements, usize max_count) noexcept -> void {
            auto& self = get_self();
            auto element = self.next();
            usize index = 0;

            while(element && index < max_count) {
                *(elements + index) = utils::move(*element);
                element = self.next();
                index++;
            }
        }

        [[nodiscard]] constexpr auto evaluate() noexcept -> OwningIteratorStreamable<std::vector<T>> {
            return owning(collect<std::vector>());
        }
    };

    template<typename C>
    [[nodiscard]] constexpr auto stream(const C& container) noexcept
            -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        using Streamable = IteratorStreamable<typename C::const_iterator>;
        return BasicStream<Streamable>(Streamable(container.cbegin(), container.cend()));
    }

    template<typename C>
    [[nodiscard]] constexpr auto owning(C container) noexcept -> BasicStream<OwningIteratorStreamable<C>> {
        using Streamable = OwningIteratorStreamable<C>;
        return BasicStream<Streamable>(Streamable(utils::move(container)));
    }

    template<typename C>
    [[nodiscard]] constexpr auto reverse(const C& container) noexcept
            -> BasicStream<IteratorStreamable<typename C::const_iterator>> {
        using Streamable = IteratorStreamable<typename C::const_iterator>;
        return BasicStream<Streamable>(Streamable(container.crbegin(), container.crend()));
    }

    template<typename C>
    [[nodiscard]] constexpr auto draining(C& container) noexcept -> BasicStream<DrainingStreamable<C>> {
        using Streamable = DrainingStreamable<C>;
        return BasicStream<Streamable>(Streamable(container));
    }

    template<typename T>
    [[nodiscard]] constexpr auto singlet(T value) noexcept -> BasicStream<SingletStreamable<T>> {
        using Streamable = SingletStreamable<T>;
        return BasicStream<Streamable>(Streamable(utils::move(value)));
    }

    template<typename T>
    [[nodiscard]] constexpr auto counting(T value, usize max_count) noexcept -> BasicStream<CountingStreamable<T>> {
        using Streamable = CountingStreamable<T>;
        return BasicStream<Streamable>(Streamable(utils::move(value), max_count));
    }

}// namespace kstd::streams