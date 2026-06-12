//#include <iostream>

#include "Server01.h"

#include <string>
#include <exception>

using std::string;

template <typename QItem1>
class MyQueue;

template <typename QItem1>
class QueueItem
{
    QItem1 item;
    QueueItem* next;
    QueueItem* prev;

    friend class MyQueue<QItem1>;
public:
    QueueItem(QItem1 item):item(item)
    {
        
    }
};

template <typename QItem1>
class MyQueue
{
    QueueItem<QItem1>* start   = nullptr;
    QueueItem<QItem1>* end     = nullptr;
    int count = 0;
    // TODO: REMOVE?
//    QueueItem<QItem1>* current = nullptr;
public:

    MyQueue()
    {
        
    }
    ~MyQueue()
    {
        this->removeAll();
    }

    void removeAll()
    {
        if (this->end == nullptr) {
            return;
        }

        QueueItem<QItem1>* ptr = this->end;

        while (ptr->next != nullptr) {
            ptr = ptr->next;
            delete ptr->prev;
            ptr->prev = nullptr;
            //this->count--;
        }
        delete ptr;

        this->end   = nullptr;
        this->start = nullptr;
        this->count = 0;
    }

    int getCount()
    {
        return this->count;
    }

    // to next
    void push_start(QItem1 _item)
    {
        if (this->start == nullptr) {
            QueueItem<QItem1>* newItem = new QueueItem<QItem1>(_item);

            this->start = newItem;
            this->end   = newItem;

            newItem->next = nullptr;
            newItem->prev = nullptr;
        } else {
            QueueItem<QItem1>* newItem = new QueueItem<QItem1>(_item);

            this->start->next = newItem;
            newItem->prev = this->start;
            newItem->next = nullptr;
            // do step
            this->start = newItem;
        }
        this->count++;
    }

    QItem1 pop_start()
    {
        if (this->start == nullptr) {
            throw "Warning. The queue is empty.";
        }

        QItem1 result = this->start->item;
        if (this->start == this->end) {
            delete this->start;

            this->start = nullptr;
            this->end   = nullptr;
        } else {
            QueueItem<QItem1> *ptr = this->start;

            this->start = ptr->prev;
            this->start->next = nullptr;

            delete ptr;
        }
        this->count--;

        return result;
    }

/*
    // prev
    void push_end(QItem1 _item)
    {
        if (this->end == nullptr) {
            this->end = new QueueItem(_item);
            this->start = this->end;
            this->end->prev = nullptr;
            this->end->next = nullptr;
        } else {
            this->end->prev = new QueueItem(_item);
            this->end->prev->prev = nullptr;
            this->end->prev->next = this;
            this->end = this->end->prev;
        }
    }/**/
    QItem1 pop_end()
    {
        if (this->end == nullptr) {
            throw "Warning. The queue is empty.";
        }

        QItem1 result = this->end->item;
        if (this->end == this->start) {
            delete this->end;
            this->end = nullptr;
            this->start = nullptr;
        } else if (this->end != this->start) {
            QueueItem<QItem1>* ptr = this->end;

            this->end = this->end->next;
            this->end->prev = nullptr;

            delete ptr;
        }
        this->count--;

        return result;
    }
};

// TODO: template
class ConsumerProducer
{

};

int main() {
    try {
        MyQueue<int> mq{};
        mq.push_start(77);
        mq.push_start(78);
        mq.push_start(79);

        int val11 = mq.pop_start();
        int val101 = mq.pop_end();

//        int val102 = mq.pop_end();
//        int val12 = mq.pop_start();
//        int val111 = mq.pop_end();
//        int val112 = mq.pop_end();

        MyQueue<int> mq2{};

        MyQueue<int> mq3{};
        mq3.push_start(88);
//        int val2 = mq2.pop_end();
//        int val3 = mq3.pop_end();


        cout << "q1=" << mq.getCount() << endl;
        cout << "q2=" << mq2.getCount() << endl;
        cout << "q3=" << mq3.getCount() << endl;
    } catch (const char* msg) {
        cout << "Exc=" << msg << endl;
    }

//    mq.pus

    cout  << "NED" << endl;












    //int ret = Server01::runServer01();

//    return ret;
return 0;
}

