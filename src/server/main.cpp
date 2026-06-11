//#include <iostream>

#include "Server01.h"

#include <string>

using std::string;

template <typename QueueItem>
struct QueueItemStruct
{
    QueueItem item;
    QueueItemStruct* next;
    QueueItemStruct* prev;
};

template <typename QueueItem>
class MyQueue
{
    static QueueItemStruct<QueueItem>* top     = nullptr;
    static QueueItemStruct<QueueItem>* bottom  = nullptr;
public:

    MyQueue()= delete;
    MyQueue(QueueItem &_item)
    {
        this->item = _item;
    }
    ~MyQueue()
    {
//        delete this->item;
    }

    // to next
    void push_next(QueueItem _item)
    {
        if (MyQueue::top == nullptr) {
            MyQueue::top = new MyQueue(_item);
            MyQueue::bottom = MyQueue::top;
            this->next = nullptr;
            this->prev = nullptr;
        } else {
            MyQueue::top->next = new MyQueue(_item);
            MyQueue::top->next->prev = this;
            MyQueue::top->next->next = nullptr;
            // step forward
            MyQueue::top = MyQueue::top->next;
        }
    }

    QueueItem pop_next()
    {
        QueueItem result = nullptr;
        if (MyQueue::bottom == nullptr) {

        } else if (MyQueue::bottom == MyQueue::top) {
            result = MyQueue::bottom->item;
            delete MyQueue::bottom;
            MyQueue::bottom = nullptr;
            MyQueue::top = nullptr;
        } else if (MyQueue::bottom != MyQueue::top) {
            MyQueue *ptr = MyQueue::top;
            MyQueue::top = MyQueue::top->prev;
            ptr->prev = nullptr;
//            ptr->next = nullptr;
            result = ptr->item;
            delete ptr;
        }

        return result;
    }


    // prev
    void push_prev(QueueItem _item)
    {
        if (MyQueue::bottom == nullptr) {
            MyQueue::bottom = new MyQueue(_item);
            MyQueue::top = MyQueue::bottom;
            MyQueue::bottom->prev = nullptr;
            MyQueue::bottom->next = nullptr;
        } else {
            MyQueue::bottom->prev = new MyQueue(_item);
            MyQueue::bottom->prev->prev = nullptr;
            MyQueue::bottom->prev->next = this;
            MyQueue::bottom = MyQueue::bottom->prev;
        }
    }
    QueueItem pop_prev()
    {
        QueueItem result = nullptr;
        if (MyQueue::bottom == nullptr) {

        } else if (MyQueue::bottom == MyQueue::top) {
            result = MyQueue::bottom->item;
            delete MyQueue::bottom;
            MyQueue::bottom = nullptr;
            MyQueue::top = nullptr;
        } else if (MyQueue::bottom != MyQueue::top) {
            MyQueue* ptr = MyQueue::bottom;
            MyQueue::bottom = MyQueue::bottom->next;
            result = ptr->item;
            ptr->next = nullptr;
//            ptr->prev = nullptr;
            delete ptr;
        }
        return result;
    }

//    void getItem()
//    {
//        return this->item;
//    }
};

// TODO: template
class ConsumerProducer
{

};

int main() {
    MyQueue mq();















    //int ret = Server01::runServer01();

//    return ret;
return 0;
}

