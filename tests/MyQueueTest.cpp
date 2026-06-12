
#include <gtest/gtest.h>

#include "../include/tmp/MyQueue.h"

class MyQueueTest: public testing::Test
{
protected:

    MyQueueTest()
    {
        mq1.push_start(77);
        mq1.push_start(78);
        mq1.push_start(79);

        mq2.push_end(-11);
        mq2.push_end(-12);
        mq2.push_end(-13);

        mq3.push_start(21);
        mq3.push_end(-21);
        mq3.push_end(-22);
        mq3.push_start(22);
        mq3.push_end(-23);
        mq3.push_start(23);
    }

    MyQueue<int> mq1;
    MyQueue<int> mq2;
    MyQueue<int> mq3;
};

TEST_F(MyQueueTest, Mq1_pop_start)
{
    ASSERT_EQ(mq1.getCount(), 3);

    ASSERT_EQ(mq1.pop_start(), 79);
    ASSERT_EQ(mq1.pop_start(), 78);
    ASSERT_EQ(mq1.pop_start(), 77);

    ASSERT_EQ(mq1.getCount(), 0);

    ASSERT_THROW(mq1.pop_start(), const char*);
}

TEST_F(MyQueueTest, Mq2_pop_end)
{
    ASSERT_EQ(mq2.getCount(), 3);

    ASSERT_EQ(mq2.pop_end(), -13);
    ASSERT_EQ(mq2.pop_end(), -12);
    ASSERT_EQ(mq2.pop_end(), -11);

    ASSERT_EQ(mq2.getCount(), 0);

    ASSERT_THROW(mq2.pop_start(), const char*);
}

TEST_F(MyQueueTest, Mq3_pop)
{
    ASSERT_EQ(mq3.getCount(), 6);

    ASSERT_EQ(mq3.pop_end(), -23);
    ASSERT_EQ(mq3.pop_end(), -22);
    ASSERT_EQ(mq3.pop_start(), 23);
    ASSERT_EQ(mq3.pop_start(), 22);

    ASSERT_EQ(mq3.getCount(), 2);
}