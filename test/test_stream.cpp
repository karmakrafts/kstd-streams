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

#include <array>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <string_view>
#include <gtest/gtest.h>
#include "kstd/streams/stream.hpp"

const std::vector<float> test_values({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
const size_t num_test_values = test_values.size();

struct Foo final {
    int32_t i_value;
    float f_value;

    [[nodiscard]] constexpr auto operator ==(const Foo& other) const noexcept -> bool {
        return i_value == other.i_value && f_value == other.f_value;
    }

    [[nodiscard]] constexpr auto operator <(const Foo& other) const noexcept -> bool {
        return i_value < other.i_value && f_value < other.f_value;
    }
};

[[nodiscard]] inline auto generate_string(size_t length = 16) noexcept -> std::string {
    constexpr std::string_view allowed_chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<size_t> dist(0, allowed_chars.size() - 1);
    std::string result;
    result.resize(length);

    for (size_t i = 0;
         i < length;
         i++) {
        result[i] = allowed_chars[dist(generator)];
    }

    return result;
}

TEST(kstd_streams_Stream, TestCollectFromMap) {
    constexpr size_t num_entries = 32;
    std::unordered_map<std::string, float> entries;
    std::vector<std::string> ordered_keys(num_entries);

    for (size_t i = 0;
         i < num_entries;
         ++i) {
        const auto key = generate_string();
        entries[key] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX); // NOLINT
        ordered_keys[i] = key;
    }

    const auto keys = kstd::streams::stream(entries).map([](auto& x) {
        return x.first;
    }).collect<std::vector>();

    ASSERT_EQ(keys.size(), ordered_keys.size());

    for (size_t i = 0;
         i < num_entries;
         ++i) {
        ASSERT_TRUE(std::find(ordered_keys.cbegin(), ordered_keys.cend(), keys[i]) != ordered_keys.cend());
    }

    const auto values = kstd::streams::stream(entries).map([](auto& x) {
        return x.second;
    }).collect<std::vector>();

    ASSERT_EQ(values.size(), entries.size());

    for (const auto& [key, value]: entries) {
        ASSERT_TRUE(std::find(values.cbegin(), values.cend(), value) != values.cend());
    }
}

TEST(kstd_streams_Stream, TestSorted) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .sorted([](const auto& a, const auto& b) { return a > b; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    ASSERT_EQ(result[0], 4.5F);
    ASSERT_EQ(result[1], 4.0F);
    ASSERT_EQ(result[2], 3.5F);
    ASSERT_EQ(result[3], 3.0F);
    ASSERT_EQ(result[4], 2.5F);
    ASSERT_EQ(result[5], 2.0F);
    ASSERT_EQ(result[6], 1.5F);
    ASSERT_EQ(result[7], 1.0F);
}

TEST(kstd_streams_Stream, TestSortedDefault) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .sorted()
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    ASSERT_EQ(result[7], 4.5F);
    ASSERT_EQ(result[6], 4.0F);
    ASSERT_EQ(result[5], 3.5F);
    ASSERT_EQ(result[4], 3.0F);
    ASSERT_EQ(result[3], 2.5F);
    ASSERT_EQ(result[2], 2.0F);
    ASSERT_EQ(result[1], 1.5F);
    ASSERT_EQ(result[0], 1.0F);
}

TEST(kstd_streams_Stream, TestDerefAll) {
    std::vector<const float*> pointers;

    for (const auto& value: test_values) {
        pointers.push_back(&value);
    }

    const auto result = kstd::streams::stream(pointers).deref_all().collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestDerefNotNull) {
    std::vector<const float*> pointers;

    for (const auto& value: test_values) {
        pointers.push_back(&value);
        pointers.push_back(nullptr);
    }

    const auto result = kstd::streams::stream(pointers).deref_not_null().collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestAnyMatch) {
    ASSERT_TRUE(kstd::streams::stream(test_values).any_match([](auto& x) {
        return x > 1.0F;
    }));

    ASSERT_FALSE(kstd::streams::stream(test_values).any_match([](auto& x) {
        return x < 0.0F;
    }));
}

TEST(kstd_streams_Stream, TestAllMatch) {
    ASSERT_TRUE(kstd::streams::stream(test_values).all_match([](auto& x) {
        return x > 0.0F;
    }));

    ASSERT_FALSE(kstd::streams::stream(test_values).all_match([](auto& x) {
        return x > 2.0F;
    }));
}

TEST(kstd_streams_Stream, TestNoneMatch) {
    ASSERT_TRUE(kstd::streams::stream(test_values).none_match([](auto& x) {
        return x > 5.0F;
    }));

    ASSERT_FALSE(kstd::streams::stream(test_values).none_match([](auto& x) {
        return x > 0.0F;
    }));
}

TEST(kstd_streams_Stream, TestPeek) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .peek([](auto& x) { x += 0.5F; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i] + 0.5F);
    }
}

