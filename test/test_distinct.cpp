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
 * @since 04/08/2023
 */

#include <algorithm>
#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_Stream, test_distinct_value) {
    using namespace kstd::streams;
    using namespace std::string_literals;

    const std::vector values {"Hello"s, "World"s, "Hello"s, "!"s, "Hello"s, ":3"s};
    const auto distinct_values = stream(values).distinct().collect<std::vector>(collectors::push_back);

    ASSERT_FALSE(distinct_values.empty());
    ASSERT_EQ(distinct_values.size(), 4);

    const auto contains = [&distinct_values](const std::string& value) -> bool {
        return std::find(distinct_values.cbegin(), distinct_values.cend(), value) != distinct_values.cend();
    };

    ASSERT_TRUE(contains("Hello"));
    ASSERT_TRUE(contains("World"));
    ASSERT_TRUE(contains("!"));
    ASSERT_TRUE(contains(":3"));
}

TEST(kstd_streams_Stream, test_distinct_pointer) {
    using namespace kstd::streams;

    std::string value1 {"Hello World"};
    std::string value2 {"!:3"};
    const std::vector<std::string*> addresses {&value2, &value2, &value1, &value2, &value1};
    // clang-format off
    const auto distinct_values = stream(addresses)
        .distinct()
        .deref_all()
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    ASSERT_FALSE(distinct_values.empty());
    ASSERT_EQ(distinct_values.size(), 2);

    const auto contains = [&distinct_values](const std::string& value) -> bool {
        return std::find(distinct_values.cbegin(), distinct_values.cend(), value) != distinct_values.cend();
    };

    ASSERT_TRUE(contains("Hello World"));
    ASSERT_TRUE(contains("!:3"));
}

TEST(kstd_streams_Stream, test_distinct_const_pointer) {
    using namespace kstd::streams;

    const std::string value1 {"Hello World"};
    const std::string value2 {"!:3"};
    const std::vector<const std::string*> addresses {&value2, &value2, &value1, &value2, &value1};
    // clang-format off
    const auto distinct_values = stream(addresses)
        .distinct()
        .deref_all()
        .collect<std::vector>(collectors::push_back);
    // clang-format on

    ASSERT_FALSE(distinct_values.empty());
    ASSERT_EQ(distinct_values.size(), 2);

    const auto contains = [&distinct_values](const std::string& value) -> bool {
        return std::find(distinct_values.cbegin(), distinct_values.cend(), value) != distinct_values.cend();
    };

    ASSERT_TRUE(contains("Hello World"));
    ASSERT_TRUE(contains("!:3"));
}