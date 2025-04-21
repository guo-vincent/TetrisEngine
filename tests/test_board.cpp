#include <gtest/gtest.h>

// Add this new test suite at the end of the file
TEST(BoardTest, SanityCheck) {
    EXPECT_EQ(1 + 1, 2);  // Basic arithmetic verification
    ASSERT_TRUE(1 + 1 == 2);  // Alternative assertion style
}

// Or create a separate test suite for fundamental checks
TEST(SanityTests, BasicMath) {
    const int result = 1 + 1;
    EXPECT_EQ(result, 2) << "1+1 should equal 2, got " << result;
}