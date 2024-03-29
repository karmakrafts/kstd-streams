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
 * @since 21/07/2023
 */

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_Stream, test_map_value) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    const std::vector values {"Hello"s, "World"s, "Test"s};

    // clang-format off
    const auto mapped_values = stream(values)
        .map([](const std::string& value) {
            return value + '!';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    const auto num_values = values.size();
    ASSERT_EQ(mapped_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(mapped_values[index], values[index] + '!');
    }
}

TEST(kstd_streams_Stream, test_map_pointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector values {"Hello"s, "World"s, "Test"s};
    std::vector<std::string*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    // clang-format off
    const auto mapped_values = stream(addresses)
        .map([](std::string* value) {
            return *value + '!';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    const auto num_values = values.size();
    ASSERT_EQ(mapped_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(mapped_values[index], values[index] + '!');
    }
}

TEST(kstd_streams_Stream, test_map_const_pointer) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    std::vector values {"Hello"s, "World"s, "Test"s};
    std::vector<const std::string*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    // clang-format off
    const auto mapped_values = stream(addresses)
        .map([](const std::string* value) {
            return *value + '!';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    const auto num_values = values.size();
    ASSERT_EQ(mapped_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(mapped_values[index], values[index] + '!');
    }
}

TEST(kstd_streams_Stream, test_map_format_value) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    const std::vector values {"Hello"s, "World"s, "Test"s};

    // clang-format off
    const auto mapped_values = stream(values)
        .map(mappers::format("{}!"))
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    const auto num_values = values.size();
    ASSERT_EQ(mapped_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(mapped_values[index], values[index] + "!");
    }
}