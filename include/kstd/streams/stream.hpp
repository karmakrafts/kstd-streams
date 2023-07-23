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
        using pipe_type     = PIPE;
        using value_type    = typename pipe_type::value_type;
        using self          = Stream<pipe_type>;
        // clang-format on

        private:
        pipe_type _pipe;

        template<typename F>
        [[nodiscard]] constexpr auto make_filter_sleeve(F&& predicate) noexcept -> decltype(auto) {
            return [&predicate](pipe_type& pipe) noexcept -> Option<value_type> {
                auto element = pipe.get_next();
                while(element && !predicate(*element)) {
                    element = pipe.get_next();
                }
                return element;
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_map_sleeve(F&& mapper) noexcept -> decltype(auto) {
            return [&mapper](pipe_type& pipe) noexcept -> Option<std::invoke_result_t<F, value_type>> {
                auto element = pipe.get_next();
                if(!element) {
                    return {};
                }
                return mapper(*element);
            };
        }

        template<typename F>
        [[nodiscard]] constexpr auto make_peek_sleeve(F&& function) noexcept -> decltype(auto) {
            return [&function](pipe_type& pipe) noexcept -> Option<value_type> {
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
        KSTD_NO_MOVE_COPY(Stream, self, constexpr)

        explicit constexpr Stream(pipe_type pipe) noexcept :
                _pipe {std::move(pipe)} {
        }

        ~Stream() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<value_type> {
            return _pipe.get_next();
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> Option<value_type> {
            return get_next();
        }

        template<typename F>
        [[nodiscard]] constexpr auto map(F&& mapper) noexcept
                -> Stream<Pipe<pipe_type, decltype(make_map_sleeve(std::forward<F>(mapper)))>> {
            auto sleeve = make_map_sleeve(std::forward<F>(mapper));
            using pipe = Pipe<pipe_type, decltype(sleeve)>;
            return Stream<pipe> {pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto filter(F&& predicate) noexcept
                -> Stream<Pipe<pipe_type, decltype(make_filter_sleeve(std::forward<F>(predicate)))>> {
            static_assert(std::is_convertible_v<F, std::function<bool(value_type&)>>,
                          "Predicate signature does not match");
            auto sleeve = make_filter_sleeve(std::forward<F>(predicate));
            using pipe = Pipe<pipe_type, decltype(sleeve)>;
            return Stream<pipe> {pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek(F&& function) noexcept
                -> Stream<Pipe<pipe_type, decltype(make_peek_sleeve(std::forward<F>(function)))>> {
            auto sleeve = make_peek_sleeve(std::forward<F>(function));
            using pipe = Pipe<pipe_type, decltype(sleeve)>;
            return Stream<pipe> {pipe {std::move(_pipe), std::move(sleeve)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto peek_all(F&& function) noexcept -> Stream<BufferedPipe<pipe_type, F>> {
            using pipe = BufferedPipe<pipe_type, F>;
            return Stream<pipe> {pipe {std::move(_pipe), std::forward<F>(function)}};
        }

        template<typename F>
        constexpr auto for_each(F&& function) noexcept -> void {
            auto element = get_next();
            while(element) {
                function(*element);
                element = get_next();
            }
        }

        [[nodiscard]] constexpr auto sort() noexcept
                -> Stream<BufferedPipe<pipe_type, decltype(make_sort_callback())>> {
            auto callback = make_sort_callback();
            using pipe = BufferedPipe<pipe_type, decltype(callback)>;
            return Stream<pipe> {pipe {std::move(_pipe), std::move(callback)}};
        }

        template<typename F>
        [[nodiscard]] constexpr auto sort(F&& comparator) noexcept
                -> Stream<BufferedPipe<pipe_type, decltype(make_sort_callback(std::forward<F>(comparator)))>> {
            auto callback = make_sort_callback(std::forward<F>(comparator));
            using pipe = BufferedPipe<pipe_type, decltype(callback)>;
            return Stream<pipe> {pipe {std::move(_pipe), std::move(callback)}};
        }

        template<template<typename, typename...> typename CONTAINER, typename COLLECTOR>
        [[nodiscard]] constexpr auto collect(COLLECTOR&& collector) noexcept
                -> CONTAINER<std::remove_cv_t<std::remove_reference_t<value_type>>> {
            CONTAINER<std::remove_cv_t<std::remove_reference_t<value_type>>> result {};
            collector(*this, result);
            return result;
        }

        template<template<typename, typename, typename...> typename MAP, typename KM, typename VM>
        [[nodiscard]] constexpr auto collect_map(KM&& key_mapper, VM&& value_mapper) noexcept
                -> MAP<std::invoke_result_t<KM, value_type&>, std::invoke_result_t<VM, value_type&>> {
            MAP<std::invoke_result_t<KM, value_type&>, std::invoke_result_t<VM, value_type&>> result {};
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
        using pipe = IteratorPipe<ITERATOR>;
        return Stream<pipe> {pipe {begin, end}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::iterator>> {
        using pipe = IteratorPipe<typename CONTAINER::iterator>;
        return Stream<pipe> {pipe {container.begin(), container.end()}};
    }

    template<typename CONTAINER>
    [[nodiscard]] constexpr auto stream(const CONTAINER& container) noexcept
            -> Stream<IteratorPipe<typename CONTAINER::const_iterator>> {
        using pipe = IteratorPipe<typename CONTAINER::const_iterator>;
        return Stream<pipe> {pipe {container.cbegin(), container.cend()}};
    }

    template<typename SUPPLIER>
    [[nodiscard]] constexpr auto stream(SUPPLIER&& supplier) noexcept -> Stream<SupplierPipe<SUPPLIER>> {
        using pipe = SupplierPipe<SUPPLIER>;
        return Stream<pipe> {pipe {std::forward<SUPPLIER>(supplier)}};
    }
}// namespace kstd::streams