TEST(kstd_streams_Stream, TestForEach) {
    size_t index = 0;

    kstd::streams::stream(test_values).for_each([&index](auto& x) {
        ASSERT_EQ(x, test_values[index++]);
    });
}

TEST(kstd_streams_Stream, TestForEachIndexed) {
    kstd::streams::stream(test_values).for_each_indexed([](auto& x, auto i) {
        ASSERT_EQ(x, test_values[i]);
    });
}

TEST(kstd_streams_Stream, TestZip) {
    // @formatter:off
    std::vector<Foo> values({{10, 12.0F},
        {12, 14.0F},
        {14, 16.0F}});
    // @formatter:on

    // @formatter:off
    const auto result = kstd::streams::stream(values)
        .zip(
            [](auto& x) { return x.i_value; },
            [](auto& x) { return x.f_value; }
        )
        .collect<std::vector>();
    // @formatter:on

    const auto num_values = values.size();
    ASSERT_EQ(result.size(), num_values);

    for (size_t i = 0;
         i < num_values;
         i++) {
        auto& [expected_int, expected_float] = values[i];
        auto& [actual_int, actual_float] = result[i];
        ASSERT_EQ(actual_int, expected_int);
        ASSERT_EQ(actual_float, actual_float);
    }
}

TEST(kstd_streams_Stream, TestFlatZip) {
    // @formatter:off
    std::vector<Foo> values({{10, 12.0F},
        {12, 14.0F},
        {14, 16.0F}});
    // @formatter:on
    const auto num_values = values.size();

    std::vector<std::vector<Foo>> nested_values({values, values, values});

    // @formatter:off
    const auto result = kstd::streams::stream(nested_values)
        .flat_zip(
            [](auto& x) { return kstd::streams::owning(x); },
            [](auto& x) { return kstd::streams::owning(x); }
        )
        .collect<std::vector>();
    // @formatter:on

    const auto flat_count = 9;
    ASSERT_EQ(result.size(), flat_count);
    size_t sub_index = 0;

    for (size_t i = 0;
         i < flat_count;
         i++) {
        auto& [expected_int, expected_float] = values[sub_index];
        auto& [left_foo, right_foo] = result[i];

        ASSERT_EQ(left_foo.i_value, expected_int);
        ASSERT_EQ(left_foo.f_value, expected_float);

        ASSERT_EQ(right_foo.i_value, expected_int);
        ASSERT_EQ(right_foo.f_value, expected_float);

        if (sub_index < num_values - 1) {
            sub_index++;
        }
        else {
            sub_index = 0;
        }
    }
}

TEST(kstd_streams_Stream, TestCollectToMemory) {
    std::vector<float> target(num_test_values);
    kstd::streams::stream(test_values).collect_to_memory(target.data(), num_test_values);

    ASSERT_EQ(target.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(target[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestSkip) {
    const auto num_values = num_test_values - 2;

    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .skip(2)
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_values);

    for (size_t i = 0;
         i < num_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i + 2]);
    }
}

TEST(kstd_streams_Stream, TestDistinct) {
    std::vector<std::vector<float>> nested_values({test_values, test_values, test_values, test_values});

    // @formatter:off
    const auto result = kstd::streams::stream(nested_values)
        .flat_map([](auto& x) { return kstd::streams::owning(x); })
        .distinct()
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestChain) {
    std::vector<float> expected({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F, 1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    const auto chained_count = expected.size();
    auto result = (kstd::streams::stream(test_values).chain(kstd::streams::stream(test_values))).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0;
         i < chained_count;
         i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(kstd_streams_Stream, TestPreChain) {
    std::vector<float> expected({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F, 1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    const auto chained_count = expected.size();
    auto result = (kstd::streams::stream(test_values).pre_chain(kstd::streams::stream(test_values))).collect<std::vector>();

    ASSERT_EQ(result.size(), chained_count);

    for (size_t i = 0;
         i < chained_count;
         i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(kstd_streams_Stream, TestReduce) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .reduce([](auto a, auto b) { return a * b; });
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1417.5F);
}

TEST(kstd_streams_Stream, TestSum) {
    const auto result = kstd::streams::stream(test_values).sum();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 22.0F);
}

TEST(kstd_streams_Stream, TestFindFirst) {
    auto result = kstd::streams::stream(test_values).find_first();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);

    // @formatter:off
    result = kstd::streams::stream(test_values)
        .filter([](auto& x) { return x == 3.0F; })
        .find_first();
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 3.0F);

    // @formatter:off
    result = kstd::streams::stream(test_values)
        .filter([](auto& x) { return x > 10.0F; })
        .find_first();
    // @formatter:on

    ASSERT_FALSE(result);
}

TEST(kstd_streams_Stream, TestFindLast) {
    auto result = kstd::streams::stream(test_values).find_last();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 4.5F);

    // @formatter:off
    result = kstd::streams::stream(test_values)
        .filter([](auto& x) { return x == 3.0F; })
        .find_last();
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 3.0F);

    // @formatter:off
    result = kstd::streams::stream(test_values)
        .filter([](auto& x) { return x > 2.0F && x < 4.0F; })
        .find_last();
    // @formatter:on

    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 3.5F);
}

