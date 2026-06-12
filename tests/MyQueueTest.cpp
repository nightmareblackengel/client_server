
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

        mq2.push_start(11);
//        mq2.push_end(11)
    }

    MyQueue<int> mq1;
    MyQueue<int> mq2;
    MyQueue<int> mq3;
};

TEST_F(MyQueueTest, CreateMq3)
{
    ASSERT_EQ(mq1.getCount(), 3);
    mq1.pop_start();
    mq1.pop_start();
    mq1.pop_start();
    ASSERT_EQ(mq1.getCount(), 0);

    ASSERT_THROW(mq1.pop_start(), const char*);
}
