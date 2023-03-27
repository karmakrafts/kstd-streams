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

#include <concepts>
#include <optional>

namespace cxxstreams::concepts {
    template<typename T> //
    concept has_add_assign = requires(T type) {
        type += type;
    };

    template<typename T> //
    concept has_push_back = requires(T type, typename T::value_type value) {
        type.push_back(value);
    };

    template<typename T> //
    concept is_streamable = requires(T type) {
        typename T::value_type;
        requires std::same_as<decltype(type.next()), std::optional<typename T::value_type>>;
    };

    template<typename T> //
    concept is_iterator = requires(T type) {
        typename T::value_type;
        ++type;
        type++;
        *type;
        type == type;
        type != type;
        requires std::convertible_to<decltype(++type), T>;
        requires std::convertible_to<decltype(type++), T>;
        requires std::convertible_to<decltype(*type), typename T::value_type>;
        requires std::same_as<decltype(type == type), bool>;
        requires std::same_as<decltype(type != type), bool>;
    };

    template<typename T> //
    concept is_const_iterable = requires(T type) {
        typename T::const_iterator;
        type.cbegin();
        type.cend();
        requires std::same_as<decltype(type.cbegin()), typename T::const_iterator>;
        requires std::same_as<decltype(type.cend()), typename T::const_iterator>;
    };

    template<typename T> //
    concept is_const_reverse_iterable = requires(T type) {
        typename T::const_iterator;
        type.crbegin();
        type.crend();
        requires std::same_as<decltype(type.crbegin()), typename T::const_iterator>;
        requires std::same_as<decltype(type.crend()), typename T::const_iterator>;
    };
}