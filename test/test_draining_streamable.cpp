/**
 * @author Alexander Hinze
 * @since 03/04/2023
 */

#include <gtest/gtest.h>
#include "kstd/streams/stream.hpp"

TEST(kstd_streams_DrainingStreamable, TestIterate) {
    std::vector<float> values({1.0F, 2.0F, 3.0F, 4.0F, 1.5F, 2.5F, 3.5F, 4.5F});
    std::vector<float> to_drain(values);
    auto streamable = kstd::streams::DrainingStreamable(to_drain);

    for (const auto value: values) {
        auto element = streamable.next();
        ASSERT_TRUE(element);
        ASSERT_EQ(*element, value);
    }

    auto element = streamable.next();
    ASSERT_FALSE(element);

    ASSERT_TRUE(to_drain.empty());
}