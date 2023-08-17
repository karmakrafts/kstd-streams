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
 * @since 16/07/2023
 */

#pragma once

#include <algorithm>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <kstd/pack.hpp>
#include <unordered_set>

#include "buffered_pipe.hpp"
#include "iterator_pipe.hpp"
#include "linked_struct_pipe.hpp"
#include "pipe.hpp"
#include "supplier_pipe.hpp"

#include "collectors.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "mappers.hpp"
#include "reducers.hpp"

namespace kstd::streams {
    template<typename PIPE>
    struct Stream final {
        // clang-format off
        using PipeType          = PIPE;
        using ValueType         = typename PipeType::ValueType;
        using NakedValueType    = std::remove_cv_t<std::remove_reference_t<ValueType>>;
        using Self              = Stream<PipeType>;
        // clang-format on

        private:
        PipeType _pipe;

        template<typename F>
        [[nodiscard]] constexpr auto make_filter_sleeve(F predicate) noexcept -> decltype(auto) {
            return [predicate = std::move(predicate)](PipeType& pipe) noexcept -> Option<ValueType> {
                auto element = pipe.get_next();
                while(element && !predicate(*element)) {
                    element = pipe.get_next();
                }
                return element;
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_map_sleeve(F mapper) noexcept -> decltype(auto) {
            return [mapper = std::move(mapper)](PipeType& pipe) noexcept -> Option<std::invoke_result_t<F, ValueType>> {
                auto element = pipe.get_next();
                if(!element) {
                    return {};
                }
                return mapper(*element);
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_peek_sleeve(F function) noexcept -> decltype(auto) {
            return [function = std::move(function)](PipeType& pipe) noexcept -> Option<ValueType> {
                auto element = pipe.get_next();
                if(!element) {
                    return {};
                }
                function(*element);
                return element;
            };
        }

        [[nodiscard]] constexpr auto make_sort_callback() noexcept -> decltype(auto) {
            return [](auto& buffer) noexcept -> void {
                std::sort(buffer.begin(), buffer.end());
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_sort_callback(F comparator) noexcept -> decltype(auto) {// NOLINT
            return [comparator = std::move(comparator)](auto& buffer) noexcept -> void {
                std::sort(buffer.begin(), buffer.end(), comparator);
            };
        }

        [[nodiscard]] constexpr auto make_reverse_sort_callback() noexcept -> decltype(auto) {
            return [](auto& buffer) noexcept -> void {
                std::sort(buffer.rbegin(), buffer.rend());
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_reverse_sort_callback(F comparator) noexcept -> decltype(auto) {// NOLINT
            return [comparator = std::move(comparator)](auto& buffer) noexcept -> void {
                std::sort(buffer.rbegin(), buffer.rend(), comparator);
            };
        }

        [[nodiscard]] constexpr auto make_distinct_callback() noexcept -> decltype(auto) {
            return [](auto& buffer) noexcept -> void {
                using Buffer = std::remove_reference_t<decltype(buffer)>;
                using Type = std::decay_t<typename Buffer::value_type>;
                const std::unordered_set<Type> elements {buffer.cbegin(), buffer.cend()};
                buffer = {elements.cbegin(), elements.cend()};
            };
        }

        public:
        KSTD_NO_MOVE_COPY(Stream, Self, constexpr)// Streams are temporary only

        explicit constexpr Stream(PipeType pipe) noexcept :
                _pipe {std::move(pipe)} {
        }

        ~Stream() noexcept = default;

        template<typename F>
        [[nodiscard]] constexpr auto map(F mapper) noexcept
                -> Stream<Pipe<PipeType, decltype(make_map_sleeve(std::move(mapper)))>> {
            auto sleeve = make_map_sleeve(std::move(mapper));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        [[nodiscard]] constexpr auto deref_all() noexcept -> decltype(auto) {
            return map(mappers::dereference);
        }

        [[nodiscard]] constexpr auto address_of_all() noexcept -> decltype(auto) {
            return map(mappers::address_of);
        }

        template<typename F>
        [[nodiscard]] constexpr auto filter(F predicate) noexcept
                -> Stream<Pipe<PipeType, decltype(make_filter_sleeve(std::move(predicate)))>> {
            static_assert(std::is_convertible_v<F, std::function<bool(ValueType)>>,
                          "Predicate signature does not match");
            auto sleeve = make_filter_sleeve(std::move(predicate));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek(F function) noexcept
                -> Stream<Pipe<PipeType, decltype(make_peek_sleeve(std::move(function)))>> {
            static_assert(std::is_convertible_v<F, std::function<void(ValueType)>>,
                          "Function signature does not match");
            auto sleeve = make_peek_sleeve(std::move(function));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek_all(F function) noexcept -> Stream<BufferedPipe<PipeType, F>> {
            using Pipe = BufferedPipe<PipeType, F>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(function)}};
        }

        template<typename F>
        constexpr auto for_each(F function) noexcept -> void {
            auto element = _pipe.get_next();
            while(element) {
                function(*element);
                element = _pipe.get_next();
            }
        }

        template<typename F>
        [[nodiscard]] constexpr auto reduce(F function, NakedValueType value = NakedValueType {}) noexcept
                -> NakedValueType {
            auto element = _pipe.get_next();
            while(element) {
                value = function(value, *element);
                element = _pipe.get_next();
            }
            return value;
        }

        [[nodiscard]] constexpr auto distinct() noexcept
                -> Stream<BufferedPipe<PipeType, decltype(make_distinct_callback())>> {
            auto callback = make_distinct_callback();
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        [[nodiscard]] constexpr auto distinct_by_address() noexcept -> decltype(auto) {
            return map(mappers::address_of).distinct().map(mappers::dereference);
        }

        [[nodiscard]] constexpr auto distinct_by_value() noexcept -> decltype(auto) {
            return map(mappers::dereference).distinct().map(mappers::address_of);
        }

        [[nodiscard]] constexpr auto sum() noexcept -> decltype(auto) {
            return reduce(reducers::add);
        }

        [[nodiscard]] constexpr auto count() noexcept -> usize {
            usize count = 0;
            auto element = _pipe.get_next();
            while(element) {
                ++count;
                element = _pipe.get_next();
            }
            return count;
        }

        template<typename F>
        [[nodiscard]] constexpr auto index_of(F predicate) noexcept -> usize {
            usize index = 0;
            auto element = _pipe.get_next();
            while(element && !predicate(*element)) {
                ++index;
                element = _pipe.get_next();
            }
            return index;
        }

        template<typename F>
        [[nodiscard]] constexpr auto index_of_last(F predicate) noexcept -> usize {
            usize index = 0;
            usize result = 0;
            auto element = _pipe.get_next();
            while(element) {
                ++index;
                if(predicate(*element)) {
                    result = index;
                }
                element = _pipe.get_next();
            }
            return result;
        }

        template<typename F>
        [[nodiscard]] constexpr auto find(F predicate) noexcept -> Option<ValueType> {
            auto element = _pipe.get_next();
            while(element && !predicate(*element)) {
                element = _pipe.get_next();
            }
            return element;
        }

        template<typename F>
        [[nodiscard]] constexpr auto find_last(F predicate) noexcept -> Option<ValueType> {
            auto element = _pipe.get_next();
            Option<ValueType> result {};
            while(element) {
                if(predicate(*element)) {
                    result = std::move(element);
                }
                element = _pipe.get_next();
            }
            return result;
        }

        [[nodiscard]] constexpr auto sort() noexcept -> Stream<BufferedPipe<PipeType, decltype(make_sort_callback())>> {
            auto callback = make_sort_callback();
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto sort(F comparator) noexcept
                -> Stream<BufferedPipe<PipeType, decltype(make_sort_callback(std::move(comparator)))>> {
            auto callback = make_sort_callback(std::move(comparator));
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        [[nodiscard]] constexpr auto reverse_sort() noexcept
                -> Stream<BufferedPipe<PipeType, decltype(make_reverse_sort_callback())>> {
            auto callback = make_reverse_sort_callback();
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto reverse_sort(F comparator) noexcept
                -> Stream<BufferedPipe<PipeType, decltype(make_reverse_sort_callback(std::move(comparator)))>> {
            auto callback = make_reverse_sort_callback(std::move(comparator));
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        template<template<typename, typename...> typename CONTAINER, typename... PROPS, typename COLLECTOR,
                 typename... ARGS>
        [[nodiscard]] constexpr auto collect(COLLECTOR collector, ARGS&&... args) noexcept
                -> CONTAINER<std::remove_cv_t<std::remove_reference_t<ValueType>>, PROPS...> {
            CONTAINER<std::remove_cv_t<std::remove_reference_t<ValueType>>, PROPS...> result {
                    std::forward<ARGS>(args)...};
            collector(_pipe, result);
            return result;
        }

        template<template<typename, typename, typename...> typename MAP, typename... PROPS, typename KM, typename VM,
                 typename... ARGS>
        [[nodiscard]] constexpr auto collect_map(KM key_mapper, VM value_mapper, ARGS&&... args) noexcept
                -> MAP<std::invoke_result_t<KM, ValueType&>, std::invoke_result_t<VM, ValueType&>, PROPS...> {
            MAP<std::invoke_result_t<KM, ValueType&>, std::invoke_result_t<VM, ValueType&>, PROPS...> result {
                    std::forward<ARGS>(args)...};
            auto element = _pipe.get_next();
            while(element) {
                auto& value = *element;
                result[key_mapper(value)] = value_mapper(value);
                element = _pipe.get_next();
            }
            return result;
        }
    };

    template<typename ITERATOR>
    [[nodiscard]] constexpr auto stream(ITERATOR begin, ITERATOR end) noexcept -> Stream<IteratorPipe<ITERATOR>> {
        using Pipe = IteratorPipe<ITERATOR>;
        return Stream<Pipe> {Pipe {begin, end}};
    }

    template<typename SUPPLIER>
    [[nodiscard]] constexpr auto stream(SUPPLIER&& supplier) noexcept -> Stream<SupplierPipe<SUPPLIER>> {
        using Pipe = SupplierPipe<SUPPLIER>;
        return Stream<Pipe> {Pipe {std::forward<SUPPLIER>(supplier)}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::iterator>> {
        return stream<typename CONTAINER::iterator>(container.begin(), container.end());
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(const CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::const_iterator>> {
        return stream<typename CONTAINER::const_iterator>(container.cbegin(), container.cend());
    }

    template<typename A, typename F>
    [[nodiscard]] constexpr auto stream(A address, F&& functor) noexcept -> Stream<LinkedStructPipe<A, F>> {
        using Pipe = LinkedStructPipe<A, F>;
        return Stream<Pipe> {Pipe {address, std::forward<F>(functor)}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto reverse_stream(CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::reverse_iterator>> {
        return stream<typename CONTAINER::reverse_iterator>(container.rbegin(), container.rend());
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto reverse_stream(const CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::const_reverse_iterator>> {
        return stream<typename CONTAINER::const_reverse_iterator>(container.crbegin(), container.crend());
    }
}// namespace kstd::streams