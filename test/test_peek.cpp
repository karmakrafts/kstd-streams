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
 * @since 23/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_Stream, TestPeekValue) {
    using namespace kstd::streams;

    std::vector<std::string> values {"Hello", "World", "OwO"};
    const auto num_values = values.size();
    // clang-format off
    const auto peeked_values = stream(values)
        .peek([](std::string& value) {
            value += '!';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(peeked_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(peeked_values[index], values[index]);
    }
}

TEST(kstd_streams_Stream, TestPeekPointer) {
    using namespace kstd::streams;

    std::vector<std::string> values {"Hello", "World", "OwO"};
    std::vector<std::string*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    // clang-format off
    const auto peeked_values = stream(addresses)
        .peek([](std::string* value) {
            (*value) += '!';
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(peeked_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(*peeked_values[index], values[index]);
    }
}

TEST(kstd_streams_Stream, TestPeekConstPointer) {
    using namespace kstd::streams;

    std::vector<std::string> values {"Hello", "World", "OwO"};
    std::vector<const std::string*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    kstd::usize peek_index = 0;
    // clang-format off
    const auto peeked_values = stream(addresses)
        .peek([&peek_index, &values](const std::string* value) {
            ASSERT_EQ(*value, values[peek_index]);
            ++peek_index;
        })
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(peeked_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(*peeked_values[index], values[index]);
    }
}