
#ifndef NBE_CHAT_SERVER_MYTHREADPOOL01_H
#define NBE_CHAT_SERVER_MYTHREADPOOL01_H

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using std::thread;
using std::cout;
using std::endl;
using std::queue;
using std::vector;
using std::mutex;
using std::unique_lock;
using std::condition_variable;

class MyThreadPool01
{
    vector<thread> tPool;
    queue<std::function<void()>> tasksQueue;
    mutex queueMutex;
    condition_variable cvWorkers;


public:
    static bool isThreadPoolStopped;

    MyThreadPool01(int count)
    {
        MyThreadPool01::isThreadPoolStopped = false;
        //
        this->tPool.reserve(count);

        for (int ind = 0; ind < count; ind++) {
            this->tPool.push_back(thread([ind] {
                cout << " added to Front with i=" << ind << endl;
            }));
        }
    }

    ~MyThreadPool01()
    {
        unique_lock ul2(this->queueMutex);
        this->isThreadPoolStopped = true;
        ul2.unlock();

        this->cvWorkers.notify_all();

        for (int ind = 0; ind < this->tPool.size(); ind++) {
            if (this->tPool[ind].joinable()) {
                this->tPool[ind].join();
            }
        }
    }

    void enqueueTask(std::function<void()>& newTask)
    {
        unique_lock ul1(this->queueMutex);
        this->tasksQueue.push(newTask);
        ul1.unlock();

        this->cvWorkers.notify_one();
    }
};

bool MyThreadPool01::isThreadPoolStopped = false;

#endif //NBE_CHAT_SERVER_MYTHREADPOOL01_H
