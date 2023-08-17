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
 * @since 17/08/2023
 */

#include <gtest/gtest.h>
#include <kstd/defaults.hpp>
#include <kstd/libc.hpp>
#include <kstd/streams/stream.hpp>

struct SomeLinkedList final {
    kstd::u32 value;
    SomeLinkedList* next;

    KSTD_DEFAULT_MOVE_COPY(SomeLinkedList, SomeLinkedList)

    explicit SomeLinkedList(kstd::u32 val) noexcept :
            value {val},
            next {nullptr} {
    }

    ~SomeLinkedList() noexcept = default;
};

TEST(kstd_streams_Stream, test_linked_struct_pointer) {
    using namespace kstd::streams;

    auto* first = new SomeLinkedList {1};
    auto* second = first->next = new SomeLinkedList {2};
    auto* third = second->next = new SomeLinkedList {3};
    auto* fourth = third->next = new SomeLinkedList {4};

    kstd::usize index = 0;

    // clang-format off
    stream(first, [](auto* value) -> auto* {
        return value->next;
    }).for_each([&](auto& value) {
        switch(index) {
            case 0: ASSERT_EQ(first, &value);   break;
            case 1: ASSERT_EQ(second, &value);  break;
            case 2: ASSERT_EQ(third, &value);   break;
            case 3: ASSERT_EQ(fourth, &value);  break;
        }
        ++index;
    });
    // clang-format on

    delete fourth;
    delete third;
    delete second;
    delete first;
}

TEST(kstd_streams_Stream, test_linked_struct_const_pointer) {
    using namespace kstd::streams;

    auto* first = new SomeLinkedList {1};
    auto* second = first->next = new SomeLinkedList {2};
    auto* third = second->next = new SomeLinkedList {3};
    auto* fourth = third->next = new SomeLinkedList {4};

    kstd::usize index = 0;
    const auto* const_first = reinterpret_cast<const SomeLinkedList*>(first);// NOLINT

    // clang-format off
    stream(const_first, [](const auto* value) -> const auto* {
        return value->next;
    }).for_each([&](const auto& value) {
        switch(index) {
            case 0: ASSERT_EQ(first, &value);   break;
            case 1: ASSERT_EQ(second, &value);  break;
            case 2: ASSERT_EQ(third, &value);   break;
            case 3: ASSERT_EQ(fourth, &value);  break;
        }
        ++index;
    });
    // clang-format on

    delete fourth;
    delete third;
    delete second;
    delete first;
}