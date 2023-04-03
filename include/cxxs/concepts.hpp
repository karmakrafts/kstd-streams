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

namespace cxxs::concepts {
    template<typename T> concept has_add = requires(T type) {
        type + type;
        requires std::convertible_to<decltype(type + type), T>;
    };

    template<typename T> //
    concept has_lth = requires(T type) {
        type < type;
        requires std::same_as<decltype(type < type), bool>;
    };

    template<typename T> //
    concept has_gth = requires(T type) {
        type > type;
        requires std::same_as<decltype(type > type), bool>;
    };

    template<typename T> //
    concept has_push_back = requires(T type, typename T::value_type value) {
        type.push_back(value);
    };

    template<typename T> //
    concept has_erase = requires(T type, typename T::iterator value) {
        type.erase(value);
    };

    template<typename K, typename V, template<typename, typename, typename...> typename M> //
    concept has_key_value_indexer = requires(M<K, V> type, K key, V value) {
        type[key] = value;
    };

    template<typename T> //
    concept is_streamable = requires(T type) {
        typename T::value_type;
        type.next();
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
        requires is_iterator<decltype(type.cbegin())>;
        requires is_iterator<decltype(type.cend())>;
    };

    template<typename T> //
    concept is_const_reverse_iterable = requires(T type) {
        typename T::const_iterator;
        type.crbegin();
        type.crend();
        requires is_iterator<decltype(type.crbegin())>;
        requires is_iterator<decltype(type.crend())>;
    };

    template<typename T> //
    concept is_iterable = requires(T type) {
        typename T::iterator;
        type.begin();
        type.end();
        requires is_iterator<decltype(type.begin())>;
        requires is_iterator<decltype(type.end())>;
    };
}