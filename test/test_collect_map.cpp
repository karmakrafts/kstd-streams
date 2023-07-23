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
 * @since 19/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <kstd/tuple.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_Stream, TestCollectMapValue) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector<kstd::Tuple<std::string, kstd::i32>> values {{"Hello"s, 20}, {"World"s, 30}, {"Test"s, 40}};

    const auto map = stream(values).collect_map<std::unordered_map>(
            // clang-format off
        [](kstd::Tuple<std::string, kstd::i32>& value) {
            return value.get<0>();
        },
        [](kstd::Tuple<std::string, kstd::i32>& value) {
            return value.get<1>();
        }// clang-format on
    );

    const auto num_values = values.size();
    ASSERT_EQ(map.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        const auto& [key, value] = values[index];
        auto iterator = map.find(key);
        ASSERT_NE(iterator, map.end());
        ASSERT_EQ(iterator->second, value);
    }
}

TEST(kstd_streams_Stream, TestCollectMapPointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector<kstd::Tuple<std::string, kstd::i32>> values {{"Hello"s, 20}, {"World"s, 30}, {"Test"s, 40}};
    std::vector<kstd::Tuple<std::string, kstd::i32>*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto map = stream(addresses).collect_map<std::unordered_map>(
            // clang-format off
        [](kstd::Tuple<std::string, kstd::i32>* value) {
            return value->get<0>();
        },
        [](kstd::Tuple<std::string, kstd::i32>* value) {
            return value->get<1>();
        }// clang-format on
    );

    const auto num_values = values.size();
    ASSERT_EQ(map.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        const auto& [key, value] = values[index];
        auto iterator = map.find(key);
        ASSERT_NE(iterator, map.end());
        ASSERT_EQ(iterator->second, value);
    }
}

TEST(kstd_streams_Stream, TestCollectMapConstPointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector<kstd::Tuple<std::string, kstd::i32>> values {{"Hello"s, 20}, {"World"s, 30}, {"Test"s, 40}};
    std::vector<const kstd::Tuple<std::string, kstd::i32>*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto map = stream(addresses).collect_map<std::unordered_map>(
            // clang-format off
        [](const kstd::Tuple<std::string, kstd::i32>* value) {
            return value->get<0>();
        },
        [](const kstd::Tuple<std::string, kstd::i32>* value) {
            return value->get<1>();
        }// clang-format on
    );

    const auto num_values = values.size();
    ASSERT_EQ(map.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        const auto& [key, value] = values[index];
        auto iterator = map.find(key);
        ASSERT_NE(iterator, map.end());
        ASSERT_EQ(iterator->second, value);
    }
}