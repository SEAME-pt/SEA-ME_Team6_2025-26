#include <gtest/gtest.h>
#include "math.hpp"

TEST(MathStudy, Add) {
    EXPECT_EQ(add(2, 3), 5);
    EXPECT_EQ(add(-1, 1), 0);
}

TEST(MathStudy, Sub) {
    EXPECT_EQ(sub(5, 3), 2);
    EXPECT_EQ(sub(0, 4), -4);
}