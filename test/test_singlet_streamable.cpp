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

#include <gtest/gtest.h>
#include <cxxs/singlet_streamable.hpp>

TEST(cxxs_SingletStreamable, TestIterate) {
    float value = 1337.0F;
    auto streamable = cxxs::SingletStreamable(value);
    auto element = streamable.next();

    ASSERT_TRUE(element);
    ASSERT_EQ(*element, value);

    element = streamable.next();

    ASSERT_FALSE(element);
}