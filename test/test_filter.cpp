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
#include <string>
#include <vector>

TEST(kstd_streams_Stream, TestFilterValue) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    const std::vector<std::string> values {"Hello"s, "World"s, "OwO"s};
    const auto num_values = values.size();
    // clang-format off
    auto filtered_values = stream(values)
        .filter([](auto) {
            return true;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(num_values, filtered_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], filtered_values[index]);
    }

    // clang-format off
    filtered_values = stream(values)
        .filter([](auto&) {
            return false;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_TRUE(filtered_values.empty());

    // clang-format off
    filtered_values = stream(values)
        .filter([](const std::string& value) {
            return !value.empty() && value[0] == 'H';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(filtered_values.size(), 1);
    ASSERT_EQ(filtered_values[0], "Hello"s);
}

TEST(kstd_streams_Stream, TestFilterPointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector<std::string> values {"Hello"s, "World"s, "OwO"s};
    std::vector<std::string*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = values.size();
    // clang-format off
    auto filtered_values = stream(addresses)
        .filter([](auto*) {
            return true;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(num_values, filtered_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], *filtered_values[index]);
    }

    // clang-format off
    filtered_values = stream(addresses)
        .filter([](auto*) {
            return false;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_TRUE(filtered_values.empty());

    // clang-format off
    filtered_values = stream(addresses)
        .filter([](auto* value) {
            return !value->empty() && (*value)[0] == 'H';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(filtered_values.size(), 1);
    ASSERT_EQ(*filtered_values[0], "Hello"s);
}

TEST(kstd_streams_Stream, TestFilterConstPointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector<std::string> values {"Hello"s, "World"s, "OwO"s};
    std::vector<const std::string*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = values.size();
    // clang-format off
    auto filtered_values = stream(addresses)
        .filter([](const std::string*) {
            return true;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(num_values, filtered_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], *filtered_values[index]);
    }

    // clang-format off
    filtered_values = stream(addresses)
        .filter([](const auto*) {
            return false;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_TRUE(filtered_values.empty());

    // clang-format off
    filtered_values = stream(addresses)
        .filter([](const std::string* value) {
            return !value->empty() && (*value)[0] == 'H';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(filtered_values.size(), 1);
    ASSERT_EQ(*filtered_values[0], "Hello"s);
}