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
#include <functional>

namespace kstd::streams::concepts {
    template<typename T, typename S> concept Function = requires {
        requires std::convertible_to<T, std::function<S>>;
    };

    template<typename T> concept Addable = requires(T type) {
        type + type;
        requires std::convertible_to<decltype(type + type), T>;
    };

    template<typename T> //
    concept LessThanComparable = requires(T type) {
        type < type;
        requires std::same_as<decltype(type < type), bool>;
    };

    template<typename T> //
    concept GreaterThanComparable = requires(T type) {
        type > type;
        requires std::same_as<decltype(type > type), bool>;
    };

    template<typename T> //
    concept Pushable = requires(T type, typename T::value_type value) {
        type.push_back(value);
    };

    template<typename T> //
    concept Erasable = requires(T type, typename T::iterator value) {
        type.erase(value);
    };

    template<typename K, typename V, template<typename, typename, typename...> typename M> //
    concept Indexable = requires(M<K, V> type, K key, V value) {
        type[key] = value;
    };

    template<typename T> //
    concept Streamable = requires(T type) {
        typename T::value_type;
        type.next();
        requires std::same_as<decltype(type.next()), std::optional<typename T::value_type>>;
    };

    template<typename T> //
    concept Iterator = requires(T type) {
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
    concept ConstIterable = requires(T type) {
        typename T::const_iterator;
        type.cbegin();
        type.cend();
        requires Iterator<decltype(type.cbegin())>;
        requires Iterator<decltype(type.cend())>;
    };

    template<typename T> //
    concept ConstReverseIterable = requires(T type) {
        typename T::const_iterator;
        type.crbegin();
        type.crend();
        requires Iterator<decltype(type.crbegin())>;
        requires Iterator<decltype(type.crend())>;
    };

    template<typename T> //
    concept Iterable = requires(T type) {
        typename T::iterator;
        type.begin();
        type.end();
        requires Iterator<decltype(type.begin())>;
        requires Iterator<decltype(type.end())>;
    };
}