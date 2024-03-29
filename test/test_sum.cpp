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
 * @since 24/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <vector>

TEST(kstd_streams_Stream, test_sum_value) {
    using namespace kstd::streams;

    const std::vector<kstd::u32> values {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    const auto num_values = values.size();

    const auto sum = stream(values).sum();
    kstd::u32 expected_sum = 0;

    for(kstd::usize index = 0; index < num_values; ++index) {
        expected_sum += values[index];
    }

    ASSERT_EQ(sum, expected_sum);
}

TEST(kstd_streams_Stream, test_sum_pointer) {
    using namespace kstd::streams;

    std::vector<kstd::u32> values {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    std::vector<kstd::u32*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    const auto sum = stream(addresses).map(mappers::dereference).sum();
    kstd::u32 expected_sum = 0;

    for(kstd::usize index = 0; index < num_values; ++index) {
        expected_sum += values[index];
    }

    ASSERT_EQ(sum, expected_sum);
}

TEST(kstd_streams_Stream, test_sum_const_pointer) {
    using namespace kstd::streams;

    std::vector<kstd::u32> values {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    std::vector<const kstd::u32*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto num_values = addresses.size();
    const auto sum = stream(addresses).map(mappers::dereference).sum();
    kstd::u32 expected_sum = 0;

    for(kstd::usize index = 0; index < num_values; ++index) {
        expected_sum += values[index];
    }

    ASSERT_EQ(sum, expected_sum);
}