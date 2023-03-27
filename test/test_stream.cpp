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

TEST(cxxstreams_Stream, TestCollect) {
    const auto result = cxxstreams::make_stream(test_values).collect<std::vector>();
    ASSERT_EQ(result.size(), num_test_values);

    for(size_t i = 0; i < num_test_values; i++) {
        ASSERT_EQ(result[i], test_values[i]);
    }
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