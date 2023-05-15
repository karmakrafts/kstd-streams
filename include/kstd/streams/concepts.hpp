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

#include "kstd/concepts.hpp"
#include "kstd/option.hpp"

namespace kstd::streams::concepts {
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
        requires std::same_as<decltype(type.next()), Option<typename T::value_type>>;
    };
}