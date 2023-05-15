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
 * @since 29/03/2023
 */

#include <vector>
#include <gtest/gtest.h>
#include "kstd/streams/owning_iterator_streamable.hpp"

TEST(kstd_streams_OwningIteratorStreamable, TestIterate) {
    std::vector<float> values({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    auto owned_values = values;
    auto streamable = kstd::streams::OwningIteratorStreamable(std::move(owned_values));

    for (const auto value: values) {
        auto element = streamable.next();
        ASSERT_TRUE(element);
        ASSERT_EQ(element.borrow_value(), value);
    }

    auto element = streamable.next();
    ASSERT_FALSE(element);
}