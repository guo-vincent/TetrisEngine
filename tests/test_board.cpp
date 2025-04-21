#include <gtest/gtest.h>

// If gtest is downloaded correctly, this should say success
TEST(BoardTest, SanityCheck) {
    EXPECT_EQ(1 + 1, 2);
    ASSERT_TRUE(1 + 1 == 2);
}

TEST(SanityTests, BasicMath) {
    const int result = 1 + 1;
    EXPECT_EQ(result, 2) << "1+1 should equal 2, got " << result;
}