TEST(kstd_streams_Stream, TestCollect) {
    const auto result = kstd::streams::stream(test_values).collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestCollectMap) {
    size_t index = 0;

    // @formatter:off
    auto result = kstd::streams::stream(test_values).collect_map<std::unordered_map>(
        [&index](auto& x) { return index++; },
        [](auto& x) { return x; }
    );
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestCollectToSequence) {
    constexpr size_t extent = 4;
    const auto result = kstd::streams::stream(test_values).collect_sequence<extent, std::array>();
    ASSERT_EQ(result.size(), extent);

    for (size_t i = 0;
         i < extent;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestCount) {
    const auto result = kstd::streams::stream(test_values).count();
    ASSERT_EQ(result, num_test_values);
}

TEST(kstd_streams_Stream, TestFilter) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .filter([](auto& x) { return (static_cast<int32_t>(x) & 1) == 0; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), 4);

    ASSERT_EQ(result[0], 2.0F);
    ASSERT_EQ(result[1], 4.0F);
    ASSERT_EQ(result[2], 2.5F);
    ASSERT_EQ(result[3], 4.5F);
}

TEST(kstd_streams_Stream, TestDropWhile) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .drop_while([](auto& x) { return x < 3.0F; })
        .collect<std::vector>();
    // @formatter:on

    const auto num_remaining = test_values.size() - 2;
    ASSERT_EQ(result.size(), num_remaining);

    for (size_t i = 0;
         i < num_remaining;
         i++) {
        ASSERT_EQ(result[i], test_values[i + 2]);
    }
}

TEST(kstd_streams_Stream, TestTakeWhile) {
    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .take_while([](auto& x) { return x <= 4.0F; })
        .collect<std::vector>();
    // @formatter:on

    const auto num_taken = test_values.size() - 1;
    ASSERT_EQ(result.size(), num_taken);

    for (size_t i = 0;
         i < num_taken;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestFilterNotNull) {
    std::vector<const float*> pointers;

    for (const auto& value: test_values) {
        pointers.push_back(&value);
        pointers.push_back(nullptr);
    }

    // @formatter:off
    const auto result = kstd::streams::stream(pointers)
        .filter_not_null()
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(*result[i], test_values[i]);
    }
}

TEST(kstd_streams_Stream, TestMap) {
    // @formatter:off
    const auto result1 = kstd::streams::stream(test_values)
        .map([](auto& x) { return x; })
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result1.size(), num_test_values);

    for (size_t i = 0;
         i < num_test_values;
         i++) {
        ASSERT_EQ(result1[i], test_values[i]);
    }

    // @formatter:off
    const auto result2 = kstd::streams::stream(test_values)
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

TEST(kstd_streams_Stream, TestFlatMap) {
    std::vector<std::vector<float>> nested_values({test_values, test_values, test_values, test_values});

    // @formatter:off
    const auto result = kstd::streams::stream(nested_values)
        .flat_map([](auto& x) { return kstd::streams::owning(x); })
        .collect<std::vector>();
    // @formatter:on

    const auto flat_count = test_values.size() << 2;
    ASSERT_EQ(result.size(), flat_count);

    size_t sub_index = 0;

    for (size_t i = 0;
         i < flat_count;
         i++) {
        ASSERT_EQ(result[i], test_values[sub_index]);

        if (sub_index < num_test_values - 1) {
            sub_index++;
        }
        else {
            sub_index = 0;
        }
    }
}

TEST(kstd_streams_Stream, TestMin) {
    const auto result = kstd::streams::stream(test_values).min();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 1.0F);
}

TEST(kstd_streams_Stream, TestMax) {
    const auto result = kstd::streams::stream(test_values).max();
    ASSERT_TRUE(result);
    ASSERT_EQ(*result, 4.5F);
}

TEST(kstd_streams_Stream, TestLimit) {
    constexpr std::size_t max_count = 4;

    // @formatter:off
    const auto result = kstd::streams::stream(test_values)
        .limit(max_count)
        .collect<std::vector>();
    // @formatter:on

    ASSERT_EQ(result.size(), max_count);

    for (size_t i = 0;
         i < max_count;
         i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
}