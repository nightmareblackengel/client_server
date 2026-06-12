#ifndef NBE_CHAT_SERVER_MYQUEUE_H
#define NBE_CHAT_SERVER_MYQUEUE_H

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
        QueueItem<QItem1>* newItem = new QueueItem<QItem1>(_item);
        if (this->start == nullptr) {
            this->start = newItem;
            this->end   = newItem;

            newItem->next = nullptr;
            newItem->prev = nullptr;
        } else {
            newItem->prev = this->start;
            newItem->next = nullptr;
            // do step
            this->start->next = newItem;
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

    void push_end(QItem1 _item)
    {
        QueueItem<QItem1>* newItem = new QueueItem<QItem1>(_item);
        if (this->end == nullptr) {
            this->start = newItem;
            this->end = newItem;

            this->end->prev = nullptr;
            this->end->next = nullptr;
        } else {
            newItem->prev = nullptr;
            newItem->next = this->end;

            this->end->prev = newItem;
            this->end = newItem;
        }
        this->count++;
    }

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

#endif //NBE_CHAT_SERVER_MYQUEUE_H
