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
 * @since 14/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/streams.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_IdentityPipe, TestGetNext) {
    std::vector<std::string> values({"Hello", "World", "OwO"});
    auto stream = kstd::streams::stream(values);
    ASSERT_EQ(*stream.get_next(), "Hello");
    ASSERT_EQ(*stream.get_next(), "World");
    ASSERT_EQ(*stream.get_next(), "OwO");
    ASSERT_FALSE(stream.get_next());
}

TEST(kstd_streams_IdentityPipe, TestGetNextConst) {
    const std::vector<std::string> values({"Hello", "World", "OwO"});
    auto stream = kstd::streams::stream(values);
    ASSERT_EQ(*stream.get_next(), "Hello");
    ASSERT_EQ(*stream.get_next(), "World");
    ASSERT_EQ(*stream.get_next(), "OwO");
    ASSERT_FALSE(stream.get_next());
}