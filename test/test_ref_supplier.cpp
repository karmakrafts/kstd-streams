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
 * @since 12/07/2023
 */

#include <gtest/gtest.h>
#include <kstd/streams/ref_supplier.hpp>
#include <string>
#include <vector>

TEST(kstd_streams_RefSupplier, TestGetNextValue) {
    using namespace std::string_view_literals;

    std::vector<std::string> values({"Hello", "World", "!"});
    kstd::streams::RefSupplier supplier(values.begin(), values.end());
    kstd::usize index = 0;

    auto element = supplier.get_next();

    while(element) {
        auto& expected_value = values[index++];
        ASSERT_TRUE(element);
        ASSERT_EQ(*element, expected_value);
        ASSERT_EQ(&*element, &expected_value);
        element = supplier.get_next();
    }
}

TEST(kstd_streams_RefSupplier, TestGetNextPointer) {
    using namespace std::string_view_literals;

    std::vector<std::string> values({"Hello", "World", "!"});
    const auto num_values = values.size();
    std::vector<std::string*> addresses(num_values);

    for(kstd::usize index = 0; index < num_values; ++index) {
        addresses[index] = &values[index];
    }

    kstd::streams::RefSupplier supplier(addresses.begin(), addresses.end());
    kstd::usize index = 0;

    auto element = supplier.get_next();

    while(element) {
        auto& expected_value = values[index++];
        ASSERT_TRUE(element);
        ASSERT_EQ(**element, expected_value);
        ASSERT_EQ(*element, &expected_value);
        element = supplier.get_next();
    }
}