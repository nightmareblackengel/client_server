
#include <gtest/gtest.h>

#include "../include/tmp/MyQueue.h"

TEST(MyQueueTest, CreateEmptyQueue)
{
    MyQueue<int> mq1;
    mq1.push_start(77);

    EXPECT_EQ(mq1.pop_start(), 77);
}