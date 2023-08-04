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

TEST(kstd_streams_Stream, test_find_value) {
    using namespace kstd::streams;

    const std::vector<kstd::u32> values {1, 11, 111, 1111, 11111, 111111};
    // clang-format off
    const auto first_value = stream(values)
        .find([](auto& value) {
            return value << 1 == 222;
        });
    // clang-format on

    ASSERT_TRUE(first_value);
    ASSERT_EQ(*first_value, 111);
}