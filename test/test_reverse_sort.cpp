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
#include <vector>

TEST(kstd_streams_Stream, test_reverse_sort_value) {
    using namespace kstd::streams;

    const std::vector<kstd::u32> values {4, 5, 6, 1, 3, 2, 8, 7, 0, 9};
    const auto num_values = values.size();
    // clang-format off
    const auto sorted_values = stream(values)
        .reverse_sort()
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(sorted_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(sorted_values[index], num_values - 1 - index);
    }
}

TEST(kstd_streams_Stream, test_reverse_sort_pointer) {
    using namespace kstd::streams;

    std::vector<kstd::u32> values {4, 5, 6, 1, 3, 2, 8, 7, 0, 9};
    std::vector<kstd::u32*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    // clang-format off
    const auto sorted_values = stream(addresses)
        .reverse_sort(comparators::deref_less_than)
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(sorted_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(*sorted_values[index], num_values - 1 - index);
    }
}

TEST(kstd_streams_Stream, test_reverse_sort_const_pointer) {
    using namespace kstd::streams;

    std::vector<kstd::u32> values {4, 5, 6, 1, 3, 2, 8, 7, 0, 9};
    std::vector<const kstd::u32*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    // clang-format off
    const auto sorted_values = stream(addresses)
        .reverse_sort(comparators::deref_less_than)
        .collect<std::vector>(collectors::push_back);
    // clang-format on
    ASSERT_EQ(sorted_values.size(), num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(*sorted_values[index], num_values - 1 - index);
    }
}