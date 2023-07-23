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

#include "buffered_pipe.hpp"
#include "iterator_pipe.hpp"
#include "pipe.hpp"
#include "supplier_pipe.hpp"

#include "collectors.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "mappers.hpp"

namespace kstd::streams {
    template<typename PIPE>
    struct Stream final {
        // clang-format off
        using PipeType  = PIPE;
        using ValueType = typename PipeType::ValueType;
        using Self      = Stream<PipeType>;
        // clang-format on

        private:
        PipeType _pipe;

        template<typename F>
        [[nodiscard]] constexpr auto make_filter_sleeve(F&& predicate) noexcept -> decltype(auto) {
            return [&predicate](PipeType& pipe) noexcept -> Option<ValueType> {
                auto element = pipe.get_next();
                while(element && !predicate(*element)) {
                    element = pipe.get_next();
                }
                return element;
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_map_sleeve(F&& mapper) noexcept -> decltype(auto) {
            return [&mapper](PipeType& pipe) noexcept -> Option<std::invoke_result_t<F, ValueType>> {
                auto element = pipe.get_next();
                if(!element) {
                    return {};
                }
                return mapper(*element);
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_peek_sleeve(F&& function) noexcept -> decltype(auto) {
            return [&function](PipeType& pipe) noexcept -> Option<ValueType> {
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
        [[nodiscard]] constexpr auto make_sort_callback(F&& comparator) noexcept -> decltype(auto) {
            return [&comparator](auto& buffer) noexcept -> void {
                std::sort(buffer.begin(), buffer.end(), std::forward<F>(comparator));
            };
        }

        public:
        KSTD_NO_MOVE_COPY(Stream, Self, constexpr)

        explicit constexpr Stream(PipeType pipe) noexcept :
                _pipe {std::move(pipe)} {
        }

        ~Stream() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<ValueType> {
            return _pipe.get_next();
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> Option<ValueType> {
            return get_next();
        }

        template<typename F>
        [[nodiscard]] constexpr auto map(F&& mapper) noexcept
                -> Stream<Pipe<PipeType, decltype(make_map_sleeve(std::forward<F>(mapper)))>> {
            auto sleeve = make_map_sleeve(std::forward<F>(mapper));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto filter(F&& predicate) noexcept
                -> Stream<Pipe<PipeType, decltype(make_filter_sleeve(std::forward<F>(predicate)))>> {
            static_assert(std::is_convertible_v<F, std::function<bool(ValueType&)>>,
                          "Predicate signature does not match");
            auto sleeve = make_filter_sleeve(std::forward<F>(predicate));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek(F&& function) noexcept
                -> Stream<Pipe<PipeType, decltype(make_peek_sleeve(std::forward<F>(function)))>> {
            auto sleeve = make_peek_sleeve(std::forward<F>(function));
            using Pipe = Pipe<PipeType, decltype(sleeve)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek_all(F&& function) noexcept -> Stream<BufferedPipe<PipeType, F>> {
            using Pipe = BufferedPipe<PipeType, F>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::forward<F>(function)}};
        }

        template<typename F>
        constexpr auto for_each(F&& function) noexcept -> void {
            auto element = get_next();
            while(element) {
                function(*element);
                element = get_next();
            }
        }

        [[nodiscard]] constexpr auto sort() noexcept -> Stream<BufferedPipe<PipeType, decltype(make_sort_callback())>> {
            auto callback = make_sort_callback();
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto sort(F&& comparator) noexcept
                -> Stream<BufferedPipe<PipeType, decltype(make_sort_callback(std::forward<F>(comparator)))>> {
            auto callback = make_sort_callback(std::forward<F>(comparator));
            using Pipe = BufferedPipe<PipeType, decltype(callback)>;
            return Stream<Pipe> {Pipe {std::move(_pipe), std::move(callback)}};
        }

        template<template<typename, typename...> typename CONTAINER, typename COLLECTOR>
        [[nodiscard]] constexpr auto collect(COLLECTOR&& collector) noexcept
                -> CONTAINER<std::remove_cv_t<std::remove_reference_t<ValueType>>> {
            CONTAINER<std::remove_cv_t<std::remove_reference_t<ValueType>>> result {};
            collector(*this, result);
            return result;
        }

        template<template<typename, typename, typename...> typename MAP, typename KM, typename VM>
        [[nodiscard]] constexpr auto collect_map(KM&& key_mapper, VM&& value_mapper) noexcept
                -> MAP<std::invoke_result_t<KM, ValueType&>, std::invoke_result_t<VM, ValueType&>> {
            MAP<std::invoke_result_t<KM, ValueType&>, std::invoke_result_t<VM, ValueType&>> result {};
            auto element = get_next();
            while(element) {
                auto& value = *element;
                result[key_mapper(value)] = value_mapper(value);
                element = get_next();
            }
            return result;
        }
    };

    template<typename ITERATOR>
    [[nodiscard]] constexpr auto stream(ITERATOR begin, ITERATOR end) noexcept -> Stream<IteratorPipe<ITERATOR>> {
        using Pipe = IteratorPipe<ITERATOR>;
        return Stream<Pipe> {Pipe {begin, end}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::iterator>> {
        using Pipe = IteratorPipe<typename CONTAINER::iterator>;
        return Stream<Pipe> {Pipe {container.begin(), container.end()}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(const CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::const_iterator>> {
        using Pipe = IteratorPipe<typename CONTAINER::const_iterator>;
        return Stream<Pipe> {Pipe {container.cbegin(), container.cend()}};
    }

    template<typename SUPPLIER>
    [[nodiscard]] constexpr auto stream(SUPPLIER&& supplier) noexcept -> Stream<SupplierPipe<SUPPLIER>> {
        using Pipe = SupplierPipe<SUPPLIER>;
        return Stream<Pipe> {Pipe {std::forward<SUPPLIER>(supplier)}};
    }
}// namespace kstd::streams