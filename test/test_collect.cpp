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
 * @since 18/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/stream.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_Stream, test_collect_value) {
    using namespace kstd::streams;

    const std::vector<std::string> values {"Hello", "World", "OwO"};
    const auto cloned_values = stream(values).collect<std::vector>(collectors::push_back);
    const auto num_values = values.size();
    ASSERT_EQ(num_values, cloned_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], cloned_values[index]);
    }
}

TEST(kstd_streams_Stream, test_collect_pointer) {
    using namespace kstd::streams;

    std::vector<std::string> values {"Hello", "World", "OwO"};
    std::vector<std::string*> addresses {};

    for(auto& value : values) {
        addresses.push_back(&value);
    }

    const auto cloned_values = stream(addresses).collect<std::vector>(collectors::push_back);
    const auto num_values = values.size();
    ASSERT_EQ(num_values, cloned_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], *cloned_values[index]);
    }
}

TEST(kstd_streams_Stream, test_collect_const_pointer) {
    using namespace kstd::streams;

    std::vector<std::string> values {"Hello", "World", "OwO"};
    std::vector<const std::string*> addresses {};

    for(const auto& value : values) {
        addresses.push_back(&value);
    }

    const auto cloned_values = stream(addresses).collect<std::vector>(collectors::push_back);
    const auto num_values = values.size();
    ASSERT_EQ(num_values, cloned_values.size());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], *cloned_values[index]);
    }
}

TEST(kstd_streams_Stream, test_collect_string) {
    using namespace kstd::streams;

    const std::vector<char> values {'O', 'w', 'O', '!'};
    const auto value = stream(values).collect<std::basic_string>(collectors::plus_assign);
    const auto num_values = values.size();
    ASSERT_EQ(num_values, value.length());

    for(kstd::usize index = 0; index < num_values; ++index) {
        ASSERT_EQ(values[index], value[index]);
    }
}

TEST(kstd_streams_Stream, test_collect_string_joining) {
    using namespace kstd::streams;

    const std::vector<char> values {'O', 'w', 'O', '!'};
    const auto value = stream(values).collect<std::basic_string>(collectors::joining('\t'));
    const auto num_values = values.size();
    ASSERT_EQ((num_values << 1) - 1, value.length());
    ASSERT_EQ(value, "O\tw\tO\t!");
}