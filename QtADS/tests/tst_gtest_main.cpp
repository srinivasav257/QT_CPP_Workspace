#include <gtest/gtest.h>
#include <QString>

TEST(SimpleQtTest, StringCheck) {
    QString str = "Hello";
    EXPECT_EQ(str.length(), 5);
    EXPECT_STREQ(str.toUtf8().constData(), "Hello");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
