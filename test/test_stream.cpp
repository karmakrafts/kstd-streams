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

#include <vector>
#include <gtest/gtest.h>
#include <cxxstreams/stream.hpp>

const std::vector<float> test_values({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
const std::size_t num_test_values = test_values.size();

TEST(cxxstreams_Stream, TestReduce) {
    // @formatter:off
    const auto result = cxxstreams::make_stream(test_values)
        .reduce([](auto a, auto b) { return a * b; });
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1417.5F);
}

TEST(cxxstreams_Stream, TestSum) {
    const auto result = cxxstreams::make_stream(test_values).sum();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 22.0F);
}

TEST(cxxstreams_Stream, TestFindFirst) {
    auto result = cxxstreams::make_stream(test_values).find_first();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);

    // @formatter:off
    result = cxxstreams::make_stream(test_values)
        .filter([](auto x) { return x == 3.0F; })
        .find_first();
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 3.0F);

    // @formatter:off
    result = cxxstreams::make_stream(test_values)
        .filter([](auto x) { return x > 10.0F; })
        .find_first();
    // @formatter:on

    ASSERT_FALSE(result);
}

TEST(cxxstreams_Stream, TestCollect) {
    const auto result = cxxstreams::make_stream(test_values).collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(cxxstreams_Stream, TestCount) {
    const auto result = cxxstreams::make_stream(test_values).count();
    ASSERT_EQ(result, num_test_values);
}

TEST(cxxstreams_Stream, TestFilter) {
    // @formatter:off
    const auto result = cxxstreams::make_stream(test_values)
        .filter([](auto x) { return (static_cast<std::int32_t>(x) & 1) == 0; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), 4);

    ASSERT_EQ(result[0], 2.0F);
    ASSERT_EQ(result[1], 4.0F);
    ASSERT_EQ(result[2], 2.5F);
    ASSERT_EQ(result[3], 4.5F);
}

TEST(cxxstreams_Stream, TestMap) {
    // @formatter:off
    const auto result1 = cxxstreams::make_stream(test_values)
        .map([](auto x) { return x; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result1.size(), num_test_values);

    for (size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result1[i], test_values[i]);
    }

    // @formatter:off
    const auto result2 = cxxstreams::make_stream(test_values)
        .map([](auto x) { return static_cast<int32_t>(x); })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result2.size(), num_test_values);

    ASSERT_EQ(result2[0], 1);
    ASSERT_EQ(result2[1], 2);
    ASSERT_EQ(result2[2], 3);
    ASSERT_EQ(result2[3], 4);

    ASSERT_EQ(result2[4], 1);
    ASSERT_EQ(result2[5], 2);
    ASSERT_EQ(result2[6], 3);
    ASSERT_EQ(result2[7], 4);
}

TEST(cxxstreams_Stream, TestMin) {
    const auto result = cxxstreams::make_stream(test_values).min();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);
}

TEST(cxxstreams_Stream, TestMax) {
    const auto result = cxxstreams::make_stream(test_values).max();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 4.5F);
}

TEST(cxxstreams_Stream, TestLimit) {
    constexpr std::size_t max_count = 4;

    // @formatter:off
    const auto result = cxxstreams::make_stream(test_values)
        .limit(max_count)
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), max_count);

    for (size_t i = 0; i < max_count; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}