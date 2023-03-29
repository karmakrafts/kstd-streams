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
#include <cxxs/stream.hpp>

const std::vector<float> test_values({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
const size_t num_test_values = test_values.size();

TEST(cxxs_Stream, TestDistinct) {
    std::vector<std::vector<float>> nested_values({test_values, test_values, test_values, test_values});

    // @formatter:off
    const auto result = cxxs::stream(nested_values)
            .flat_map([](auto& x) { return cxxs::owning(x); })
            .distinct()
            .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(cxxs_Stream, TestChain) {
    std::vector<float> expected({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F, 1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    const auto chained_count = expected.size();
    auto result = (cxxs::stream(test_values) | cxxs::stream(test_values)).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0; i < chained_count; i++) {
        ASSERT_EQ(result[i], expected[i]);
    }

    result = (cxxs::stream(test_values) | test_values).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0; i < chained_count; i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(cxxs_Stream, TestPreChain) {
    std::vector<float> expected({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F, 1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    const auto chained_count = expected.size();
    auto result = (cxxs::stream(test_values) || cxxs::stream(test_values)).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0; i < chained_count; i++) {
        ASSERT_EQ(result[i], expected[i]);
    }

    result = (cxxs::stream(test_values) || test_values).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0; i < chained_count; i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(cxxs_Stream, TestReduce) {
    // @formatter:off
    const auto result = cxxs::stream(test_values)
        .reduce([](auto a, auto b) { return a * b; });
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1417.5F);
}

TEST(cxxs_Stream, TestSum) {
    const auto result = cxxs::stream(test_values).sum();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 22.0F);
}

TEST(cxxs_Stream, TestFindFirst) {
    auto result = cxxs::stream(test_values).find_first();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);

    // @formatter:off
    result = cxxs::stream(test_values)
        .filter([](auto& x) { return x == 3.0F; })
        .find_first();
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 3.0F);

    // @formatter:off
    result = cxxs::stream(test_values)
        .filter([](auto& x) { return x > 10.0F; })
        .find_first();
    // @formatter:on

    ASSERT_FALSE(result);
}

TEST(cxxs_Stream, TestCollect) {
    const auto result = cxxs::stream(test_values).collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(cxxs_Stream, TestCount) {
    const auto result = cxxs::stream(test_values).count();
    ASSERT_EQ(result, num_test_values);
}

TEST(cxxs_Stream, TestFilter) {
    // @formatter:off
    const auto result = cxxs::stream(test_values)
        .filter([](auto& x) { return (static_cast<int32_t>(x) & 1) == 0; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), 4);

    ASSERT_EQ(result[0], 2.0F);
    ASSERT_EQ(result[1], 4.0F);
    ASSERT_EQ(result[2], 2.5F);
    ASSERT_EQ(result[3], 4.5F);
}

TEST(cxxs_Stream, TestMap) {
    // @formatter:off
    const auto result1 = cxxs::stream(test_values)
        .map([](auto& x) { return x; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result1.size(), num_test_values);

    for (size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result1[i], test_values[i]);
    }

    // @formatter:off
    const auto result2 = cxxs::stream(test_values)
        .map([](auto& x) { return static_cast<int32_t>(x); })
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

TEST(cxxs_Stream, TestFlatMap) {
    std::vector<std::vector<float>> nested_values({test_values, test_values, test_values, test_values});

    // @formatter:off
    const auto result = cxxs::stream(nested_values)
        .flat_map([](auto& x) { return cxxs::owning(x); })
        .collect<std::vector>();
    // @formatter:on

    const auto flat_count = test_values.size() << 2;
    ASSERT_EQ(result.size(), flat_count);

    size_t sub_index = 0;

    for (size_t i = 0; i < flat_count; i++) {
        ASSERT_EQ(result[i], test_values[sub_index]);

        if (sub_index < num_test_values - 1) {
            sub_index++;
        }
        else {
            sub_index = 0;
        }
    }
}

TEST(cxxs_Stream, TestMin) {
    const auto result = cxxs::stream(test_values).min();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);
}

TEST(cxxs_Stream, TestMax) {
    const auto result = cxxs::stream(test_values).max();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 4.5F);
}

TEST(cxxs_Stream, TestLimit) {
    constexpr std::size_t max_count = 4;

    // @formatter:off
    const auto result = cxxs::stream(test_values)
        .limit(max_count)
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), max_count);

    for (size_t i = 0; i < max_count; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}