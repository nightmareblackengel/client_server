
#ifndef NBE_CHAT_SERVER_MYTHREADPOOL_H
#define NBE_CHAT_SERVER_MYTHREADPOOL_H

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

class MyThreadPool
{
    vector<thread> tPool;
    queue<std::function<void()>> tasksQueue;
    mutex queueMutex;
    condition_variable cvWorkers;


public:
    bool isThreadPoolStopped = false;

    MyThreadPool(int count)
    {
        this->tPool.reserve(count);

        for (int ind = 0; ind < count; ind++) {
            this->tPool.emplace_back(thread(&MyThreadPool::workerThread, this, ind));
        }
    }

    ~MyThreadPool()
    {
        this->stopTaskEvent();
    }

    void stopTaskEvent()
    {
        if (this->isThreadPoolStopped == true) {
            return;
        }

//        cout << "destructor"<< endl;
        unique_lock ul2(this->queueMutex);
//        cout << "destructor locked" << endl;
        this->isThreadPoolStopped = true;
        ul2.unlock();
//        cout << "destructor UNlocked" << endl;

        this->cvWorkers.notify_all();

        for (int ind = 0; ind < this->tPool.size(); ind++) {
            if (this->tPool[ind].joinable()) {
                this->tPool[ind].join();
//                cout << "worker with i=[" << ind << "] joined" << endl;
            }
        }
    }

    void enqueueTask(const std::function<void()> newTask)
    {
//        cout << "locking TASK for push" << endl;
//        sleep(2);
        unique_lock ul1(this->queueMutex);
//        cout << "ADDED  TASK" << endl;
        this->tasksQueue.push(newTask);
        ul1.unlock();

//        sleep(3);
//        cout << "Notify one " << endl;
        this->cvWorkers.notify_one();
    }

    void workerThread(int ind)
    {
//        cout << " added to Front with i=" << ind << endl;
        while(true) {
//            cout << "[" << ind << "] try locking..." << endl;
            unique_lock ul3(this->queueMutex);

//            cout << "[" << ind << "] go to sleep" << endl;
            this->cvWorkers.wait(ul3, [this, ind] {
//                cout << "[" << ind << "] checked " << endl;
                // need false
                return !this->tasksQueue.empty() || this->isThreadPoolStopped;
            });

//            cout << "[" << ind << "] wake up..." << endl;
            if (this->isThreadPoolStopped && this->tasksQueue.empty()) {
//                cout << "cycle ended"<< endl;
                ul3.unlock();
                break;
            }

            std::function<void()> task = std::move(this->tasksQueue.front());
            this->tasksQueue.pop();
            ul3.unlock();

//            cout << "[" << ind << "] run task()" << endl;
            // run task function
            task();
//            cout << "[" << ind << "] END task()" << endl;
        }
    }
};

#endif //NBE_CHAT_SERVER_MYTHREADPOOL_H
