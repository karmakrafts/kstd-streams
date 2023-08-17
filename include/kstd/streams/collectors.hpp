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
 * @since 22/07/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/types.hpp>
#include <type_traits>

namespace kstd::streams::collectors {
    constexpr auto insert = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        while(element) {
            result.insert(*element);
            element = pipe.get_next();
        }
    };

    constexpr auto push_back = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        while(element) {
            result.push_back(*element);
            element = pipe.get_next();
        }
    };

    constexpr auto plus_assign = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        while(element) {
            result += *element;
            element = pipe.get_next();
        }
    };

    constexpr auto subscript = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        while(element) {
            auto& [key, value] = *element;
            result[key] = value;
            element = pipe.get_next();
        }
    };

    constexpr auto subscript_indexed = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        usize index = 0;
        while(element) {
            result[index] = *element;
            ++index;
            element = pipe.get_next();
        }
    };

    constexpr auto subscript_indexed_reverse = [](auto& pipe, auto& result) noexcept -> void {
        auto element = pipe.get_next();
        usize index = 0;
        while(element) {
            result[index] = *element;
            ++index;
            element = pipe.get_next();
        }
        result = decltype(result) {result.crbegin(), result.crend()};
    };

    template<typename D>
    [[nodiscard]] constexpr auto joining(D delimiter) noexcept -> decltype(auto) {
        return [delimiter](auto& pipe, auto& result) noexcept -> void {
            using PipeType = std::remove_reference_t<decltype(pipe)>;
            using Type = std::remove_const_t<std::remove_reference_t<typename PipeType::ValueType>>;
            static_assert(std::is_same_v<Type, D>, "Delimiter type must match pipe value type");

            auto element = pipe.get_next();
            while(element) {
                result += *element;
                element = pipe.get_next();
                if(element) {
                    result += delimiter;
                }
            }
        };
    }
}// namespace kstd::streams::collectors